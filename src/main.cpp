// === Clang LibTooling and AST Matcher Headers ===
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <llvm/Support/raw_ostream.h>


// === Custom Match Handlers and Memory Tracker ===
#include "MatchHandlers/NewExprHandler.hpp"
#include "MemoryTracker/AllocationTable.hpp"
#include "MatchHandlers/DeleteExprHandler.hpp"
#include "MatchHandlers/SmartPtrHandler.hpp"
#include "MatchHandlers/RecursionHandler.hpp"
#include "MatchHandlers/WeakPtrHandler.hpp"
#include "MatchHandlers/DataFlowHandler.hpp" 


using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

// for debugging
class FunctionPrinter : public MatchFinder::MatchCallback {
public:
    void run(const MatchFinder::MatchResult &Result) override {
        if (const FunctionDecl *FD = Result.Nodes.getNodeAs<FunctionDecl>("func")) {
            llvm::outs() << "Function: " << FD->getNameAsString() << "\n";
        }
    }
};

// === Command-line option category ===
static llvm::cl::OptionCategory ToolCategory("tool options");

// Include/exclude filters for controlling analysis scope via path substrings
static llvm::cl::list<std::string>
    IncludeHeaders("include-headers",
                   llvm::cl::desc("Only analyze locations whose file path contains one of these substrings (comma separated)"),
                   llvm::cl::CommaSeparated,
                   llvm::cl::cat(ToolCategory));

static llvm::cl::list<std::string>
    ExcludeHeaders("exclude-headers",
                   llvm::cl::desc("Skip any location whose file path contains one of these substrings"),
                   llvm::cl::CommaSeparated,
                   llvm::cl::cat(ToolCategory));

// File filtering function using include/exclude command-line args
bool pathAllows(const clang::SourceManager &SM, clang::SourceLocation Loc) {
    auto FileName = SM.getFilename(Loc).str();

    // Exclude first if matched
    for (auto &pat : ExcludeHeaders)
        if (FileName.find(pat) != std::string::npos)
            return false;

    // If no includes specified, accept everything not excluded
    if (IncludeHeaders.empty())
        return true;

    // Otherwise, only accept paths matching one of the include filters
    for (auto &pat : IncludeHeaders)
        if (FileName.find(pat) != std::string::npos)
            return true;

    return false;
}

int main(int argc, const char **argv) {
    llvm::outs() << "[Tool] starting\n\n";

    // Parse command-line options
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }

    CommonOptionsParser &OptionsParser = ExpectedParser.get();
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    // Suppress Clang's internal diagnostics
    Tool.appendArgumentsAdjuster(
        [](const std::vector<std::string> &args, llvm::StringRef) {
            std::vector<std::string> adjustedArgs = args;
            adjustedArgs.push_back("-w");  // disable all warnings
            return adjustedArgs;
        }
    );

    // === Initialize memory tracker and handlers ===
    AllocationTable tracker;
    NewExprHandler newHandler(tracker);
    DeleteExprHandler deleteHandler(tracker);
    SmartPtrHandler upSpHandler;
    WeakPtrHandler weakHandler;
    RecursionHandler recHandler;

    MatchFinder finder;
    // std::unique_ptr<CFG> cfg = CFG::buildCFG(FD, FD->getBody(), &Ctx, 
    //                             CFG::BuildOptions());
    
    // === Matcher: new allocations to local/global variables ===
    finder.addMatcher(
        varDecl(
            hasInitializer(
                ignoringParenImpCasts(
                    ignoringImplicit(
                        cxxNewExpr().bind("newExpr")
                    )
                )
            ),
            anyOf(isExpansionInMainFile(), hasGlobalStorage())
        ).bind("lhsVar"),
        &newHandler
    );

    // === Matcher: heap allocation via assignment (e.g., p = new T();) ===
    finder.addMatcher(
        binaryOperator(
            hasOperatorName("="),
            hasRHS(cxxNewExpr().bind("newExpr")),
            hasLHS(ignoringParenImpCasts(declRefExpr(to(varDecl().bind("lhsVar")))))
        ),
        &newHandler
    );

    // === Matcher: delete/delete[] expressions ===
    finder.addMatcher(
        cxxDeleteExpr(
            unless(isExpansionInSystemHeader())
        ).bind("deleteExpr"),
        &deleteHandler
    );

    // === Matcher: suggest replacing raw pointers with smart pointers === 
    finder.addMatcher(
    varDecl(
      hasInitializer(
        ignoringParenImpCasts(
          // strip away any implicit / parens so we always land on the CXXNewExpr
          ignoringImplicit(
            cxxNewExpr().bind("newExpr")
          )
        )
      ),
      hasType(pointerType()),
      isExpansionInMainFile()    // optional: skip system headers
    ).bind("ptrDecl"),
    &upSpHandler
  );

    // === Matcher: suggest replacing raw pointer fields with weak_ptr/shared_ptr ===
    finder.addMatcher(
        fieldDecl(
            hasType(pointerType())
        ).bind("fieldPtr"),
        &weakHandler
    );

    // === Matcher: detect direct recursion (function calls itself) ===

    finder.addMatcher(
      callExpr(unless(isExpansionInSystemHeader())).bind("anyCall"),
      &recHandler
    );

    // CFG-based null-deref & uninit tracking
    // DataFlowHandler dfHandler;
    // finder.addMatcher(
    //    functionDecl(isDefinition(), isExpansionInMainFile()).bind("func"),
    //    &dfHandler
    //  );
     // Null-deref detection
  NullDerefHandler ndHandler;
  finder.addMatcher(
    varDecl(hasInitializer(ignoringParenImpCasts(cxxNullPtrLiteralExpr())))
      .bind("nullVar"),
    &ndHandler
  );
  finder.addMatcher(
    unaryOperator(hasOperatorName("*"),
                  hasUnaryOperand(declRefExpr(to(varDecl()))))
      .bind("derefOp"),
    &ndHandler
  );

  // Uninitialized-var detection
  UninitVarHandler uvHandler;
  finder.addMatcher(
    varDecl(hasLocalStorage(), unless(hasInitializer(expr()))).bind("uninitVar"),
    &uvHandler
  );
  finder.addMatcher(
    declRefExpr(to(varDecl())).bind("useVar"),
    &uvHandler
  );


    // === Run the tool with all registered matchers ===
    // runYourAnalysis(FD, std::move(cfg), Ctx);
    int result = Tool.run(newFrontendActionFactory(&finder).get());

    // === Finalize and report analysis ===
    tracker.finalizeDeletes(); // Validate and report delete correctness
    tracker.reportLeaks();     // Report memory leaks

    llvm::outs() << "\n\033[1m[Analysis Complete]\033[0m\n";
    return result;
}

/*
Structure Overview:

AllocationTable tracker;               → Tracks heap allocations and deletes
NewExprHandler newHandler(tracker);    → Handles `new` / `new[]` allocations
finder.addMatcher(...)                 → Registers AST patterns for heap use
DeleteExprHandler deleteHandler;       → Tracks delete/delete[] and analyzes mismatches
SmartPtrHandler                        → Recommends smart pointer replacements
WeakPtrHandler                         → Flags raw pointer class fields
RecursionHandler                       → Detects direct recursion
Tool.run(...)                          → Triggers AST traversal and handler callbacks
tracker.finalizeDeletes();             → Post-AST cleanup: detect double delete / invalid delete
tracker.reportLeaks();                 → Emits unfreed memory report
*/

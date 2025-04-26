// LibTooling Headers
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <llvm/Support/raw_ostream.h>


// My headers
#include "MatchHandlers/NewExprHandler.hpp"
#include "MemoryTracker/AllocationTable.hpp"
#include "MatchHandlers/DeleteExprHandler.hpp"
#include "MatchHandlers/SmartPtrHandler.hpp"


using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

class FunctionPrinter : public MatchFinder::MatchCallback {
public:
    void run(const MatchFinder::MatchResult &Result) override {
        if (const FunctionDecl *FD = Result.Nodes.getNodeAs<FunctionDecl>("func")) {
            llvm::outs() << "Function: " << FD->getNameAsString() << "\n";
        }
    }
};

static llvm::cl::OptionCategory ToolCategory("tool options");


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


bool pathAllows(const clang::SourceManager &SM, clang::SourceLocation Loc) {
    auto FileName = SM.getFilename(Loc).str();
  
    // Exclude wins out first:
    for (auto &pat : ExcludeHeaders)
      if (FileName.find(pat) != std::string::npos)
        return false;
  
    // If no includes were specified, allow everything not excluded
    if (IncludeHeaders.empty())
      return true;
  
    // Otherwise only allow if it matches one of the includes
    for (auto &pat : IncludeHeaders)
      if (FileName.find(pat) != std::string::npos)
        return true;
  
    return false;
  }
  
int main(int argc, const char **argv) {
    llvm::outs() << "[Tool] starting\n\n";

    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }

    CommonOptionsParser &OptionsParser = ExpectedParser.get();
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
    // Disable all Clang's built-in warnings
    Tool.appendArgumentsAdjuster(
        [](const std::vector<std::string> &args, llvm::StringRef) {
            // Disable all built-in warnings
            std::vector<std::string> adjustedArgs = args;
            adjustedArgs.push_back("-w");
            return adjustedArgs;
        }
    );

    // FunctionPrinter Printer;
    // Finder.addMatcher(functionDecl().bind("func"), &Printer);

    AllocationTable tracker;
    NewExprHandler newHandler(tracker);  // <-- this creates the actual handler
    MatchFinder finder;
    

    finder.addMatcher(
        varDecl(hasInitializer(cxxNewExpr().bind("newExpr"))).bind("lhsVar"),
        &newHandler
    );

    finder.addMatcher(
        binaryOperator(
            hasOperatorName("="),
            hasRHS(cxxNewExpr().bind("newExpr")),
            hasLHS(ignoringParenImpCasts(declRefExpr(to(varDecl().bind("lhsVar")))))
        ),
        &newHandler
    );

    DeleteExprHandler deleteHandler(tracker);

    finder.addMatcher(
        cxxDeleteExpr(
            unless(isExpansionInSystemHeader())
        ).bind("deleteExpr"),
        &deleteHandler
    );

    SmartPtrHandler spHandler;
    finder.addMatcher(
        varDecl(
        hasInitializer(cxxNewExpr()),
        hasType(pointerType())
    ).bind("ptrDecl"),
    &spHandler
   );



    // return Tool.run(newFrontendActionFactory(&finder).get());
    int result = Tool.run(newFrontendActionFactory(&finder).get());

    
    // Analyze all deferred delete calls AFTER AST traversal is done
    tracker.finalizeDeletes();
    tracker.reportLeaks();
    return result;

}


/*
basic info abt tf is going on

AllocationTable tracker;	Keeps track of heap-allocated variables
NewExprHandler newHandler(tracker);	Creates the matcher callback
finder.addMatcher(...)	Registers two different new patterns
Tool.run(...)	Starts AST traversal and triggers callbacks
finder.addMatcher(...)	Registers the delete pattern
deleteHandler(tracker);	Creates the matcher callback for delete
finder.addMatcher(...)	Registers the delete pattern
deleteHandler(tracker);	Creates the matcher callback for delete
finder.addMatcher(...)	Registers the delete patterns
*/

#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <llvm/Support/raw_ostream.h>

#include "MatchHandlers/NewExprHandler.hpp"
#include "MemoryTracker/AllocationTable.hpp"


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

int main(int argc, const char **argv) {
    llvm::outs() << "[Tool] starting\n";

    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }

    CommonOptionsParser &OptionsParser = ExpectedParser.get();
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

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


    return Tool.run(newFrontendActionFactory(&finder).get());
}

/*
AllocationTable tracker;	Keeps track of heap-allocated variables
NewExprHandler newHandler(tracker);	Creates the matcher callback
finder.addMatcher(...)	Registers two different new patterns
Tool.run(...)	Starts AST traversal and triggers callbacks

*/

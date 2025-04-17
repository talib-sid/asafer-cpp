// LibTooling Headers
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/FrontendActions.h>

// #include <clang/ASTMatchers/ASTMatchers.h>
// #include <clang/ASTMatchers/ASTMatchFinder.h>

#include <llvm/Support/raw_ostream.h>


<<<<<<< HEAD
// My matcher headers (only alloc table in use rn, replacing all traversal logics with custom AST traversals)
// #include "MatchHandlers/NewExprHandler.hpp"
// #include "MatchHandlers/DeleteExprHandler.hpp"
#include "MemoryTracker/AllocationTable.hpp"

// my AST headers
#include "AST/MyASTConsumer.hpp"
#include "AST/MyASTVisitor.hpp"
#include "AST/MyFrontendAction.hpp"
=======
// My headers
#include "MatchHandlers/NewExprHandler.hpp"
#include "MemoryTracker/AllocationTable.hpp"
#include "MatchHandlers/DeleteExprHandler.hpp"
>>>>>>> 8984be5 (Delete expr handled (order-of-visitation issue))

using namespace clang;
using namespace clang::tooling;
// using namespace clang::ast_matchers;

// class FunctionPrinter : public MatchFinder::MatchCallback {
// public:
//     void run(const MatchFinder::MatchResult &Result) override {
//         if (const FunctionDecl *FD = Result.Nodes.getNodeAs<FunctionDecl>("func")) {
//             llvm::outs() << "Function: " << FD->getNameAsString() << "\n";
//         }
//     }
// };

static llvm::cl::OptionCategory ToolCategory("tool options");

// int main(int argc, const char **argv) {
//     llvm::outs() << "[Tool] starting\n\n";

//     auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolCategory);
//     if (!ExpectedParser) {
//         llvm::errs() << ExpectedParser.takeError();
//         return 1;
//     }

//     CommonOptionsParser &OptionsParser = ExpectedParser.get();
//     ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

//     // FunctionPrinter Printer;
//     // Finder.addMatcher(functionDecl().bind("func"), &Printer);

//     AllocationTable tracker;
//     NewExprHandler newHandler(tracker);  // <-- this creates the actual handler
//     MatchFinder finder;

//     finder.addMatcher(
//         varDecl(hasInitializer(cxxNewExpr().bind("newExpr"))).bind("lhsVar"),
//         &newHandler
//     );

//     finder.addMatcher(
//         binaryOperator(
//             hasOperatorName("="),
//             hasRHS(cxxNewExpr().bind("newExpr")),
//             hasLHS(ignoringParenImpCasts(declRefExpr(to(varDecl().bind("lhsVar")))))
//         ),
//         &newHandler
//     );

//     DeleteExprHandler deleteHandler(tracker);

//     finder.addMatcher(
//         cxxDeleteExpr().bind("deleteExpr"),
//         &deleteHandler
//     );



//     return Tool.run(newFrontendActionFactory(&finder).get());
// }




int main(int argc, const char **argv) {
<<<<<<< HEAD
    llvm::outs() << "[Tool] Starting RecursiveASTVisitor Analysis\n";
=======
    llvm::outs() << "[Tool] starting\n\n";
>>>>>>> 8984be5 (Delete expr handled (order-of-visitation issue))

    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }

    CommonOptionsParser &OptionsParser = ExpectedParser.get();
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    AllocationTable tracker;

    
    MyFrontendActionFactory factory(tracker);
    return Tool.run(&factory);    

<<<<<<< HEAD
    // return Tool.run(newFrontendActionFactory<MyFrontendAction>(&tracker).get());    
=======
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
        cxxDeleteExpr().bind("deleteExpr"),
        &deleteHandler
    );



    return Tool.run(newFrontendActionFactory(&finder).get());
>>>>>>> 8984be5 (Delete expr handled (order-of-visitation issue))
}



/*
basic info abt tf is going on

AllocationTable tracker;	Keeps track of heap-allocated variables
NewExprHandler newHandler(tracker);	Creates the matcher callback
nder.addMatcher(...)	Registers two different new patterl̥ns
Tool.run(...)	Starts AST traversal and triggersl̥l̥ callbacks

*/
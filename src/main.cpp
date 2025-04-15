// #include <clang/Tooling/CommonOptionsParser.h>
// #include <clang/Tooling/Tooling.h>
// #include <clang/Frontend/FrontendActions.h>
// #include <clang/ASTMatchers/ASTMatchers.h>
// #include <clang/ASTMatchers/ASTMatchFinder.h>
// #include <clang/AST/AST.h>
// #include <llvm/Support/CommandLine.h>
// #include <llvm/Support/raw_ostream.h>


// using namespace clang;
// using namespace clang::tooling;
// using namespace clang::ast_matchers;


// // My headers
// // matchings
// #include "MatchHandlers/NewExprHandler.hpp"
// #include "MatchHandlers/VarDeclHandler.hpp"

// // memory trackings
// #include "MemoryTracker/AllocationTable.hpp"




// // // simple class to handle variable declarations
// // class VarDeclHandler : public MatchFinder::MatchCallback {
// // public:
// //     void run(const MatchFinder::MatchResult &Result) override {
// //         if (const VarDecl *VD = Result.Nodes.getNodeAs<VarDecl>("varDecl")) {
// //             llvm::outs() << "Found variable: " << VD->getNameAsString() << "\n";
// //             llvm::outs() << "Type: " << VD->getType().getAsString() << "\n";
// //             llvm::outs() << "Location: " << VD->getLocation().printToString(Result.Context->getSourceManager()) << "\n";
// //             llvm::outs() << "------------------------\n";
// //         }
// //     }
// // };

// // // cl optioning, will be used to add flags as we go along
// // static llvm::cl::OptionCategory MyToolCategory("my-tool options");

// // int main(int argc, const char **argv) {
// //     auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
// //     if (!ExpectedParser) {
// //         llvm::errs() << ExpectedParser.takeError();
// //         return 1;
// //     }

// //     CommonOptionsParser &OptionsParser = ExpectedParser.get();

// //     // CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
// //     ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
 
// //     VarDeclHandler Handler;
// //     MatchFinder Finder;
// //     Finder.addMatcher(varDecl().bind("varDecl"), &Handler);

// //     return Tool.run(newFrontendActionFactory(&Finder).get());
// // }


// static llvm::cl::OptionCategory MyToolCategory("memory-analyzer options");


// int main(int argc, const char **argv) {
//     auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
//     if (!ExpectedParser) {
//         llvm::errs() << ExpectedParser.takeError();
//         return 1;
//     }
//     CommonOptionsParser &OptionsParser = ExpectedParser.get();

//     ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

//     AllocationTable tracker;
//     NewExprHandler newHandler(tracker);

//     MatchFinder finder;
//     finder.addMatcher(cxxNewExpr().bind("newExpr"), &newHandler);

//     return Tool.run(newFrontendActionFactory(&finder).get());
// }


#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <llvm/Support/CommandLine.h>

#include "MatchHandlers/NewExprHandler.hpp"
#include "MemoryTracker/AllocationTable.hpp"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

static llvm::cl::OptionCategory MyToolCategory("memory-analyzer options");



// debugging
#include <clang/AST/Decl.h>
class DebugFunctionHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
    public:
        void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override {
            if (const auto *FD = Result.Nodes.getNodeAs<clang::FunctionDecl>("anyFunc")) {
                llvm::outs() << "[MATCH] Function: " << FD->getNameAsString() << "\n";
            }
        }
    };



int main(int argc, const char **argv) {
    //debug 
    // llvm::outs() << "[Tool] Clang Memory Analyzer started\n";

    // auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
    // if (!ExpectedParser) {
    //     llvm::errs() << ExpectedParser.takeError();
    //     return 1;
    // }
    // CommonOptionsParser &OptionsParser = ExpectedParser.get();
    // ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    // AllocationTable tracker;
    // NewExprHandler newHandler(tracker);

    // MatchFinder finder;
    // finder.addMatcher(
    //     binaryOperator(
    //         hasOperatorName("="),
    //         hasRHS(cxxNewExpr().bind("newExpr")),
    //         hasLHS(declRefExpr(to(varDecl().bind("lhsVar"))))
    //     ),
    //     &newHandler
    // );

    // // debug
    // for (const auto &path : OptionsParser.getSourcePathList()) {
    //     llvm::outs() << "[File] Parsing: " << path << "\n";
    // }
    // llvm::outs() << "[Tool] Running tool...\n";    

    // return Tool.run(newFrontendActionFactory(&finder).get());

    DebugFunctionHandler debugHandler;

MatchFinder finder;
finder.addMatcher(
    functionDecl().bind("anyFunc"),
    &debugHandler
);

}

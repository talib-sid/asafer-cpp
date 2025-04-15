#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/AST/AST.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

// simple class to handle variable declarations
class VarDeclHandler : public MatchFinder::MatchCallback {
public:
    void run(const MatchFinder::MatchResult &Result) override {
        if (const VarDecl *VD = Result.Nodes.getNodeAs<VarDecl>("varDecl")) {
            llvm::outs() << "Found variable: " << VD->getNameAsString() << "\n";
            llvm::outs() << "Type: " << VD->getType().getAsString() << "\n";
            llvm::outs() << "Location: " << VD->getLocation().printToString(Result.Context->getSourceManager()) << "\n";
            llvm::outs() << "------------------------\n";
        }
    }
};

// cl optioning, will be used to add flags as we go along
static llvm::cl::OptionCategory MyToolCategory("my-tool options");

int main(int argc, const char **argv) {
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }

    CommonOptionsParser &OptionsParser = ExpectedParser.get();

    // CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
 
    VarDeclHandler Handler;
    MatchFinder Finder;
    Finder.addMatcher(varDecl().bind("varDecl"), &Handler);

    return Tool.run(newFrontendActionFactory(&Finder).get());
}

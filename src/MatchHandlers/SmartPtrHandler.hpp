#pragma once
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <llvm/Support/raw_ostream.h>
using namespace clang;
using namespace clang::ast_matchers;

class SmartPtrHandler : public MatchFinder::MatchCallback {
public:
    void run(const MatchFinder::MatchResult &Result) override {
        // Match a VarDecl of pointer type with a new-expression initializer
        const auto *VD = Result.Nodes.getNodeAs<VarDecl>("ptrDecl");
        if (!VD) return;

        // Only in main file (skip system headers)
        const auto &SM  = Result.Context->getSourceManager();
        auto Loc = VD->getBeginLoc();
        if (!SM.isInMainFile(Loc)) return;

        // Get the pointee type as string
        QualType QT = VD->getType()->getPointeeType();
        if (QT.isNull()) return;
        std::string Ty = QT.getAsString();

        // Emit a suggestion
        llvm::outs()
            << "\033[1;36m[HINT]\033[0m consider replacing\n    "
            << VD->getType().getAsString() << " " << VD->getNameAsString()
            << " = new " << Ty << "(...);\n  with:\n    "
            << "auto " << VD->getNameAsString()
            << " = std::make_unique<" << Ty << ">(...);\n\n";
    }
};

#pragma once
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <llvm/Support/raw_ostream.h>
using namespace clang;
using namespace clang::ast_matchers;

class WeakPtrHandler : public MatchFinder::MatchCallback {
public:
    void run(const MatchFinder::MatchResult &Result) override {
        // Bind "fieldPtr" on any fieldDecl whose type is pointerType()
        const auto *FD = Result.Nodes.getNodeAs<FieldDecl>("fieldPtr");
        if (!FD) return;

        // Only in your main file
        const auto &SM = Result.Context->getSourceManager();
        SourceLocation Loc = FD->getBeginLoc();
        if (!SM.isInMainFile(Loc)) return;

        QualType QT = FD->getType()->getPointeeType();
        if (QT.isNull()) return;
        std::string Ty = QT.getAsString();

        llvm::outs()
          << "\033[1;36m[HINT]\033[0m consider replacing field\n    "
          << FD->getType().getAsString() << " " << FD->getNameAsString() << ";\n"
          << "  with:\n    std::weak_ptr<" << Ty << "> "
          << FD->getNameAsString() << ";\n\n";
    }
};

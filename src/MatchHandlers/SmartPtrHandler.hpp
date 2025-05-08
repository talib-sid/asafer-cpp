#pragma once
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <llvm/Support/raw_ostream.h>
using namespace clang;
using namespace clang::ast_matchers;

class SmartPtrHandler : public MatchFinder::MatchCallback {
public:
    void run(const MatchFinder::MatchResult &R) override {
            // 1) find the VarDecl and NewExpr
    const auto *VD = R.Nodes.getNodeAs<VarDecl>("ptrDecl");
    const auto *NE = R.Nodes.getNodeAs<CXXNewExpr>("newExpr");
    if (!VD || !NE) return;

    // 2) filter out system headers
    const auto &SM = R.Context->getSourceManager();
    if (!SM.isInMainFile(VD->getLocation())) return;

    // 3) figure out the pointee type
    auto QT = VD->getType()->getPointeeType();
    if (QT.isNull()) return;
    std::string Ty = QT.getAsString();

    // 4) decide unique vs shared
    bool isGlobalOrStatic = VD->hasGlobalStorage() || VD->isStaticLocal();
    if (isGlobalOrStatic) {
      llvm::outs()
        << "\033[1;36m[HINT]\033[0m consider replacing\n    "
        << VD->getType().getAsString() << " " << VD->getNameAsString()
        << " = new " << Ty << "(...);\n  with:\n    "
        << (VD->isStaticLocal() ? "static auto " : "auto ")
        << VD->getNameAsString()
        << " = std::make_shared<" << Ty << ">(...);\n"
        << "  // and for non-owning observers use std::weak_ptr<" << Ty << ">.\n\n";
    } else {
      llvm::outs()
        << "\033[1;36m[HINT]\033[0m consider replacing\n    "
        << VD->getType().getAsString() << " " << VD->getNameAsString()
        << " = new " << Ty << "(...);\n  with:\n    "
        << "auto " << VD->getNameAsString()
        << " = std::make_unique<" << Ty << ">(...);\n\n";
    }
}

    //     // We bind "ptrDecl" only for VarDecls with new-initializer
    //     const auto *VD = Result.Nodes.getNodeAs<VarDecl>("ptrDecl");
    //     if (!VD) return;

    //     // Only in your main file
    //     const auto &SM = Result.Context->getSourceManager();
    //     SourceLocation Loc = VD->getBeginLoc();
    //     if (!SM.isInMainFile(Loc)) return;

    //     QualType QT = VD->getType()->getPointeeType();
    //     if (QT.isNull()) return;
    //     std::string Ty = QT.getAsString();

    //     // Determine storage: global/static vs local
    //     bool isGlobalOrStatic = VD->hasGlobalStorage() || VD->isStaticLocal();

    //     if (isGlobalOrStatic) {
    //         // Suggest shared_ptr
    //         llvm::outs()
    //           << "\033[1;36m[HINT]\033[0m consider replacing\n    "
    //           << VD->getType().getAsString() << " " << VD->getNameAsString()
    //           << " = new " << Ty << "(...);\n  with:\n    "
    //           << (VD->isStaticLocal() ? "static auto " : "auto ")
    //           << VD->getNameAsString()
    //           << " = std::make_shared<" << Ty << ">(...);\n"
    //           << "  // and if you need non-owning observers, use std::weak_ptr<"
    //           << Ty << ">.\n\n";
    //     } else {
    //         // Suggest unique_ptr
    //         llvm::outs()
    //           << "\033[1;36m[HINT]\033[0m consider replacing\n    "
    //           << VD->getType().getAsString() << " " << VD->getNameAsString()
    //           << " = new " << Ty << "(...);\n  with:\n    "
    //           << "auto " << VD->getNameAsString()
    //           << " = std::make_unique<" << Ty << ">(...);\n\n";
    //     }
    // }
};

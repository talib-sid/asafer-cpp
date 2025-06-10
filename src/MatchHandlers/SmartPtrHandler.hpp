#pragma once

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/AST/ASTContext.h>
#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace clang::ast_matchers;

/*
 * SmartPtrHandler suggests replacing raw `new` allocations with smart pointers
 * like `std::unique_ptr` or `std::shared_ptr` based on variable scope and usage.
 */
class SmartPtrHandler : public MatchFinder::MatchCallback {
public:
    /*
     * Called when a match is found (a variable initialized using `new`).
     * Suggests replacing raw pointers with smart pointers.
     */

    void run(const MatchFinder::MatchResult &Result) override {
        // Get the matched variable declaration and new expression
        auto *VD = Result.Nodes.getNodeAs<VarDecl>("ptrDecl");
        auto *NE = Result.Nodes.getNodeAs<CXXNewExpr>("newExpr");
        if (!VD || !NE) return; // Skip if either is missing

        // Only run this check in the main file
        const auto &SM = Result.Context->getSourceManager();
        if (!SM.isInMainFile(VD->getBeginLoc())) return;

        // Extract the pointee type (e.g., T from T* or std::shared_ptr<T>)
        QualType QT = VD->getType()->getPointeeType();
        if (QT.isNull()) return;

        std::string Ty = QT.getAsString(); // Type name as string

        // Determine whether the variable is global/static or escapes local scope
        bool globalOrStatic = VD->hasGlobalStorage() || VD->isStaticLocal();
        bool escapesLocal   = !globalOrStatic && pointerEscapes(VD, *Result.Context);

        // Emit appropriate suggestion based on ownership pattern
        if (globalOrStatic || escapesLocal) {
          
            // Suggest using shared_ptr (for escaping or static/global variables)
            llvm::outs()
              << "\033[1;36m[HINT]\033[0m consider replacing\n    "
              << VD->getType().getAsString() << " " << VD->getNameAsString()
              << " = new " << Ty << "(...);\n  with:\n    "
              << (VD->isStaticLocal() ? "static auto " : "auto ")
              << VD->getNameAsString()
              << " = std::make_shared<" << Ty << ">(...);\n"
              << "  // for non-owning, use std::weak_ptr<" << Ty << ">.\n\n";
        } else {
            // Suggest using unique_ptr (for local non-escaping pointers)
            llvm::outs()
              << "\033[1;36m[HINT]\033[0m consider replacing\n    "
              << VD->getType().getAsString() << " " << VD->getNameAsString()
              << " = new " << Ty << "(...);\n  with:\n    "
              << "auto " << VD->getNameAsString()
              << " = std::make_unique<" << Ty << ">(...);\n\n";
        }
    }

private:
    /*
     * Checks whether the pointer escapes the local scope (e.g., by being returned,
     * passed to another function, or assigned to a field).
     */
    bool pointerEscapes(const VarDecl *VD, ASTContext &Ctx) {
        const FunctionDecl *FD = dyn_cast<FunctionDecl>(VD->getDeclContext());
        if (!FD || !FD->hasBody()) return false;

        // Match any use of the variable in a return statement: return ptr;
        auto refToMe = declRefExpr(to(varDecl(hasName(VD->getNameAsString()))));

        auto retMatches = match(
          returnStmt(hasReturnValue(ignoringParenImpCasts(refToMe))),
          *FD->getBody(), Ctx
        );
        if (!retMatches.empty()) return true;

        // Match cases like: foo(ptr); → pointer passed as function argument
        auto callMatches = match(
          callExpr(hasAnyArgument(ignoringParenImpCasts(refToMe))),
          *FD->getBody(), Ctx
        );
        if (!callMatches.empty()) return true;

        // Match assignments like: someField = ptr;
        auto assignMatches = match(
          binaryOperator(
            isAssignmentOperator(),
            hasRHS(ignoringParenImpCasts(refToMe))
          ),
          *FD->getBody(), Ctx
        );
        if (!assignMatches.empty()) return true;

        // Otherwise assume it doesn't escape
        return false;
    }
};
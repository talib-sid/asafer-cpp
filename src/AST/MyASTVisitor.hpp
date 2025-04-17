#pragma once
#include <clang/AST/RecursiveASTVisitor.h>
#include <llvm/Support/raw_ostream.h>
#include "../MemoryTracker/AllocationTable.hpp"

class MyASTVisitor : public clang::RecursiveASTVisitor<MyASTVisitor> {
public:
    explicit MyASTVisitor(clang::ASTContext *Context, AllocationTable &table)
        : Context(Context), table(table) {}

    bool VisitVarDecl(clang::VarDecl *VD) {
        if (VD->hasInit()) {
            if (const auto *NewExpr = llvm::dyn_cast<clang::CXXNewExpr>(VD->getInit())) {
                std::string varName = VD->getNameAsString();
                table.markAllocated(varName, VD->getLocation());
                llvm::outs() << "[NEW] Heap allocated (decl): " << varName << "\n";
            }
        }
        return true;
    }

    bool VisitBinaryOperator(clang::BinaryOperator *BO) {
        if (BO->getOpcode() == clang::BO_Assign) {
            const auto *RHS = BO->getRHS()->IgnoreParenImpCasts();
            const auto *LHS = BO->getLHS()->IgnoreParenImpCasts();

            if (const auto *NewExpr = llvm::dyn_cast<clang::CXXNewExpr>(RHS)) {
                if (const auto *DRE = llvm::dyn_cast<clang::DeclRefExpr>(LHS)) {
                    std::string varName = DRE->getDecl()->getNameAsString();
                    table.markAllocated(varName, BO->getBeginLoc());
                    llvm::outs() << "[NEW] Heap allocated (assign): " << varName << "\n";
                }
            }
        }
        return true;
    }

    bool VisitCXXDeleteExpr(clang::CXXDeleteExpr *DelExpr) {
        const auto *Arg = DelExpr->getArgument();
        if (!Arg) return true;

        Arg = Arg->IgnoreParenImpCasts();
        if (const auto *Ref = llvm::dyn_cast<clang::DeclRefExpr>(Arg)) {
            std::string varName = Ref->getDecl()->getNameAsString();
            llvm::outs() << "[DELETE] " << varName << "\n";

            if (!table.isAllocated(varName)) {
                llvm::outs() << " Warning: '" << varName << "' was not allocated with new!\n";
            } else if (table.isFreed(varName)) {
                llvm::outs() << " Error: Double delete detected for '" << varName << "'\n";
            } else {
                table.markFreed(varName);
                llvm::outs() << " Delete OK: " << varName << "\n";
            }
        }
        return true;
    }

private:
    clang::ASTContext *Context;
    AllocationTable &table;
};

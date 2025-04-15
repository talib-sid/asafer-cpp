#pragma once
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <llvm/Support/raw_ostream.h>
#include "../MemoryTracker/AllocationTable.hpp"

class NewExprHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    explicit NewExprHandler(AllocationTable &table) : table(table) {}

    void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override {
        if (const auto *NewExpr = Result.Nodes.getNodeAs<clang::CXXNewExpr>("newExpr")) {
            const auto *Parent = Result.Context->getParents(*NewExpr).begin();
            if (Parent != Result.Context->getParents(*NewExpr).end()) {
                if (const auto *Assign = Parent->get<clang::BinaryOperator>()) {
                    if (const auto *LHS = Assign->getLHS()->IgnoreParenImpCasts()) {
                        if (const auto *DRE = llvm::dyn_cast<clang::DeclRefExpr>(LHS)) {
                            std::string varName = DRE->getDecl()->getNameAsString();
                            table.markAllocated(varName, NewExpr->getBeginLoc());
                            llvm::outs() << "[NEW] Allocated: " << varName << "\n";
                        }
                    }
                }
            }
        }
    }

private:
    AllocationTable &table;
};

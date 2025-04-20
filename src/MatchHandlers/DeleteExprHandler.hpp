#pragma once
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <llvm/Support/raw_ostream.h>

#include "../MemoryTracker/AllocationTable.hpp"

using namespace clang;
using namespace clang::ast_matchers;

class DeleteExprHandler : public MatchFinder::MatchCallback {
public:
    explicit DeleteExprHandler(AllocationTable &table) : table(table) {}

    void run(const MatchFinder::MatchResult &Result) override {
        if (const auto *DelExpr = Result.Nodes.getNodeAs<CXXDeleteExpr>("deleteExpr")) {
            const auto *Arg = DelExpr->getArgument();
            if (!Arg) return;

            Arg = Arg->IgnoreParenImpCasts();
            // if (const auto *Ref = dyn_cast<DeclRefExpr>(Arg)) {
            //     const std::string varName = Ref->getDecl()->getNameAsString();
            //     llvm::outs() << "[DELETE] Attempt to delete: " << varName << "\n";

                // if (!table.isAllocated(varName)) {
                //     llvm::outs() << " Warning: '" << varName << "' was not allocated with new!\n";
                // } else if (table.isFreed(varName)) {
                //     llvm::outs() << " Error: Double delete detected for '" << varName << "'\n";
                // } else {
                //     table.markFreed(varName);
                //     llvm::outs() << "Delete OK: " << varName << "\n";
                // }
            // }
            if (const auto *Ref = dyn_cast<DeclRefExpr>(Arg)) {
                std::string varName = Ref->getDecl()->getNameAsString();
                table.recordDeleteAttempt(varName, DelExpr->getBeginLoc());
            }
        }
    }

private:
    AllocationTable &table;
};



#pragma once
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <llvm/Support/raw_ostream.h>
#include "../MemoryTracker/AllocationTable.hpp"

using namespace clang;
using namespace clang::ast_matchers;

class NewExprHandler : public MatchFinder::MatchCallback {
public:
    explicit NewExprHandler(AllocationTable &table) : table(table) {}

    void run(const MatchFinder::MatchResult &Result) override {

        // debug
        llvm::outs() << "[Handler] NewExprHandler fired\n";


        const auto *NewExpr = Result.Nodes.getNodeAs<CXXNewExpr>("newExpr");
        const auto *Var = Result.Nodes.getNodeAs<VarDecl>("lhsVar");

        if (NewExpr && Var) {
            std::string varName = Var->getNameAsString();
            table.markAllocated(varName, NewExpr->getBeginLoc());
            llvm::outs() << "[NEW] Allocated: " << varName << "\n";
        }
    }

private:
    AllocationTable &table;
};

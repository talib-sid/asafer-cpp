#pragma once
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <llvm/Support/raw_ostream.h>
#include "../MemoryTracker/AllocationTable.hpp"

using namespace clang;
using namespace clang::ast_matchers;

class NewExprHandler : public MatchFinder::MatchCallback {
public:
    explicit NewExprHandler(AllocationTable &tracker) : table(tracker) {}

    void run(const MatchFinder::MatchResult &Result) override {

        llvm::outs() << "[Handler] NewExprHandler activated\n";


        const auto *NewExpr = Result.Nodes.getNodeAs<CXXNewExpr>("newExpr");
        const auto *Var = Result.Nodes.getNodeAs<VarDecl>("lhsVar");

        if (NewExpr) {
            // llvm::outs() << "[Debug] CXXNewExpr matched\n";
        }
        if (Var) {
            // llvm::outs() << "[Debug] LHS VarDecl matched: " << Var->getNameAsString() << "\n";
        }
        
        if (NewExpr && Var) {
            std::string varName = Var->getNameAsString();
            table.markAllocated(varName, NewExpr->getBeginLoc());
            llvm::outs() << "[NEW] Heap allocated: " << varName << "\n";
        }
    }

private:
    AllocationTable &table;
};



#pragma once
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include "../MemoryTracker/AllocationTable.hpp"

using namespace clang;
using namespace clang::ast_matchers;

class NewExprHandler : public MatchFinder::MatchCallback {
public:
  explicit NewExprHandler(AllocationTable &table) : table(table) {}

  void run(const MatchFinder::MatchResult &Result) override {
    // 1) Grab the new-expression node
    const auto *NewE = Result.Nodes.getNodeAs<CXXNewExpr>("newExpr");
    if (!NewE) return;

    // 2) Filter out system headers
    const auto &SM  = Result.Context->getSourceManager();
    SourceLocation loc = NewE->getBeginLoc();
    if (!SM.isInMainFile(loc)) return;

    // 3) Get the VarDecl we bound as "lhsVar"
    const VarDecl *VD = Result.Nodes.getNodeAs<VarDecl>("lhsVar");
    if (!VD) return;
    std::string varName = VD->getNameAsString();

    // 4) Record allocation
    bool isArray = NewE->isArray();
    table.markAllocated(varName, loc, isArray);

    llvm::outs() << "[NEW] Heap allocated: " << varName << "\n";
  }

private:
  AllocationTable &table;
};

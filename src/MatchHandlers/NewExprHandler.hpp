#pragma once

// AST Matchers and Clang internals
#include <clang/ASTMatchers/ASTMatchFinder.h>
// Your custom memory tracking table for allocation logging
#include "../MemoryTracker/AllocationTable.hpp"

using namespace clang;
using namespace clang::ast_matchers;

/*
 * NewExprHandler handles matched `new` (or `new[]`) expressions in the AST.
 * It records the allocation in the AllocationTable for later analysis.
 */

class NewExprHandler : public MatchFinder::MatchCallback {
public:
  // Constructor takes reference to shared AllocationTable used for tracking
  explicit NewExprHandler(AllocationTable &table) : table(table) {}

  /**
   * Called when a match is found for a `CXXNewExpr` and its associated `VarDecl`.
   */
  void run(const MatchFinder::MatchResult &Result) override {
    // 1) Retrieve the matched C++ new-expression node
    const auto *NewE = Result.Nodes.getNodeAs<CXXNewExpr>("newExpr");
    if (!NewE) return; // If not found, skip

    // 2) Ensure we're only analyzing user code (e.g., in .cpp file)
    const auto &SM  = Result.Context->getSourceManager();
    SourceLocation loc = NewE->getBeginLoc();
    if (!SM.isInMainFile(loc)) return;

    // 3) Retrieve the left-hand side variable to which the new-expression is assigned
    const VarDecl *VD = Result.Nodes.getNodeAs<VarDecl>("lhsVar");
    if (!VD) return;

    std::string varName = VD->getNameAsString(); // Extract variable name

    // 4) Record this heap allocation in the allocation table
    bool isArray = NewE->isArray(); // Detect if it's a `new[]` expression
    table.markAllocated(varName, loc, isArray); // Log the allocation for tracking

    // 5) Output allocation info (optional; useful during development/debugging)
    llvm::outs() << "[NEW] Heap allocated: " << varName << "\n";
  }

private:
  AllocationTable &table; // Reference to central allocation tracker
};

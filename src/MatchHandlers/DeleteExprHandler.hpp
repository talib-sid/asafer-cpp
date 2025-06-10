#pragma once

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include "../MemoryTracker/AllocationTable.hpp"

using namespace clang;
using namespace clang::ast_matchers;

/*
 * DeleteExprHandler is a Clang AST MatchCallback that handles `delete` expressions
 * in C++ code. It records information about delete attempts using an AllocationTable.
 */

class DeleteExprHandler : public MatchFinder::MatchCallback {
public:
  // Constructor takes a reference to the AllocationTable for recording delete attempts
  explicit DeleteExprHandler(AllocationTable &table) : table(table) {}

  /**
   * This method is called whenever a match is found (a `CXXDeleteExpr` in this case).
   */
  void run(const MatchFinder::MatchResult &Result) override {
    // 1) Grab the matched CXXDeleteExpr (represents a `delete` or `delete[]` operation)
    const auto *DelExpr = Result.Nodes.getNodeAs<CXXDeleteExpr>("deleteExpr");
    if (!DelExpr) return; // If no delete expression matched, skip

    // 2) Filter out code not in the main file (optional: you can use custom logic)
    const auto &SM  = Result.Context->getSourceManager();
    SourceLocation loc = DelExpr->getBeginLoc();
    if (!SM.isInMainFile(loc))
      return;

    // Optional custom logic (e.g., allow/delete based on file paths):
    // if (!pathAllows(SM, loc))
    //   return;

    // 3) Process the delete expression
    const Expr *Arg = DelExpr->getArgument(); // Get the expression being deleted
    if (!Arg) return;

    // Ignore any parentheses, implicit casts, etc., to get the underlying variable
    Arg = Arg->IgnoreParenImpCasts();

    // Check if the deleted expression is a reference to a named variable
    if (auto *Ref = dyn_cast<DeclRefExpr>(Arg)) {
      // Get the name of the variable being deleted
      std::string varName = Ref->getDecl()->getNameAsString();

      // Check whether this is an array delete (`delete[]`)
      bool isArrayDelete = DelExpr->isArrayForm();

      // Record this delete attempt in the allocation table
      table.recordDeleteAttempt(varName, loc, isArrayDelete);

      // We do not report anything here directly; reporting is deferred
      // (e.g., to a `finalizeDeletes()` stage later)
    }
  }

private:
  AllocationTable &table; // Reference to the memory tracker used across matchers
};

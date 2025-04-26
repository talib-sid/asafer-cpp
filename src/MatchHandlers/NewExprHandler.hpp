// #pragma once
// #include <clang/ASTMatchers/ASTMatchFinder.h>
// #include <llvm/Support/raw_ostream.h>
// #include "../MemoryTracker/AllocationTable.hpp"

// using namespace clang;
// using namespace clang::ast_matchers;

// class NewExprHandler : public MatchFinder::MatchCallback {
// public:
//     explicit NewExprHandler(AllocationTable &tracker) : table(tracker) {}

//     void run(const MatchFinder::MatchResult &Result) override {
//         llvm::outs() << "[Handler] NewExprHandler activated\n";

//         const auto *NewExpr = Result.Nodes.getNodeAs<CXXNewExpr>("newExpr");
//         const auto *Var = Result.Nodes.getNodeAs<VarDecl>("lhsVar");

//         // 
//         bool isArray = NewExpr->isArray();


//         if (NewExpr) {
//             // llvm::outs() << "[Debug] CXXNewExpr matched\n";
//         }
//         if (Var) {
//             // llvm::outs() << "[Debug] LHS VarDecl matched: " << Var->getNameAsString() << "\n";
//         }
        
//         if (NewExpr && Var) {
//             std::string varName = Var->getNameAsString();
//             bool isArray = NewExpr->isArray();
//             table.markAllocated(varName, NewExpr->getBeginLoc(), isArray);

//             llvm::outs() << "[NEW] Heap allocated: " << varName << "\n";
//         }
//     }


    

// private:
//     AllocationTable &table;
// };



#pragma once
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include "../MemoryTracker/AllocationTable.hpp"

using namespace clang;
using namespace clang::ast_matchers;

class NewExprHandler : public MatchFinder::MatchCallback {
public:
  explicit NewExprHandler(AllocationTable &table) : table(table) {}

  void run(const MatchFinder::MatchResult &Result) override {
    // 1) grab the matched new‑expression
    const auto *NewExpr = Result.Nodes.getNodeAs<CXXNewExpr>("newExpr");
    if (!NewExpr) return;

    // 2) filter by main file (or your include/exclude logic)
    const auto &SM  = Result.Context->getSourceManager();
    SourceLocation loc = NewExpr->getBeginLoc();
    if (!SM.isInMainFile(loc))
      return;
    // // OR, for the include/exclude system:
    // if (!pathAllows(SM, loc))
    //   return;

    // 3) OK to process this allocation
    //    find the LHS variable name
    const Decl *D = Result.Nodes.getNodeAs<Decl>("lhsVar");
    std::string varName;
    if (auto *VD = dyn_cast_or_null<VarDecl>(D))
      varName = VD->getNameAsString();
    else if (auto *DRE = dyn_cast_or_null<DeclRefExpr>(D))
      varName = DRE->getDecl()->getNameAsString();
    else
      return;

    bool isArray = NewExpr->isArray();
    table.markAllocated(varName, loc, isArray);

    llvm::outs() << "[NEW] Heap allocated: " << varName << "\n";
  }

private:
  AllocationTable &table;
};

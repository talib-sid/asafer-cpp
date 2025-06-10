#pragma once

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/AST/ASTContext.h>
#include <llvm/Support/raw_ostream.h>
#include <map>
#include <set>

using namespace clang;
using namespace clang::ast_matchers;

// -----------------------------------------------------------------------------
// Helper Function
// Walks up the AST parent chain to find the enclosing FunctionDecl for a given Stmt
// Returns nullptr if no enclosing function is found or the location is not in main file
// -----------------------------------------------------------------------------
static const FunctionDecl* findEnclosingFunction(const Stmt *S, ASTContext &Ctx) {
  auto Parents = Ctx.getParents(*S);
  while (!Parents.empty()) {
    // If we found a FunctionDecl parent, return it
    if (auto *FD = Parents[0].get<FunctionDecl>())
      return FD->getCanonicalDecl();
    // Otherwise, climb up if parent is another Stmt
    if (auto *St = Parents[0].get<Stmt>())
      Parents = Ctx.getParents(*St);
    else
      break;
  }
  return nullptr;
}

// -----------------------------------------------------------------------------
// NullDerefHandler
// - Records local pointer VarDecls initialized to nullptr
// - Matches dereference operations (*p) and warns if p was recorded null
// -----------------------------------------------------------------------------
class NullDerefHandler : public MatchFinder::MatchCallback {
  // Map from FunctionDecl to set of VarDecls known to be null
  std::map<const FunctionDecl*, std::set<const VarDecl*>> nullVars;

public:
  // Called for each AST matcher match
  void run(const MatchFinder::MatchResult &Result) override {
    ASTContext &Ctx = *Result.Context;
    SourceManager &SM = Ctx.getSourceManager();

    // 1) If we matched a VarDecl with nullptr initializer, record it
    if (auto *VD = Result.Nodes.getNodeAs<VarDecl>("nullVar")) {
      // Get the initializer expression
      const Expr *Init = VD->getInit();
      // Determine which function this var belongs to
      const FunctionDecl *Caller = findEnclosingFunction(Init, Ctx);
      if (!Caller || !SM.isInMainFile(Caller->getLocation()))
        return; // skip system headers or outside main file
      // Record the variable as null in this function
      nullVars[Caller].insert(VD->getCanonicalDecl());
      return;
    }

    // 2) If we matched a dereference operation (*p), check if p is recorded null
    if (auto *UO = Result.Nodes.getNodeAs<UnaryOperator>("derefOp")) {
      // Get the operand expression, ignore parentheses
      const Expr *Sub = UO->getSubExpr()->IgnoreParens();
      if (auto *DR = dyn_cast<DeclRefExpr>(Sub)) {
        if (auto *VD = dyn_cast<VarDecl>(DR->getDecl())) {
          // Find enclosing function for this deref
          const FunctionDecl *Caller = findEnclosingFunction(UO, Ctx);
          if (!Caller)
            return;
          // Check if VD is in our null-vars set
          auto &setNV = nullVars[Caller];
          if (setNV.count(VD->getCanonicalDecl())) {
            // Emit warning for null pointer dereference
            llvm::errs()
              << "⚠️ [NullDeref] null deref of '"
              << VD->getNameAsString() << "' at ";
            UO->getExprLoc().print(llvm::errs(), SM);
            llvm::errs() << "\n";
          }
        }
      }
    }
  }
};

// -----------------------------------------------------------------------------
// UninitVarHandler
// - Records local VarDecls without any initializer
// - Matches uses (DeclRefExpr) of these uninitialized vars and warns
// -----------------------------------------------------------------------------
class UninitVarHandler : public MatchFinder::MatchCallback {
  // Map from FunctionDecl to set of VarDecls known to be uninitialized
  std::map<const FunctionDecl*, std::set<const VarDecl*>> uninitVars;

public:
  void run(const MatchFinder::MatchResult &Result) override {
    ASTContext &Ctx = *Result.Context;
    SourceManager &SM = Ctx.getSourceManager();

    // 1) If we matched a VarDecl without initializer, record it
    if (auto *VD = Result.Nodes.getNodeAs<VarDecl>("uninitVar")) {
      // Walk up DeclContext chain to find enclosing FunctionDecl
      const DeclContext *DCtx = VD->getDeclContext();
      const FunctionDecl *Caller = nullptr;
      while (DCtx) {
        if (auto *FD = dyn_cast<FunctionDecl>(DCtx)) {
          Caller = FD->getCanonicalDecl();
          break;
        }
        DCtx = DCtx->getParent();
      }
      if (!Caller || !SM.isInMainFile(Caller->getLocation()))
        return; // skip if outside main file
      // Record the variable as uninitialized
      uninitVars[Caller].insert(VD->getCanonicalDecl());
      return;
    }

    // 2) If we matched a use of a VarDecl, check if it was uninitialized
    if (auto *DR = Result.Nodes.getNodeAs<DeclRefExpr>("useVar")) {
      if (auto *VD = dyn_cast<VarDecl>(DR->getDecl())) {
        // Find enclosing function
        const DeclContext *DCtx = VD->getDeclContext();
        const FunctionDecl *Caller = nullptr;
        while (DCtx) {
          if (auto *FD = dyn_cast<FunctionDecl>(DCtx)) {
            Caller = FD->getCanonicalDecl();
            break;
          }
          DCtx = DCtx->getParent();
        }
        if (!Caller)
          return;
        // If this var is in our uninitialized set, emit warning
        auto &setUV = uninitVars[Caller];
        if (setUV.count(VD->getCanonicalDecl())) {
          llvm::errs()
            << "⚠️ [UninitUse] use of uninitialized var '" 
            << VD->getNameAsString() << "' at ";
          DR->getExprLoc().print(llvm::errs(), SM);
          llvm::errs() << "\n";
        }
      }
    }
  }
};

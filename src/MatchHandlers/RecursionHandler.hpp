#pragma once
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/AST/ASTContext.h>
#include <llvm/Support/raw_ostream.h>
#include <map>
#include <set>

using namespace clang;
using namespace clang::ast_matchers;

class RecursionHandler : public MatchFinder::MatchCallback {
public:
  RecursionHandler() = default;

  void run(const MatchFinder::MatchResult &Result) override {
    auto *CE = Result.Nodes.getNodeAs<CallExpr>("anyCall");
    if (!CE) return;

    // 1) Identify callee
    const FunctionDecl *Callee = CE->getDirectCallee();
    if (!Callee) return;
    Callee = Callee->getCanonicalDecl();

    // 2) Identify caller by walking up to its enclosing FunctionDecl
    const FunctionDecl *Caller = findEnclosingFunction(CE, *Result.Context);
    if (!Caller) return;
    Caller = Caller->getCanonicalDecl();

    // 3) Only your code
    auto &SM = Result.Context->getSourceManager();
    if (!SM.isInMainFile(Caller->getLocation())) return;

    // 4) Record this edge Caller -> Callee
    calls[Caller].insert(Callee);

    // 5a) Direct recursion?
    if (Caller == Callee) {
      warnOnce(Caller);
      return;
    }

    // 5b) Mutual recursion?  Caller→Callee and Callee→Caller
    auto it = calls.find(Callee);
    if (it != calls.end() && it->second.count(Caller)) {
      warnOnce(Caller);
      warnOnce(Callee);
    }
  }

private:
  std::map<const FunctionDecl*, std::set<const FunctionDecl*>> calls;
  std::set<const FunctionDecl*> warned;

  void warnOnce(const FunctionDecl *FD) {
    if (!warned.insert(FD).second) return;
    llvm::outs()
      << "\033[1;33m[WARN]\033[0m potential deep recursion in function '"
      << FD->getNameAsString() << "'\n\n";
  }

  // Climb parents to find the nearest FunctionDecl
  const FunctionDecl* findEnclosingFunction(const Stmt *S, ASTContext &Ctx) {
    auto Parents = Ctx.getParents(*S);
    while (!Parents.empty()) {
      if (auto *FD = Parents[0].get<FunctionDecl>())
        return FD;
      if (auto *St = Parents[0].get<Stmt>())
        Parents = Ctx.getParents(*St);
      else
        break;
    }
    return nullptr;
  }
};

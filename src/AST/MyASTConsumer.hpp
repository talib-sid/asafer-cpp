#pragma once
#include <clang/AST/ASTConsumer.h>
#include "MyASTVisitor.hpp"

class MyASTConsumer : public clang::ASTConsumer {
public:
    MyASTConsumer(clang::ASTContext *Context, AllocationTable &table)
        : Visitor(Context, table) {}

    void HandleTranslationUnit(clang::ASTContext &Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    MyASTVisitor Visitor;
};


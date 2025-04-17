#pragma once
// #include <clang/Frontend/FrontendAction.h>

#include <clang/Frontend/CompilerInstance.h>

#include "MyASTConsumer.hpp"
#include "../MemoryTracker/AllocationTable.hpp"


class MyFrontendAction : public clang::ASTFrontendAction {
public:
    virtual void anchor();  // declaration
    MyFrontendAction(AllocationTable &table) : table(table) {}

    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance &CI, llvm::StringRef) override {

        return std::make_unique<MyASTConsumer>(&CI.getASTContext(), table);
    }

private:
    AllocationTable &table;
};

class MyFrontendActionFactory : public clang::tooling::FrontendActionFactory {
    public:
        MyFrontendActionFactory(AllocationTable &table) : table(table) {}
    
        std::unique_ptr<clang::FrontendAction> create() override {
            return std::make_unique<MyFrontendAction>(table);
        }
    
    private:
        AllocationTable &table;
    };
    
    
inline void MyFrontendAction::anchor() {}

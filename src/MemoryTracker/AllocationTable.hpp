#pragma once
#include <map>
#include <vector>
#include <string>

#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <llvm/Support/raw_ostream.h>

class AllocationTable {
public:
    void markAllocated(const std::string &var, clang::SourceLocation loc) {
        allocations[var] = {true, loc};
    }

    bool wasEverAllocated(const std::string &var) const {
        return allocations.find(var) != allocations.end();
    }
    
    bool isCurrentlyAllocated(const std::string &var) const {
        auto it = allocations.find(var);
        return it != allocations.end() && it->second.allocated;
    }
    

    void markFreed(const std::string &var) {
        if (allocations.count(var))
            allocations[var].allocated = false;
    }

    
    void recordDeleteAttempt(const std::string &var, clang::SourceLocation loc) {
        deletes.push_back({var, loc});
    }

    void finalizeDeletes() {
        llvm::outs() << "\n\033[1m[Summary] Delete Analysis Report\033[0m\n\n";
    
        for (const auto &d : deletes) {
            llvm::outs() << "\033[1;36m[DELETE]\033[0m Attempt to delete: \033[1m" << d.varName << "\033[0m\n";
    
            if (!wasEverAllocated(d.varName)) {
                llvm::outs() << "  \033[1;33m⚠️  Warning:\033[0m '" << d.varName << "' was not allocated with new!\n\n";
            } else if (!isCurrentlyAllocated(d.varName)) {
                llvm::outs() << "  \033[1;31m❌ Error:\033[0m Double delete detected for '" << d.varName << "'\n\n";
            } else {
                markFreed(d.varName);
                llvm::outs() << "  \033[1;32m✅ Delete OK:\033[0m " << d.varName << "\n\n";
            }
        }
    }
    

private:
    struct AllocationInfo {
        bool allocated;
        clang::SourceLocation location;
    };

    struct DeleteInfo {
        std::string varName;
        clang::SourceLocation location;
    };

    std::map<std::string, AllocationInfo> allocations;
    std::vector<DeleteInfo> deletes;
};

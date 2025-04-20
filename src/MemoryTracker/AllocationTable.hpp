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
        for (const auto &d : deletes) {
            llvm::outs() << "[DELETE] Attempt to delete: " << d.varName << "\n";

            if (!wasEverAllocated(d.varName)) {
                llvm::outs() << " Warning: '" << d.varName << "' was not allocated with new!\n";
            } else if (!isCurrentlyAllocated(d.varName)) {
                llvm::outs() << "Error: Double delete detected for '" << d.varName << "'\n";
            } else {
                markFreed(d.varName);
                llvm::outs() << "Delete OK: " << d.varName << "\n";
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

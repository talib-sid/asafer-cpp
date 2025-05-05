#pragma once
#include <map>
#include <vector>
#include <string>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <llvm/Support/raw_ostream.h>

class AllocationTable {
public:
    void markAllocated(const std::string &var, clang::SourceLocation loc, bool isArray) {
        AllocationInfo info;
        info.allocated = true;
        info.isArray = isArray;
        info.location = loc;
        allocations[var] = info;
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

    void recordDeleteAttempt(const std::string &var, clang::SourceLocation loc, bool isArrayDelete) {
        deletes.push_back({var, loc, isArrayDelete});
    }

    void finalizeDeletes() {
        llvm::outs() << "\n\033[1m[Delete] Analysis Report\033[0m\n\n";

        for (const auto &d : deletes) {
            llvm::outs() << "\033[1;36m[DELETE]\033[0m Attempt to delete: \033[1m" << d.varName << "\033[0m\n";

            if (!wasEverAllocated(d.varName)) {
                llvm::outs() << "  \033[1;33m⚠️  Warning:\033[0m '" << d.varName << "' was not allocated with new!\n\n";
                continue;
            }

            const auto &alloc = allocations[d.varName];
            checkDeleteMismatch(d, alloc);

            if (!isCurrentlyAllocated(d.varName)) {
                llvm::outs() << "  \033[1;31m❌ Error:\033[0m Double delete detected for '" << d.varName << "'\n\n";
            } else {
                markFreed(d.varName);
                llvm::outs() << "  \033[1;32m✅ Delete OK:\033[0m " << d.varName << "\n\n";
            }
        }
    }
    void reportLeaks() const {
        llvm::outs() << "\033[1m[Leaks] Unfreed Allocations\033[0m\n\n";
        for (const auto &pair : allocations) {
            const auto &var  = pair.first;
            const auto &info = pair.second;
            if (info.allocated) {
                llvm::outs()
                  << "  \033[1;33m⚠️  Leak:\033[0m '"
                  << var
                  << "' allocated here but never deleted\n";
            }
        }
        llvm::outs() << "\n";
    }



private:
    struct AllocationInfo {
        bool allocated;
        bool isArray;
        clang::SourceLocation location;
    };

    struct DeleteInfo {
        std::string varName;
        clang::SourceLocation location;
        bool isArrayDelete;
    };

    std::map<std::string, AllocationInfo> allocations;
    std::vector<DeleteInfo> deletes;

    void checkDeleteMismatch(const DeleteInfo &d, const AllocationInfo &alloc) const {
        if (alloc.isArray && !d.isArrayDelete) {
            llvm::outs() << "  \033[1;33m⚠️  Warning:\033[0m '" << d.varName << "' was allocated with new[] but deleted with delete\n";
        } else if (!alloc.isArray && d.isArrayDelete) {
            llvm::outs() << "  \033[1;33m⚠️  Warning:\033[0m '" << d.varName << "' was allocated with new but deleted with delete[]\n";
        }
    }
};

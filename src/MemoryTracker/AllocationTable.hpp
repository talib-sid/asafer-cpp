#pragma once

#include <map>
#include <vector>
#include <string>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <llvm/Support/raw_ostream.h>

/*
 * AllocationTable keeps track of dynamic memory allocations (`new` / `new[]`)
 * and their corresponding deallocations (`delete` / `delete[]`) for variables
 * detected in the user's source code. It supports:
 *   - Logging allocation and delete attempts
 *   - Checking for double deletes
 *   - Detecting mismatches (new[] vs delete)
 *   - Reporting memory leaks
 */


class AllocationTable {
public:
    /*
     * Mark a variable as allocated on the heap.
     * @param var       The variable name
     * @param loc       The source location where the allocation occurred
     * @param isArray   True if it was allocated with new[], false for new
     */
    void markAllocated(const std::string &var, clang::SourceLocation loc, bool isArray) {
        AllocationInfo info;
        info.allocated = true;
        info.isArray = isArray;
        info.location = loc;
        allocations[var] = info;
    }
    
    /*
     * Check if a variable was ever allocated (used `new` or `new[]`)
     */
    bool wasEverAllocated(const std::string &var) const {
        return allocations.find(var) != allocations.end();
    }

    /*
     * Check if a variable is currently considered allocated
     * (i.e., not yet deleted)
     */
    bool isCurrentlyAllocated(const std::string &var) const {
        auto it = allocations.find(var);
        return it != allocations.end() && it->second.allocated;
    }

    /*
     * Mark a variable as freed (after delete or delete[])
     */
    void markFreed(const std::string &var) {
        if (allocations.count(var))
            allocations[var].allocated = false;
    }

    /**
     * Log a delete attempt for a variable
     * @param var             Variable name
     * @param loc             Location where delete occurred
     * @param isArrayDelete   True for delete[], false for delete
     */
    void recordDeleteAttempt(const std::string &var, clang::SourceLocation loc, bool isArrayDelete) {
        deletes.push_back({var, loc, isArrayDelete});
    }

    /*
     * Analyze and finalize all recorded delete attempts.
     * Reports:
     *  - Deletes of variables never allocated
     *  - Mismatched delete type (new vs new[])
     *  - Double deletes
     *  - Valid deletes
     */
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

    /*
     * Report all variables that were allocated but never freed
     */
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
    // Struct to store allocation metadata
    struct AllocationInfo {
        bool allocated;                      // Whether the variable is currently allocated
        bool isArray;                        // True for new[], false for new
        clang::SourceLocation location;      // Where the allocation occurred
    };

    // Struct to store delete attempt metadata
    struct DeleteInfo {
        std::string varName;
        clang::SourceLocation location;
        bool isArrayDelete;                  // True for delete[], false for delete
    };

    std::map<std::string, AllocationInfo> allocations;  // Track allocations by variable name
    std::vector<DeleteInfo> deletes;                    // List of all delete attempts

    /*
     * Checks if a delete/delete[] mismatch exists and prints a warning if needed.
     */
    void checkDeleteMismatch(const DeleteInfo &d, const AllocationInfo &alloc) const {
        if (alloc.isArray && !d.isArrayDelete) {
            llvm::outs() << "  \033[1;33m⚠️  Warning:\033[0m '" << d.varName << "' was allocated with new[] but deleted with delete\n";
        } else if (!alloc.isArray && d.isArrayDelete) {
            llvm::outs() << "  \033[1;33m⚠️  Warning:\033[0m '" << d.varName << "' was allocated with new but deleted with delete[]\n";
        }
    }
};

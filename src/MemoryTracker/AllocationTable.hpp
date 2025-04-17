#pragma once
#include <map>
#include <string>
#include <clang/Basic/SourceLocation.h>

class AllocationTable {
public:
    void markAllocated(const std::string &var, clang::SourceLocation loc) {
        allocations[var] = {true, loc};
    }

    bool isAllocated(const std::string &var) const {
        auto it = allocations.find(var);
        return it != allocations.end() && it->second.allocated;
    }

    void markFreed(const std::string &var) {
        if (allocations.count(var))
            allocations[var].allocated = false;
    }
    
    bool isFreed(const std::string &var) const {
        auto it = allocations.find(var);
        return it != allocations.end() && !it->second.allocated;
    }
    

private:
    struct AllocationInfo {
        bool allocated;
        clang::SourceLocation location;
    };

    std::map<std::string, AllocationInfo> allocations;
};

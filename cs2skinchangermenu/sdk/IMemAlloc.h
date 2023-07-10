#pragma once

#pragma warning(push)
#pragma warning(disable:4191)

#include <cstdint>

// too lazy
namespace Virtual {
    template <typename T>
    __forceinline T GetEnt(void* instance, const unsigned int index) {
        return (*static_cast<T**>(instance))[index];
    }

    template <typename T>
    inline T Read(const std::uintptr_t location) {
        return *reinterpret_cast<T*>(location);
    }

    template <typename T>
    inline void Write(const std::uintptr_t location, const T& data) {
        *reinterpret_cast<T*>(location) = data;
    }
}

class IMemAlloc {
public:
    void* Alloc(size_t size) {
        return Virtual::GetEnt<void* (__thiscall*)(IMemAlloc*, size_t)>(this, 1)(this, size);
    }

    void* ReAlloc(const void* p, size_t size) {
        return Virtual::GetEnt<void* (__thiscall*)(IMemAlloc*, const void*, size_t)>(this, 3)(this, p, size);
    }

    void Free(const void* p) {
        return Virtual::GetEnt<void(__thiscall*)(IMemAlloc*, const void*)>(this, 5)(this, p);
    }

    size_t GetSize(const void* p) {
        return Virtual::GetEnt<size_t(__thiscall*)(IMemAlloc*, const void*)>(this, 21)(this, p);
    }
};

extern IMemAlloc* GetMemAlloc();

_VCRT_ALLOCATOR void* __CRTDECL operator new(size_t s);
_VCRT_ALLOCATOR void* __CRTDECL operator new[](size_t s);
void __CRTDECL operator delete(void* p);
void __CRTDECL operator delete(void* p, size_t s);
void __CRTDECL operator delete[](void* p);

#pragma warning(pop)
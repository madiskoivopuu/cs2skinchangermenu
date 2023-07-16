#pragma once
#include <cstring>
#include <cstdint>

namespace fnv
{
    constexpr uint64_t base = 0xcbf29ce484222325;
    constexpr uint64_t prime = 0x00000100000001B3;

    // Compiletime hash
    constexpr uint64_t HashConst(const char* data, const uint64_t value = base) noexcept
    {
        return (data[0] == '\0') ? value : HashConst(&data[1], (value ^ uint64_t(data[0])) * prime);
    }

    // Runtime hash
    inline uint64_t Hash(const char* data) noexcept
    {
        uint64_t hashed = base;

        for (size_t i = 0U; i < strlen(data); ++i)
        {
            hashed ^= data[i];
            hashed *= prime;
        }

        return hashed;
    }
}
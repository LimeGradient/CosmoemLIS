#pragma once

#include <cstdint>
#include <string>

#pragma pack(push, 1)
struct Il2CppObjectHeader {
    void* klass;   // 0x00
    void* monitor; // 0x08
};

struct Il2CppString {
    Il2CppObjectHeader header; // 0x00
    int32_t            length; // 0x10
    wchar_t            chars[];// 0x14
};

struct Il2CppList {
    Il2CppObjectHeader header;  // 0x00
    void*              items;   // 0x10
    int32_t            size;    // 0x18
    int32_t            version; // 0x1C
};

template <typename T>
struct Il2CppArray {
    Il2CppObjectHeader header;
    void* bounds;
    uintptr_t max_length;
    T vector[0];
};

inline std::string ReadIl2CppString(Il2CppString* str) {
    if (!str) return "";

    int32_t length = str->length;
    if (length <= 0 || length > 1024) return "";

    std::wstring wide(str->chars, length);

    return std::string(wide.begin(), wide.end());
}

template<typename T>
T GetListItem(Il2CppList* list, int index) {
    if (!list || !list->items)             return nullptr;
    if (index < 0 || index >= list->size)  return nullptr;

    uintptr_t arrayBase = (uintptr_t)list->items + 0x20;
    return *(T*)(arrayBase + index * sizeof(T));
}
#pragma pack(pop)
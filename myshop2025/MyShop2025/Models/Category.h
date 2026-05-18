#pragma once
#include <string>

namespace MyShop2025::Models
{
    struct Category
    {
        int64_t categoryId = 0;
        std::wstring name;
        std::wstring description;
    };
}

#pragma once
#include <string>
#include <vector>

namespace MyShop2025::Models
{
    struct Product
    {
        int64_t productId = 0;
        std::wstring sku;
        std::wstring name;
        int64_t importPrice = 0;
        int64_t salePrice = 0;
        int count = 0;
        std::wstring description;
        int64_t categoryId = 0;
        std::wstring categoryName;
        std::vector<std::wstring> images;
    };
}

#pragma once
#include <string>

namespace MyShop2025::Models
{
    struct OrderItem
    {
        int64_t orderItemId = 0;
        int quantity = 0;
        double unitSalePrice = 0.0;
        int64_t totalPrice = 0;
        int64_t orderId = 0;
        int64_t productId = 0;
        std::wstring productName;
    };
}

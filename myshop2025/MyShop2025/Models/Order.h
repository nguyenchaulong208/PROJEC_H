#pragma once
#include <string>
#include <chrono>
#include <vector>
#include "OrderItem.h"

namespace MyShop2025::Models
{
    enum class OrderStatus
    {
        New,
        Paid,
        Cancelled
    };

    struct Order
    {
        int64_t orderId = 0;
        std::chrono::system_clock::time_point createdTime;
        int64_t finalPrice = 0;
        OrderStatus status = OrderStatus::New;
        std::vector<OrderItem> items;
    };
}

#pragma once
#include <string>
#include <vector>
#include "Order.h"

namespace MyShop2025::Models
{
    struct DashboardData
    {
        int64_t totalProducts = 0;
        int64_t totalOrdersToday = 0;
        int64_t totalRevenueToday = 0;
        int64_t totalProfitToday = 0;
        std::vector<Product> lowStockProducts;
        std::vector<std::pair<Product, int64_t>> topSellingProducts;
        std::vector<Order> recentOrders;
        std::vector<std::pair<int, int64_t>> dailyRevenue;
    };
}

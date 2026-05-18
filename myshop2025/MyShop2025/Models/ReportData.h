#pragma once
#include <string>
#include <vector>

namespace MyShop2025::Models
{
    struct ReportData
    {
        std::vector<std::pair<std::wstring, int64_t>> productSales;
        std::vector<std::pair<std::wstring, int64_t>> revenue;
        std::vector<std::pair<std::wstring, int64_t>> profit;
    };
}

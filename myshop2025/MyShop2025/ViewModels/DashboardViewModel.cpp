#include "pch.h"
#include "DashboardViewModel.h"
#include "Services/DatabaseService.h"

namespace winrt::MyShop2025::ViewModels
{
    void DashboardViewModel::LoadData()
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (!dbSvc) return;

        auto data = dbSvc->GetDashboardData();

        TotalProducts(data.totalProducts);
        TotalOrdersToday(data.totalOrdersToday);
        TotalRevenueToday(data.totalRevenueToday);
        TotalProfitToday(data.totalProfitToday);

        std::vector<ProductInfo> lowStock;
        for (const auto& p : data.lowStockProducts)
        {
            ProductInfo info;
            info.Name = p.name;
            info.Sku = p.sku;
            info.Count = p.count;
            info.SalePrice = p.salePrice;
            lowStock.push_back(info);
        }
        LowStockProducts(lowStock);

        std::vector<ProductInfo> topSelling;
        for (const auto& [p, sold] : data.topSellingProducts)
        {
            ProductInfo info;
            info.Name = p.name;
            info.Sku = p.sku;
            info.Count = p.count;
            info.SalePrice = p.salePrice;
            info.TotalSold = sold;
            topSelling.push_back(info);
        }
        TopSellingProducts(topSelling);

        std::vector<OrderInfo> orders;
        for (const auto& o : data.recentOrders)
        {
            OrderInfo info;
            info.OrderId = o.orderId;
            auto tt = std::chrono::system_clock::to_time_t(o.createdTime);
            std::tm tm;
            localtime_s(&tm, &tt);
            std::wstringstream wss;
            wss << std::put_time(&tm, L"%H:%M %d/%m/%Y");
            info.CreatedTime = wss.str();
            switch (o.status)
            {
            case Models::OrderStatus::New: info.Status = L"Mới tạo"; break;
            case Models::OrderStatus::Paid: info.Status = L"Đã thanh toán"; break;
            case Models::OrderStatus::Cancelled: info.Status = L"Đã hủy"; break;
            }
            info.FinalPrice = o.finalPrice;
            orders.push_back(info);
        }
        RecentOrders(orders);

        std::vector<RevenuePoint> revenue;
        for (const auto& [day, rev] : data.dailyRevenue)
        {
            RevenuePoint rp;
            rp.Label = std::to_wstring(day);
            rp.Value = rev;
            revenue.push_back(rp);
        }
        DailyRevenue(revenue);
    }
}

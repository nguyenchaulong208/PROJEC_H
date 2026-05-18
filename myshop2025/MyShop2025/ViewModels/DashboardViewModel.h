#pragma once
#include "ViewModelBase.h"
#include "Models/DashboardData.h"
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Data.h>

namespace winrt::MyShop2025::ViewModels
{
    struct ProductInfo
    {
        std::wstring Name;
        std::wstring Sku;
        int Count;
        int64_t SalePrice;
        int64_t TotalSold;
    };

    struct OrderInfo
    {
        int64_t OrderId;
        std::wstring CreatedTime;
        std::wstring Status;
        int64_t FinalPrice;
    };

    struct RevenuePoint
    {
        std::wstring Label;
        int64_t Value;
    };

    class DashboardViewModel : public ViewModelBase
    {
    private:
        int64_t m_totalProducts = 0;
        int64_t m_totalOrdersToday = 0;
        int64_t m_totalRevenueToday = 0;
        int64_t m_totalProfitToday = 0;
        std::vector<ProductInfo> m_lowStockProducts;
        std::vector<ProductInfo> m_topSellingProducts;
        std::vector<OrderInfo> m_recentOrders;
        std::vector<RevenuePoint> m_dailyRevenue;

    public:
        int64_t TotalProducts() const { return m_totalProducts; }
        void TotalProducts(int64_t value)
        {
            if (m_totalProducts != value)
            {
                m_totalProducts = value;
                RaisePropertyChanged(L"TotalProducts");
            }
        }

        int64_t TotalOrdersToday() const { return m_totalOrdersToday; }
        void TotalOrdersToday(int64_t value)
        {
            if (m_totalOrdersToday != value)
            {
                m_totalOrdersToday = value;
                RaisePropertyChanged(L"TotalOrdersToday");
            }
        }

        int64_t TotalRevenueToday() const { return m_totalRevenueToday; }
        void TotalRevenueToday(int64_t value)
        {
            if (m_totalRevenueToday != value)
            {
                m_totalRevenueToday = value;
                RaisePropertyChanged(L"TotalRevenueToday");
            }
        }

        int64_t TotalProfitToday() const { return m_totalProfitToday; }
        void TotalProfitToday(int64_t value)
        {
            if (m_totalProfitToday != value)
            {
                m_totalProfitToday = value;
                RaisePropertyChanged(L"TotalProfitToday");
            }
        }

        std::vector<ProductInfo> LowStockProducts() const { return m_lowStockProducts; }
        void LowStockProducts(const std::vector<ProductInfo>& value)
        {
            m_lowStockProducts = value;
            RaisePropertyChanged(L"LowStockProducts");
        }

        std::vector<ProductInfo> TopSellingProducts() const { return m_topSellingProducts; }
        void TopSellingProducts(const std::vector<ProductInfo>& value)
        {
            m_topSellingProducts = value;
            RaisePropertyChanged(L"TopSellingProducts");
        }

        std::vector<OrderInfo> RecentOrders() const { return m_recentOrders; }
        void RecentOrders(const std::vector<OrderInfo>& value)
        {
            m_recentOrders = value;
            RaisePropertyChanged(L"RecentOrders");
        }

        std::vector<RevenuePoint> DailyRevenue() const { return m_dailyRevenue; }
        void DailyRevenue(const std::vector<RevenuePoint>& value)
        {
            m_dailyRevenue = value;
            RaisePropertyChanged(L"DailyRevenue");
        }

        void LoadData();
    };
}

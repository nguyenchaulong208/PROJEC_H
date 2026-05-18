#pragma once
#include <memory>
#include "Database/Database.h"
#include "Models/Category.h"
#include "Models/Product.h"
#include "Models/Order.h"
#include "Models/OrderItem.h"
#include "Models/DashboardData.h"
#include "Models/ReportData.h"
#include "Models/AppSettings.h"

namespace MyShop2025::Services
{
    class DatabaseService
    {
    private:
        std::unique_ptr<Database::Database> m_db;
        static DatabaseService* s_instance;

    public:
        DatabaseService();
        ~DatabaseService();

        static DatabaseService* Instance();
        bool Initialize(const std::wstring& dbPath = L"");

        Database::Database* DB() { return m_db.get(); }

        std::vector<Models::Category> GetCategories();
        Models::Category GetCategory(int64_t id);
        int64_t InsertCategory(const Models::Category& cat);
        bool UpdateCategory(const Models::Category& cat);
        bool DeleteCategory(int64_t id);

        std::vector<Models::Product> GetProducts(int64_t categoryId = -1, int page = 0, int pageSize = 10,
            const std::wstring& sortBy = L"name", bool asc = true,
            const std::wstring& search = L"", int64_t minPrice = 0, int64_t maxPrice = 999999999);
        int64_t GetProductCount(int64_t categoryId = -1, const std::wstring& search = L"",
            int64_t minPrice = 0, int64_t maxPrice = 999999999);
        Models::Product GetProduct(int64_t id);
        int64_t InsertProduct(const Models::Product& prod);
        bool UpdateProduct(const Models::Product& prod);
        bool DeleteProduct(int64_t id);
        int64_t GetTotalProducts();

        int64_t InsertOrder(const Models::Order& order);
        bool UpdateOrderStatus(int64_t orderId, Models::OrderStatus status);
        bool DeleteOrder(int64_t id);
        Models::Order GetOrder(int64_t id);
        std::vector<Models::Order> GetOrders(int page = 0, int pageSize = 10,
            const std::wstring& dateFrom = L"", const std::wstring& dateTo = L"");
        int64_t GetOrderCount(const std::wstring& dateFrom = L"", const std::wstring& dateTo = L"");

        Models::DashboardData GetDashboardData();
        Models::ReportData GetReportData(const std::wstring& period, const std::wstring& dateFrom, const std::wstring& dateTo);

        Models::AppSettings GetSettings();
        bool SaveSettings(const Models::AppSettings& settings);
        std::wstring GetSetting(const std::wstring& key, const std::wstring& defaultVal);
        bool SetSetting(const std::wstring& key, const std::wstring& value);
    };
}

#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include "Models/Category.h"
#include "Models/Product.h"
#include "Models/Order.h"
#include "Models/OrderItem.h"

namespace MyShop2025::Database
{
    class Database
    {
    private:
        sqlite3* m_db = nullptr;
        std::wstring m_dbPath;
        bool m_open = false;

    public:
        static std::string WStringToUTF8(const std::wstring& wstr);
        static std::wstring UTF8ToWString(const std::string& utf8);

        Database();
        ~Database();

        bool Open(const std::wstring& dbPath = L"");
        void Close();
        bool IsOpen() const { return m_open; }
        sqlite3* GetHandle() { return m_db; }

        bool InitializeSchema();

        int64_t ExecuteScalar(const std::wstring& sql);
        bool ExecuteNonQuery(const std::wstring& sql);
        bool ExecuteWithParams(const std::wstring& sql, std::vector<std::wstring> params);

        std::vector<Models::Category> GetCategories();
        Models::Category GetCategory(int64_t id);
        int64_t InsertCategory(const Models::Category& cat);
        bool UpdateCategory(const Models::Category& cat);
        bool DeleteCategory(int64_t id);
        int64_t GetCategoryCount();

        std::vector<Models::Product> GetProducts(int64_t categoryId = -1, int page = 0, int pageSize = 10,
            const std::wstring& sortBy = L"name", bool asc = true,
            const std::wstring& search = L"", int64_t minPrice = 0, int64_t maxPrice = 999999999);
        int64_t GetProductCount(int64_t categoryId = -1, const std::wstring& search = L"",
            int64_t minPrice = 0, int64_t maxPrice = 999999999);
        Models::Product GetProduct(int64_t id);
        int64_t InsertProduct(const Models::Product& prod);
        bool UpdateProduct(const Models::Product& prod);
        bool DeleteProduct(int64_t id);
        std::vector<std::wstring> GetProductImages(int64_t productId);
        bool InsertProductImage(int64_t productId, const std::wstring& imagePath);
        bool DeleteProductImages(int64_t productId);
        int64_t GetTotalProducts();
        std::vector<Models::Product> GetLowStockProducts(int threshold = 5, int limit = 5);

        int64_t InsertOrder(const Models::Order& order);
        bool UpdateOrder(const Models::Order& order);
        bool DeleteOrder(int64_t id);
        Models::Order GetOrder(int64_t id);
        std::vector<Models::Order> GetOrders(int page = 0, int pageSize = 10, const std::wstring& dateFrom = L"", const std::wstring& dateTo = L"");
        int64_t GetOrderCount(const std::wstring& dateFrom = L"", const std::wstring& dateTo = L"");
        int64_t GetOrdersToday();
        int64_t GetRevenueToday();
        int64_t GetProfitToday();
        std::vector<Models::Order> GetRecentOrders(int limit = 3);

        int64_t InsertOrderItem(const Models::OrderItem& item);
        std::vector<Models::OrderItem> GetOrderItems(int64_t orderId);

        std::vector<std::pair<Models::Product, int64_t>> GetTopSellingProducts(int limit = 5);
        std::vector<std::pair<int, int64_t>> GetDailyRevenueThisMonth();

        std::vector<std::pair<std::wstring, int64_t>> GetProductSalesReport(const std::wstring& period, const std::wstring& dateFrom, const std::wstring& dateTo);
        std::vector<std::pair<std::wstring, int64_t>> GetRevenueReport(const std::wstring& period, const std::wstring& dateFrom, const std::wstring& dateTo);
        std::vector<std::pair<std::wstring, int64_t>> GetProfitReport(const std::wstring& period, const std::wstring& dateFrom, const std::wstring& dateTo);
    };
}

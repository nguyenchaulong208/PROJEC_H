#include "pch.h"
#include "DatabaseService.h"

using namespace MyShop2025::Database;

namespace MyShop2025::Services
{
    DatabaseService* DatabaseService::s_instance = nullptr;

    DatabaseService::DatabaseService()
        : m_db(std::make_unique<Database::Database>())
    {
        s_instance = this;
    }

    DatabaseService::~DatabaseService()
    {
        s_instance = nullptr;
    }

    DatabaseService* DatabaseService::Instance()
    {
        return s_instance;
    }

    bool DatabaseService::Initialize(const std::wstring& dbPath)
    {
        return m_db->Open(dbPath);
    }

    std::vector<Models::Category> DatabaseService::GetCategories()
    {
        return m_db->GetCategories();
    }

    Models::Category DatabaseService::GetCategory(int64_t id)
    {
        return m_db->GetCategory(id);
    }

    int64_t DatabaseService::InsertCategory(const Models::Category& cat)
    {
        return m_db->InsertCategory(cat);
    }

    bool DatabaseService::UpdateCategory(const Models::Category& cat)
    {
        return m_db->UpdateCategory(cat);
    }

    bool DatabaseService::DeleteCategory(int64_t id)
    {
        return m_db->DeleteCategory(id);
    }

    std::vector<Models::Product> DatabaseService::GetProducts(int64_t categoryId, int page, int pageSize,
        const std::wstring& sortBy, bool asc, const std::wstring& search,
        int64_t minPrice, int64_t maxPrice)
    {
        return m_db->GetProducts(categoryId, page, pageSize, sortBy, asc, search, minPrice, maxPrice);
    }

    int64_t DatabaseService::GetProductCount(int64_t categoryId, const std::wstring& search,
        int64_t minPrice, int64_t maxPrice)
    {
        return m_db->GetProductCount(categoryId, search, minPrice, maxPrice);
    }

    Models::Product DatabaseService::GetProduct(int64_t id)
    {
        return m_db->GetProduct(id);
    }

    int64_t DatabaseService::InsertProduct(const Models::Product& prod)
    {
        return m_db->InsertProduct(prod);
    }

    bool DatabaseService::UpdateProduct(const Models::Product& prod)
    {
        return m_db->UpdateProduct(prod);
    }

    bool DatabaseService::DeleteProduct(int64_t id)
    {
        return m_db->DeleteProduct(id);
    }

    int64_t DatabaseService::GetTotalProducts()
    {
        return m_db->GetTotalProducts();
    }

    int64_t DatabaseService::InsertOrder(const Models::Order& order)
    {
        return m_db->InsertOrder(order);
    }

    bool DatabaseService::UpdateOrderStatus(int64_t orderId, Models::OrderStatus status)
    {
        auto order = m_db->GetOrder(orderId);
        order.status = status;
        return m_db->UpdateOrder(order);
    }

    bool DatabaseService::DeleteOrder(int64_t id)
    {
        return m_db->DeleteOrder(id);
    }

    Models::Order DatabaseService::GetOrder(int64_t id)
    {
        return m_db->GetOrder(id);
    }

    std::vector<Models::Order> DatabaseService::GetOrders(int page, int pageSize,
        const std::wstring& dateFrom, const std::wstring& dateTo)
    {
        return m_db->GetOrders(page, pageSize, dateFrom, dateTo);
    }

    int64_t DatabaseService::GetOrderCount(const std::wstring& dateFrom, const std::wstring& dateTo)
    {
        return m_db->GetOrderCount(dateFrom, dateTo);
    }

    Models::DashboardData DatabaseService::GetDashboardData()
    {
        Models::DashboardData data;
        data.totalProducts = m_db->GetTotalProducts();
        data.totalOrdersToday = m_db->GetOrdersToday();
        data.totalRevenueToday = m_db->GetRevenueToday();
        data.totalProfitToday = m_db->GetProfitToday();
        data.lowStockProducts = m_db->GetLowStockProducts(5, 5);
        data.topSellingProducts = m_db->GetTopSellingProducts(5);
        data.recentOrders = m_db->GetRecentOrders(3);
        data.dailyRevenue = m_db->GetDailyRevenueThisMonth();
        return data;
    }

    Models::ReportData DatabaseService::GetReportData(const std::wstring& period,
        const std::wstring& dateFrom, const std::wstring& dateTo)
    {
        Models::ReportData data;
        data.productSales = m_db->GetProductSalesReport(period, dateFrom, dateTo);
        data.revenue = m_db->GetRevenueReport(period, dateFrom, dateTo);
        data.profit = m_db->GetProfitReport(period, dateFrom, dateTo);
        return data;
    }

    Models::AppSettings DatabaseService::GetSettings()
    {
        Models::AppSettings settings;
        settings.serverAddress = GetSetting(L"server_address", L"localhost");
        auto pageSizeStr = GetSetting(L"page_size", L"10");
        settings.pageSize = std::stoi(pageSizeStr);
        settings.lastScreen = GetSetting(L"last_screen", L"Dashboard");
        auto autoSaveStr = GetSetting(L"auto_save", L"true");
        settings.autoSave = (autoSaveStr == L"true");
        settings.rememberPassword = (GetSetting(L"remember_password", L"false") == L"true");
        settings.savedUsername = GetSetting(L"saved_username", L"");
        settings.savedPassword = GetSetting(L"saved_password", L"");
        return settings;
    }

    bool DatabaseService::SaveSettings(const Models::AppSettings& settings)
    {
        SetSetting(L"server_address", settings.serverAddress);
        SetSetting(L"page_size", std::to_wstring(settings.pageSize));
        SetSetting(L"last_screen", settings.lastScreen);
        SetSetting(L"auto_save", settings.autoSave ? L"true" : L"false");
        SetSetting(L"remember_password", settings.rememberPassword ? L"true" : L"false");
        SetSetting(L"saved_username", settings.savedUsername);
        SetSetting(L"saved_password", settings.savedPassword);
        return true;
    }

    std::wstring DatabaseService::GetSetting(const std::wstring& key, const std::wstring& defaultVal)
    {
        std::string sql = "SELECT value FROM SETTINGS WHERE key = ?";
        sqlite3_stmt* stmt = nullptr;
        std::wstring result = defaultVal;

        auto keyUtf8 = Database::WStringToUTF8(key);
        if (sqlite3_prepare_v2(m_db->GetHandle(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_text(stmt, 1, keyUtf8.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                if (sqlite3_column_type(stmt, 0) != SQLITE_NULL)
                    result = Database::UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            }
            sqlite3_finalize(stmt);
        }
        return result;
    }

    bool DatabaseService::SetSetting(const std::wstring& key, const std::wstring& value)
    {
        std::string sql = "INSERT OR REPLACE INTO SETTINGS (key, value) VALUES (?, ?)";
        sqlite3_stmt* stmt = nullptr;
        bool ok = false;

        auto keyUtf8 = Database::WStringToUTF8(key);
        auto valUtf8 = Database::WStringToUTF8(value);
        if (sqlite3_prepare_v2(m_db->GetHandle(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_text(stmt, 1, keyUtf8.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, valUtf8.c_str(), -1, SQLITE_TRANSIENT);
            ok = (sqlite3_step(stmt) == SQLITE_DONE);
            sqlite3_finalize(stmt);
        }
        return ok;
    }
}

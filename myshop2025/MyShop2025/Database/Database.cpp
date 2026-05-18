#include "pch.h"
#include "Database.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace MyShop2025::Database
{
    Database::Database() {}

    Database::~Database()
    {
        Close();
    }

    bool Database::Open(const std::wstring& dbPath)
    {
        if (m_open) Close();

        m_dbPath = dbPath.empty() ? (fs::current_path() / L"myshop.db").wstring() : dbPath;

        if (sqlite3_open16(m_dbPath.c_str(), &m_db) != SQLITE_OK)
        {
            m_db = nullptr;
            m_open = false;
            return false;
        }
        m_open = true;
        return InitializeSchema();
    }

    void Database::Close()
    {
        if (m_db)
        {
            sqlite3_close(m_db);
            m_db = nullptr;
        }
        m_open = false;
    }

    bool Database::InitializeSchema()
    {
        const wchar_t* schema = LR"(
            CREATE TABLE IF NOT EXISTS CATEGORY (
                category_id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                description TEXT DEFAULT ''
            );
            CREATE TABLE IF NOT EXISTS PRODUCT (
                product_id INTEGER PRIMARY KEY AUTOINCREMENT,
                sku TEXT UNIQUE NOT NULL,
                name TEXT NOT NULL,
                import_price INTEGER DEFAULT 0,
                sale_price INTEGER DEFAULT 0,
                count INTEGER DEFAULT 0,
                description TEXT DEFAULT '',
                category_id INTEGER,
                FOREIGN KEY (category_id) REFERENCES CATEGORY(category_id)
            );
            CREATE TABLE IF NOT EXISTS PRODUCT_IMAGE (
                image_id INTEGER PRIMARY KEY AUTOINCREMENT,
                product_id INTEGER NOT NULL,
                image_path TEXT NOT NULL,
                FOREIGN KEY (product_id) REFERENCES PRODUCT(product_id) ON DELETE CASCADE
            );
            CREATE TABLE IF NOT EXISTS [ORDER] (
                order_id INTEGER PRIMARY KEY AUTOINCREMENT,
                created_time TEXT NOT NULL,
                final_price INTEGER DEFAULT 0,
                status INTEGER DEFAULT 0
            );
            CREATE TABLE IF NOT EXISTS ORDER_ITEM (
                order_item_id INTEGER PRIMARY KEY AUTOINCREMENT,
                quantity INTEGER DEFAULT 1,
                unit_sale_price REAL DEFAULT 0.0,
                total_price INTEGER DEFAULT 0,
                order_id INTEGER NOT NULL,
                product_id INTEGER NOT NULL,
                FOREIGN KEY (order_id) REFERENCES [ORDER](order_id) ON DELETE CASCADE,
                FOREIGN KEY (product_id) REFERENCES PRODUCT(product_id)
            );
            CREATE TABLE IF NOT EXISTS SETTINGS (
                key TEXT PRIMARY KEY,
                value TEXT
            );
        )";

        char* errMsg = nullptr;
        if (sqlite3_exec(m_db, reinterpret_cast<const char*>(schema), nullptr, nullptr, &errMsg) != SQLITE_OK)
        {
            if (errMsg)
            {
                OutputDebugStringA(errMsg);
                sqlite3_free(errMsg);
            }
            return false;
        }
        return true;
    }

    int64_t Database::ExecuteScalar(const std::wstring& sql)
    {
        sqlite3_stmt* stmt = nullptr;
        int64_t result = 0;
        std::string sqlUtf8;
        int len = WideCharToMultiByte(CP_UTF8, 0, sql.c_str(), -1, nullptr, 0, nullptr, nullptr);
        sqlUtf8.resize(len);
        WideCharToMultiByte(CP_UTF8, 0, sql.c_str(), -1, &sqlUtf8[0], len, nullptr, nullptr);

        if (sqlite3_prepare_v2(m_db, sqlUtf8.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                result = sqlite3_column_int64(stmt, 0);
            }
            sqlite3_finalize(stmt);
        }
        return result;
    }

    bool Database::ExecuteNonQuery(const std::wstring& sql)
    {
        char* errMsg = nullptr;
        std::string sqlUtf8;
        int len = WideCharToMultiByte(CP_UTF8, 0, sql.c_str(), -1, nullptr, 0, nullptr, nullptr);
        sqlUtf8.resize(len);
        WideCharToMultiByte(CP_UTF8, 0, sql.c_str(), -1, &sqlUtf8[0], len, nullptr, nullptr);

        if (sqlite3_exec(m_db, sqlUtf8.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
        {
            if (errMsg) sqlite3_free(errMsg);
            return false;
        }
        return true;
    }

std::string Database::WStringToUTF8(const std::wstring& wstr)
{
    if (wstr.empty()) return {};
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string result(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], len, nullptr, nullptr);
    result.resize(len - 1);
    return result;
}

std::wstring Database::UTF8ToWString(const std::string& utf8)
{
    if (utf8.empty()) return {};
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    std::wstring result(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &result[0], len);
    result.resize(len - 1);
    return result;
}

    // ==================== CATEGORY ====================

    std::vector<Models::Category> Database::GetCategories()
    {
        std::vector<Models::Category> categories;
        sqlite3_stmt* stmt = nullptr;
        std::string sql = "SELECT category_id, name, description FROM CATEGORY ORDER BY name";

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                Models::Category cat;
                cat.categoryId = sqlite3_column_int64(stmt, 0);
                if (sqlite3_column_type(stmt, 1) != SQLITE_NULL)
                    cat.name = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
                if (sqlite3_column_type(stmt, 2) != SQLITE_NULL)
                    cat.description = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
                categories.push_back(cat);
            }
            sqlite3_finalize(stmt);
        }
        return categories;
    }

    Models::Category Database::GetCategory(int64_t id)
    {
        Models::Category cat;
        sqlite3_stmt* stmt = nullptr;
        std::string sql = "SELECT category_id, name, description FROM CATEGORY WHERE category_id = ?";

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int64(stmt, 1, id);
            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                cat.categoryId = sqlite3_column_int64(stmt, 0);
                if (sqlite3_column_type(stmt, 1) != SQLITE_NULL)
                    cat.name = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
                if (sqlite3_column_type(stmt, 2) != SQLITE_NULL)
                    cat.description = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            }
            sqlite3_finalize(stmt);
        }
        return cat;
    }

    int64_t Database::InsertCategory(const Models::Category& cat)
    {
        std::string sql = "INSERT INTO CATEGORY (name, description) VALUES (?, ?)";
        sqlite3_stmt* stmt = nullptr;
        int64_t id = 0;

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            auto nameUtf8 = WStringToUTF8(cat.name);
            auto descUtf8 = WStringToUTF8(cat.description);
            sqlite3_bind_text(stmt, 1, nameUtf8.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, descUtf8.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) == SQLITE_DONE)
            {
                id = sqlite3_last_insert_rowid(m_db);
            }
            sqlite3_finalize(stmt);
        }
        return id;
    }

    bool Database::UpdateCategory(const Models::Category& cat)
    {
        std::string sql = "UPDATE CATEGORY SET name = ?, description = ? WHERE category_id = ?";
        sqlite3_stmt* stmt = nullptr;
        bool ok = false;

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            auto nameUtf8 = WStringToUTF8(cat.name);
            auto descUtf8 = WStringToUTF8(cat.description);
            sqlite3_bind_text(stmt, 1, nameUtf8.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, descUtf8.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int64(stmt, 3, cat.categoryId);
            ok = (sqlite3_step(stmt) == SQLITE_DONE);
            sqlite3_finalize(stmt);
        }
        return ok;
    }

    bool Database::DeleteCategory(int64_t id)
    {
        std::string sql = "DELETE FROM CATEGORY WHERE category_id = ?";
        sqlite3_stmt* stmt = nullptr;
        bool ok = false;

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int64(stmt, 1, id);
            ok = (sqlite3_step(stmt) == SQLITE_DONE);
            sqlite3_finalize(stmt);
        }
        return ok;
    }

    int64_t Database::GetCategoryCount()
    {
        return ExecuteScalar(L"SELECT COUNT(*) FROM CATEGORY");
    }

    // ==================== PRODUCT ====================

    std::vector<Models::Product> Database::GetProducts(int64_t categoryId, int page, int pageSize,
        const std::wstring& sortBy, bool asc, const std::wstring& search,
        int64_t minPrice, int64_t maxPrice)
    {
        std::vector<Models::Product> products;
        std::string sql = "SELECT p.product_id, p.sku, p.name, p.import_price, p.sale_price, "
            "p.count, p.description, p.category_id, COALESCE(c.name, '') AS cat_name "
            "FROM PRODUCT p LEFT JOIN CATEGORY c ON p.category_id = c.category_id WHERE 1=1";

        if (categoryId > 0)
            sql += " AND p.category_id = " + std::to_string(categoryId);
        if (!search.empty())
        {
            auto s = WStringToUTF8(search);
            sql += " AND (p.name LIKE '%" + s + "%' OR p.sku LIKE '%" + s + "%')";
        }
        sql += " AND p.sale_price >= " + std::to_string(minPrice) + " AND p.sale_price <= " + std::to_string(maxPrice);

        std::string orderCol = "p.name";
        if (sortBy == L"sku") orderCol = "p.sku";
        else if (sortBy == L"price" || sortBy == L"sale_price") orderCol = "p.sale_price";
        else if (sortBy == L"count") orderCol = "p.count";
        else if (sortBy == L"import_price") orderCol = "p.import_price";

        sql += " ORDER BY " + orderCol + (asc ? " ASC" : " DESC");
        sql += " LIMIT " + std::to_string(pageSize) + " OFFSET " + std::to_string(page * pageSize);

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                Models::Product prod;
                prod.productId = sqlite3_column_int64(stmt, 0);
                if (sqlite3_column_type(stmt, 1) != SQLITE_NULL)
                    prod.sku = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
                if (sqlite3_column_type(stmt, 2) != SQLITE_NULL)
                    prod.name = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
                prod.importPrice = sqlite3_column_int64(stmt, 3);
                prod.salePrice = sqlite3_column_int64(stmt, 4);
                prod.count = sqlite3_column_int(stmt, 5);
                if (sqlite3_column_type(stmt, 6) != SQLITE_NULL)
                    prod.description = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
                prod.categoryId = sqlite3_column_int64(stmt, 7);
                if (sqlite3_column_type(stmt, 8) != SQLITE_NULL)
                    prod.categoryName = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));
                prod.images = GetProductImages(prod.productId);
                products.push_back(prod);
            }
            sqlite3_finalize(stmt);
        }
        return products;
    }

    int64_t Database::GetProductCount(int64_t categoryId, const std::wstring& search, int64_t minPrice, int64_t maxPrice)
    {
        std::string sql = "SELECT COUNT(*) FROM PRODUCT p WHERE 1=1";
        if (categoryId > 0)
            sql += " AND p.category_id = " + std::to_string(categoryId);
        if (!search.empty())
        {
            auto s = WStringToUTF8(search);
            sql += " AND (p.name LIKE '%" + s + "%' OR p.sku LIKE '%" + s + "%')";
        }
        sql += " AND p.sale_price >= " + std::to_string(minPrice) + " AND p.sale_price <= " + std::to_string(maxPrice);

        sqlite3_stmt* stmt = nullptr;
        int64_t count = 0;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            if (sqlite3_step(stmt) == SQLITE_ROW)
                count = sqlite3_column_int64(stmt, 0);
            sqlite3_finalize(stmt);
        }
        return count;
    }

    Models::Product Database::GetProduct(int64_t id)
    {
        Models::Product prod;
        sqlite3_stmt* stmt = nullptr;
        std::string sql = "SELECT p.product_id, p.sku, p.name, p.import_price, p.sale_price, "
            "p.count, p.description, p.category_id, COALESCE(c.name, '') FROM PRODUCT p "
            "LEFT JOIN CATEGORY c ON p.category_id = c.category_id WHERE p.product_id = ?";

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int64(stmt, 1, id);
            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                prod.productId = sqlite3_column_int64(stmt, 0);
                if (sqlite3_column_type(stmt, 1) != SQLITE_NULL)
                    prod.sku = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
                if (sqlite3_column_type(stmt, 2) != SQLITE_NULL)
                    prod.name = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
                prod.importPrice = sqlite3_column_int64(stmt, 3);
                prod.salePrice = sqlite3_column_int64(stmt, 4);
                prod.count = sqlite3_column_int(stmt, 5);
                if (sqlite3_column_type(stmt, 6) != SQLITE_NULL)
                    prod.description = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
                prod.categoryId = sqlite3_column_int64(stmt, 7);
                if (sqlite3_column_type(stmt, 8) != SQLITE_NULL)
                    prod.categoryName = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));
                prod.images = GetProductImages(prod.productId);
            }
            sqlite3_finalize(stmt);
        }
        return prod;
    }

    int64_t Database::InsertProduct(const Models::Product& prod)
    {
        std::string sql = "INSERT INTO PRODUCT (sku, name, import_price, sale_price, count, description, category_id) "
            "VALUES (?, ?, ?, ?, ?, ?, ?)";
        sqlite3_stmt* stmt = nullptr;
        int64_t id = 0;

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            auto sku = WStringToUTF8(prod.sku);
            auto name = WStringToUTF8(prod.name);
            auto desc = WStringToUTF8(prod.description);
            sqlite3_bind_text(stmt, 1, sku.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int64(stmt, 3, prod.importPrice);
            sqlite3_bind_int64(stmt, 4, prod.salePrice);
            sqlite3_bind_int(stmt, 5, prod.count);
            sqlite3_bind_text(stmt, 6, desc.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int64(stmt, 7, prod.categoryId);

            if (sqlite3_step(stmt) == SQLITE_DONE)
            {
                id = sqlite3_last_insert_rowid(m_db);
                for (const auto& img : prod.images)
                    InsertProductImage(id, img);
            }
            sqlite3_finalize(stmt);
        }
        return id;
    }

    bool Database::UpdateProduct(const Models::Product& prod)
    {
        std::string sql = "UPDATE PRODUCT SET sku = ?, name = ?, import_price = ?, sale_price = ?, "
            "count = ?, description = ?, category_id = ? WHERE product_id = ?";
        sqlite3_stmt* stmt = nullptr;
        bool ok = false;

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            auto sku = WStringToUTF8(prod.sku);
            auto name = WStringToUTF8(prod.name);
            auto desc = WStringToUTF8(prod.description);
            sqlite3_bind_text(stmt, 1, sku.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int64(stmt, 3, prod.importPrice);
            sqlite3_bind_int64(stmt, 4, prod.salePrice);
            sqlite3_bind_int(stmt, 5, prod.count);
            sqlite3_bind_text(stmt, 6, desc.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int64(stmt, 7, prod.categoryId);
            sqlite3_bind_int64(stmt, 8, prod.productId);
            ok = (sqlite3_step(stmt) == SQLITE_DONE);
            sqlite3_finalize(stmt);
        }
        DeleteProductImages(prod.productId);
        for (const auto& img : prod.images)
            InsertProductImage(prod.productId, img);
        return ok;
    }

    bool Database::DeleteProduct(int64_t id)
    {
        DeleteProductImages(id);
        std::string sql = "DELETE FROM PRODUCT WHERE product_id = ?";
        sqlite3_stmt* stmt = nullptr;
        bool ok = false;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int64(stmt, 1, id);
            ok = (sqlite3_step(stmt) == SQLITE_DONE);
            sqlite3_finalize(stmt);
        }
        return ok;
    }

    std::vector<std::wstring> Database::GetProductImages(int64_t productId)
    {
        std::vector<std::wstring> images;
        sqlite3_stmt* stmt = nullptr;
        std::string sql = "SELECT image_path FROM PRODUCT_IMAGE WHERE product_id = ? ORDER BY image_id";

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int64(stmt, 1, productId);
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                if (sqlite3_column_type(stmt, 0) != SQLITE_NULL)
                    images.push_back(UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
            }
            sqlite3_finalize(stmt);
        }
        return images;
    }

    bool Database::InsertProductImage(int64_t productId, const std::wstring& imagePath)
    {
        std::string sql = "INSERT INTO PRODUCT_IMAGE (product_id, image_path) VALUES (?, ?)";
        sqlite3_stmt* stmt = nullptr;
        bool ok = false;

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            auto path = WStringToUTF8(imagePath);
            sqlite3_bind_int64(stmt, 1, productId);
            sqlite3_bind_text(stmt, 2, path.c_str(), -1, SQLITE_TRANSIENT);
            ok = (sqlite3_step(stmt) == SQLITE_DONE);
            sqlite3_finalize(stmt);
        }
        return ok;
    }

    bool Database::DeleteProductImages(int64_t productId)
    {
        std::string sql = "DELETE FROM PRODUCT_IMAGE WHERE product_id = ?";
        sqlite3_stmt* stmt = nullptr;
        bool ok = false;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int64(stmt, 1, productId);
            ok = (sqlite3_step(stmt) == SQLITE_DONE);
            sqlite3_finalize(stmt);
        }
        return ok;
    }

    int64_t Database::GetTotalProducts()
    {
        return ExecuteScalar(L"SELECT COUNT(*) FROM PRODUCT");
    }

    std::vector<Models::Product> Database::GetLowStockProducts(int threshold, int limit)
    {
        std::vector<Models::Product> products;
        std::string sql = "SELECT p.product_id, p.sku, p.name, p.import_price, p.sale_price, "
            "p.count, p.description, p.category_id, COALESCE(c.name, '') FROM PRODUCT p "
            "LEFT JOIN CATEGORY c ON p.category_id = c.category_id "
            "WHERE p.count < ? AND p.count > 0 ORDER BY p.count ASC LIMIT ?";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int(stmt, 1, threshold);
            sqlite3_bind_int(stmt, 2, limit);
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                Models::Product prod;
                prod.productId = sqlite3_column_int64(stmt, 0);
                if (sqlite3_column_type(stmt, 1) != SQLITE_NULL)
                    prod.sku = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
                if (sqlite3_column_type(stmt, 2) != SQLITE_NULL)
                    prod.name = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
                prod.importPrice = sqlite3_column_int64(stmt, 3);
                prod.salePrice = sqlite3_column_int64(stmt, 4);
                prod.count = sqlite3_column_int(stmt, 5);
                if (sqlite3_column_type(stmt, 6) != SQLITE_NULL)
                    prod.description = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
                prod.categoryId = sqlite3_column_int64(stmt, 7);
                if (sqlite3_column_type(stmt, 8) != SQLITE_NULL)
                    prod.categoryName = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));
                products.push_back(prod);
            }
            sqlite3_finalize(stmt);
        }
        return products;
    }

    // ==================== ORDER ====================

    int64_t Database::InsertOrder(const Models::Order& order)
    {
        auto now = std::chrono::system_clock::to_time_t(order.createdTime);
        std::tm tm;
        localtime_s(&tm, &now);
        std::wstringstream wss;
        wss << std::put_time(&tm, L"%Y-%m-%d %H:%M:%S");
        auto timeStr = wss.str();

        std::string sql = "INSERT INTO [ORDER] (created_time, final_price, status) VALUES (?, ?, ?)";
        sqlite3_stmt* stmt = nullptr;
        int64_t id = 0;

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            auto t = WStringToUTF8(timeStr);
            sqlite3_bind_text(stmt, 1, t.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int64(stmt, 2, order.finalPrice);
            sqlite3_bind_int(stmt, 3, static_cast<int>(order.status));
            if (sqlite3_step(stmt) == SQLITE_DONE)
            {
                id = sqlite3_last_insert_rowid(m_db);
                for (const auto& item : order.items)
                {
                    auto itemCopy = item;
                    itemCopy.orderId = id;
                    InsertOrderItem(itemCopy);

                    sqlite3_stmt* upStmt = nullptr;
                    std::string upSql = "UPDATE PRODUCT SET count = count - ? WHERE product_id = ?";
                    if (sqlite3_prepare_v2(m_db, upSql.c_str(), -1, &upStmt, nullptr) == SQLITE_OK)
                    {
                        sqlite3_bind_int(upStmt, 1, item.quantity);
                        sqlite3_bind_int64(upStmt, 2, item.productId);
                        sqlite3_step(upStmt);
                        sqlite3_finalize(upStmt);
                    }
                }
            }
            sqlite3_finalize(stmt);
        }
        return id;
    }

    bool Database::UpdateOrder(const Models::Order& order)
    {
        auto now = std::chrono::system_clock::to_time_t(order.createdTime);
        std::tm tm;
        localtime_s(&tm, &now);
        std::wstringstream wss;
        wss << std::put_time(&tm, L"%Y-%m-%d %H:%M:%S");
        auto timeStr = wss.str();

        std::string sql = "UPDATE [ORDER] SET created_time = ?, final_price = ?, status = ? WHERE order_id = ?";
        sqlite3_stmt* stmt = nullptr;
        bool ok = false;

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            auto t = WStringToUTF8(timeStr);
            sqlite3_bind_text(stmt, 1, t.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int64(stmt, 2, order.finalPrice);
            sqlite3_bind_int(stmt, 3, static_cast<int>(order.status));
            sqlite3_bind_int64(stmt, 4, order.orderId);
            ok = (sqlite3_step(stmt) == SQLITE_DONE);
            sqlite3_finalize(stmt);
        }
        return ok;
    }

    bool Database::DeleteOrder(int64_t id)
    {
        auto items = GetOrderItems(id);
        for (const auto& item : items)
        {
            sqlite3_stmt* upStmt = nullptr;
            std::string upSql = "UPDATE PRODUCT SET count = count + ? WHERE product_id = ?";
            if (sqlite3_prepare_v2(m_db, upSql.c_str(), -1, &upStmt, nullptr) == SQLITE_OK)
            {
                sqlite3_bind_int(upStmt, 1, item.quantity);
                sqlite3_bind_int64(upStmt, 2, item.productId);
                sqlite3_step(upStmt);
                sqlite3_finalize(upStmt);
            }
        }

        std::string sql = "DELETE FROM ORDER_ITEM WHERE order_id = ?";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int64(stmt, 1, id);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }

        sql = "DELETE FROM [ORDER] WHERE order_id = ?";
        bool ok = false;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int64(stmt, 1, id);
            ok = (sqlite3_step(stmt) == SQLITE_DONE);
            sqlite3_finalize(stmt);
        }
        return ok;
    }

    Models::Order Database::GetOrder(int64_t id)
    {
        Models::Order order;
        sqlite3_stmt* stmt = nullptr;
        std::string sql = "SELECT order_id, created_time, final_price, status FROM [ORDER] WHERE order_id = ?";

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int64(stmt, 1, id);
            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                order.orderId = sqlite3_column_int64(stmt, 0);
                if (sqlite3_column_type(stmt, 1) != SQLITE_NULL)
                {
                    auto timeStr = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
                    std::wistringstream wiss(timeStr);
                    std::tm tm = {};
                    wiss >> std::get_time(&tm, L"%Y-%m-%d %H:%M:%S");
                    auto tp = std::chrono::system_clock::from_time_t(_mktime32(&tm));
                    order.createdTime = tp;
                }
                order.finalPrice = sqlite3_column_int64(stmt, 2);
                order.status = static_cast<Models::OrderStatus>(sqlite3_column_int(stmt, 3));
                order.items = GetOrderItems(order.orderId);
            }
            sqlite3_finalize(stmt);
        }
        return order;
    }

    std::vector<Models::Order> Database::GetOrders(int page, int pageSize, const std::wstring& dateFrom, const std::wstring& dateTo)
    {
        std::vector<Models::Order> orders;
        std::string sql = "SELECT order_id, created_time, final_price, status FROM [ORDER] WHERE 1=1";

        if (!dateFrom.empty())
        {
            auto df = WStringToUTF8(dateFrom);
            sql += " AND created_time >= '" + df + "'";
        }
        if (!dateTo.empty())
        {
            auto dt = WStringToUTF8(dateTo);
            sql += " AND created_time <= '" + dt + " 23:59:59'";
        }

        sql += " ORDER BY created_time DESC LIMIT " + std::to_string(pageSize) + " OFFSET " + std::to_string(page * pageSize);

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                Models::Order order;
                order.orderId = sqlite3_column_int64(stmt, 0);
                if (sqlite3_column_type(stmt, 1) != SQLITE_NULL)
                {
                    auto timeStr = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
                    std::wistringstream wiss(timeStr);
                    std::tm tm = {};
                    wiss >> std::get_time(&tm, L"%Y-%m-%d %H:%M:%S");
                    order.createdTime = std::chrono::system_clock::from_time_t(_mktime32(&tm));
                }
                order.finalPrice = sqlite3_column_int64(stmt, 2);
                order.status = static_cast<Models::OrderStatus>(sqlite3_column_int(stmt, 3));
                order.items = GetOrderItems(order.orderId);
                orders.push_back(order);
            }
            sqlite3_finalize(stmt);
        }
        return orders;
    }

    int64_t Database::GetOrderCount(const std::wstring& dateFrom, const std::wstring& dateTo)
    {
        std::string sql = "SELECT COUNT(*) FROM [ORDER] WHERE 1=1";
        if (!dateFrom.empty())
        {
            auto df = WStringToUTF8(dateFrom);
            sql += " AND created_time >= '" + df + "'";
        }
        if (!dateTo.empty())
        {
            auto dt = WStringToUTF8(dateTo);
            sql += " AND created_time <= '" + dt + " 23:59:59'";
        }

        sqlite3_stmt* stmt = nullptr;
        int64_t count = 0;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            if (sqlite3_step(stmt) == SQLITE_ROW)
                count = sqlite3_column_int64(stmt, 0);
            sqlite3_finalize(stmt);
        }
        return count;
    }

    int64_t Database::GetOrdersToday()
    {
        auto now = std::chrono::system_clock::now();
        auto tt = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        localtime_s(&tm, &tt);
        std::wstringstream wss;
        wss << std::put_time(&tm, L"%Y-%m-%d");
        auto today = wss.str();
        auto todayUtf8 = WStringToUTF8(today);

        std::string sql = "SELECT COUNT(*) FROM [ORDER] WHERE created_time >= '" + todayUtf8 + "'";
        sqlite3_stmt* stmt = nullptr;
        int64_t count = 0;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            if (sqlite3_step(stmt) == SQLITE_ROW)
                count = sqlite3_column_int64(stmt, 0);
            sqlite3_finalize(stmt);
        }
        return count;
    }

    int64_t Database::GetRevenueToday()
    {
        auto now = std::chrono::system_clock::now();
        auto tt = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        localtime_s(&tm, &tt);
        std::wstringstream wss;
        wss << std::put_time(&tm, L"%Y-%m-%d");
        auto today = wss.str();
        auto todayUtf8 = WStringToUTF8(today);

        std::string sql = "SELECT COALESCE(SUM(final_price), 0) FROM [ORDER] WHERE created_time >= '" + todayUtf8 + "' AND status = 1";
        sqlite3_stmt* stmt = nullptr;
        int64_t revenue = 0;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            if (sqlite3_step(stmt) == SQLITE_ROW)
                revenue = sqlite3_column_int64(stmt, 0);
            sqlite3_finalize(stmt);
        }
        return revenue;
    }

    int64_t Database::GetProfitToday()
    {
        auto now = std::chrono::system_clock::now();
        auto tt = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        localtime_s(&tm, &tt);
        std::wstringstream wss;
        wss << std::put_time(&tm, L"%Y-%m-%d");
        auto today = wss.str();
        auto todayUtf8 = WStringToUTF8(today);

        std::string sql = "SELECT COALESCE(SUM(oi.total_price - (oi.quantity * p.import_price)), 0) "
            "FROM ORDER_ITEM oi JOIN PRODUCT p ON oi.product_id = p.product_id "
            "JOIN [ORDER] o ON oi.order_id = o.order_id "
            "WHERE o.created_time >= '" + todayUtf8 + "' AND o.status = 1";
        sqlite3_stmt* stmt = nullptr;
        int64_t profit = 0;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            if (sqlite3_step(stmt) == SQLITE_ROW)
                profit = sqlite3_column_int64(stmt, 0);
            sqlite3_finalize(stmt);
        }
        return profit;
    }

    std::vector<Models::Order> Database::GetRecentOrders(int limit)
    {
        return GetOrders(0, limit);
    }

    // ==================== ORDER ITEM ====================

    int64_t Database::InsertOrderItem(const Models::OrderItem& item)
    {
        std::string sql = "INSERT INTO ORDER_ITEM (quantity, unit_sale_price, total_price, order_id, product_id) "
            "VALUES (?, ?, ?, ?, ?)";
        sqlite3_stmt* stmt = nullptr;
        int64_t id = 0;

        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int(stmt, 1, item.quantity);
            sqlite3_bind_double(stmt, 2, item.unitSalePrice);
            sqlite3_bind_int64(stmt, 3, item.totalPrice);
            sqlite3_bind_int64(stmt, 4, item.orderId);
            sqlite3_bind_int64(stmt, 5, item.productId);
            if (sqlite3_step(stmt) == SQLITE_DONE)
                id = sqlite3_last_insert_rowid(m_db);
            sqlite3_finalize(stmt);
        }
        return id;
    }

    std::vector<Models::OrderItem> Database::GetOrderItems(int64_t orderId)
    {
        std::vector<Models::OrderItem> items;
        std::string sql = "SELECT oi.order_item_id, oi.quantity, oi.unit_sale_price, oi.total_price, "
            "oi.order_id, oi.product_id, COALESCE(p.name, '') FROM ORDER_ITEM oi "
            "LEFT JOIN PRODUCT p ON oi.product_id = p.product_id WHERE oi.order_id = ?";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int64(stmt, 1, orderId);
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                Models::OrderItem item;
                item.orderItemId = sqlite3_column_int64(stmt, 0);
                item.quantity = sqlite3_column_int(stmt, 1);
                item.unitSalePrice = sqlite3_column_double(stmt, 2);
                item.totalPrice = sqlite3_column_int64(stmt, 3);
                item.orderId = sqlite3_column_int64(stmt, 4);
                item.productId = sqlite3_column_int64(stmt, 5);
                if (sqlite3_column_type(stmt, 6) != SQLITE_NULL)
                    item.productName = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
                items.push_back(item);
            }
            sqlite3_finalize(stmt);
        }
        return items;
    }

    // ==================== DASHBOARD ====================

    std::vector<std::pair<Models::Product, int64_t>> Database::GetTopSellingProducts(int limit)
    {
        std::vector<std::pair<Models::Product, int64_t>> results;
        std::string sql = "SELECT p.product_id, p.sku, p.name, p.import_price, p.sale_price, "
            "p.count, p.description, p.category_id, COALESCE(c.name, ''), "
            "COALESCE(SUM(oi.quantity), 0) AS total_sold "
            "FROM PRODUCT p LEFT JOIN ORDER_ITEM oi ON p.product_id = oi.product_id "
            "LEFT JOIN CATEGORY c ON p.category_id = c.category_id "
            "GROUP BY p.product_id ORDER BY total_sold DESC LIMIT ?";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int(stmt, 1, limit);
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                Models::Product prod;
                prod.productId = sqlite3_column_int64(stmt, 0);
                if (sqlite3_column_type(stmt, 1) != SQLITE_NULL)
                    prod.sku = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
                if (sqlite3_column_type(stmt, 2) != SQLITE_NULL)
                    prod.name = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
                prod.importPrice = sqlite3_column_int64(stmt, 3);
                prod.salePrice = sqlite3_column_int64(stmt, 4);
                prod.count = sqlite3_column_int(stmt, 5);
                if (sqlite3_column_type(stmt, 6) != SQLITE_NULL)
                    prod.description = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
                prod.categoryId = sqlite3_column_int64(stmt, 7);
                if (sqlite3_column_type(stmt, 8) != SQLITE_NULL)
                    prod.categoryName = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));
                int64_t totalSold = sqlite3_column_int64(stmt, 9);
                results.push_back({ prod, totalSold });
            }
            sqlite3_finalize(stmt);
        }
        return results;
    }

    std::vector<std::pair<int, int64_t>> Database::GetDailyRevenueThisMonth()
    {
        std::vector<std::pair<int, int64_t>> results;
        auto now = std::chrono::system_clock::now();
        auto tt = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        localtime_s(&tm, &tt);
        std::wstringstream wss;
        wss << std::put_time(&tm, L"%Y-%m");
        auto monthPrefix = wss.str();
        auto monthPrefixUtf8 = WStringToUTF8(monthPrefix);

        std::string sql = "SELECT CAST(strftime('%d', created_time) AS INTEGER) AS day, "
            "COALESCE(SUM(final_price), 0) FROM [ORDER] "
            "WHERE created_time LIKE '" + monthPrefixUtf8 + "%' AND status = 1 "
            "GROUP BY day ORDER BY day";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                int day = sqlite3_column_int(stmt, 0);
                int64_t rev = sqlite3_column_int64(stmt, 1);
                results.push_back({ day, rev });
            }
            sqlite3_finalize(stmt);
        }
        return results;
    }

    // ==================== REPORTS ====================

    std::vector<std::pair<std::wstring, int64_t>> Database::GetProductSalesReport(const std::wstring& period,
        const std::wstring& dateFrom, const std::wstring& dateTo)
    {
        std::vector<std::pair<std::wstring, int64_t>> results;
        std::string sql;

        if (period == L"day")
            sql = "SELECT strftime('%H', o.created_time) AS label, COALESCE(SUM(oi.quantity), 0) "
                "FROM ORDER_ITEM oi JOIN [ORDER] o ON oi.order_id = o.order_id "
                "WHERE o.created_time >= '" + WStringToUTF8(dateFrom) + "' AND o.created_time <= '" + WStringToUTF8(dateTo) + " 23:59:59' "
                "GROUP BY label ORDER BY label";
        else if (period == L"week")
            sql = "SELECT strftime('%w', o.created_time) AS label, COALESCE(SUM(oi.quantity), 0) "
                "FROM ORDER_ITEM oi JOIN [ORDER] o ON oi.order_id = o.order_id "
                "WHERE o.created_time >= '" + WStringToUTF8(dateFrom) + "' AND o.created_time <= '" + WStringToUTF8(dateTo) + " 23:59:59' "
                "GROUP BY label ORDER BY label";
        else if (period == L"month")
            sql = "SELECT strftime('%d', o.created_time) AS label, COALESCE(SUM(oi.quantity), 0) "
                "FROM ORDER_ITEM oi JOIN [ORDER] o ON oi.order_id = o.order_id "
                "WHERE o.created_time >= '" + WStringToUTF8(dateFrom) + "' AND o.created_time <= '" + WStringToUTF8(dateTo) + " 23:59:59' "
                "GROUP BY label ORDER BY label";
        else
            sql = "SELECT strftime('%m', o.created_time) AS label, COALESCE(SUM(oi.quantity), 0) "
                "FROM ORDER_ITEM oi JOIN [ORDER] o ON oi.order_id = o.order_id "
                "WHERE o.created_time >= '" + WStringToUTF8(dateFrom) + "' AND o.created_time <= '" + WStringToUTF8(dateTo) + " 23:59:59' "
                "GROUP BY label ORDER BY label";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                std::wstring label;
                if (sqlite3_column_type(stmt, 0) != SQLITE_NULL)
                    label = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
                int64_t val = sqlite3_column_int64(stmt, 1);
                results.push_back({ label, val });
            }
            sqlite3_finalize(stmt);
        }
        return results;
    }

    std::vector<std::pair<std::wstring, int64_t>> Database::GetRevenueReport(const std::wstring& period,
        const std::wstring& dateFrom, const std::wstring& dateTo)
    {
        std::vector<std::pair<std::wstring, int64_t>> results;
        std::string sql;

        if (period == L"day")
            sql = "SELECT strftime('%H', created_time) AS label, COALESCE(SUM(final_price), 0) "
                "FROM [ORDER] WHERE created_time >= '" + WStringToUTF8(dateFrom) + "' AND created_time <= '" + WStringToUTF8(dateTo) + " 23:59:59' AND status = 1 "
                "GROUP BY label ORDER BY label";
        else if (period == L"week")
            sql = "SELECT strftime('%w', created_time) AS label, COALESCE(SUM(final_price), 0) "
                "FROM [ORDER] WHERE created_time >= '" + WStringToUTF8(dateFrom) + "' AND created_time <= '" + WStringToUTF8(dateTo) + " 23:59:59' AND status = 1 "
                "GROUP BY label ORDER BY label";
        else if (period == L"month")
            sql = "SELECT strftime('%d', created_time) AS label, COALESCE(SUM(final_price), 0) "
                "FROM [ORDER] WHERE created_time >= '" + WStringToUTF8(dateFrom) + "' AND created_time <= '" + WStringToUTF8(dateTo) + " 23:59:59' AND status = 1 "
                "GROUP BY label ORDER BY label";
        else
            sql = "SELECT strftime('%m', created_time) AS label, COALESCE(SUM(final_price), 0) "
                "FROM [ORDER] WHERE created_time >= '" + WStringToUTF8(dateFrom) + "' AND created_time <= '" + WStringToUTF8(dateTo) + " 23:59:59' AND status = 1 "
                "GROUP BY label ORDER BY label";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                std::wstring label;
                if (sqlite3_column_type(stmt, 0) != SQLITE_NULL)
                    label = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
                int64_t val = sqlite3_column_int64(stmt, 1);
                results.push_back({ label, val });
            }
            sqlite3_finalize(stmt);
        }
        return results;
    }

    std::vector<std::pair<std::wstring, int64_t>> Database::GetProfitReport(const std::wstring& period,
        const std::wstring& dateFrom, const std::wstring& dateTo)
    {
        std::vector<std::pair<std::wstring, int64_t>> results;
        std::string sql;

        if (period == L"day")
            sql = "SELECT strftime('%H', o.created_time) AS label, "
                "COALESCE(SUM(oi.total_price - (oi.quantity * p.import_price)), 0) "
                "FROM ORDER_ITEM oi JOIN PRODUCT p ON oi.product_id = p.product_id "
                "JOIN [ORDER] o ON oi.order_id = o.order_id "
                "WHERE o.created_time >= '" + WStringToUTF8(dateFrom) + "' AND o.created_time <= '" + WStringToUTF8(dateTo) + " 23:59:59' AND o.status = 1 "
                "GROUP BY label ORDER BY label";
        else if (period == L"week")
            sql = "SELECT strftime('%w', o.created_time) AS label, "
                "COALESCE(SUM(oi.total_price - (oi.quantity * p.import_price)), 0) "
                "FROM ORDER_ITEM oi JOIN PRODUCT p ON oi.product_id = p.product_id "
                "JOIN [ORDER] o ON oi.order_id = o.order_id "
                "WHERE o.created_time >= '" + WStringToUTF8(dateFrom) + "' AND o.created_time <= '" + WStringToUTF8(dateTo) + " 23:59:59' AND o.status = 1 "
                "GROUP BY label ORDER BY label";
        else if (period == L"month")
            sql = "SELECT strftime('%d', o.created_time) AS label, "
                "COALESCE(SUM(oi.total_price - (oi.quantity * p.import_price)), 0) "
                "FROM ORDER_ITEM oi JOIN PRODUCT p ON oi.product_id = p.product_id "
                "JOIN [ORDER] o ON oi.order_id = o.order_id "
                "WHERE o.created_time >= '" + WStringToUTF8(dateFrom) + "' AND o.created_time <= '" + WStringToUTF8(dateTo) + " 23:59:59' AND o.status = 1 "
                "GROUP BY label ORDER BY label";
        else
            sql = "SELECT strftime('%m', o.created_time) AS label, "
                "COALESCE(SUM(oi.total_price - (oi.quantity * p.import_price)), 0) "
                "FROM ORDER_ITEM oi JOIN PRODUCT p ON oi.product_id = p.product_id "
                "JOIN [ORDER] o ON oi.order_id = o.order_id "
                "WHERE o.created_time >= '" + WStringToUTF8(dateFrom) + "' AND o.created_time <= '" + WStringToUTF8(dateTo) + " 23:59:59' AND o.status = 1 "
                "GROUP BY label ORDER BY label";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                std::wstring label;
                if (sqlite3_column_type(stmt, 0) != SQLITE_NULL)
                    label = UTF8ToWString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
                int64_t val = sqlite3_column_int64(stmt, 1);
                results.push_back({ label, val });
            }
            sqlite3_finalize(stmt);
        }
        return results;
    }
}

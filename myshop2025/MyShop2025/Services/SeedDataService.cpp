#include "pch.h"
#include "SeedDataService.h"
#include "DatabaseService.h"
#include "Models/Category.h"
#include "Models/Product.h"
#include "Models/Order.h"
#include "Models/OrderItem.h"
#include <random>

namespace MyShop2025::Services
{
    bool SeedDataService::IsDatabaseSeeded()
    {
        auto dbSvc = DatabaseService::Instance();
        if (!dbSvc) return false;
        return dbSvc->GetTotalProducts() > 0;
    }

    bool SeedDataService::SeedDatabase()
    {
        auto dbSvc = DatabaseService::Instance();
        if (!dbSvc || !dbSvc->DB() || !dbSvc->DB()->IsOpen())
            return false;

        if (IsDatabaseSeeded())
            return true;

        // Categories
        std::vector<std::pair<std::wstring, std::wstring>> catData = {
            {L"Điện thoại thông minh", L"Các dòng điện thoại di động"},
            {L"Laptop & Máy tính", L"Máy tính xách tay và linh kiện"},
            {L"Phụ kiện công nghệ", L"Tai nghe, sạc, ốp lưng và phụ kiện"}
        };

        std::vector<int64_t> catIds;
        for (const auto& [name, desc] : catData)
        {
            Models::Category cat;
            cat.name = name;
            cat.description = desc;
            int64_t id = dbSvc->InsertCategory(cat);
            if (id > 0) catIds.push_back(id);
        }

        if (catIds.empty()) return false;

        // Products
        std::vector<std::vector<std::tuple<std::wstring, std::wstring, int64_t, int64_t, int, std::wstring>>> productsByCategory = {
            // Category 1: Điện thoại
            {
                {L"IP14PM-256", L"iPhone 14 Pro Max 256GB", 25000000, 29990000, 15, L"Màu tím deep purple"},
                {L"IP14P-128", L"iPhone 14 Pro 128GB", 22000000, 26990000, 12, L"Màu vàng gold"},
                {L"IP14-128", L"iPhone 14 128GB", 18000000, 21990000, 20, L"Màu xanh midnight"},
                {L"IP13-128", L"iPhone 13 128GB", 15000000, 17990000, 8, L"Màu đỏ product red"},
                {L"SS23U-512", L"Samsung Galaxy S23 Ultra 512GB", 22000000, 27990000, 10, L"Màu đen phantom black"},
                {L"SS23-256", L"Samsung Galaxy S23 256GB", 16000000, 19990000, 18, L"Màu kem cream"},
                {L"SSA54-128", L"Samsung Galaxy A54 128GB", 7000000, 8990000, 25, L"Màu xanh lime"},
                {L"OPF5-256", L"Oppo Find N5 Flip 256GB", 15000000, 18990000, 7, L"Màu tím thạch anh"},
                {L"OPR9-128", L"Oppo Reno9 128GB", 9000000, 11990000, 14, L"Màu hồng sunset"},
                {L"XM13-256", L"Xiaomi 13 Pro 256GB", 18000000, 22990000, 9, L"Màu trắng ceramic"},
                {L"XMRN11-128", L"Xiaomi Redmi Note 11 128GB", 5000000, 6490000, 30, L"Màu xám graphite"},
                {L"VIVX90-256", L"Vivo X90 Pro 256GB", 17000000, 21990000, 6, L"Màu đen legend"},
                {L"VIVV27-128", L"Vivo V27 128GB", 10000000, 12990000, 11, L"Màu xanh blue"},
                {L"R9-128", L"Realme 9 Pro 128GB", 6000000, 7990000, 22, L"Màu xanh aurora"},
                {L"RGT7-256", L"Realme GT Neo 7 256GB", 12000000, 15990000, 5, L"Màu vàng racing"},
                {L"NK300-128", L"Nokia G300 128GB", 4000000, 5490000, 16, L"Màu xanh dương ocean"},
                {L"SSA14-64", L"Samsung Galaxy A14 64GB", 3500000, 4490000, 28, L"Màu đen black"},
                {L"OPA78-128", L"Oppo A78 128GB", 5500000, 7290000, 13, L"Màu xanh mint"},
                {L"IPSE3-64", L"iPhone SE 3 64GB", 10000000, 12990000, 4, L"Màu trắng starlight"},
                {L"XM13L-256", L"Xiaomi 13 Lite 256GB", 11000000, 14990000, 17, L"Màu hồng lavender"},
                {L"SSFE23-256", L"Samsung Galaxy S23 FE 256GB", 13000000, 16990000, 8, L"Màu xanh mint"},
                {L"OPF21-128", L"Oppo Find X5 Lite 128GB", 8000000, 10990000, 19, L"Màu đen midnight"}
            },
            // Category 2: Laptop
            {
                {L"MBP14-M3", L"MacBook Pro 14 M3 Pro 18GB", 35000000, 45990000, 7, L"Space gray"},
                {L"MBA15-M3", L"MacBook Air 15 M3 16GB", 28000000, 35990000, 10, L"Midnight"},
                {L"MBA13-M2", L"MacBook Air 13 M2 8GB", 22000000, 28990000, 12, L"Starlight"},
                {L"DELLXPS15", L"Dell XPS 15 i7-13700H", 30000000, 38990000, 5, L"Bạc platinum"},
                {L"DELLLAT5420", L"Dell Latitude 5420 i5", 18000000, 23990000, 8, L"Đen graphite"},
                {L"LPG14-G7", L"Lenovo Legion G7 16IRX9", 35000000, 44990000, 4, L"Xám storm"},
                {L"LPTHINK14", L"Lenovo ThinkPad X1 Carbon Gen 11", 32000000, 41990000, 6, L"Đen classic"},
                {L"HPENVY16", L"HP Envy 16 i7-13700H", 26000000, 33990000, 9, L"Bạc natural"},
                {L"HPPAV15", L"HP Pavilion 15 i5-1340P", 16000000, 21990000, 14, L"Xám silver"},
                {L"ASUSROG16", L"Asus ROG Strix G16 i9", 35000000, 44990000, 3, L"Đen eclipse"},
                {L"ASUSVIV14", L"Asus Vivobook 14 OLED i5", 15000000, 19990000, 18, L"Xanh indigo"},
                {L"ACERNITRO16", L"Acer Nitro 16 i7-13620H", 25000000, 32990000, 5, L"Đen obsidian"},
                {L"ACERSWIFT5", L"Acer Swift 5 i7-1355U", 22000000, 28990000, 7, L"Xanh forest"},
                {L"MSIGE66", L"MSI GE66 Raider i9-13980HX", 40000000, 51990000, 2, L"Đen titan"},
                {L"MSIMOD14", L"MSI Modern 14 i5-13420H", 17000000, 22990000, 11, L"Bạc champagne"},
                {L"SURFACEL5", L"Microsoft Surface Laptop 5 i5", 25000000, 32990000, 6, L"Bạch kim"},
                {L"SURFACEPRO9", L"Microsoft Surface Pro 9 i7", 28000000, 36990000, 4, L"Đen graphite"},
                {L"RZBLADE15", L"Razer Blade 15 i7-13800H", 38000000, 49990000, 3, L"Đen mercury"},
                {L"HUAPRO16", L"Huawei MateBook 16s i7", 24000000, 31990000, 8, L"Bạc space"},
                {L"GIGA3070", L"Gigabyte Aorus 15X i9-13900H", 35000000, 45990000, 3, L"Đen matte"},
                {L"SAMSUNGBOOK3", L"Samsung Galaxy Book3 Ultra i7", 30000000, 39990000, 5, L"Xám graphite"},
                {L"FRAMEWORK13", L"Framework Laptop 13 i5-1340P", 20000000, 26990000, 4, L"Bạc aluminum"}
            },
            // Category 3: Phụ kiện
            {
                {L"AIRPODPRO2", L"AirPods Pro 2 USB-C", 4500000, 6490000, 25, L"Trắng"},
                {L"AIRPODS3", L"AirPods 3 Lightning", 3500000, 4990000, 20, L"Trắng"},
                {L"GALBUDS2", L"Samsung Galaxy Buds2 Pro", 3000000, 4490000, 18, L"Tím lavender"},
                {L"SONYWF1000", L"Sony WF-1000XM5", 5500000, 7990000, 10, L"Đen"},
                {L"SONYWH1000", L"Sony WH-1000XM5", 6500000, 9490000, 8, L"Bạc"},
                {L"ANKER20W", L"Anker PowerPort III 20W", 200000, 350000, 50, L"Trắng"},
                {L"ANKER65W", L"Anker PowerPort Atom III 65W", 500000, 799000, 35, L"Trắng"},
                {L"BASEUS30W", L"Baseus GaN3 Pro 30W", 350000, 549000, 40, L"Đen"},
                {L"BASEUS65W", L"Baseus GaN5 Pro 65W", 600000, 949000, 30, L"Xám"},
                {L"SPIGENIP14", L"Spigen Ultra Hybrid iPhone 14", 150000, 249000, 45, L"Trong suốt"},
                {L"SPIGENIP13", L"Spigen Liquid Air iPhone 13", 120000, 199000, 55, L"Đen matte"},
                {L"RINGKESS23", L"Ringke Fusion Samsung S23", 130000, 219000, 38, L"Trong suốt"},
                {L"MOUSLIMIT", L"Mous Limitless 5.0 iPhone", 800000, 1299000, 12, L"Carbon fiber"},
                {L"JBLTUNE720", L"JBL Tune 720BT", 1200000, 1799000, 15, L"Đen"},
                {L"JBLCHARGE5", L"JBL Charge 5", 2500000, 3699000, 10, L"Xanh dương"},
                {L"MARSHALL2", L"Marshall II Bluetooth", 2200000, 3299000, 7, L"Đen vintage"},
                {L"LOGI270S", L"Logitech G Pro X Superlight 2", 3000000, 4499000, 9, L"Trắng"},
                {L"LOGI915", L"Logitech G915 TKL", 3500000, 5199000, 6, L"Đen carbon"},
                {L"RAZERV2PRO", L"Razer Viper V2 Pro", 2500000, 3799000, 8, L"Trắng"},
                {L"LAMY2000", L"Bút cảm ứng Lamy Safari", 500000, 799000, 20, L"Xanh"},
                {L"ADAMTAB", L"Mi Tab 5 10.1 inch", 5000000, 6990000, 8, L"Xám"},
                {L"BALMUDA", L"Đèn bàn Balmuda The Light", 3500000, 5199000, 5, L"Trắng"}
            }
        };

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> priceOffset(-500000, 500000);
        std::uniform_int_distribution<> stockDist(2, 50);

        int64_t productIndex = 0;
        for (size_t catIdx = 0; catIdx < productsByCategory.size(); catIdx++)
        {
            for (const auto& [sku, name, importPrice, salePrice, count, desc] : productsByCategory[catIdx])
            {
                Models::Product prod;
                prod.sku = sku;
                prod.name = name;
                prod.importPrice = importPrice;
                prod.salePrice = salePrice;
                prod.count = count;
                prod.description = desc;
                prod.categoryId = catIds[catIdx];

                // Add 3 sample images per product
                for (int imgIdx = 1; imgIdx <= 3; imgIdx++)
                {
                    prod.images.push_back(L"Assets/Products/" + sku + L"_" + std::to_wstring(imgIdx) + L".jpg");
                }

                dbSvc->InsertProduct(prod);
                productIndex++;
            }
        }

        // Create sample orders
        auto now = std::chrono::system_clock::now();

        for (int orderIdx = 0; orderIdx < 10; orderIdx++)
        {
            Models::Order order;
            auto orderTime = now - std::chrono::hours(24 * orderIdx) - std::chrono::hours(std::uniform_int_distribution<>(0, 12)(gen));
            order.createdTime = orderTime;
            order.status = static_cast<Models::OrderStatus>(std::uniform_int_distribution<>(0, 1)(gen));

            int64_t total = 0;
            int numItems = std::uniform_int_distribution<>(1, 5)(gen);
            for (int itemIdx = 0; itemIdx < numItems; itemIdx++)
            {
                int64_t prodId = (int64_t)std::uniform_int_distribution<>(1, (int)productIndex)(gen);
                auto product = dbSvc->GetProduct(prodId);
                if (product.productId == 0) continue;

                int qty = std::uniform_int_distribution<>(1, 3)(gen);
                Models::OrderItem item;
                item.productId = prodId;
                item.quantity = qty;
                item.unitSalePrice = static_cast<double>(product.salePrice);
                item.totalPrice = product.salePrice * qty;
                total += item.totalPrice;
                order.items.push_back(item);
            }
            order.finalPrice = total;
            dbSvc->InsertOrder(order);
        }

        return true;
    }
}

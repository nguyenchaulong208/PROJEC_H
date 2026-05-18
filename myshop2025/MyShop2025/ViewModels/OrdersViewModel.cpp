#include "pch.h"
#include "OrdersViewModel.h"
#include "Services/DatabaseService.h"

namespace winrt::MyShop2025::ViewModels
{
    OrdersViewModel::OrdersViewModel()
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (dbSvc)
        {
            auto settings = dbSvc->GetSettings();
            m_pageSize = settings.pageSize;
        }
        LoadOrders();
        LoadAvailableProducts();
    }

    void OrdersViewModel::LoadOrders()
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (!dbSvc) return;

        auto total = dbSvc->GetOrderCount(m_dateFrom, m_dateTo);
        TotalOrders(total);
        int pages = (int)(total / m_pageSize);
        if (total % m_pageSize > 0) pages++;
        TotalPages(pages > 0 ? pages : 1);

        if (m_currentPage >= m_totalPages)
            m_currentPage = std::max(0, m_totalPages - 1);

        Orders(dbSvc->GetOrders(m_currentPage, m_pageSize, m_dateFrom, m_dateTo));
    }

    void OrdersViewModel::LoadAvailableProducts()
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (dbSvc)
        {
            AvailableProducts(dbSvc->GetProducts(-1, 0, 9999));
        }
    }

    void OrdersViewModel::SearchByDate()
    {
        m_currentPage = 0;
        RaisePropertyChanged(L"CurrentPage");
        LoadOrders();
    }

    void OrdersViewModel::NextPage()
    {
        if (HasNextPage())
            CurrentPage(m_currentPage + 1);
    }

    void OrdersViewModel::PreviousPage()
    {
        if (HasPreviousPage())
            CurrentPage(m_currentPage - 1);
    }

    void OrdersViewModel::StartCreateOrder()
    {
        m_cartItems.clear();
        CartTotal(0);
        IsCreating(true);
        LoadAvailableProducts();
    }

    void OrdersViewModel::CancelCreateOrder()
    {
        m_cartItems.clear();
        CartTotal(0);
        IsCreating(false);
    }

    void OrdersViewModel::AddToCart(int64_t productId)
    {
        auto it = std::find_if(m_cartItems.begin(), m_cartItems.end(),
            [productId](const auto& pair) { return pair.first.productId == productId; });

        if (it != m_cartItems.end())
        {
            it->second++;
        }
        else
        {
            auto dbSvc = Services::DatabaseService::Instance();
            if (dbSvc)
            {
                auto product = dbSvc->GetProduct(productId);
                m_cartItems.push_back({ product, 1 });
            }
        }
        CartItems(m_cartItems);
    }

    void OrdersViewModel::RemoveFromCart(int index)
    {
        if (index >= 0 && index < (int)m_cartItems.size())
        {
            m_cartItems.erase(m_cartItems.begin() + index);
            CartItems(m_cartItems);
        }
    }

    void OrdersViewModel::UpdateQuantity(int index, int quantity)
    {
        if (index >= 0 && index < (int)m_cartItems.size() && quantity > 0)
        {
            m_cartItems[index].second = quantity;
            CartItems(m_cartItems);
        }
    }

    void OrdersViewModel::SaveOrder()
    {
        if (m_cartItems.empty()) return;

        auto dbSvc = Services::DatabaseService::Instance();
        if (!dbSvc) return;

        Models::Order order;
        order.createdTime = std::chrono::system_clock::now();
        order.status = Models::OrderStatus::New;

        int64_t totalPrice = 0;
        for (const auto& [product, quantity] : m_cartItems)
        {
            Models::OrderItem item;
            item.productId = product.productId;
            item.quantity = quantity;
            item.unitSalePrice = static_cast<double>(product.salePrice);
            item.totalPrice = product.salePrice * quantity;
            totalPrice += item.totalPrice;
            order.items.push_back(item);
        }
        order.finalPrice = totalPrice;

        dbSvc->InsertOrder(order);

        m_cartItems.clear();
        CartTotal(0);
        IsCreating(false);
        LoadOrders();
    }

    void OrdersViewModel::ViewOrderDetail(int64_t orderId)
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (dbSvc)
        {
            SelectedOrder(dbSvc->GetOrder(orderId));
            IsDetailView(true);
        }
    }

    void OrdersViewModel::UpdateOrderStatus(int64_t orderId, Models::OrderStatus status)
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (dbSvc)
        {
            dbSvc->UpdateOrderStatus(orderId, status);
            LoadOrders();
        }
    }

    void OrdersViewModel::DeleteOrder(int64_t orderId)
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (dbSvc)
        {
            dbSvc->DeleteOrder(orderId);
            IsDetailView(false);
            LoadOrders();
        }
    }

    void OrdersViewModel::UpdateCartTotal()
    {
        int64_t total = 0;
        for (const auto& [product, quantity] : m_cartItems)
        {
            total += product.salePrice * quantity;
        }
        CartTotal(total);
    }
}

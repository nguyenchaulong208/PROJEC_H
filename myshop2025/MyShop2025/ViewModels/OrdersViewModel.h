#pragma once
#include "ViewModelBase.h"
#include "Models/Order.h"
#include "Models/Product.h"

namespace winrt::MyShop2025::ViewModels
{
    class OrdersViewModel : public ViewModelBase
    {
    private:
        std::vector<Models::Order> m_orders;
        Models::Order m_selectedOrder;
        std::vector<Models::Product> m_availableProducts;
        std::vector<std::pair<Models::Product, int>> m_cartItems;
        std::wstring m_dateFrom;
        std::wstring m_dateTo;
        int m_currentPage = 0;
        int m_pageSize = 10;
        int m_totalPages = 0;
        int64_t m_totalOrders = 0;
        bool m_isCreating = false;
        bool m_isDetailView = false;
        int64_t m_cartTotal = 0;

    public:
        OrdersViewModel();

        std::vector<Models::Order> Orders() const { return m_orders; }
        void Orders(const std::vector<Models::Order>& value)
        {
            m_orders = value;
            RaisePropertyChanged(L"Orders");
        }

        Models::Order SelectedOrder() const { return m_selectedOrder; }
        void SelectedOrder(const Models::Order& value)
        {
            m_selectedOrder = value;
            RaisePropertyChanged(L"SelectedOrder");
        }

        std::vector<Models::Product> AvailableProducts() const { return m_availableProducts; }
        void AvailableProducts(const std::vector<Models::Product>& value)
        {
            m_availableProducts = value;
            RaisePropertyChanged(L"AvailableProducts");
        }

        std::vector<std::pair<Models::Product, int>> CartItems() const { return m_cartItems; }
        void CartItems(const std::vector<std::pair<Models::Product, int>>& value)
        {
            m_cartItems = value;
            RaisePropertyChanged(L"CartItems");
            UpdateCartTotal();
        }

        std::wstring DateFrom() const { return m_dateFrom; }
        void DateFrom(const std::wstring& value)
        {
            m_dateFrom = value;
            RaisePropertyChanged(L"DateFrom");
        }

        std::wstring DateTo() const { return m_dateTo; }
        void DateTo(const std::wstring& value)
        {
            m_dateTo = value;
            RaisePropertyChanged(L"DateTo");
        }

        int CurrentPage() const { return m_currentPage; }
        void CurrentPage(int value)
        {
            m_currentPage = value;
            RaisePropertyChanged(L"CurrentPage");
            LoadOrders();
        }

        int TotalPages() const { return m_totalPages; }
        void TotalPages(int value)
        {
            m_totalPages = value;
            RaisePropertyChanged(L"TotalPages");
            RaisePropertyChanged(L"HasPreviousPage");
            RaisePropertyChanged(L"HasNextPage");
        }

        int64_t TotalOrders() const { return m_totalOrders; }
        void TotalOrders(int64_t value)
        {
            m_totalOrders = value;
            RaisePropertyChanged(L"TotalOrders");
        }

        bool IsCreating() const { return m_isCreating; }
        void IsCreating(bool value)
        {
            m_isCreating = value;
            RaisePropertyChanged(L"IsCreating");
        }

        bool IsDetailView() const { return m_isDetailView; }
        void IsDetailView(bool value)
        {
            m_isDetailView = value;
            RaisePropertyChanged(L"IsDetailView");
        }

        int64_t CartTotal() const { return m_cartTotal; }
        void CartTotal(int64_t value)
        {
            m_cartTotal = value;
            RaisePropertyChanged(L"CartTotal");
        }

        bool HasPreviousPage() const { return m_currentPage > 0; }
        bool HasNextPage() const { return m_currentPage < m_totalPages - 1; }

        void LoadOrders();
        void LoadAvailableProducts();
        void SearchByDate();
        void NextPage();
        void PreviousPage();
        void StartCreateOrder();
        void CancelCreateOrder();
        void AddToCart(int64_t productId);
        void RemoveFromCart(int index);
        void UpdateQuantity(int index, int quantity);
        void SaveOrder();
        void ViewOrderDetail(int64_t orderId);
        void UpdateOrderStatus(int64_t orderId, Models::OrderStatus status);
        void DeleteOrder(int64_t orderId);

    private:
        void UpdateCartTotal();
    };
}

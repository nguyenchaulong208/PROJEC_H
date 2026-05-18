#pragma once
#include "OrdersPage.g.h"
#include "ViewModels/OrdersViewModel.h"
#include <memory>

namespace winrt::MyShop2025::Views::implementation
{
    struct OrdersPage : OrdersPageT<OrdersPage>
    {
        OrdersPage();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void SearchButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void CreateOrderButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void OrderList_SelectionChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void DetailButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void DeleteOrderButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void PrevButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NextButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        std::shared_ptr<ViewModels::OrdersViewModel> m_viewModel;
        void RefreshDisplay();
        void ShowCreateOrderDialog();
        void ShowOrderDetailDialog(const Models::Order& order);
    };
}

namespace winrt::MyShop2025::Views::factory_implementation
{
    struct OrdersPage : OrdersPageT<OrdersPage, implementation::OrdersPage>
    {
    };
}

#pragma once
#include "ProductsPage.g.h"
#include "ViewModels/ProductsViewModel.h"
#include <memory>

namespace winrt::MyShop2025::Views::implementation
{
    struct ProductsPage : ProductsPageT<ProductsPage>
    {
        ProductsPage();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void CategoryCombo_SelectionChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void SearchBox_TextChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);
        void FilterButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void AddProductButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void AddCategoryButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ImportButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ProductList_SelectionChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void EditProductButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void DeleteProductButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void PrevButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NextButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        std::shared_ptr<ViewModels::ProductsViewModel> m_viewModel;
        void RefreshDisplay();
        void ShowProductDialog(const Models::Product& product);
        void ShowCategoryDialog();
    };
}

namespace winrt::MyShop2025::Views::factory_implementation
{
    struct ProductsPage : ProductsPageT<ProductsPage, implementation::ProductsPage>
    {
    };
}

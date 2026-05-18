#pragma once
#include "ViewModelBase.h"
#include "Models/Product.h"
#include "Models/Category.h"

namespace winrt::MyShop2025::ViewModels
{
    class ProductsViewModel : public ViewModelBase
    {
    private:
        std::vector<Models::Category> m_categories;
        std::vector<Models::Product> m_products;
        Models::Product m_selectedProduct;
        int64_t m_selectedCategoryId = -1;
        std::wstring m_searchText;
        int64_t m_minPrice = 0;
        int64_t m_maxPrice = 999999999;
        std::wstring m_sortBy = L"name";
        bool m_sortAsc = true;
        int m_currentPage = 0;
        int m_pageSize = 10;
        int m_totalPages = 0;
        int64_t m_totalProducts = 0;
        bool m_isEditing = false;
        bool m_isAddingCategory = false;
        Models::Category m_editingCategory;

    public:
        ProductsViewModel();

        std::vector<Models::Category> Categories() const { return m_categories; }
        void Categories(const std::vector<Models::Category>& value)
        {
            m_categories = value;
            RaisePropertyChanged(L"Categories");
        }

        std::vector<Models::Product> Products() const { return m_products; }
        void Products(const std::vector<Models::Product>& value)
        {
            m_products = value;
            RaisePropertyChanged(L"Products");
        }

        Models::Product SelectedProduct() const { return m_selectedProduct; }
        void SelectedProduct(const Models::Product& value)
        {
            m_selectedProduct = value;
            RaisePropertyChanged(L"SelectedProduct");
        }

        int64_t SelectedCategoryId() const { return m_selectedCategoryId; }
        void SelectedCategoryId(int64_t value)
        {
            if (m_selectedCategoryId != value)
            {
                m_selectedCategoryId = value;
                RaisePropertyChanged(L"SelectedCategoryId");
                LoadProducts();
            }
        }

        std::wstring SearchText() const { return m_searchText; }
        void SearchText(const std::wstring& value)
        {
            if (m_searchText != value)
            {
                m_searchText = value;
                RaisePropertyChanged(L"SearchText");
            }
        }

        int64_t MinPrice() const { return m_minPrice; }
        void MinPrice(int64_t value)
        {
            m_minPrice = value;
            RaisePropertyChanged(L"MinPrice");
        }

        int64_t MaxPrice() const { return m_maxPrice; }
        void MaxPrice(int64_t value)
        {
            m_maxPrice = value;
            RaisePropertyChanged(L"MaxPrice");
        }

        std::wstring SortBy() const { return m_sortBy; }
        void SortBy(const std::wstring& value)
        {
            m_sortBy = value;
            RaisePropertyChanged(L"SortBy");
        }

        bool SortAsc() const { return m_sortAsc; }
        void SortAsc(bool value)
        {
            m_sortAsc = value;
            RaisePropertyChanged(L"SortAsc");
        }

        int CurrentPage() const { return m_currentPage; }
        void CurrentPage(int value)
        {
            if (m_currentPage != value)
            {
                m_currentPage = value;
                RaisePropertyChanged(L"CurrentPage");
                LoadProducts();
            }
        }

        int PageSize() const { return m_pageSize; }
        void PageSize(int value)
        {
            if (m_pageSize != value)
            {
                m_pageSize = value;
                RaisePropertyChanged(L"PageSize");
                m_currentPage = 0;
                RaisePropertyChanged(L"CurrentPage");
                LoadProducts();
            }
        }

        int TotalPages() const { return m_totalPages; }
        void TotalPages(int value)
        {
            m_totalPages = value;
            RaisePropertyChanged(L"TotalPages");
            RaisePropertyChanged(L"HasPreviousPage");
            RaisePropertyChanged(L"HasNextPage");
        }

        int64_t TotalProducts() const { return m_totalProducts; }
        void TotalProducts(int64_t value)
        {
            m_totalProducts = value;
            RaisePropertyChanged(L"TotalProducts");
        }

        bool IsEditing() const { return m_isEditing; }
        void IsEditing(bool value)
        {
            m_isEditing = value;
            RaisePropertyChanged(L"IsEditing");
        }

        bool IsAddingCategory() const { return m_isAddingCategory; }
        void IsAddingCategory(bool value)
        {
            m_isAddingCategory = value;
            RaisePropertyChanged(L"IsAddingCategory");
        }

        Models::Category EditingCategory() const { return m_editingCategory; }
        void EditingCategory(const Models::Category& value)
        {
            m_editingCategory = value;
            RaisePropertyChanged(L"EditingCategory");
        }

        bool HasPreviousPage() const { return m_currentPage > 0; }
        bool HasNextPage() const { return m_currentPage < m_totalPages - 1; }

        void LoadCategories();
        void LoadProducts();
        void Search();
        void Sort(const std::wstring& column);
        void FilterByPrice();
        void NextPage();
        void PreviousPage();

        void DeleteProduct(int64_t productId);
        void SaveProduct(const Models::Product& product);
        void AddCategory(const std::wstring& name, const std::wstring& description);
        void DeleteCategory(int64_t categoryId);

        std::wstring FormatMoney(int64_t value);
    };
}

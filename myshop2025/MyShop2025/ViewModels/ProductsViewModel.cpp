#include "pch.h"
#include "ProductsViewModel.h"
#include "Services/DatabaseService.h"

namespace winrt::MyShop2025::ViewModels
{
    ProductsViewModel::ProductsViewModel()
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (dbSvc)
        {
            auto settings = dbSvc->GetSettings();
            m_pageSize = settings.pageSize;
        }
        LoadCategories();
        LoadProducts();
    }

    void ProductsViewModel::LoadCategories()
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (dbSvc)
        {
            Categories(dbSvc->GetCategories());
        }
    }

    void ProductsViewModel::LoadProducts()
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (!dbSvc) return;

        auto totalCount = dbSvc->GetProductCount(
            m_selectedCategoryId > 0 ? m_selectedCategoryId : -1,
            m_searchText, m_minPrice, m_maxPrice);

        TotalProducts(totalCount);
        int pages = (int)(totalCount / m_pageSize);
        if (totalCount % m_pageSize > 0) pages++;
        TotalPages(pages > 0 ? pages : 1);
        if (m_currentPage >= m_totalPages) m_currentPage = m_totalPages - 1;

        auto products = dbSvc->GetProducts(
            m_selectedCategoryId > 0 ? m_selectedCategoryId : -1,
            m_currentPage, m_pageSize,
            m_sortBy, m_sortAsc,
            m_searchText, m_minPrice, m_maxPrice);

        Products(products);
    }

    void ProductsViewModel::Search()
    {
        m_currentPage = 0;
        RaisePropertyChanged(L"CurrentPage");
        LoadProducts();
    }

    void ProductsViewModel::Sort(const std::wstring& column)
    {
        if (m_sortBy == column)
            m_sortAsc = !m_sortAsc;
        else
        {
            m_sortBy = column;
            m_sortAsc = true;
        }
        LoadProducts();
    }

    void ProductsViewModel::FilterByPrice()
    {
        m_currentPage = 0;
        RaisePropertyChanged(L"CurrentPage");
        LoadProducts();
    }

    void ProductsViewModel::NextPage()
    {
        if (HasNextPage())
            CurrentPage(m_currentPage + 1);
    }

    void ProductsViewModel::PreviousPage()
    {
        if (HasPreviousPage())
            CurrentPage(m_currentPage - 1);
    }

    void ProductsViewModel::DeleteProduct(int64_t productId)
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (dbSvc)
        {
            dbSvc->DeleteProduct(productId);
            LoadProducts();
        }
    }

    void ProductsViewModel::SaveProduct(const Models::Product& product)
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (dbSvc)
        {
            if (product.productId > 0)
                dbSvc->UpdateProduct(product);
            else
                dbSvc->InsertProduct(product);
            LoadProducts();
        }
    }

    void ProductsViewModel::AddCategory(const std::wstring& name, const std::wstring& description)
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (dbSvc)
        {
            Models::Category cat;
            cat.name = name;
            cat.description = description;
            dbSvc->InsertCategory(cat);
            LoadCategories();
        }
    }

    void ProductsViewModel::DeleteCategory(int64_t categoryId)
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (dbSvc)
        {
            dbSvc->DeleteCategory(categoryId);
            LoadCategories();
        }
    }

    std::wstring ProductsViewModel::FormatMoney(int64_t value)
    {
        std::wstringstream wss;
        wss << value << L" ₫";
        return wss.str();
    }
}

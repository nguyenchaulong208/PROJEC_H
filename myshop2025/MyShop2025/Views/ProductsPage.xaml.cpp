#include "pch.h"
#include "ProductsPage.xaml.h"
#include "Services/DatabaseService.h"

using namespace Microsoft::UI::Xaml;

namespace winrt::MyShop2025::Views::implementation
{
    ProductsPage::ProductsPage()
    {
        InitializeComponent();
        m_viewModel = std::make_shared<ViewModels::ProductsViewModel>();
    }

    void ProductsPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&)
    {
        m_viewModel->LoadCategories();
        m_viewModel->LoadProducts();
        RefreshDisplay();
    }

    void ProductsPage::RefreshDisplay()
    {
        CategoryCombo().ItemsSource(winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>());
        auto catItems = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();

        Models::Category allCat;
        allCat.categoryId = -1;
        allCat.name = L"Tất cả";
        catItems.Append(winrt::box_value(allCat));

        for (const auto& cat : m_viewModel->Categories())
        {
            catItems.Append(winrt::box_value(cat));
        }
        CategoryCombo().ItemsSource(catItems);
        CategoryCombo().SelectedIndex(0);

        auto prodItems = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();
        for (const auto& prod : m_viewModel->Products())
        {
            prodItems.Append(winrt::box_value(prod));
        }
        ProductList().ItemsSource(prodItems);

        TotalText().Text((L"Tổng: " + std::to_wstring(m_viewModel->TotalProducts()) + L" sản phẩm").c_str());
        PageText().Text((std::to_wstring(m_viewModel->CurrentPage() + 1) + L" / " + std::to_wstring(m_viewModel->TotalPages())).c_str());
    }

    void ProductsPage::CategoryCombo_SelectionChanged(Windows::Foundation::IInspectable const&,
        Controls::SelectionChangedEventArgs const&)
    {
        if (CategoryCombo().SelectedItem())
        {
            auto cat = winrt::unbox_value<Models::Category>(CategoryCombo().SelectedItem());
            m_viewModel->SelectedCategoryId(cat.categoryId);
            RefreshDisplay();
        }
    }

    void ProductsPage::SearchBox_TextChanged(Windows::Foundation::IInspectable const&,
        Controls::TextChangedEventArgs const&)
    {
        m_viewModel->SearchText(SearchBox().Text().c_str());
    }

    void ProductsPage::FilterButton_Click(Windows::Foundation::IInspectable const&,
        RoutedEventArgs const&)
    {
        if (!MinPriceBox().Text().empty())
            m_viewModel->MinPrice(std::stoll(MinPriceBox().Text().c_str()));
        if (!MaxPriceBox().Text().empty())
            m_viewModel->MaxPrice(std::stoll(MaxPriceBox().Text().c_str()));
        m_viewModel->FilterByPrice();
        RefreshDisplay();
    }

    void ProductsPage::AddProductButton_Click(Windows::Foundation::IInspectable const&,
        RoutedEventArgs const&)
    {
        Models::Product emptyProd;
        ShowProductDialog(emptyProd);
    }

    void ProductsPage::AddCategoryButton_Click(Windows::Foundation::IInspectable const&,
        RoutedEventArgs const&)
    {
        ShowCategoryDialog();
    }

    void ProductsPage::ImportButton_Click(Windows::Foundation::IInspectable const&,
        RoutedEventArgs const&)
    {
        auto dialog = Controls::ContentDialog();
        dialog.Title(winrt::box_value(L"Nhập từ Excel"));
        dialog.Content(winrt::box_value(L"Vui lòng đặt file Excel (.xlsx, .csv) trong thư mục Import.\n\n"
            L"Định dạng: SKU, Tên, Giá nhập, Giá bán, Số lượng, Mô tả, ID loại"));
        dialog.PrimaryButtonText(L"Đóng");
        dialog.ShowAsync();
    }

    void ProductsPage::ProductList_SelectionChanged(Windows::Foundation::IInspectable const&,
        Controls::SelectionChangedEventArgs const&)
    {
    }

    void ProductsPage::EditProductButton_Click(Windows::Foundation::IInspectable const& sender,
        RoutedEventArgs const&)
    {
        auto button = sender.as<Controls::Button>();
        auto product = winrt::unbox_value<Models::Product>(button.Tag());
        ShowProductDialog(product);
    }

    void ProductsPage::DeleteProductButton_Click(Windows::Foundation::IInspectable const& sender,
        RoutedEventArgs const&)
    {
        auto button = sender.as<Controls::Button>();
        auto product = winrt::unbox_value<Models::Product>(button.Tag());
        m_viewModel->DeleteProduct(product.productId);
        RefreshDisplay();
    }

    void ProductsPage::ShowProductDialog(const Models::Product& product)
    {
        auto dialog = Controls::ContentDialog();
        dialog.Title(winrt::box_value(product.productId > 0 ? L"Sửa sản phẩm" : L"Thêm sản phẩm"));

        auto panel = StackPanel();
        panel.Spacing(8);

        auto skuBox = TextBox();
        skuBox.Header(winrt::box_value(L"SKU"));
        skuBox.Text(product.sku.c_str());

        auto nameBox = TextBox();
        nameBox.Header(winrt::box_value(L"Tên sản phẩm"));
        nameBox.Text(product.name.c_str());

        auto importBox = TextBox();
        importBox.Header(winrt::box_value(L"Giá nhập"));
        importBox.Text(product.importPrice > 0 ? std::to_wstring(product.importPrice).c_str() : L"");

        auto saleBox = TextBox();
        saleBox.Header(winrt::box_value(L"Giá bán"));
        saleBox.Text(product.salePrice > 0 ? std::to_wstring(product.salePrice).c_str() : L"");

        auto countBox = TextBox();
        countBox.Header(winrt::box_value(L"Số lượng"));
        countBox.Text(product.count > 0 ? std::to_wstring(product.count).c_str() : L"");

        auto catCombo = Controls::ComboBox();
        catCombo.Header(winrt::box_value(L"Loại sản phẩm"));
        auto catItems = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();
        int selectedIdx = 0;
        for (size_t i = 0; i < m_viewModel->Categories().size(); i++)
        {
            catItems.Append(winrt::box_value(m_viewModel->Categories()[i].name));
            if (m_viewModel->Categories()[i].categoryId == product.categoryId)
                selectedIdx = (int)i;
        }
        catCombo.ItemsSource(catItems);
        catCombo.SelectedIndex(selectedIdx);

        auto descBox = TextBox();
        descBox.Header(winrt::box_value(L"Mô tả"));
        descBox.Text(product.description.c_str());
        descBox.AcceptsReturn(true);
        descBox.MinHeight(60);

        panel.Children().Append(skuBox);
        panel.Children().Append(nameBox);
        panel.Children().Append(importBox);
        panel.Children().Append(saleBox);
        panel.Children().Append(countBox);
        panel.Children().Append(catCombo);
        panel.Children().Append(descBox);

        dialog.Content(panel);
        dialog.PrimaryButtonText(L"Lưu");
        dialog.CloseButtonText(L"Hủy");
        dialog.PrimaryButtonClick([this, product, &skuBox, &nameBox, &importBox, &saleBox, &countBox, &catCombo, &descBox](
            Controls::ContentDialog const&, Controls::ContentDialogButtonClickEventArgs const& args)
        {
            auto prod = product;
            prod.sku = skuBox.Text().c_str();
            prod.name = nameBox.Text().c_str();
            try { prod.importPrice = std::stoll(importBox.Text().c_str()); } catch (...) {}
            try { prod.salePrice = std::stoll(saleBox.Text().c_str()); } catch (...) {}
            try { prod.count = std::stoi(countBox.Text().c_str()); } catch (...) {}
            if (catCombo.SelectedIndex() >= 0 && catCombo.SelectedIndex() < (int)m_viewModel->Categories().size())
                prod.categoryId = m_viewModel->Categories()[catCombo.SelectedIndex()].categoryId;
            prod.description = descBox.Text().c_str();

            m_viewModel->SaveProduct(prod);
            RefreshDisplay();
        });
        dialog.ShowAsync();
    }

    void ProductsPage::ShowCategoryDialog()
    {
        auto dialog = Controls::ContentDialog();
        dialog.Title(winrt::box_value(L"Thêm loại sản phẩm"));

        auto panel = StackPanel();
        panel.Spacing(8);

        auto nameBox = TextBox();
        nameBox.Header(winrt::box_value(L"Tên loại"));

        auto descBox = TextBox();
        descBox.Header(winrt::box_value(L"Mô tả"));

        panel.Children().Append(nameBox);
        panel.Children().Append(descBox);

        dialog.Content(panel);
        dialog.PrimaryButtonText(L"Thêm");
        dialog.CloseButtonText(L"Hủy");
        dialog.PrimaryButtonClick([this, &nameBox, &descBox](
            Controls::ContentDialog const&, Controls::ContentDialogButtonClickEventArgs const&)
        {
            m_viewModel->AddCategory(nameBox.Text().c_str(), descBox.Text().c_str());
            RefreshDisplay();
        });
        dialog.ShowAsync();
    }

    void ProductsPage::PrevButton_Click(Windows::Foundation::IInspectable const&,
        RoutedEventArgs const&)
    {
        m_viewModel->PreviousPage();
        RefreshDisplay();
    }

    void ProductsPage::NextButton_Click(Windows::Foundation::IInspectable const&,
        RoutedEventArgs const&)
    {
        m_viewModel->NextPage();
        RefreshDisplay();
    }
}

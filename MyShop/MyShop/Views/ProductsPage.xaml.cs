using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using MyShop.Models;
using MyShop.ViewModels;

namespace MyShop.Views;

public sealed partial class ProductsPage : Page
{
    private readonly ProductsViewModel _vm;

    public ProductsPage()
    {
        InitializeComponent();
        _vm = new ProductsViewModel(App.ProductService!);
        SortCombo.ItemsSource = _vm.SortOptions;
        SortCombo.SelectedIndex = 0;
        Loaded += OnLoaded;
    }

    private async void OnLoaded(object sender, RoutedEventArgs e)
    {
        try
        {
            await _vm.LoadCategoriesAsync();
            CategoryCombo.ItemsSource = _vm.Categories;
            CategoryCombo.SelectedIndex = 0;
            await _vm.LoadProductsAsync();
            UpdateUI();
        }
        catch { }
    }

    private void UpdateUI()
    {
        ProductList.ItemsSource = _vm.Products;
        PageInfoText.Text = _vm.PageInfo;
    }

    private async void OnCategoryChanged(object sender, SelectionChangedEventArgs e)
    {
        _vm.SelectedCategory = CategoryCombo.SelectedItem as Category;
        _vm.CurrentPage = 1;
        await _vm.LoadProductsAsync();
        UpdateUI();
    }

    private async void OnSearch(object sender, AutoSuggestBoxQuerySubmittedEventArgs e)
    {
        _vm.SearchKeyword = e.QueryText;
        _vm.CurrentPage = 1;
        await _vm.LoadProductsAsync();
        UpdateUI();
    }

    private async void OnSearchClick(object sender, RoutedEventArgs e)
    {
        _vm.SearchKeyword = SearchBox.Text;
        _vm.MinPrice = (int)(MinPriceBox.Value);
        _vm.MaxPrice = (int)(MaxPriceBox.Value > 0 ? MaxPriceBox.Value : 1000000);
        _vm.CurrentPage = 1;
        await _vm.LoadProductsAsync();
        UpdateUI();
    }

    private async void OnSortChanged(object sender, SelectionChangedEventArgs e)
    {
        _vm.SelectedSort = SortCombo.SelectedItem as string ?? "Mới nhất";
        _vm.CurrentPage = 1;
        await _vm.LoadProductsAsync();
        UpdateUI();
    }

    private void OnAddProduct(object sender, RoutedEventArgs e)
    {
        _vm.OpenAddDialog();
        ShowProductDialog();
    }

    private void OnEditProduct(object sender, RoutedEventArgs e)
    {
        if ((sender as Button)?.Tag is int id)
        {
            var product = _vm.Products.FirstOrDefault(p => p.ProductId == id);
            if (product != null)
            {
                _vm.OpenEditDialog(product);
                ShowProductDialog();
            }
        }
    }

    private async void OnDeleteProduct(object sender, RoutedEventArgs e)
    {
        if ((sender as Button)?.Tag is int id)
        {
            var product = _vm.Products.FirstOrDefault(p => p.ProductId == id);
            if (product != null)
            {
                var dialog = new ContentDialog
                {
                    Title = "Xác nhận xóa",
                    Content = $"Bạn có chắc muốn xóa sản phẩm '{product.Name}'?",
                    PrimaryButtonText = "Xóa",
                    CloseButtonText = "Hủy",
                    XamlRoot = this.XamlRoot
                };
                if (await dialog.ShowAsync() == ContentDialogResult.Primary)
                {
                    await _vm.DeleteProductAsync(product);
                    UpdateUI();
                }
            }
        }
    }

    private void ShowProductDialog()
    {
        var skuBox = new TextBox { Header = "Mã SKU", Text = _vm.EditingProduct.Sku };
        var nameBox = new TextBox { Header = "Tên sản phẩm", Text = _vm.EditingProduct.Name };
        var importBox = new NumberBox { Header = "Giá nhập", Value = _vm.EditingProduct.ImportPrice };
        var priceBox = new NumberBox { Header = "Giá bán", Value = _vm.EditingProduct.SalePrice };
        var countBox = new NumberBox { Header = "Số lượng tồn", Value = _vm.EditingProduct.Count };
        var descBox = new TextBox { Header = "Mô tả", Text = _vm.EditingProduct.Description, AcceptsReturn = true };

        var dialog = new ContentDialog
        {
            Title = _vm.DialogTitle,
            PrimaryButtonText = "Lưu",
            CloseButtonText = "Hủy",
            XamlRoot = this.XamlRoot,
            Content = new StackPanel
            {
                Spacing = 8,
                MinWidth = 400,
                Children = { skuBox, nameBox, importBox, priceBox, countBox, descBox }
            }
        };

        _ = ShowProductDialogAsync(dialog, skuBox, nameBox, importBox, priceBox, countBox, descBox);
    }

    private async Task ShowProductDialogAsync(ContentDialog dialog, TextBox skuBox, TextBox nameBox,
        NumberBox importBox, NumberBox priceBox, NumberBox countBox, TextBox descBox)
    {
        if (await dialog.ShowAsync() == ContentDialogResult.Primary)
        {
            _vm.EditingProduct.Sku = skuBox.Text;
            _vm.EditingProduct.Name = nameBox.Text;
            _vm.EditingProduct.ImportPrice = (int)importBox.Value;
            _vm.EditingProduct.SalePrice = (int)priceBox.Value;
            _vm.EditingProduct.Count = (int)countBox.Value;
            _vm.EditingProduct.Description = descBox.Text;

            await _vm.SaveProductAsync();
            UpdateUI();
        }
    }

    private void OnAddCategory(object sender, RoutedEventArgs e)
    {
        var catNameBox = new TextBox { Header = "Tên loại" };
        var catDescBox = new TextBox { Header = "Mô tả", AcceptsReturn = true };

        var dialog = new ContentDialog
        {
            Title = "Thêm loại sản phẩm",
            PrimaryButtonText = "Thêm",
            CloseButtonText = "Hủy",
            XamlRoot = this.XamlRoot,
            Content = new StackPanel
            {
                Spacing = 8,
                MinWidth = 300,
                Children = { catNameBox, catDescBox }
            }
        };

        _ = AddCategoryAsync(dialog, catNameBox, catDescBox);
    }

    private async Task AddCategoryAsync(ContentDialog dialog, TextBox catNameBox, TextBox catDescBox)
    {
        if (await dialog.ShowAsync() == ContentDialogResult.Primary)
        {
            _vm.NewCategoryName = catNameBox.Text;
            _vm.NewCategoryDescription = catDescBox.Text;
            await _vm.AddCategoryAsync();
            CategoryCombo.ItemsSource = _vm.Categories;
            CategoryCombo.SelectedIndex = CategoryCombo.Items.Count - 1;
        }
    }

    private async void OnImport(object sender, RoutedEventArgs e)
    {
        await _vm.ImportFromExcelAsync();
        UpdateUI();
    }

    private async void OnPrevPage(object sender, RoutedEventArgs e)
    {
        if (_vm.CurrentPage > 1)
        {
            _vm.CurrentPage--;
            await _vm.LoadProductsAsync();
            UpdateUI();
        }
    }

    private async void OnNextPage(object sender, RoutedEventArgs e)
    {
        if (_vm.CurrentPage < _vm.TotalPages)
        {
            _vm.CurrentPage++;
            await _vm.LoadProductsAsync();
            UpdateUI();
        }
    }
}

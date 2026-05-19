using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MyShop.Models;
using MyShop.Services;

namespace MyShop.ViewModels;

public partial class ProductsViewModel : BaseViewModel
{
    private readonly IProductService _productService;

    [ObservableProperty]
    private ObservableCollection<Category> _categories = new();

    [ObservableProperty]
    private Category? _selectedCategory;

    [ObservableProperty]
    private ObservableCollection<Product> _products = new();

    [ObservableProperty]
    private Product? _selectedProduct;

    [ObservableProperty]
    private string _searchKeyword = string.Empty;

    [ObservableProperty]
    private int _minPrice;

    [ObservableProperty]
    private int _maxPrice = 1000000;

    [ObservableProperty]
    private string _selectedSort = "Mới nhất";

    [ObservableProperty]
    private int _currentPage = 1;

    [ObservableProperty]
    private int _totalPages = 1;

    [ObservableProperty]
    private int _pageSize = 10;

    [ObservableProperty]
    private int _totalItems;

    [ObservableProperty]
    private string _pageInfo = "";

    [ObservableProperty]
    private bool _isAddDialogOpen;

    [ObservableProperty]
    private Product _editingProduct = new();

    [ObservableProperty]
    private string _dialogTitle = "Thêm sản phẩm";

    [ObservableProperty]
    private bool _isEditMode;

    [ObservableProperty]
    private string _newCategoryName = string.Empty;

    [ObservableProperty]
    private string _newCategoryDescription = string.Empty;

    [ObservableProperty]
    private bool _isCategoryDialogOpen;

    public List<string> SortOptions { get; } = new()
    {
        "Mới nhất", "Tên A-Z", "Tên Z-A", "Giá tăng dần", "Giá giảm dần",
        "Tồn kho tăng", "Tồn kho giảm"
    };

    public ProductsViewModel(IProductService productService)
    {
        _productService = productService;
    }

    [RelayCommand]
    public async Task LoadCategoriesAsync()
    {
        try
        {
            var cats = await _productService.GetCategoriesAsync();
            Categories.Clear();
            Categories.Add(new Category { CategoryId = 0, Name = "Tất cả" });
            foreach (var c in cats)
                Categories.Add(c);
        }
        catch (Exception ex)
        {
            SetError($"Lỗi tải loại sản phẩm: {ex.Message}");
        }
    }

    [RelayCommand]
    public async Task LoadProductsAsync()
    {
        IsLoading = true;
        ClearError();
        try
        {
            var sortBy = SelectedSort switch
            {
                "Tên A-Z" => "name_asc",
                "Tên Z-A" => "name_desc",
                "Giá tăng dần" => "price_asc",
                "Giá giảm dần" => "price_desc",
                "Tồn kho tăng" => "count_asc",
                "Tồn kho giảm" => "count_desc",
                _ => null
            };

            TotalItems = await _productService.GetProductCountAsync(
                SelectedCategory?.CategoryId ?? 0,
                string.IsNullOrWhiteSpace(SearchKeyword) ? null : SearchKeyword,
                MinPrice > 0 ? MinPrice : null,
                MaxPrice < 1000000 ? MaxPrice : null);
            TotalPages = Math.Max(1, (int)Math.Ceiling((double)TotalItems / PageSize));
            if (CurrentPage > TotalPages) CurrentPage = TotalPages;
            PageInfo = $"Trang {CurrentPage}/{TotalPages} ({TotalItems} sản phẩm)";

            var list = await _productService.GetProductsAsync(
                SelectedCategory?.CategoryId ?? 0,
                string.IsNullOrWhiteSpace(SearchKeyword) ? null : SearchKeyword,
                MinPrice > 0 ? MinPrice : null,
                MaxPrice < 1000000 ? MaxPrice : null,
                sortBy, CurrentPage, PageSize);

            Products.Clear();
            foreach (var p in list)
                Products.Add(p);
        }
        catch (Exception ex)
        {
            SetError($"Lỗi tải sản phẩm: {ex.Message}");
        }
        finally
        {
            IsLoading = false;
        }
    }

    public void OpenAddDialog()
    {
        EditingProduct = new Product();
        DialogTitle = "Thêm sản phẩm";
        IsEditMode = false;
        IsAddDialogOpen = true;
    }

    public void OpenEditDialog(Product product)
    {
        EditingProduct = new Product
        {
            ProductId = product.ProductId,
            Sku = product.Sku,
            Name = product.Name,
            ImportPrice = product.ImportPrice,
            SalePrice = product.SalePrice,
            Count = product.Count,
            Description = product.Description,
            CategoryId = product.CategoryId,
            CategoryName = product.CategoryName
        };
        DialogTitle = "Sửa sản phẩm";
        IsEditMode = true;
        IsAddDialogOpen = true;
    }

    public async Task SaveProductAsync()
    {
        if (string.IsNullOrWhiteSpace(EditingProduct.Name) || string.IsNullOrWhiteSpace(EditingProduct.Sku))
        {
            SetError("Tên sản phẩm và mã SKU không được để trống");
            return;
        }
        ClearError();
        IsLoading = true;
        try
        {
            if (IsEditMode)
                await _productService.UpdateProductAsync(EditingProduct);
            else
                await _productService.AddProductAsync(EditingProduct);
            IsAddDialogOpen = false;
            await LoadProductsAsync();
        }
        catch (Exception ex)
        {
            SetError($"Lỗi lưu: {ex.Message}");
        }
        finally
        {
            IsLoading = false;
        }
    }

    public async Task DeleteProductAsync(Product product)
    {
        try
        {
            await _productService.DeleteProductAsync(product.ProductId);
            await LoadProductsAsync();
        }
        catch (Exception ex)
        {
            SetError($"Lỗi xóa: {ex.Message}");
        }
    }

    [RelayCommand]
    private void OpenCategoryDialog()
    {
        NewCategoryName = "";
        NewCategoryDescription = "";
        IsCategoryDialogOpen = true;
    }

    public async Task AddCategoryAsync()
    {
        if (string.IsNullOrWhiteSpace(NewCategoryName)) return;
        try
        {
            await _productService.AddCategoryAsync(new Category
            {
                Name = NewCategoryName,
                Description = NewCategoryDescription
            });
            IsCategoryDialogOpen = false;
            await LoadCategoriesAsync();
        }
        catch (Exception ex)
        {
            SetError($"Lỗi thêm loại: {ex.Message}");
        }
    }

    public async Task ImportFromExcelAsync()
    {
        var picker = new Windows.Storage.Pickers.FileOpenPicker
        {
            ViewMode = Windows.Storage.Pickers.PickerViewMode.List
        };
        picker.FileTypeFilter.Add(".xlsx");
        picker.FileTypeFilter.Add(".xls");

        var window = new Microsoft.UI.Xaml.Window();
        var handle = WinRT.Interop.WindowNative.GetWindowHandle(window);
        WinRT.Interop.InitializeWithWindow.Initialize(picker, handle);

        var file = await picker.PickSingleFileAsync();
        if (file == null) return;

        IsLoading = true;
        try
        {
            await _productService.ImportFromExcelAsync(file.Path);
            await LoadProductsAsync();
        }
        catch (Exception ex)
        {
            SetError($"Lỗi import: {ex.Message}");
        }
        finally
        {
            IsLoading = false;
        }
    }

    [RelayCommand]
    private void GoToPage(int page)
    {
        CurrentPage = Math.Clamp(page, 1, TotalPages);
        _ = LoadProductsAsync();
    }
}

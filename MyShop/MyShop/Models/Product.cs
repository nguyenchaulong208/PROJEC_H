using CommunityToolkit.Mvvm.ComponentModel;

namespace MyShop.Models;

public partial class Product : ObservableObject
{
    [ObservableProperty]
    private int _productId;

    [ObservableProperty]
    private string _sku = string.Empty;

    [ObservableProperty]
    private string _name = string.Empty;

    [ObservableProperty]
    private int _importPrice;

    [ObservableProperty]
    private int _count;

    [ObservableProperty]
    private string _description = string.Empty;

    [ObservableProperty]
    private int _categoryId;

    [ObservableProperty]
    private string _categoryName = string.Empty;

    [ObservableProperty]
    private int _salePrice;

    public List<string> Images { get; set; } = new();
}

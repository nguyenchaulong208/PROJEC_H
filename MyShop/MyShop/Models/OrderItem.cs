using CommunityToolkit.Mvvm.ComponentModel;

namespace MyShop.Models;

public partial class OrderItem : ObservableObject
{
    [ObservableProperty]
    private int _orderItemId;

    [ObservableProperty]
    private int _quantity;

    [ObservableProperty]
    private double _unitSalePrice;

    [ObservableProperty]
    private int _totalPrice;

    [ObservableProperty]
    private int _orderId;

    [ObservableProperty]
    private int _productId;

    [ObservableProperty]
    private string _productName = string.Empty;
}

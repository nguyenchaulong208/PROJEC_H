using CommunityToolkit.Mvvm.ComponentModel;

namespace MyShop.Models;

public partial class Order : ObservableObject
{
    [ObservableProperty]
    private int _orderId;

    [ObservableProperty]
    private DateTime _createdTime = DateTime.Now;

    [ObservableProperty]
    private int _finalPrice;

    [ObservableProperty]
    private string _status = OrderStatus.New;

    [ObservableProperty]
    private List<OrderItem> _items = new();
}

public static class OrderStatus
{
    public const string New = "Mới tạo";
    public const string Paid = "Đã thanh toán";
    public const string Cancelled = "Đã hủy";
}

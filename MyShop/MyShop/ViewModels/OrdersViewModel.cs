using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MyShop.Models;
using MyShop.Services;

namespace MyShop.ViewModels;

public partial class OrdersViewModel : BaseViewModel
{
    private readonly IOrderService _orderService;
    private readonly IProductService _productService;

    [ObservableProperty]
    private ObservableCollection<Order> _orders = new();

    [ObservableProperty]
    private ObservableCollection<Product> _availableProducts = new();

    [ObservableProperty]
    private Order? _selectedOrder;

    [ObservableProperty]
    private Order _editingOrder = new() { Status = OrderStatus.New };

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
    private DateTimeOffset? _filterFromDate;

    [ObservableProperty]
    private DateTimeOffset? _filterToDate;

    [ObservableProperty]
    private string _filterStatus = "";

    [ObservableProperty]
    private bool _isOrderDialogOpen;

    [ObservableProperty]
    private string _orderDialogTitle = "Tạo đơn hàng";

    [ObservableProperty]
    private bool _isEditMode;

    [ObservableProperty]
    private Product? _selectedAddProduct;

    [ObservableProperty]
    private int _addQuantity = 1;

    [ObservableProperty]
    private bool _isDetailDialogOpen;

    [ObservableProperty]
    private Order? _detailOrder;

    [ObservableProperty]
    private int _totalAmount;

    public List<string> StatusFilters { get; } = new()
    {
        "Tất cả", OrderStatus.New, OrderStatus.Paid, OrderStatus.Cancelled
    };

    public OrdersViewModel(IOrderService orderService, IProductService productService)
    {
        _orderService = orderService;
        _productService = productService;
    }

    [RelayCommand]
    public async Task LoadOrdersAsync()
    {
        IsLoading = true;
        ClearError();
        try
        {
            DateTime? from = FilterFromDate?.DateTime;
            DateTime? to = FilterToDate?.DateTime;
            string? status = FilterStatus == "Tất cả" || string.IsNullOrEmpty(FilterStatus) ? null : FilterStatus;

            TotalItems = await _orderService.GetOrderCountAsync(from, to, status);
            TotalPages = Math.Max(1, (int)Math.Ceiling((double)TotalItems / PageSize));
            if (CurrentPage > TotalPages) CurrentPage = TotalPages;
            PageInfo = $"Trang {CurrentPage}/{TotalPages} ({TotalItems} đơn hàng)";

            var list = await _orderService.GetOrdersAsync(from, to, status, CurrentPage, PageSize);
            Orders.Clear();
            foreach (var o in list)
                Orders.Add(o);
        }
        catch (Exception ex)
        {
            SetError($"Lỗi tải đơn hàng: {ex.Message}");
        }
        finally
        {
            IsLoading = false;
        }
    }

    public async Task OpenCreateOrderAsync()
    {
        EditingOrder = new Order { Status = OrderStatus.New, CreatedTime = DateTime.Now };
        EditingOrder.Items.Clear();
        OrderDialogTitle = "Tạo đơn hàng";
        IsEditMode = false;
        IsOrderDialogOpen = true;
        TotalAmount = 0;
        await LoadAvailableProductsAsync();
    }

    public async Task OpenEditOrderAsync(Order order)
    {
        var full = await _orderService.GetOrderByIdAsync(order.OrderId);
        if (full == null) return;
        EditingOrder = new Order
        {
            OrderId = full.OrderId,
            CreatedTime = full.CreatedTime,
            FinalPrice = full.FinalPrice,
            Status = full.Status
        };
        EditingOrder.Items = full.Items.Select(i => new OrderItem
        {
            OrderItemId = i.OrderItemId,
            Quantity = i.Quantity,
            UnitSalePrice = i.UnitSalePrice,
            TotalPrice = i.TotalPrice,
            ProductId = i.ProductId,
            ProductName = i.ProductName,
            OrderId = i.OrderId
        }).ToList();

        OrderDialogTitle = "Sửa đơn hàng";
        IsEditMode = true;
        IsOrderDialogOpen = true;
        TotalAmount = EditingOrder.FinalPrice;
        await LoadAvailableProductsAsync();
    }

    private async Task LoadAvailableProductsAsync()
    {
        var prods = await _productService.GetProductsAsync(page: 1, pageSize: 1000);
        AvailableProducts.Clear();
        foreach (var p in prods)
            AvailableProducts.Add(p);
    }

    public void AddProductToOrder()
    {
        if (SelectedAddProduct == null || AddQuantity <= 0) return;

        var existing = EditingOrder.Items.FirstOrDefault(i => i.ProductId == SelectedAddProduct.ProductId);
        if (existing != null)
        {
            existing.Quantity += AddQuantity;
            existing.TotalPrice = (int)(existing.Quantity * existing.UnitSalePrice);
        }
        else
        {
            EditingOrder.Items.Add(new OrderItem
            {
                ProductId = SelectedAddProduct.ProductId,
                ProductName = SelectedAddProduct.Name,
                Quantity = AddQuantity,
                UnitSalePrice = SelectedAddProduct.SalePrice,
                TotalPrice = (int)(AddQuantity * SelectedAddProduct.SalePrice),
                OrderId = EditingOrder.OrderId
            });
        }
        RecalculateTotal();
    }

    public void RemoveOrderItem(OrderItem item)
    {
        EditingOrder.Items.Remove(item);
        RecalculateTotal();
    }

    private void RecalculateTotal()
    {
        TotalAmount = EditingOrder.Items.Sum(i => i.TotalPrice);
        EditingOrder.FinalPrice = TotalAmount;
    }

    public async Task SaveOrderAsync()
    {
        if (EditingOrder.Items.Count == 0)
        {
            SetError("Vui lòng thêm ít nhất 1 sản phẩm");
            return;
        }
        ClearError();
        IsLoading = true;
        try
        {
            if (IsEditMode)
                await _orderService.UpdateOrderAsync(EditingOrder);
            else
                await _orderService.CreateOrderAsync(EditingOrder);
            IsOrderDialogOpen = false;
            await LoadOrdersAsync();
        }
        catch (Exception ex)
        {
            SetError($"Lỗi lưu đơn hàng: {ex.Message}");
        }
        finally
        {
            IsLoading = false;
        }
    }

    public async Task DeleteOrderAsync(Order order)
    {
        try
        {
            await _orderService.DeleteOrderAsync(order.OrderId);
            await LoadOrdersAsync();
        }
        catch (Exception ex)
        {
            SetError($"Lỗi xóa: {ex.Message}");
        }
    }

    public async Task ViewOrderDetailAsync(Order order)
    {
        DetailOrder = await _orderService.GetOrderByIdAsync(order.OrderId);
        IsDetailDialogOpen = true;
    }

    [RelayCommand]
    private async Task SearchAsync()
    {
        CurrentPage = 1;
        await LoadOrdersAsync();
    }
}

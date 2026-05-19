using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using MyShop.Models;
using MyShop.ViewModels;

namespace MyShop.Views;

public sealed partial class OrdersPage : Page
{
    private readonly OrdersViewModel _vm;

    public OrdersPage()
    {
        InitializeComponent();
        _vm = new OrdersViewModel(App.OrderService!, App.ProductService!);
        StatusFilter.ItemsSource = _vm.StatusFilters;
        StatusFilter.SelectedIndex = 0;
        FromDate.SelectedDate = DateTime.Today.AddDays(-7);
        ToDate.SelectedDate = DateTime.Today;
        Loaded += OnLoaded;
    }

    private async void OnLoaded(object sender, RoutedEventArgs e)
    {
        try { await LoadDataAsync(); } catch { }
    }

    private async Task LoadDataAsync()
    {
        await _vm.LoadOrdersAsync();
        OrderList.ItemsSource = _vm.Orders;
        PageInfoText.Text = _vm.PageInfo;
    }

    private async void OnSearchClick(object sender, RoutedEventArgs e)
    {
        _vm.FilterFromDate = FromDate.SelectedDate;
        _vm.FilterToDate = ToDate.SelectedDate;
        _vm.FilterStatus = StatusFilter.SelectedItem as string ?? "";
        _vm.CurrentPage = 1;
        await LoadDataAsync();
    }

    private async void OnCreateOrder(object sender, RoutedEventArgs e)
    {
        await _vm.OpenCreateOrderAsync();
        ShowOrderDialog();
    }

    private async void OnEditOrder(object sender, RoutedEventArgs e)
    {
        if ((sender as Button)?.Tag is int id)
        {
            var order = _vm.Orders.FirstOrDefault(o => o.OrderId == id);
            if (order != null)
            {
                await _vm.OpenEditOrderAsync(order);
                ShowOrderDialog();
            }
        }
    }

    private async void OnDeleteOrder(object sender, RoutedEventArgs e)
    {
        if ((sender as Button)?.Tag is int id)
        {
            var order = _vm.Orders.FirstOrDefault(o => o.OrderId == id);
            if (order != null)
            {
                var dialog = new ContentDialog
                {
                    Title = "Xác nhận xóa",
                    Content = $"Xóa đơn hàng #{order.OrderId}?",
                    PrimaryButtonText = "Xóa",
                    CloseButtonText = "Hủy",
                    XamlRoot = this.XamlRoot
                };
                if (await dialog.ShowAsync() == ContentDialogResult.Primary)
                {
                    await _vm.DeleteOrderAsync(order);
                    await LoadDataAsync();
                }
            }
        }
    }

    private async void OnViewDetail(object sender, RoutedEventArgs e)
    {
        if ((sender as Button)?.Tag is int id)
        {
            var order = _vm.Orders.FirstOrDefault(o => o.OrderId == id);
            if (order != null)
            {
                await _vm.ViewOrderDetailAsync(order);
                if (_vm.DetailOrder != null)
                    ShowDetailDialog(_vm.DetailOrder);
            }
        }
    }

    private void ShowDetailDialog(Order order)
    {
        var itemsStack = new StackPanel { Spacing = 4 };
        foreach (var item in order.Items)
        {
            var grid = new Grid
            {
                ColumnDefinitions =
                {
                    new ColumnDefinition { Width = GridLength.Auto },
                    new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) },
                    new ColumnDefinition { Width = GridLength.Auto }
                },
                ColumnSpacing = 8
            };
            var qtyText = new TextBlock
            {
                Text = $"{item.Quantity}x",
                FontWeight = Microsoft.UI.Text.FontWeights.SemiBold,
                VerticalAlignment = VerticalAlignment.Center
            };
            grid.Children.Add(qtyText);

            var nameText = new TextBlock
            {
                Text = item.ProductName,
                VerticalAlignment = VerticalAlignment.Center
            };
            Grid.SetColumn(nameText, 1);
            grid.Children.Add(nameText);

            var priceText = new TextBlock
            {
                Text = $"{item.TotalPrice:N0} đ",
                VerticalAlignment = VerticalAlignment.Center
            };
            Grid.SetColumn(priceText, 2);
            grid.Children.Add(priceText);

            itemsStack.Children.Add(grid);
        }

        var separator1 = new Border { Height = 1, Background = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.LightGray), Margin = new Thickness(0, 4, 0, 4) };
        var separator2 = new Border { Height = 1, Background = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.LightGray), Margin = new Thickness(0, 4, 0, 4) };

        var dialog = new ContentDialog
        {
            Title = $"Đơn hàng #{order.OrderId}",
            CloseButtonText = "Đóng",
            XamlRoot = this.XamlRoot,
            Content = new StackPanel
            {
                Spacing = 8,
                MinWidth = 350,
                Children =
                {
                    new TextBlock { Text = $"Thời gian: {order.CreatedTime:dd/MM/yyyy HH:mm}" },
                    new TextBlock { Text = $"Trạng thái: {order.Status}" },
                    separator1,
                    new TextBlock { Text = "Sản phẩm:", FontWeight = Microsoft.UI.Text.FontWeights.SemiBold },
                    itemsStack,
                    separator2,
                    new TextBlock
                    {
                        Text = $"Tổng tiền: {order.FinalPrice:N0} đ",
                        FontWeight = Microsoft.UI.Text.FontWeights.Bold,
                        Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.DodgerBlue)
                    }
                }
            }
        };
        _ = dialog.ShowAsync();
    }

    private void ShowOrderDialog()
    {
        var productsCombo = new ComboBox
        {
            Header = "Chọn sản phẩm",
            MinWidth = 300,
            DisplayMemberPath = "Name"
        };
        foreach (var p in _vm.AvailableProducts)
            productsCombo.Items.Add(p);

        var qtyBox = new NumberBox { Header = "Số lượng", Value = 1, MinWidth = 100 };

        var totalText = new TextBlock
        {
            FontSize = 18,
            FontWeight = Microsoft.UI.Text.FontWeights.Bold,
            Text = $"Tổng: {_vm.TotalAmount:N0} đ"
        };

        var itemsStack = new StackPanel { Spacing = 4 };
        void RefreshItemsDisplay()
        {
            itemsStack.Children.Clear();
            foreach (var item in _vm.EditingOrder.Items)
            {
                var row = new Grid
                {
                    ColumnDefinitions =
                    {
                        new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) },
                        new ColumnDefinition { Width = GridLength.Auto },
                        new ColumnDefinition { Width = GridLength.Auto }
                    },
                    ColumnSpacing = 8,
                    Padding = new Thickness(4),
                    Tag = item
                };

                var nameText = new TextBlock
                {
                    Text = $"{item.ProductName} x{item.Quantity}",
                    VerticalAlignment = VerticalAlignment.Center
                };
                row.Children.Add(nameText);

                var priceText = new TextBlock
                {
                    Text = $"{item.TotalPrice:N0} đ",
                    VerticalAlignment = VerticalAlignment.Center,
                    FontWeight = Microsoft.UI.Text.FontWeights.SemiBold
                };
                Grid.SetColumn(priceText, 1);
                row.Children.Add(priceText);

                var removeBtn = new Button
                {
                    Content = "X",
                    Tag = item,
                    Width = 32,
                    Height = 32
                };
                Grid.SetColumn(removeBtn, 2);
                removeBtn.Click += (s, args) =>
                {
                    if ((s as Button)?.Tag is OrderItem oi)
                        _vm.RemoveOrderItem(oi);
                    RefreshItemsDisplay();
                    totalText.Text = $"Tổng: {_vm.TotalAmount:N0} đ";
                };
                row.Children.Add(removeBtn);

                itemsStack.Children.Add(row);
            }
        }

        var scrollItems = new ScrollViewer
        {
            Content = itemsStack,
            Height = 200
        };

        var statusCombo = new ComboBox
        {
            Header = "Trạng thái",
            ItemsSource = new List<string> { OrderStatus.New, OrderStatus.Paid, OrderStatus.Cancelled },
            SelectedItem = _vm.EditingOrder.Status
        };

        var addButton = new Button { Content = "Thêm vào đơn", Margin = new Thickness(0, 8, 0, 0) };
        addButton.Click += (s, args) =>
        {
            if (productsCombo.SelectedItem is Product sel)
            {
                _vm.SelectedAddProduct = sel;
                _vm.AddQuantity = (int)qtyBox.Value;
                _vm.AddProductToOrder();
                RefreshItemsDisplay();
                totalText.Text = $"Tổng: {_vm.TotalAmount:N0} đ";
            }
        };

        statusCombo.SelectionChanged += (s, args) =>
        {
            _vm.EditingOrder.Status = statusCombo.SelectedItem as string ?? OrderStatus.New;
        };

        var dialog = new ContentDialog
        {
            Title = _vm.OrderDialogTitle,
            PrimaryButtonText = "Lưu",
            CloseButtonText = "Hủy",
            XamlRoot = this.XamlRoot,
            Content = new ScrollViewer
            {
                Content = new StackPanel
                {
                    Spacing = 8,
                    MinWidth = 450,
                    Children =
                    {
                        new Grid
                        {
                            ColumnDefinitions = { new ColumnDefinition(), new ColumnDefinition() },
                            ColumnSpacing = 8,
                            Children = { productsCombo, qtyBox }
                        },
                        addButton,
                        new TextBlock { Text = "Sản phẩm trong đơn:", FontWeight = Microsoft.UI.Text.FontWeights.SemiBold },
                        scrollItems,
                        totalText,
                        statusCombo
                    }
                }
            }
        };

        _ = ShowOrderDialogAsync(dialog);
    }

    private async Task ShowOrderDialogAsync(ContentDialog dialog)
    {
        if (await dialog.ShowAsync() == ContentDialogResult.Primary)
        {
            await _vm.SaveOrderAsync();
            await LoadDataAsync();
        }
    }

    private async void OnPrevPage(object sender, RoutedEventArgs e)
    {
        if (_vm.CurrentPage > 1)
        {
            _vm.CurrentPage--;
            await LoadDataAsync();
        }
    }

    private async void OnNextPage(object sender, RoutedEventArgs e)
    {
        if (_vm.CurrentPage < _vm.TotalPages)
        {
            _vm.CurrentPage++;
            await LoadDataAsync();
        }
    }
}

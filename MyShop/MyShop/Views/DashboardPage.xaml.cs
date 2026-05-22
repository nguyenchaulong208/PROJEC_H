using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using MyShop.ViewModels;

namespace MyShop.Views;

public sealed partial class DashboardPage : Page
{
    private readonly DashboardViewModel _vm;

    public DashboardPage()
    {
        InitializeComponent();
        _vm = new DashboardViewModel(
            App.ProductService!,
            App.OrderService!,
            App.ReportService!);
        Loaded += OnLoaded;
    }

    private async void OnLoaded(object sender, RoutedEventArgs e)
    {
        try
        {
            await _vm.LoadDataAsync();
            TotalProductsText.Text = _vm.TotalProducts.ToString("N0");
            TodayOrdersText.Text = _vm.TodayOrderCount.ToString("N0");
            TodayRevenueText.Text = _vm.TodayRevenueText;
            LowStockText.Text = _vm.LowStockProducts.Count.ToString();

            TopSellingList.ItemsSource = _vm.TopSellingProducts.Select(x => new { x.Name, x.Total }).ToList();
            LowStockList.ItemsSource = _vm.LowStockProducts.Select(p => new { p.Name, p.Count }).ToList();
            RecentOrdersList.ItemsSource = _vm.RecentOrders;

            // RenderChart(); // temporarily disabled - SkiaSharp native DLL issue
        }
        catch { }
    }

    private void RenderChart()
    {
        try
        {
            var chart = new LiveChartsCore.SkiaSharpView.WinUI.CartesianChart
            {
                Series = _vm.RevenueChartSeries,
                XAxes = _vm.RevenueXAxes,
                YAxes = _vm.RevenueYAxes,
                Height = 300
            };
            ChartContainer.Child = chart;
        }
        catch { }
    }
}

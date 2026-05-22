using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using LiveChartsCore;
using LiveChartsCore.SkiaSharpView;
using LiveChartsCore.SkiaSharpView.Painting;
using SkiaSharp;
using MyShop.Models;
using MyShop.Services;

namespace MyShop.ViewModels;

public partial class DashboardViewModel : BaseViewModel
{
    private readonly IProductService _productService;
    private readonly IOrderService _orderService;
    private readonly IReportService _reportService;

    [ObservableProperty]
    private int _totalProducts;

    [ObservableProperty]
    private int _todayOrderCount;

    [ObservableProperty]
    private int _todayRevenue;

    [ObservableProperty]
    private string _todayRevenueText = "0 đ";

    [ObservableProperty]
    private List<Product> _lowStockProducts = new();

    [ObservableProperty]
    private List<(string Name, int Total)> _topSellingProducts = new();

    [ObservableProperty]
    private List<Order> _recentOrders = new();

    [ObservableProperty]
    private ISeries[] _revenueChartSeries = Array.Empty<ISeries>();

    [ObservableProperty]
    private Axis[] _revenueXAxes = Array.Empty<Axis>();

    [ObservableProperty]
    private Axis[] _revenueYAxes = Array.Empty<Axis>();

    public DashboardViewModel(IProductService productService, IOrderService orderService, IReportService reportService)
    {
        _productService = productService;
        _orderService = orderService;
        _reportService = reportService;
    }

    [RelayCommand]
    public async Task LoadDataAsync()
    {
        IsLoading = true;
        ClearError();
        try
        {
            var products = await _productService.GetProductsAsync(page: 1, pageSize: 10000);
            TotalProducts = products.Count;
            LowStockProducts = products.Where(p => p.Count < 5)
                .OrderBy(p => p.Count).Take(5).ToList();

            TopSellingProducts = await _orderService.GetTopSellingProductsAsync(5);
            TodayOrderCount = await _orderService.GetTodayOrderCountAsync();
            TodayRevenue = await _orderService.GetTodayRevenueAsync();
            TodayRevenueText = $"{TodayRevenue:N0} đ";
            RecentOrders = await _orderService.GetRecentOrdersAsync(3);

            // await LoadRevenueChartAsync(); // disable temporarily - SkiaSharp native DLL issue
        }
        catch (Exception ex)
        {
            SetError($"Lỗi tải dữ liệu: {ex.Message}");
        }
        finally
        {
            IsLoading = false;
        }
    }

    private async Task LoadRevenueChartAsync()
    {
        var today = DateTime.Today;
        var firstDay = new DateTime(today.Year, today.Month, 1);
        var data = await _reportService.GetRevenueProfitByDayAsync(firstDay, today);

        var values = new List<int>();
        var labels = new List<string>();
        for (int d = 1; d <= today.Day; d++)
        {
            var dayStr = $"{today.Year}-{today.Month:D2}-{d:D2}";
            var match = data.FirstOrDefault(x => x.Label == dayStr);
            values.Add(match.Revenue);
            labels.Add($"Ngày {d}");
        }

        RevenueChartSeries = new ISeries[]
        {
            new ColumnSeries<int>
            {
                Values = values,
                Name = "Doanh thu",
                Fill = new SolidColorPaint(SKColors.DodgerBlue)
            }
        };

        RevenueXAxes = new Axis[]
        {
            new Axis { Labels = labels, LabelsRotation = 45 }
        };

        RevenueYAxes = new Axis[]
        {
            new Axis { Name = "VNĐ" }
        };
    }
}

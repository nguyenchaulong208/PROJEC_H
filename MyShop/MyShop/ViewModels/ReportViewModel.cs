using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using LiveChartsCore;
using LiveChartsCore.SkiaSharpView;
using LiveChartsCore.SkiaSharpView.Painting;
using SkiaSharp;
using MyShop.Services;

namespace MyShop.ViewModels;

public partial class ReportViewModel : BaseViewModel
{
    private readonly IReportService _reportService;

    [ObservableProperty]
    private int _selectedReportType;

    [ObservableProperty]
    private int _selectedViewType;

    [ObservableProperty]
    private ISeries[] _chartSeries = Array.Empty<ISeries>();

    [ObservableProperty]
    private Axis[] _chartXAxes = Array.Empty<Axis>();

    [ObservableProperty]
    private Axis[] _chartYAxes = Array.Empty<Axis>();

    [ObservableProperty]
    private string _reportTitle = "Báo cáo doanh thu";

    public List<string> ReportTypes { get; } = new()
    {
        "Sản phẩm bán theo ngày",
        "Sản phẩm bán theo tuần",
        "Sản phẩm bán theo tháng",
        "Sản phẩm bán theo năm",
        "Doanh thu & Lợi nhuận theo ngày",
        "Doanh thu & Lợi nhuận theo tháng"
    };

    public List<string> ChartTypes { get; } = new()
    {
        "Biểu đồ đường",
        "Biểu đồ cột",
        "Biểu đồ bánh"
    };

    public ReportViewModel(IReportService reportService)
    {
        _reportService = reportService;
    }

    [RelayCommand]
    public async Task LoadReportAsync()
    {
        IsLoading = true;
        ClearError();
        try
        {
            var now = DateTime.Now;
            List<(string Label, int Value)>? salesData = null;
            List<(string Label, int Revenue, int Profit)>? revenueData = null;

            switch (SelectedReportType)
            {
                case 0:
                    salesData = await _reportService.GetSalesByDayAsync(
                        new DateTime(now.Year, now.Month, 1), now);
                    ReportTitle = "Sản phẩm bán theo ngày";
                    break;
                case 1:
                    salesData = await _reportService.GetSalesByWeekAsync(now.Year);
                    ReportTitle = "Sản phẩm bán theo tuần";
                    break;
                case 2:
                    salesData = await _reportService.GetSalesByMonthAsync(now.Year);
                    ReportTitle = "Sản phẩm bán theo tháng";
                    break;
                case 3:
                    salesData = await _reportService.GetSalesByYearAsync();
                    ReportTitle = "Sản phẩm bán theo năm";
                    break;
                case 4:
                    revenueData = await _reportService.GetRevenueProfitByDayAsync(
                        new DateTime(now.Year, now.Month, 1), now);
                    ReportTitle = "Doanh thu & Lợi nhuận theo ngày";
                    break;
                case 5:
                    revenueData = await _reportService.GetRevenueProfitByMonthAsync(now.Year);
                    ReportTitle = "Doanh thu & Lợi nhuận theo tháng";
                    break;
            }

            if (salesData != null)
                BuildSalesChart(salesData);
            else if (revenueData != null)
                BuildRevenueProfitChart(revenueData);
        }
        catch (Exception ex)
        {
            SetError($"Lỗi tải báo cáo: {ex.Message}");
        }
        finally
        {
            IsLoading = false;
        }
    }

    private void BuildSalesChart(List<(string Label, int Value)> data)
    {
        var labels = data.Select(d => d.Label).ToArray();
        var values = data.Select(d => d.Value).ToArray();

        if (SelectedViewType == 2)
        {
            ChartSeries = new ISeries[]
            {
                new PieSeries<int>
                {
                    Values = values,
                    Name = ReportTitle
                }
            };
            ChartXAxes = Array.Empty<Axis>();
            ChartYAxes = Array.Empty<Axis>();
        }
        else if (SelectedViewType == 1)
        {
            ChartSeries = new ISeries[]
            {
                new ColumnSeries<int>
                {
                    Values = values,
                    Name = ReportTitle,
                    Fill = new SolidColorPaint(SKColors.Teal)
                }
            };
            ChartXAxes = new Axis[] { new Axis { Labels = labels, LabelsRotation = 45 } };
            ChartYAxes = new Axis[] { new Axis { Name = "Số lượng" } };
        }
        else
        {
            ChartSeries = new ISeries[]
            {
                new LineSeries<int>
                {
                    Values = values,
                    Name = ReportTitle,
                    Stroke = new SolidColorPaint(SKColors.DodgerBlue, 2),
                    Fill = null,
                    GeometrySize = 8
                }
            };
            ChartXAxes = new Axis[] { new Axis { Labels = labels, LabelsRotation = 45 } };
            ChartYAxes = new Axis[] { new Axis { Name = "Số lượng" } };
        }
    }

    private void BuildRevenueProfitChart(List<(string Label, int Revenue, int Profit)> data)
    {
        var labels = data.Select(d => d.Label).ToArray();
        var revenues = data.Select(d => d.Revenue).ToArray();
        var profits = data.Select(d => d.Profit).ToArray();

        if (SelectedViewType == 2)
        {
            ChartSeries = new ISeries[]
            {
                new PieSeries<int>
                {
                    Values = revenues,
                    Name = "Doanh thu"
                }
            };
            ChartXAxes = Array.Empty<Axis>();
            ChartYAxes = Array.Empty<Axis>();
        }
        else
        {
            ChartSeries = new ISeries[]
            {
                new ColumnSeries<int>
                {
                    Values = revenues,
                    Name = "Doanh thu",
                    Fill = new SolidColorPaint(SKColors.DodgerBlue)
                },
                new ColumnSeries<int>
                {
                    Values = profits,
                    Name = "Lợi nhuận",
                    Fill = new SolidColorPaint(SKColors.LimeGreen)
                }
            };
            ChartXAxes = new Axis[] { new Axis { Labels = labels, LabelsRotation = 45 } };
            ChartYAxes = new Axis[] { new Axis { Name = "VNĐ" } };
        }
    }
}

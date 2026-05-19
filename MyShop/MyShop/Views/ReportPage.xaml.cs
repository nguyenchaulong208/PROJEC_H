using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using MyShop.ViewModels;

namespace MyShop.Views;

public sealed partial class ReportPage : Page
{
    private readonly ReportViewModel _vm;

    public ReportPage()
    {
        InitializeComponent();
        _vm = new ReportViewModel(App.ReportService!);
        ReportTypeCombo.ItemsSource = _vm.ReportTypes;
        ChartTypeCombo.ItemsSource = _vm.ChartTypes;
        ReportTypeCombo.SelectedIndex = 0;
        ChartTypeCombo.SelectedIndex = 0;
    }

    private async void OnViewReport(object sender, RoutedEventArgs e)
    {
        _vm.SelectedReportType = ReportTypeCombo.SelectedIndex;
        _vm.SelectedViewType = ChartTypeCombo.SelectedIndex;
        await _vm.LoadReportAsync();

        ReportTitleText.Text = _vm.ReportTitle;

        if (_vm.ChartSeries.Length == 0) return;

        ChartContainer.Child = null;

        if (_vm.SelectedViewType == 2)
        {
            try
            {
                ChartContainer.Child = new LiveChartsCore.SkiaSharpView.WinUI.PieChart
                {
                    Series = _vm.ChartSeries,
                    Height = 450
                };
            }
            catch { }
        }
        else
        {
            try
            {
                ChartContainer.Child = new LiveChartsCore.SkiaSharpView.WinUI.CartesianChart
                {
                    Series = _vm.ChartSeries,
                    XAxes = _vm.ChartXAxes,
                    YAxes = _vm.ChartYAxes,
                    Height = 450
                };
            }
            catch { }
        }
    }
}

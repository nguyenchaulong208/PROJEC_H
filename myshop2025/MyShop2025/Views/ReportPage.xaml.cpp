#include "pch.h"
#include "ReportPage.xaml.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::UI::Xaml;

namespace winrt::MyShop2025::Views::implementation
{
    ReportPage::ReportPage()
    {
        InitializeComponent();
        m_viewModel = std::make_shared<ViewModels::ReportViewModel>();
    }

    void ReportPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&)
    {
        m_viewModel->LoadReport();
        LoadChart(L"all");
    }

    void ReportPage::PeriodCombo_SelectionChanged(Windows::Foundation::IInspectable const&,
        Controls::SelectionChangedEventArgs const&)
    {
        switch (PeriodCombo().SelectedIndex())
        {
        case 0: m_viewModel->Period(L"day"); break;
        case 1: m_viewModel->Period(L"week"); break;
        case 2: m_viewModel->Period(L"month"); break;
        case 3: m_viewModel->Period(L"year"); break;
        }
    }

    void ReportPage::ViewReportButton_Click(Windows::Foundation::IInspectable const&,
        Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        auto dateFrom = DateFromPicker().Date();
        auto dateTo = DateToPicker().Date();

        if (dateFrom)
        {
            auto dt = dateFrom.Value();
            std::wstringstream wss;
            wss << dt.Year << L"-" << std::setw(2) << std::setfill(L'0') << dt.Month
                << L"-" << std::setw(2) << std::setfill(L'0') << dt.Day;
            m_viewModel->DateFrom(wss.str());
        }
        if (dateTo)
        {
            auto dt = dateTo.Value();
            std::wstringstream wss;
            wss << dt.Year << L"-" << std::setw(2) << std::setfill(L'0') << dt.Month
                << L"-" << std::setw(2) << std::setfill(L'0') << dt.Day;
            m_viewModel->DateTo(wss.str());
        }
        m_viewModel->LoadReportForDate();
        LoadChart(L"all");
    }

    void ReportPage::LoadChart(const std::wstring&)
    {
        if (SalesChart()) SalesChart().Invalidate();
        if (RevenueChart()) RevenueChart().Invalidate();
        if (ProfitChart()) ProfitChart().Invalidate();
    }

    void ReportPage::DrawChart(CanvasDrawingSession& drawingSession, float width, float height,
        const std::vector<ViewModels::ChartDataPoint>& data)
    {
        float padding = 40.0f;

        if (data.empty())
        {
            drawingSession.DrawText(L"Chưa có dữ liệu", width / 2 - 60, height / 2 - 10, Colors::Gray());
            return;
        }

        int64_t maxVal = 0;
        for (const auto& d : data)
            if (d.Value > maxVal) maxVal = d.Value;
        if (maxVal == 0) maxVal = 1;

        float chartWidth = width - 2 * padding;
        float chartHeight = height - 2 * padding;
        float barWidth = chartWidth / data.size() * 0.7f;
        float barGap = chartWidth / data.size() * 0.3f;

        drawingSession.DrawLine(padding, height - padding, width - padding, height - padding, Colors::Gray(), 1);
        drawingSession.DrawLine(padding, padding, padding, height - padding, Colors::Gray(), 1);

        for (size_t i = 0; i < data.size(); i++)
        {
            float barH = (float)data[i].Value / maxVal * chartHeight;
            float x = padding + (float)i * (barWidth + barGap) + barGap / 2;
            float y = height - padding - barH;

            Windows::UI::Color barColor;
            barColor.A = 200;
            barColor.R = 0;
            barColor.G = 120;
            barColor.B = 212;

            drawingSession.FillRectangle(x, y, barWidth, barH, barColor);
            drawingSession.DrawText(data[i].Label.c_str(), x, height - padding + 5, Colors::Gray());
        }
    }

    void ReportPage::SalesChart_Draw(Windows::Foundation::IInspectable const&,
        CanvasDrawEventArgs const& args)
    {
        auto ds = args.DrawingSession();
        auto size = SalesChart().Size();
        DrawChart(ds, size.Width, size.Height, m_viewModel->ProductSales());
    }

    void ReportPage::RevenueChart_Draw(Windows::Foundation::IInspectable const&,
        CanvasDrawEventArgs const& args)
    {
        auto ds = args.DrawingSession();
        auto size = RevenueChart().Size();
        DrawChart(ds, size.Width, size.Height, m_viewModel->Revenue());
    }

    void ReportPage::ProfitChart_Draw(Windows::Foundation::IInspectable const&,
        CanvasDrawEventArgs const& args)
    {
        auto ds = args.DrawingSession();
        auto size = ProfitChart().Size();
        DrawChart(ds, size.Width, size.Height, m_viewModel->Profit());
    }
}

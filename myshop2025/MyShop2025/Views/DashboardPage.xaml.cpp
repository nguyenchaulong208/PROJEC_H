#include "pch.h"
#include "DashboardPage.xaml.h"
#include "Services/DatabaseService.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::UI::Xaml;

namespace winrt::MyShop2025::Views::implementation
{
    DashboardPage::DashboardPage()
    {
        InitializeComponent();
        m_viewModel = std::make_shared<ViewModels::DashboardViewModel>();
    }

    void DashboardPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&)
    {
        m_viewModel->LoadData();
        BindData();
    }

    void DashboardPage::BindData()
    {
        TotalProductsText().Text(std::to_wstring(m_viewModel->TotalProducts()).c_str());
        TotalOrdersText().Text(std::to_wstring(m_viewModel->TotalOrdersToday()).c_str());
        TotalRevenueText().Text((std::to_wstring(m_viewModel->TotalRevenueToday()) + L" ₫").c_str());
        TotalProfitText().Text((std::to_wstring(m_viewModel->TotalProfitToday()) + L" ₫").c_str());

        LowStockList().ItemsSource(winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>());
        auto lowStockItems = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();
        for (const auto& p : m_viewModel->LowStockProducts())
        {
            auto info = winrt::Windows::Foundation::PropertyValue::CreateString(
                (p.Name + L" (Còn: " + std::to_wstring(p.Count) + L")").c_str());
            lowStockItems.Append(info);
        }
        LowStockList().ItemsSource(lowStockItems);

        auto topSellingItems = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();
        m_revenueLabels.clear();
        m_revenueValues.clear();
        for (const auto& rp : m_viewModel->DailyRevenue())
        {
            m_revenueLabels.push_back(rp.Label);
            m_revenueValues.push_back(rp.Value);
        }

        if (RevenueChart())
            RevenueChart().Invalidate();
    }

    void DashboardPage::RevenueChart_Draw(Windows::Foundation::IInspectable const&,
        CanvasDrawEventArgs const& args)
    {
        auto drawingSession = args.DrawingSession();
        auto size = RevenueChart().Size();

        float width = size.Width;
        float height = size.Height;
        float padding = 40.0f;

        if (m_revenueValues.empty())
        {
            drawingSession.DrawText(L"Chưa có dữ liệu", width / 2 - 60, height / 2 - 10, Colors::Gray());
            return;
        }

        int64_t maxVal = 0;
        for (auto v : m_revenueValues)
            if (v > maxVal) maxVal = v;

        if (maxVal == 0) maxVal = 1;

        float chartWidth = width - 2 * padding;
        float chartHeight = height - 2 * padding;
        float barWidth = chartWidth / m_revenueValues.size() * 0.7f;
        float barGap = chartWidth / m_revenueValues.size() * 0.3f;

        // Draw axis
        drawingSession.DrawLine(padding, height - padding, width - padding, height - padding, Colors::Gray(), 1);
        drawingSession.DrawLine(padding, padding, padding, height - padding, Colors::Gray(), 1);

        // Draw bars
        for (size_t i = 0; i < m_revenueValues.size(); i++)
        {
            float barHeight = (float)m_revenueValues[i] / maxVal * chartHeight;
            float x = padding + (float)i * (barWidth + barGap) + barGap / 2;
            float y = height - padding - barHeight;

            Windows::UI::Color barColor;
            barColor.A = 200;
            barColor.R = 0;
            barColor.G = 120;
            barColor.B = 212;

            drawingSession.FillRectangle(x, y, barWidth, barHeight, barColor);

            // Draw label
            if (i < m_revenueLabels.size())
            {
                drawingSession.DrawText(m_revenueLabels[i].c_str(), x, height - padding + 5, Colors::Gray());
            }

            // Draw value on top
            std::wstring valStr = std::to_wstring(m_revenueValues[i]);
            drawingSession.DrawText(valStr.c_str(), x, y - 20, Colors::Black());
        }
    }
}

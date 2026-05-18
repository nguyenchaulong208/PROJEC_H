#pragma once
#include "ReportPage.g.h"
#include "ViewModels/ReportViewModel.h"
#include <memory>

namespace winrt::MyShop2025::Views::implementation
{
    struct ReportPage : ReportPageT<ReportPage>
    {
        ReportPage();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void PeriodCombo_SelectionChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void ViewReportButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void SalesChart_Draw(Windows::Foundation::IInspectable const& sender,
            Microsoft::Graphics::Canvas::UI::Xaml::CanvasDrawEventArgs const& args);
        void RevenueChart_Draw(Windows::Foundation::IInspectable const& sender,
            Microsoft::Graphics::Canvas::UI::Xaml::CanvasDrawEventArgs const& args);
        void ProfitChart_Draw(Windows::Foundation::IInspectable const& sender,
            Microsoft::Graphics::Canvas::UI::Xaml::CanvasDrawEventArgs const& args);

    private:
        std::shared_ptr<ViewModels::ReportViewModel> m_viewModel;
        void LoadChart(const std::wstring& chartName);
        void DrawChart(Microsoft::Graphics::Canvas::CanvasDrawingSession& drawingSession,
            float width, float height,
            const std::vector<ViewModels::ChartDataPoint>& data);
    };
}

namespace winrt::MyShop2025::Views::factory_implementation
{
    struct ReportPage : ReportPageT<ReportPage, implementation::ReportPage>
    {
    };
}

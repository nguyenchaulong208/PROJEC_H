#pragma once
#include "DashboardPage.g.h"
#include "ViewModels/DashboardViewModel.h"
#include <memory>

namespace winrt::MyShop2025::Views::implementation
{
    struct DashboardPage : DashboardPageT<DashboardPage>
    {
        DashboardPage();
        ~DashboardPage() = default;

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void RevenueChart_Draw(Windows::Foundation::IInspectable const& sender,
            Microsoft::Graphics::Canvas::UI::Xaml::CanvasDrawEventArgs const& args);

    private:
        std::shared_ptr<ViewModels::DashboardViewModel> m_viewModel;
        void BindData();
        std::vector<int64_t> m_revenueValues;
        std::vector<std::wstring> m_revenueLabels;
    };
}

namespace winrt::MyShop2025::Views::factory_implementation
{
    struct DashboardPage : DashboardPageT<DashboardPage, implementation::DashboardPage>
    {
    };
}

#pragma once
#include "SettingsPage.g.h"
#include "ViewModels/SettingsViewModel.h"
#include <memory>

namespace winrt::MyShop2025::Views::implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage>
    {
        SettingsPage();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void PageSizeCombo_SelectionChanged(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void SaveButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        std::shared_ptr<ViewModels::SettingsViewModel> m_viewModel;
        void LoadDisplay();
    };
}

namespace winrt::MyShop2025::Views::factory_implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage, implementation::SettingsPage>
    {
    };
}

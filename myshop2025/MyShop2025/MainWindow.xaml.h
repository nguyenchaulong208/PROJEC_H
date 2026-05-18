#pragma once
#include "MainWindow.g.h"
#include <memory>

namespace winrt::MyShop2025::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        ~MainWindow() = default;

        void MainNavigation_Loaded(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void MainNavigation_ItemInvoked(Microsoft::UI::Xaml::Controls::NavigationView const& sender,
            Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args);

        void NavigateToPage(const std::wstring& pageTag);
        void OnLoginSuccess();
    };
}

namespace winrt::MyShop2025::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}

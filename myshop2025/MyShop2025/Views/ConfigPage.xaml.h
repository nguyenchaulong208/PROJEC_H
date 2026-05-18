#pragma once
#include "ConfigPage.g.h"

namespace winrt::MyShop2025::Views::implementation
{
    struct ConfigPage : ConfigPageT<ConfigPage>
    {
        ConfigPage() { InitializeComponent(); }

        void TestButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void SaveButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void BackButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::MyShop2025::Views::factory_implementation
{
    struct ConfigPage : ConfigPageT<ConfigPage, implementation::ConfigPage>
    {
    };
}

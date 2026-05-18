#pragma once

#include "App.xaml.g.h"

namespace winrt::MyShop2025::implementation
{
    struct App : AppT<App>
    {
        App();
        ~App() = default;
        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);
    };
}

namespace winrt::MyShop2025::factory_implementation
{
    struct App : AppT<App, implementation::App>
    {
    };
}

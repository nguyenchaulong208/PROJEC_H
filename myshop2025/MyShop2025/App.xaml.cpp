#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include "Services/DatabaseService.h"
#include "Services/AuthService.h"
#include "Services/NavigationService.h"
#include "Services/SeedDataService.h"

namespace winrt::MyShop2025::implementation
{
    App::App()
    {
        Initialize();

        // Initialize services
        auto dbSvc = std::make_unique<Services::DatabaseService>();
        auto authSvc = std::make_unique<Services::AuthService>();
        auto navSvc = std::make_unique<Services::NavigationService>();

        // Initialize database
        if (dbSvc->Initialize())
        {
            // Seed sample data if needed
            Services::SeedDataService::SeedDatabase();
        }
    }

    void App::OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&)
    {
        auto window = make<MainWindow>();
        window.Activate();
    }
}

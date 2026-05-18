#include "pch.h"
#include "MainWindow.xaml.h"
#include "Services/DatabaseService.h"
#include "Services/NavigationService.h"
#include "Services/AuthService.h"
#include "Views/LoginPage.xaml.h"
#include "Views/DashboardPage.xaml.h"
#include "Views/ProductsPage.xaml.h"
#include "Views/OrdersPage.xaml.h"
#include "Views/ReportPage.xaml.h"
#include "Views/SettingsPage.xaml.h"

namespace winrt::MyShop2025::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();

        auto navSvc = Services::NavigationService::Instance();
        navSvc->SetFrame(ContentFrame());
        navSvc->SetLoginFrame(LoginFrame());

        LoginFrame().Navigate(xaml_typename<Views::LoginPage>());
    }

    void MainWindow::OnLoginSuccess()
    {
        LoginContainer().Visibility(Windows::UI::Xaml::Visibility::Collapsed);
        MainContainer().Visibility(Windows::UI::Xaml::Visibility::Visible);

        NavDashboard.IsSelected(true);
        NavigateToPage(L"Dashboard");
    }

    void MainWindow::MainNavigation_Loaded(Windows::Foundation::IInspectable const&,
        Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (dbSvc)
        {
            auto settings = dbSvc->GetSettings();
            auto lastScreen = settings.lastScreen;
            NavigateToPage(lastScreen);
        }
    }

    void MainWindow::MainNavigation_ItemInvoked(Microsoft::UI::Xaml::Controls::NavigationView const&,
        Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args)
    {
        auto item = args.InvokedItemContainer();
        if (item)
        {
            auto tag = item.Tag().as<winrt::hstring>();
            NavigateToPage(tag.c_str());

            auto dbSvc = Services::DatabaseService::Instance();
            if (dbSvc)
            {
                dbSvc->SetSetting(L"last_screen", tag.c_str());
            }
        }
    }

    void MainWindow::NavigateToPage(const std::wstring& pageTag)
    {
        auto frame = ContentFrame();

        if (pageTag == L"Dashboard")
            frame.Navigate(xaml_typename<Views::DashboardPage>());
        else if (pageTag == L"Products")
            frame.Navigate(xaml_typename<Views::ProductsPage>());
        else if (pageTag == L"Orders")
            frame.Navigate(xaml_typename<Views::OrdersPage>());
        else if (pageTag == L"Report")
            frame.Navigate(xaml_typename<Views::ReportPage>());
        else if (pageTag == L"Settings")
            frame.Navigate(xaml_typename<Views::SettingsPage>());
        else
            frame.Navigate(xaml_typename<Views::DashboardPage>());
    }
}

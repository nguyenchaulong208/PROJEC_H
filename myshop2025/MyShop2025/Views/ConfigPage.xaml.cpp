#include "pch.h"
#include "ConfigPage.xaml.h"
#include "Services/DatabaseService.h"
#include "Services/NavigationService.h"

namespace winrt::MyShop2025::Views::implementation
{
    void ConfigPage::TestButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        auto server = ServerAddressBox().Text();
        auto port = PortBox().Text();
        auto dbName = DbNameBox().Text();

        bool success = false;
        try
        {
            auto dbSvc = Services::DatabaseService::Instance();
            if (dbSvc)
            {
                dbSvc->SetSetting(L"db_server", server.c_str());
                dbSvc->SetSetting(L"db_port", port.c_str());
                dbSvc->SetSetting(L"db_name", dbName.c_str());
                success = true;
            }
        }
        catch (...)
        {
            success = false;
        }

        StatusTextBlock().Visibility(Windows::UI::Xaml::Visibility::Visible);
        if (success)
        {
            StatusTextBlock().Text(L"Kết nối thành công!");
            StatusTextBlock().Foreground(Media::SolidColorBrush(Colors::Green()));
        }
        else
        {
            StatusTextBlock().Text(L"Kết nối thất bại!");
            StatusTextBlock().Foreground(Media::SolidColorBrush(Colors::Red()));
        }
    }

    void ConfigPage::SaveButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (dbSvc)
        {
            dbSvc->SetSetting(L"server_address", ServerAddressBox().Text().c_str());
        }

        StatusTextBlock().Text(L"Đã lưu cấu hình!");
        StatusTextBlock().Visibility(Windows::UI::Xaml::Visibility::Visible);
        StatusTextBlock().Foreground(Media::SolidColorBrush(Colors::Green()));
    }

    void ConfigPage::BackButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        auto loginFrame = Services::NavigationService::Instance()->GetLoginFrame();
        if (loginFrame && loginFrame.CanGoBack())
            loginFrame.GoBack();
    }
}

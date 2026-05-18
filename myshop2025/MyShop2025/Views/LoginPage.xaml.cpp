#include "pch.h"
#include "LoginPage.xaml.h"
#include "Services/AuthService.h"
#include "Services/DatabaseService.h"
#include "Services/NavigationService.h"
#include "Helpers/Crypto.h"
#include "MainWindow.xaml.h"

namespace winrt::MyShop2025::Views::implementation
{
    LoginPage::LoginPage()
    {
        InitializeComponent();
        m_viewModel = std::make_shared<ViewModels::LoginViewModel>();
        VersionTextBlock().Text(m_viewModel->Version().c_str());
    }

    void LoginPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&)
    {
        TryAutoLogin();
    }

    void LoginPage::TryAutoLogin()
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (!dbSvc) return;

        auto remember = dbSvc->GetSetting(L"remember_password", L"false");
        if (remember == L"true")
        {
            auto username = dbSvc->GetSetting(L"saved_username", L"");
            auto encryptedPass = dbSvc->GetSetting(L"saved_password", L"");
            if (!username.empty() && !encryptedPass.empty())
            {
                auto password = Helpers::Crypto::Decrypt(encryptedPass);
                auto auth = Services::AuthService::Instance();
                if (auth && auth->Login(username, password))
                {
                    if (auto parent = this->Parent())
                    {
                        // Find MainWindow and call OnLoginSuccess
                    }
                }
            }
        }
    }

    void LoginPage::LoginButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        m_viewModel->Username(UsernameTextBox().Text().c_str());
        m_viewModel->Password(PasswordBox().Password().c_str());
        m_viewModel->RememberPassword(RememberCheckBox().IsChecked().GetBoolean());

        if (m_viewModel->TryLogin())
        {
            auto window = Microsoft::UI::Xaml::Window::Current().as<MainWindow>();
            if (window)
                window.OnLoginSuccess();
        }
        else
        {
            ErrorTextBlock().Text(m_viewModel->ErrorMessage().c_str());
            ErrorTextBlock().Visibility(Windows::UI::Xaml::Visibility::Visible);
        }
    }

    void LoginPage::ConfigButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        auto loginFrame = Services::NavigationService::Instance()->GetLoginFrame();
        if (loginFrame)
            loginFrame.Navigate(xaml_typename<Views::ConfigPage>());
    }
}

#pragma once
#include "LoginPage.g.h"
#include "ViewModels/LoginViewModel.h"
#include <memory>

namespace winrt::MyShop2025::Views::implementation
{
    struct LoginPage : LoginPageT<LoginPage>
    {
        LoginPage();
        ~LoginPage() = default;

        void LoginButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ConfigButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        std::shared_ptr<ViewModels::LoginViewModel> m_viewModel;
        void TryAutoLogin();
    };
}

namespace winrt::MyShop2025::Views::factory_implementation
{
    struct LoginPage : LoginPageT<LoginPage, implementation::LoginPage>
    {
    };
}

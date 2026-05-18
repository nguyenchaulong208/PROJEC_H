#pragma once
#include "ViewModelBase.h"

namespace winrt::MyShop2025::ViewModels
{
    class LoginViewModel : public ViewModelBase
    {
    private:
        std::wstring m_username;
        std::wstring m_password;
        std::wstring m_errorMessage;
        std::wstring m_version;
        bool m_rememberPassword = false;
        bool m_isLoading = false;

    public:
        LoginViewModel()
        {
            m_version = L"MyShop 2025 v1.0.0";
        }

        std::wstring Username() const { return m_username; }
        void Username(const std::wstring& value)
        {
            if (m_username != value)
            {
                m_username = value;
                RaisePropertyChanged(L"Username");
            }
        }

        std::wstring Password() const { return m_password; }
        void Password(const std::wstring& value)
        {
            if (m_password != value)
            {
                m_password = value;
                RaisePropertyChanged(L"Password");
            }
        }

        std::wstring ErrorMessage() const { return m_errorMessage; }
        void ErrorMessage(const std::wstring& value)
        {
            if (m_errorMessage != value)
            {
                m_errorMessage = value;
                RaisePropertyChanged(L"ErrorMessage");
                RaisePropertyChanged(L"HasError");
            }
        }

        std::wstring Version() const { return m_version; }

        bool RememberPassword() const { return m_rememberPassword; }
        void RememberPassword(bool value)
        {
            if (m_rememberPassword != value)
            {
                m_rememberPassword = value;
                RaisePropertyChanged(L"RememberPassword");
            }
        }

        bool IsLoading() const { return m_isLoading; }
        void IsLoading(bool value)
        {
            if (m_isLoading != value)
            {
                m_isLoading = value;
                RaisePropertyChanged(L"IsLoading");
            }
        }

        bool HasError() const { return !m_errorMessage.empty(); }

        bool TryLogin();
    };
}

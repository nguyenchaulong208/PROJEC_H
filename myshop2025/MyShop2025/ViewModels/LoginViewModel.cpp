#include "pch.h"
#include "LoginViewModel.h"
#include "Services/AuthService.h"
#include "Services/DatabaseService.h"

namespace winrt::MyShop2025::ViewModels
{
    bool LoginViewModel::TryLogin()
    {
        IsLoading(true);
        ErrorMessage(L"");

        auto auth = Services::AuthService::Instance();
        if (auth && auth->Login(m_username, m_password))
        {
            auto dbSvc = Services::DatabaseService::Instance();
            if (dbSvc)
            {
                if (m_rememberPassword)
                {
                    dbSvc->SetSetting(L"remember_password", L"true");
                    dbSvc->SetSetting(L"saved_username", m_username);
                    auto encrypted = Helpers::Crypto::Encrypt(m_password);
                    dbSvc->SetSetting(L"saved_password", encrypted);
                }
                else
                {
                    dbSvc->SetSetting(L"remember_password", L"false");
                    dbSvc->SetSetting(L"saved_username", L"");
                    dbSvc->SetSetting(L"saved_password", L"");
                }
            }
            IsLoading(false);
            return true;
        }

        ErrorMessage(L"Sai tên đăng nhập hoặc mật khẩu!");
        IsLoading(false);
        return false;
    }
}

#include "pch.h"
#include "AuthService.h"
#include "DatabaseService.h"
#include "Helpers/Crypto.h"

namespace MyShop2025::Services
{
    AuthService* AuthService::s_instance = nullptr;

    AuthService::AuthService()
    {
        s_instance = this;
    }

    AuthService::~AuthService()
    {
        s_instance = nullptr;
    }

    AuthService* AuthService::Instance()
    {
        return s_instance;
    }

    bool AuthService::Login(const std::wstring& username, const std::wstring& password)
    {
        std::wstring defaultUser = L"admin";
        std::wstring defaultPass = L"admin123";

        auto dbSvc = DatabaseService::Instance();
        if (dbSvc)
        {
            auto savedPassHash = dbSvc->GetSetting(L"password_hash", L"");
            auto savedUser = dbSvc->GetSetting(L"username", L"admin");

            if (!savedPassHash.empty())
            {
                defaultUser = savedUser;
                defaultPass = Helpers::Crypto::Decrypt(savedPassHash);
            }
        }

        if (username == defaultUser && password == defaultPass)
        {
            m_loggedIn = true;
            m_username = username;
            return true;
        }
        return false;
    }

    void AuthService::Logout()
    {
        m_loggedIn = false;
        m_username.clear();
    }

    bool AuthService::CheckTrialMode()
    {
        auto dbSvc = DatabaseService::Instance();
        if (!dbSvc) return true;

        auto firstRun = dbSvc->GetSetting(L"first_run_date", L"");
        if (firstRun.empty())
        {
            auto now = std::chrono::system_clock::now();
            auto tt = std::chrono::system_clock::to_time_t(now);
            std::tm tm;
            localtime_s(&tm, &tt);
            std::wstringstream wss;
            wss << std::put_time(&tm, L"%Y-%m-%d");
            dbSvc->SetSetting(L"first_run_date", wss.str());
            return true;
        }

        std::wistringstream wiss(firstRun);
        std::tm tm = {};
        wiss >> std::get_time(&tm, L"%Y-%m-%d");
        auto firstDate = std::chrono::system_clock::from_time_t(_mktime32(&tm));
        auto now = std::chrono::system_clock::now();
        auto days = std::chrono::duration_cast<std::chrono::hours>(now - firstDate).count() / 24;
        return days < 15;
    }
}

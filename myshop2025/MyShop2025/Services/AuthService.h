#pragma once
#include <string>

namespace MyShop2025::Services
{
    class AuthService
    {
    private:
        static AuthService* s_instance;
        bool m_loggedIn = false;
        std::wstring m_username;

    public:
        AuthService();
        ~AuthService();

        static AuthService* Instance();

        bool Login(const std::wstring& username, const std::wstring& password);
        void Logout();
        bool IsLoggedIn() const { return m_loggedIn; }
        std::wstring GetUsername() const { return m_username; }
        bool CheckTrialMode();
    };
}

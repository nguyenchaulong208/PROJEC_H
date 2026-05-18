#pragma once
#include <string>

namespace MyShop2025::Models
{
    struct AppSettings
    {
        std::wstring serverAddress;
        int pageSize = 10;
        std::wstring lastScreen;
        bool autoSave = true;
        bool rememberPassword = false;
        std::wstring savedUsername;
        std::wstring savedPassword;
    };
}

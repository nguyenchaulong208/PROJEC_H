#pragma once
#include <string>

namespace MyShop2025::Services
{
    class SeedDataService
    {
    public:
        static bool SeedDatabase();
        static bool IsDatabaseSeeded();
    };
}

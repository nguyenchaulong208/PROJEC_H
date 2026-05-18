#pragma once
#include <string>
#include <vector>

namespace MyShop2025::Helpers
{
    class Crypto
    {
    public:
        static std::wstring Encrypt(const std::wstring& plainText, const std::wstring& key = L"MyShop2025Secret");
        static std::wstring Decrypt(const std::wstring& cipherText, const std::wstring& key = L"MyShop2025Secret");
        static std::wstring Base64Encode(const std::vector<unsigned char>& data);
        static std::vector<unsigned char> Base64Decode(const std::wstring& encoded);
    };
}

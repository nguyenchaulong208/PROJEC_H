#include "pch.h"
#include "Crypto.h"

namespace MyShop2025::Helpers
{
    std::wstring Crypto::Encrypt(const std::wstring& plainText, const std::wstring& key)
    {
        std::vector<unsigned char> data;
        for (size_t i = 0; i < plainText.size(); i++)
        {
            wchar_t c = plainText[i] ^ key[i % key.size()];
            data.push_back(static_cast<unsigned char>((c >> 8) & 0xFF));
            data.push_back(static_cast<unsigned char>(c & 0xFF));
        }
        return Base64Encode(data);
    }

    std::wstring Crypto::Decrypt(const std::wstring& cipherText, const std::wstring& key)
    {
        auto data = Base64Decode(cipherText);
        std::wstring result;
        for (size_t i = 0; i + 1 < data.size(); i += 2)
        {
            wchar_t c = (static_cast<wchar_t>(data[i]) << 8) | data[i + 1];
            result += c ^ key[i / 2 % key.size()];
        }
        return result;
    }

    static const wchar_t* BASE64_CHARS = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::wstring Crypto::Base64Encode(const std::vector<unsigned char>& data)
    {
        std::wstring result;
        int val = 0, valb = -6;
        for (unsigned char c : data)
        {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0)
            {
                result += BASE64_CHARS[(val >> valb) & 0x3F];
                valb -= 6;
            }
        }
        if (valb > -6)
            result += BASE64_CHARS[((val << 8) >> (valb + 8)) & 0x3F];
        while (result.size() % 4)
            result += L'=';
        return result;
    }

    static int Base64Index(wchar_t c)
    {
        if (c >= L'A' && c <= L'Z') return c - L'A';
        if (c >= L'a' && c <= L'z') return c - L'a' + 26;
        if (c >= L'0' && c <= L'9') return c - L'0' + 52;
        if (c == L'+') return 62;
        if (c == L'/') return 63;
        return -1;
    }

    std::vector<unsigned char> Crypto::Base64Decode(const std::wstring& encoded)
    {
        std::vector<unsigned char> result;
        int val = 0, valb = -8;
        for (wchar_t c : encoded)
        {
            if (c == L'=') break;
            int idx = Base64Index(c);
            if (idx < 0) continue;
            val = (val << 6) + idx;
            valb += 6;
            if (valb >= 0)
            {
                result.push_back(static_cast<unsigned char>((val >> valb) & 0xFF));
                valb -= 8;
            }
        }
        return result;
    }
}

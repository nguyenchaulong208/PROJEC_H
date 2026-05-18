#pragma once
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

namespace winrt::MyShop2025::Converters
{
    struct StatusToStringConverter : winrt::implements<StatusToStringConverter, Windows::UI::Xaml::Data::IValueConverter>
    {
        Windows::Foundation::IInspectable Convert(
            Windows::Foundation::IInspectable const& value,
            Windows::UI::Xaml::Interop::TypeName const&,
            Windows::Foundation::IInspectable const&,
            hstring const&)
        {
            if (!value) return box_value(L"Chưa xác định");
            int status = unbox_value<int>(value);
            switch (status)
            {
            case 0: return box_value(L"Mới tạo");
            case 1: return box_value(L"Đã thanh toán");
            case 2: return box_value(L"Đã hủy");
            default: return box_value(L"Chưa xác định");
            }
        }

        Windows::Foundation::IInspectable ConvertBack(
            Windows::Foundation::IInspectable const&,
            Windows::UI::Xaml::Interop::TypeName const&,
            Windows::Foundation::IInspectable const&,
            hstring const&)
        {
            return nullptr;
        }
    };

    struct BoolToVisibilityConverter : winrt::implements<BoolToVisibilityConverter, Windows::UI::Xaml::Data::IValueConverter>
    {
        Windows::Foundation::IInspectable Convert(
            Windows::Foundation::IInspectable const& value,
            Windows::UI::Xaml::Interop::TypeName const&,
            Windows::Foundation::IInspectable const& parameter,
            hstring const&)
        {
            if (!value) return box_value(Windows::UI::Xaml::Visibility::Collapsed);
            bool val = unbox_value<bool>(value);
            bool invert = false;
            if (parameter)
            {
                auto paramStr = unbox_value<hstring>(parameter);
                invert = (paramStr == L"Invert");
            }
            auto vis = (val ^ invert) ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
            return box_value(vis);
        }

        Windows::Foundation::IInspectable ConvertBack(
            Windows::Foundation::IInspectable const&,
            Windows::UI::Xaml::Interop::TypeName const&,
            Windows::Foundation::IInspectable const&,
            hstring const&)
        {
            return nullptr;
        }
    };

    struct MoneyFormatConverter : winrt::implements<MoneyFormatConverter, Windows::UI::Xaml::Data::IValueConverter>
    {
        Windows::Foundation::IInspectable Convert(
            Windows::Foundation::IInspectable const& value,
            Windows::UI::Xaml::Interop::TypeName const&,
            Windows::Foundation::IInspectable const&,
            hstring const&)
        {
            if (!value) return box_value(L"0 ₫");
            int64_t val = unbox_value<int64_t>(value);
            std::wstringstream wss;
            wss.imbue(std::locale("vi-VN"));
            wss << val << L" ₫";
            return box_value(hstring(wss.str()));
        }

        Windows::Foundation::IInspectable ConvertBack(
            Windows::Foundation::IInspectable const&,
            Windows::UI::Xaml::Interop::TypeName const&,
            Windows::Foundation::IInspectable const&,
            hstring const&)
        {
            return nullptr;
        }
    };
}

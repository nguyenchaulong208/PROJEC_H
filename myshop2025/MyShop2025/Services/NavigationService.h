#pragma once
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <functional>

namespace MyShop2025::Services
{
    class NavigationService
    {
    private:
        static NavigationService* s_instance;
        Microsoft::UI::Xaml::Controls::Frame m_frame{ nullptr };
        Microsoft::UI::Xaml::Controls::Frame m_loginFrame{ nullptr };

    public:
        NavigationService();
        ~NavigationService();

        static NavigationService* Instance();
        void SetFrame(Microsoft::UI::Xaml::Controls::Frame frame);
        Microsoft::UI::Xaml::Controls::Frame GetFrame() { return m_frame; }
        void SetLoginFrame(Microsoft::UI::Xaml::Controls::Frame frame);
        Microsoft::UI::Xaml::Controls::Frame GetLoginFrame() { return m_loginFrame; }
        bool Navigate(Windows::UI::Xaml::Interop::TypeName pageType);
        bool GoBack();
    };
}

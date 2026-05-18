#include "pch.h"
#include "NavigationService.h"

namespace MyShop2025::Services
{
    NavigationService* NavigationService::s_instance = nullptr;

    NavigationService::NavigationService()
    {
        s_instance = this;
    }

    NavigationService::~NavigationService()
    {
        s_instance = nullptr;
    }

    NavigationService* NavigationService::Instance()
    {
        return s_instance;
    }

    void NavigationService::SetFrame(Microsoft::UI::Xaml::Controls::Frame frame)
    {
        m_frame = frame;
    }

    void NavigationService::SetLoginFrame(Microsoft::UI::Xaml::Controls::Frame frame)
    {
        m_loginFrame = frame;
    }

    bool NavigationService::Navigate(Windows::UI::Xaml::Interop::TypeName pageType)
    {
        if (m_frame)
        {
            return m_frame.Navigate(pageType);
        }
        return false;
    }

    bool NavigationService::GoBack()
    {
        if (m_frame && m_frame.CanGoBack())
        {
            m_frame.GoBack();
            return true;
        }
        return false;
    }
}

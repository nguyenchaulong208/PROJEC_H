#pragma once
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.Foundation.h>

namespace winrt::MyShop2025::ViewModels
{
    class ViewModelBase
        : public winrt::implements<ViewModelBase, Windows::UI::Xaml::Data::INotifyPropertyChanged>
    {
    protected:
        Windows::UI::Xaml::Data::PropertyChangedEventArgs m_emptyArgs{ L"" };
        winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;

        void RaisePropertyChanged(const std::wstring& propertyName)
        {
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ propertyName });
        }

    public:
        winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler handler)
        {
            return m_propertyChanged.add(handler);
        }

        void PropertyChanged(winrt::event_token token)
        {
            m_propertyChanged.remove(token);
        }
    };
}

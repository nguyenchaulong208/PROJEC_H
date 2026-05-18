#pragma once
#include "ViewModelBase.h"
#include "Models/AppSettings.h"

namespace winrt::MyShop2025::ViewModels
{
    class SettingsViewModel : public ViewModelBase
    {
    private:
        Models::AppSettings m_settings;
        std::vector<int> m_pageSizeOptions;
        int m_selectedPageSizeIndex = 1;

    public:
        SettingsViewModel();

        Models::AppSettings CurrentSettings() const { return m_settings; }

        std::wstring ServerAddress() const { return m_settings.serverAddress; }
        void ServerAddress(const std::wstring& value)
        {
            m_settings.serverAddress = value;
            RaisePropertyChanged(L"ServerAddress");
        }

        int PageSize() const { return m_settings.pageSize; }
        void PageSize(int value)
        {
            m_settings.pageSize = value;
            RaisePropertyChanged(L"PageSize");
        }

        bool AutoSave() const { return m_settings.autoSave; }
        void AutoSave(bool value)
        {
            m_settings.autoSave = value;
            RaisePropertyChanged(L"AutoSave");
        }

        std::vector<int> PageSizeOptions() const { return m_pageSizeOptions; }

        int SelectedPageSizeIndex() const { return m_selectedPageSizeIndex; }
        void SelectedPageSizeIndex(int value)
        {
            m_selectedPageSizeIndex = value;
            RaisePropertyChanged(L"SelectedPageSizeIndex");
            if (value >= 0 && value < (int)m_pageSizeOptions.size())
                PageSize(m_pageSizeOptions[value]);
        }

        void LoadSettings();
        void SaveSettings();
    };
}

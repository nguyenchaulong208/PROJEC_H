#include "pch.h"
#include "SettingsViewModel.h"
#include "Services/DatabaseService.h"

namespace winrt::MyShop2025::ViewModels
{
    SettingsViewModel::SettingsViewModel()
    {
        m_pageSizeOptions = { 5, 10, 15, 20 };
        LoadSettings();
    }

    void SettingsViewModel::LoadSettings()
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (dbSvc)
        {
            m_settings = dbSvc->GetSettings();
            RaisePropertyChanged(L"ServerAddress");
            RaisePropertyChanged(L"PageSize");
            RaisePropertyChanged(L"AutoSave");

            for (size_t i = 0; i < m_pageSizeOptions.size(); i++)
            {
                if (m_pageSizeOptions[i] == m_settings.pageSize)
                {
                    m_selectedPageSizeIndex = (int)i;
                    break;
                }
            }
            RaisePropertyChanged(L"SelectedPageSizeIndex");
        }
    }

    void SettingsViewModel::SaveSettings()
    {
        auto dbSvc = Services::DatabaseService::Instance();
        if (dbSvc)
        {
            dbSvc->SaveSettings(m_settings);
        }
    }
}

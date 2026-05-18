#include "pch.h"
#include "SettingsPage.xaml.h"

namespace winrt::MyShop2025::Views::implementation
{
    SettingsPage::SettingsPage()
    {
        InitializeComponent();
        m_viewModel = std::make_shared<ViewModels::SettingsViewModel>();
    }

    void SettingsPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&)
    {
        LoadDisplay();
    }

    void SettingsPage::LoadDisplay()
    {
        m_viewModel->LoadSettings();

        for (int i = 0; i < 4; i++)
        {
            if ((i == 0 && m_viewModel->PageSize() == 5) ||
                (i == 1 && m_viewModel->PageSize() == 10) ||
                (i == 2 && m_viewModel->PageSize() == 15) ||
                (i == 3 && m_viewModel->PageSize() == 20))
            {
                PageSizeCombo().SelectedIndex(i);
                break;
            }
        }

        AutoSaveToggle().IsOn(m_viewModel->AutoSave());
    }

    void SettingsPage::PageSizeCombo_SelectionChanged(Windows::Foundation::IInspectable const&,
        Controls::SelectionChangedEventArgs const&)
    {
        switch (PageSizeCombo().SelectedIndex())
        {
        case 0: m_viewModel->PageSize(5); break;
        case 1: m_viewModel->PageSize(10); break;
        case 2: m_viewModel->PageSize(15); break;
        case 3: m_viewModel->PageSize(20); break;
        }
    }

    void SettingsPage::SaveButton_Click(Windows::Foundation::IInspectable const&,
        Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        m_viewModel->AutoSave(AutoSaveToggle().IsOn());
        m_viewModel->SaveSettings();

        StatusText().Text(L"Đã lưu cấu hình thành công!");
        StatusText().Visibility(Windows::UI::Xaml::Visibility::Visible);
    }
}

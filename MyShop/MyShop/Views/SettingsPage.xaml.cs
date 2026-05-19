using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using MyShop.ViewModels;

namespace MyShop.Views;

public sealed partial class SettingsPage : Page
{
    private readonly SettingsViewModel _vm;

    public SettingsPage()
    {
        InitializeComponent();
        _vm = new SettingsViewModel(App.ConfigService!);
        ItemsPerPageCombo.ItemsSource = _vm.ItemsPerPageOptions;
        LastScreenCombo.ItemsSource = _vm.ScreenOptions;

        ItemsPerPageCombo.SelectedItem = _vm.SelectedItemsPerPage;
        LastScreenCombo.SelectedItem = _vm.SelectedLastScreen;
    }

    private void OnSaveClick(object sender, RoutedEventArgs e)
    {
        _vm.SelectedItemsPerPage = (int)ItemsPerPageCombo.SelectedItem;
        _vm.SelectedLastScreen = (string)LastScreenCombo.SelectedItem;
        _vm.SaveSettings();
        StatusText.Text = "Đã lưu cài đặt!";
        StatusText.Visibility = Visibility.Visible;
    }
}

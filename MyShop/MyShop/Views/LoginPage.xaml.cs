using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using MyShop.Helpers;
using MyShop.ViewModels;

namespace MyShop.Views;

public sealed partial class LoginPage : Page
{
    private readonly LoginViewModel _vm;

    public LoginPage()
    {
        InitializeComponent();
        _vm = new LoginViewModel(
            App.AuthService!,
            App.ConfigService!,
            App.DatabaseService!);
        VersionText.Text = "Phiên bản 1.0.0";
        _vm.LoadSavedCredentials();

        if (!string.IsNullOrEmpty(_vm.Username))
            UsernameBox.Text = _vm.Username;
        if (!string.IsNullOrEmpty(_vm.Password))
            PasswordBox.Password = _vm.Password;
        RememberCheck.IsChecked = _vm.RememberMe;

        _ = TryAutoLoginAsync();
    }

    private async Task TryAutoLoginAsync()
    {
        var cfg = App.ConfigService!.LoadConfig();
        if (cfg.RememberLogin && !string.IsNullOrEmpty(cfg.SavedUsername))
        {
            UsernameBox.Text = cfg.SavedUsername;
            PasswordBox.Password = cfg.SavedPassword;
            await DoLoginAsync();
        }
    }

    private async void OnLoginClick(object sender, RoutedEventArgs e)
    {
        await DoLoginAsync();
    }

    private async Task DoLoginAsync()
    {
        _vm.Username = UsernameBox.Text;
        _vm.Password = PasswordBox.Password;
        _vm.RememberMe = RememberCheck.IsChecked ?? false;

        var success = await _vm.LoginAsync();
        if (success)
        {
            NavigationHelper.MainFrame?.Navigate(typeof(ShellPage));
        }
        else
        {
            ErrorText.Text = _vm.ErrorMessage;
            ErrorText.Visibility = Visibility.Visible;
        }
    }

    private void OnConfigClick(object sender, RoutedEventArgs e)
    {
        NavigationHelper.MainFrame?.Navigate(typeof(ConfigPage));
    }
}

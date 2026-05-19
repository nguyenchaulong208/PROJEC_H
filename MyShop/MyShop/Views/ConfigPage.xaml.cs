using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using MyShop.Helpers;
using MyShop.Models;
using MyShop.Services;

namespace MyShop.Views;

public sealed partial class ConfigPage : Page
{
    private readonly IConfigService _configService;
    private readonly DatabaseService _dbService;

    public ConfigPage()
    {
        InitializeComponent();
        _configService = App.ConfigService!;
        _dbService = App.DatabaseService!;
        LoadConfig();
    }

    private void LoadConfig()
    {
        var cfg = _configService.LoadConfig();
        ServerBox.Text = cfg.ServerAddress;
        PortBox.Text = cfg.Port.ToString();
        DbBox.Text = cfg.Database;
        UserBox.Text = cfg.Username;
        PassBox.Password = cfg.Password;
    }

    private async void OnTestClick(object sender, RoutedEventArgs e)
    {
        try
        {
            var cfg = new AppConfig
            {
                ServerAddress = ServerBox.Text,
                Port = int.Parse(PortBox.Text),
                Database = DbBox.Text,
                Username = UserBox.Text,
                Password = PassBox.Password
            };
            _dbService.UpdateConnectionString(cfg);
            await _dbService.InitializeDatabaseAsync();
            StatusText.Text = "Kết nối thành công!";
            StatusText.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.Green);
        }
        catch (Exception ex)
        {
            StatusText.Text = $"Lỗi: {ex.Message}";
            StatusText.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.Red);
        }
        StatusText.Visibility = Visibility.Visible;
    }

    private void OnSaveClick(object sender, RoutedEventArgs e)
    {
        var cfg = _configService.LoadConfig();
        cfg.ServerAddress = ServerBox.Text;
        cfg.Port = int.TryParse(PortBox.Text, out var p) ? p : 5432;
        cfg.Database = DbBox.Text;
        cfg.Username = UserBox.Text;
        cfg.Password = PassBox.Password;
        _configService.SaveConfig(cfg);
        StatusText.Text = "Đã lưu cấu hình!";
        StatusText.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.Green);
        StatusText.Visibility = Visibility.Visible;
    }

    private void OnBackClick(object sender, RoutedEventArgs e)
    {
        NavigationHelper.MainFrame?.Navigate(typeof(LoginPage));
    }
}

using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MyShop.Services;
using MyShop.Models;

namespace MyShop.ViewModels;

public partial class SettingsViewModel : BaseViewModel
{
    private readonly IConfigService _configService;

    [ObservableProperty]
    private int _selectedItemsPerPage = 10;

    [ObservableProperty]
    private string _selectedLastScreen = "Dashboard";

    [ObservableProperty]
    private string _statusMessage = string.Empty;

    public List<int> ItemsPerPageOptions { get; } = new() { 5, 10, 15, 20 };

    public List<string> ScreenOptions { get; } = new()
    {
        "Dashboard", "Products", "Orders", "Report", "Settings"
    };

    public SettingsViewModel(IConfigService configService)
    {
        _configService = configService;
        LoadSettings();
    }

    private void LoadSettings()
    {
        var cfg = _configService.LoadConfig();
        SelectedItemsPerPage = cfg.ItemsPerPage;
        SelectedLastScreen = cfg.LastScreen;
    }

    public void SaveSettings()
    {
        var cfg = _configService.LoadConfig();
        cfg.ItemsPerPage = SelectedItemsPerPage;
        cfg.LastScreen = SelectedLastScreen;
        _configService.SaveConfig(cfg);
        StatusMessage = "Đã lưu cấu hình!";
    }
}

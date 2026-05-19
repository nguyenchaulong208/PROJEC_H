using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MyShop.Models;
using MyShop.Services;

namespace MyShop.ViewModels;

public partial class ConfigViewModel : BaseViewModel
{
    private readonly IConfigService _configService;
    private readonly DatabaseService _dbService;

    [ObservableProperty]
    private string _serverAddress = "localhost";

    [ObservableProperty]
    private int _port = 5432;

    [ObservableProperty]
    private string _database = "myshop";

    [ObservableProperty]
    private string _username = "postgres";

    [ObservableProperty]
    private string _password = string.Empty;

    [ObservableProperty]
    private string _testResult = string.Empty;

    [ObservableProperty]
    private bool _isTestSuccess;

    public ConfigViewModel(IConfigService configService, DatabaseService dbService)
    {
        _configService = configService;
        _dbService = dbService;
        LoadConfig();
    }

    private void LoadConfig()
    {
        var cfg = _configService.LoadConfig();
        ServerAddress = cfg.ServerAddress;
        Port = cfg.Port;
        Database = cfg.Database;
        Username = cfg.Username;
        Password = cfg.Password;
    }

    [RelayCommand]
    private async Task TestConnectionAsync()
    {
        IsLoading = true;
        ClearError();
        try
        {
            var cfg = new AppConfig
            {
                ServerAddress = ServerAddress,
                Port = Port,
                Database = Database,
                Username = Username,
                Password = Password
            };
            _dbService.UpdateConnectionString(cfg);
            await _dbService.InitializeDatabaseAsync();
            IsTestSuccess = true;
            TestResult = "Kết nối thành công!";
        }
        catch (Exception ex)
        {
            IsTestSuccess = false;
            TestResult = $"Lỗi: {ex.Message}";
        }
        finally
        {
            IsLoading = false;
        }
    }

    [RelayCommand]
    private void SaveConfig()
    {
        var cfg = _configService.LoadConfig();
        cfg.ServerAddress = ServerAddress;
        cfg.Port = Port;
        cfg.Database = Database;
        cfg.Username = Username;
        cfg.Password = Password;
        _configService.SaveConfig(cfg);
        TestResult = "Đã lưu cấu hình!";
        IsTestSuccess = true;
    }
}

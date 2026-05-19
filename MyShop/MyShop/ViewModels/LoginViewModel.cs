using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MyShop.Helpers;
using MyShop.Services;

namespace MyShop.ViewModels;

public partial class LoginViewModel : BaseViewModel
{
    private readonly IAuthService _authService;
    private readonly IConfigService _configService;
    private readonly DatabaseService _dbService;

    [ObservableProperty]
    private string _username = string.Empty;

    [ObservableProperty]
    private string _password = string.Empty;

    [ObservableProperty]
    private bool _rememberMe;

    [ObservableProperty]
    private string _versionText = "MyShop 2025 v1.0.0";

    public bool AutoLoginAttempted { get; private set; }

    public LoginViewModel(IAuthService authService, IConfigService configService, DatabaseService dbService)
    {
        _authService = authService;
        _configService = configService;
        _dbService = dbService;
    }

    public void LoadSavedCredentials()
    {
        var cfg = _configService.LoadConfig();
        if (cfg.RememberLogin)
        {
            Username = cfg.SavedUsername;
            Password = cfg.SavedPassword;
            RememberMe = true;
        }
    }

    public async Task<bool> TryAutoLoginAsync()
    {
        AutoLoginAttempted = true;
        var cfg = _configService.LoadConfig();
        if (!cfg.RememberLogin || string.IsNullOrEmpty(cfg.SavedUsername))
            return false;

        Username = cfg.SavedUsername;
        Password = cfg.SavedPassword;
        RememberMe = true;
        return await LoginAsync();
    }

    public async Task<bool> LoginAsync()
    {
        ClearError();
        if (string.IsNullOrWhiteSpace(Username) || string.IsNullOrWhiteSpace(Password))
        {
            SetError("Vui lòng nhập tên đăng nhập và mật khẩu");
            return false;
        }

        IsLoading = true;
        try
        {
            var result = await _authService.LoginAsync(Username, Password);
            if (result)
            {
                var cfg = _configService.LoadConfig();
                if (RememberMe)
                {
                    cfg.SavedUsername = Username;
                    cfg.SavedPassword = Password;
                    cfg.RememberLogin = true;
                }
                else
                {
                    cfg.SavedUsername = "";
                    cfg.SavedPassword = "";
                    cfg.RememberLogin = false;
                }
                _configService.SaveConfig(cfg);
                return true;
            }
            SetError("Tên đăng nhập hoặc mật khẩu không đúng");
            return false;
        }
        catch (Exception ex)
        {
            SetError($"Lỗi kết nối: {ex.Message}");
            return false;
        }
        finally
        {
            IsLoading = false;
        }
    }

    [RelayCommand]
    private void OpenConfig()
    {
        NavigationHelper.NavigateTo("Config");
    }
}

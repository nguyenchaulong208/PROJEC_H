namespace MyShop.Services;

public interface IAuthService
{
    Task<bool> LoginAsync(string username, string password);
    string? CurrentUser { get; }
    bool IsLoggedIn { get; }
}

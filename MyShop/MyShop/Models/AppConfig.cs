namespace MyShop.Models;

public class AppConfig
{
    public string ServerAddress { get; set; } = "localhost";
    public int Port { get; set; } = 5432;
    public string Database { get; set; } = "myshop";
    public string Username { get; set; } = "postgres";
    public string Password { get; set; } = "1234";
    public int ItemsPerPage { get; set; } = 10;
    public string LastScreen { get; set; } = "Dashboard";
    public string SavedUsername { get; set; } = "";
    public string SavedPassword { get; set; } = "";
    public bool RememberLogin { get; set; } = false;
}

using System.Security.Cryptography;
using System.Text;
using Npgsql;

namespace MyShop.Services;

public class AuthService : IAuthService
{
    private readonly DatabaseService _dbService;
    public string? CurrentUser { get; private set; }
    public bool IsLoggedIn => CurrentUser != null;

    public AuthService(DatabaseService dbService)
    {
        _dbService = dbService;
    }

    public async Task<bool> LoginAsync(string username, string password)
    {
        try
        {
            using var conn = await _dbService.GetConnectionAsync();
            using var cmd = conn.CreateCommand();
            cmd.CommandText = "SELECT password_hash FROM app_user WHERE username = @u";
            cmd.Parameters.Add(new NpgsqlParameter("@u", username));

            var result = await cmd.ExecuteScalarAsync();
            if (result == null) return false;

            var hash = result.ToString();
            var inputHash = HashPassword(password);
            if (hash != inputHash) return false;

            CurrentUser = username;
            return true;
        }
        catch
        {
            return false;
        }
    }

    public static string HashPassword(string password)
    {
        var bytes = SHA256.HashData(Encoding.UTF8.GetBytes(password));
        return Convert.ToHexString(bytes).ToLowerInvariant();
    }
}

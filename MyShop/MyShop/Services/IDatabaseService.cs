using Npgsql;

namespace MyShop.Services;

public interface IDatabaseService
{
    Task<NpgsqlConnection> GetConnectionAsync();
    Task InitializeDatabaseAsync();
    Task SeedSampleDataAsync();
}

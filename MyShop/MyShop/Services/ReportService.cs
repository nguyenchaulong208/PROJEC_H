using Npgsql;

namespace MyShop.Services;

public class ReportService : IReportService
{
    private readonly DatabaseService _dbService;

    public ReportService(DatabaseService dbService)
    {
        _dbService = dbService;
    }

    public async Task<List<(string Label, int Value)>> GetSalesByDayAsync(DateTime fromDate, DateTime toDate)
    {
        var list = new List<(string, int)>();
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = @"
            SELECT TO_CHAR(o.created_time, 'YYYY-MM-DD') AS day,
                   COALESCE(SUM(oi.quantity), 0)
            FROM orders o
            JOIN order_item oi ON o.order_id = oi.order_id
            WHERE o.created_time::date >= @from AND o.created_time::date <= @to
              AND o.status = 'Đã thanh toán'
            GROUP BY day ORDER BY day";
        cmd.Parameters.Add(new NpgsqlParameter("@from", fromDate));
        cmd.Parameters.Add(new NpgsqlParameter("@to", toDate));

        using var reader = await cmd.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            list.Add((reader.GetString(0), reader.GetInt32(1)));
        }
        return list;
    }

    public async Task<List<(string Label, int Value)>> GetSalesByWeekAsync(int year)
    {
        var list = new List<(string, int)>();
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = @"
            SELECT 'Tuần ' || EXTRACT(WEEK FROM o.created_time)::text AS week,
                   COALESCE(SUM(oi.quantity), 0)
            FROM orders o
            JOIN order_item oi ON o.order_id = oi.order_id
            WHERE EXTRACT(YEAR FROM o.created_time) = @year
              AND o.status = 'Đã thanh toán'
            GROUP BY EXTRACT(WEEK FROM o.created_time)
            ORDER BY EXTRACT(WEEK FROM o.created_time)";
        cmd.Parameters.Add(new NpgsqlParameter("@year", year));

        using var reader = await cmd.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            list.Add((reader.GetString(0), reader.GetInt32(1)));
        }
        return list;
    }

    public async Task<List<(string Label, int Value)>> GetSalesByMonthAsync(int year)
    {
        var list = new List<(string, int)>();
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = @"
            SELECT TO_CHAR(o.created_time, 'MM-YYYY') AS month,
                   COALESCE(SUM(oi.quantity), 0)
            FROM orders o
            JOIN order_item oi ON o.order_id = oi.order_id
            WHERE EXTRACT(YEAR FROM o.created_time) = @year
              AND o.status = 'Đã thanh toán'
            GROUP BY month ORDER BY month";
        cmd.Parameters.Add(new NpgsqlParameter("@year", year));

        using var reader = await cmd.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            list.Add((reader.GetString(0), reader.GetInt32(1)));
        }
        return list;
    }

    public async Task<List<(string Label, int Value)>> GetSalesByYearAsync()
    {
        var list = new List<(string, int)>();
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = @"
            SELECT EXTRACT(YEAR FROM o.created_time)::text AS year,
                   COALESCE(SUM(oi.quantity), 0)
            FROM orders o
            JOIN order_item oi ON o.order_id = oi.order_id
            WHERE o.status = 'Đã thanh toán'
            GROUP BY year ORDER BY year";

        using var reader = await cmd.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            list.Add((reader.GetString(0), reader.GetInt32(1)));
        }
        return list;
    }

    public async Task<List<(string Label, int Revenue, int Profit)>> GetRevenueProfitByDayAsync(DateTime fromDate, DateTime toDate)
    {
        var list = new List<(string, int, int)>();
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = @"
            SELECT TO_CHAR(o.created_time, 'YYYY-MM-DD') AS day,
                   COALESCE(SUM(o.final_price), 0) AS revenue,
                   COALESCE(SUM(o.final_price - (oi.quantity * p.import_price)), 0) AS profit
            FROM orders o
            JOIN order_item oi ON o.order_id = oi.order_id
            JOIN product p ON oi.product_id = p.product_id
            WHERE o.created_time::date >= @from AND o.created_time::date <= @to
              AND o.status = 'Đã thanh toán'
            GROUP BY day ORDER BY day";
        cmd.Parameters.Add(new NpgsqlParameter("@from", fromDate));
        cmd.Parameters.Add(new NpgsqlParameter("@to", toDate));

        using var reader = await cmd.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            list.Add((reader.GetString(0), reader.GetInt32(1), reader.GetInt32(2)));
        }
        return list;
    }

    public async Task<List<(string Label, int Revenue, int Profit)>> GetRevenueProfitByMonthAsync(int year)
    {
        var list = new List<(string, int, int)>();
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = @"
            SELECT TO_CHAR(o.created_time, 'MM-YYYY') AS month,
                   COALESCE(SUM(o.final_price), 0) AS revenue,
                   COALESCE(SUM(o.final_price - (oi.quantity * p.import_price)), 0) AS profit
            FROM orders o
            JOIN order_item oi ON o.order_id = oi.order_id
            JOIN product p ON oi.product_id = p.product_id
            WHERE EXTRACT(YEAR FROM o.created_time) = @year
              AND o.status = 'Đã thanh toán'
            GROUP BY month ORDER BY month";
        cmd.Parameters.Add(new NpgsqlParameter("@year", year));

        using var reader = await cmd.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            list.Add((reader.GetString(0), reader.GetInt32(1), reader.GetInt32(2)));
        }
        return list;
    }
}

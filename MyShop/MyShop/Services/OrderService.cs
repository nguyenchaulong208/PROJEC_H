using System.Text;
using Npgsql;
using MyShop.Models;

namespace MyShop.Services;

public class OrderService : IOrderService
{
    private readonly DatabaseService _dbService;

    public OrderService(DatabaseService dbService)
    {
        _dbService = dbService;
    }

    public async Task<List<Order>> GetOrdersAsync(DateTime? fromDate = null, DateTime? toDate = null,
        string? status = null, int page = 1, int pageSize = 10)
    {
        var sql = new StringBuilder(@"
            SELECT o.order_id, o.created_time, o.final_price, o.status
            FROM orders o WHERE 1=1");
        var parameters = new List<NpgsqlParameter>();

        if (fromDate.HasValue)
        {
            sql.Append(" AND o.created_time >= @from");
            parameters.Add(new NpgsqlParameter("@from", fromDate.Value));
        }
        if (toDate.HasValue)
        {
            sql.Append(" AND o.created_time <= @to");
            parameters.Add(new NpgsqlParameter("@to", toDate.Value.AddDays(1)));
        }
        if (!string.IsNullOrWhiteSpace(status))
        {
            sql.Append(" AND o.status = @status");
            parameters.Add(new NpgsqlParameter("@status", status));
        }

        sql.Append(" ORDER BY o.created_time DESC LIMIT @limit OFFSET @offset");
        parameters.Add(new NpgsqlParameter("@limit", pageSize));
        parameters.Add(new NpgsqlParameter("@offset", (page - 1) * pageSize));

        var list = new List<Order>();
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = sql.ToString();
        foreach (var p in parameters)
            cmd.Parameters.Add(p);

        using var reader = await cmd.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            list.Add(new Order
            {
                OrderId = reader.GetInt32(0),
                CreatedTime = reader.GetDateTime(1),
                FinalPrice = reader.GetInt32(2),
                Status = reader.GetString(3)
            });
        }
        return list;
    }

    public async Task<int> GetOrderCountAsync(DateTime? fromDate = null, DateTime? toDate = null, string? status = null)
    {
        var sql = new StringBuilder("SELECT COUNT(*) FROM orders o WHERE 1=1");
        var parameters = new List<NpgsqlParameter>();

        if (fromDate.HasValue)
        {
            sql.Append(" AND o.created_time >= @from");
            parameters.Add(new NpgsqlParameter("@from", fromDate.Value));
        }
        if (toDate.HasValue)
        {
            sql.Append(" AND o.created_time <= @to");
            parameters.Add(new NpgsqlParameter("@to", toDate.Value.AddDays(1)));
        }
        if (!string.IsNullOrWhiteSpace(status))
        {
            sql.Append(" AND o.status = @status");
            parameters.Add(new NpgsqlParameter("@status", status));
        }

        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = sql.ToString();
        foreach (var p in parameters)
            cmd.Parameters.Add(p);
        return Convert.ToInt32(await cmd.ExecuteScalarAsync());
    }

    public async Task<Order?> GetOrderByIdAsync(int id)
    {
        Order? order = null;
        using (var conn = await _dbService.GetConnectionAsync())
        {
            using var cmd = conn.CreateCommand();
            cmd.CommandText = @"
                SELECT o.order_id, o.created_time, o.final_price, o.status
                FROM orders o WHERE o.order_id = @id";
            cmd.Parameters.Add(new NpgsqlParameter("@id", id));

            using var reader = await cmd.ExecuteReaderAsync();
            if (await reader.ReadAsync())
            {
                order = new Order
                {
                    OrderId = reader.GetInt32(0),
                    CreatedTime = reader.GetDateTime(1),
                    FinalPrice = reader.GetInt32(2),
                    Status = reader.GetString(3)
                };
            }
        }

        if (order == null) return null;

        using var conn2 = await _dbService.GetConnectionAsync();
        using var itemCmd = conn2.CreateCommand();
        itemCmd.CommandText = @"
            SELECT oi.order_item_id, oi.quantity, oi.unit_sale_price, oi.total_price,
                   oi.product_id, COALESCE(p.name, '') AS product_name
            FROM order_item oi
            LEFT JOIN product p ON oi.product_id = p.product_id
            WHERE oi.order_id = @oid";
        itemCmd.Parameters.Add(new NpgsqlParameter("@oid", id));

        using var itemReader = await itemCmd.ExecuteReaderAsync();
        while (await itemReader.ReadAsync())
        {
            order.Items.Add(new OrderItem
            {
                OrderItemId = itemReader.GetInt32(0),
                Quantity = itemReader.GetInt32(1),
                UnitSalePrice = itemReader.GetDouble(2),
                TotalPrice = itemReader.GetInt32(3),
                ProductId = itemReader.GetInt32(4),
                ProductName = itemReader.GetString(5),
                OrderId = id
            });
        }
        return order;
    }

    public async Task<int> CreateOrderAsync(Order order)
    {
        using var conn = await _dbService.GetConnectionAsync();
        using var tx = conn.BeginTransaction();

        try
        {
            using var cmd = conn.CreateCommand();
            cmd.Transaction = tx;
            cmd.CommandText = @"
                INSERT INTO orders (created_time, final_price, status)
                VALUES (@time, @price, @status) RETURNING order_id";
            cmd.Parameters.Add(new NpgsqlParameter("@time", order.CreatedTime));
            cmd.Parameters.Add(new NpgsqlParameter("@price", order.FinalPrice));
            cmd.Parameters.Add(new NpgsqlParameter("@status", order.Status));
            var orderId = Convert.ToInt32(await cmd.ExecuteScalarAsync());

            foreach (var item in order.Items)
            {
                using var itemCmd = conn.CreateCommand();
                itemCmd.Transaction = tx;
                itemCmd.CommandText = @"
                    INSERT INTO order_item (quantity, unit_sale_price, total_price, order_id, product_id)
                    VALUES (@qty, @price, @total, @oid, @pid)";
                itemCmd.Parameters.Add(new NpgsqlParameter("@qty", item.Quantity));
                itemCmd.Parameters.Add(new NpgsqlParameter("@price", item.UnitSalePrice));
                itemCmd.Parameters.Add(new NpgsqlParameter("@total", item.TotalPrice));
                itemCmd.Parameters.Add(new NpgsqlParameter("@oid", orderId));
                itemCmd.Parameters.Add(new NpgsqlParameter("@pid", item.ProductId));
                await itemCmd.ExecuteNonQueryAsync();

                using var updateCmd = conn.CreateCommand();
                updateCmd.Transaction = tx;
                updateCmd.CommandText = "UPDATE product SET count = count - @qty WHERE product_id = @pid";
                updateCmd.Parameters.Add(new NpgsqlParameter("@qty", item.Quantity));
                updateCmd.Parameters.Add(new NpgsqlParameter("@pid", item.ProductId));
                await updateCmd.ExecuteNonQueryAsync();
            }

            tx.Commit();
            return orderId;
        }
        catch
        {
            tx.Rollback();
            throw;
        }
    }

    public async Task UpdateOrderAsync(Order order)
    {
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "UPDATE orders SET final_price = @price, status = @status WHERE order_id = @id";
        cmd.Parameters.Add(new NpgsqlParameter("@id", order.OrderId));
        cmd.Parameters.Add(new NpgsqlParameter("@price", order.FinalPrice));
        cmd.Parameters.Add(new NpgsqlParameter("@status", order.Status));
        await cmd.ExecuteNonQueryAsync();
    }

    public async Task DeleteOrderAsync(int id)
    {
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "DELETE FROM orders WHERE order_id = @id";
        cmd.Parameters.Add(new NpgsqlParameter("@id", id));
        await cmd.ExecuteNonQueryAsync();
    }

    public async Task<int> GetTodayOrderCountAsync()
    {
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "SELECT COUNT(*) FROM orders WHERE created_time::date = CURRENT_DATE";
        return Convert.ToInt32(await cmd.ExecuteScalarAsync());
    }

    public async Task<int> GetTodayRevenueAsync()
    {
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "SELECT COALESCE(SUM(final_price), 0) FROM orders WHERE created_time::date = CURRENT_DATE AND status = 'Đã thanh toán'";
        return Convert.ToInt32(await cmd.ExecuteScalarAsync());
    }

    public async Task<List<Order>> GetRecentOrdersAsync(int count = 3)
    {
        var list = new List<Order>();
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = @"
            SELECT order_id, created_time, final_price, status
            FROM orders ORDER BY created_time DESC LIMIT @cnt";
        cmd.Parameters.Add(new NpgsqlParameter("@cnt", count));

        using var reader = await cmd.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            list.Add(new Order
            {
                OrderId = reader.GetInt32(0),
                CreatedTime = reader.GetDateTime(1),
                FinalPrice = reader.GetInt32(2),
                Status = reader.GetString(3)
            });
        }
        return list;
    }

    public async Task<List<(string Name, int Total)>> GetTopSellingProductsAsync(int top = 5)
    {
        var list = new List<(string, int)>();
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = @"
            SELECT p.name, COALESCE(SUM(oi.quantity), 0) AS total_qty
            FROM order_item oi
            JOIN product p ON oi.product_id = p.product_id
            GROUP BY p.name
            ORDER BY total_qty DESC LIMIT @top";
        cmd.Parameters.Add(new NpgsqlParameter("@top", top));

        using var reader = await cmd.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            list.Add((reader.GetString(0), reader.GetInt32(1)));
        }
        return list;
    }
}

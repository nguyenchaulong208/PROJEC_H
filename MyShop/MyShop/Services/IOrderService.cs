using MyShop.Models;

namespace MyShop.Services;

public interface IOrderService
{
    Task<List<Order>> GetOrdersAsync(DateTime? fromDate = null, DateTime? toDate = null,
        string? status = null, int page = 1, int pageSize = 10);
    Task<int> GetOrderCountAsync(DateTime? fromDate = null, DateTime? toDate = null, string? status = null);
    Task<Order?> GetOrderByIdAsync(int id);
    Task<int> CreateOrderAsync(Order order);
    Task UpdateOrderAsync(Order order);
    Task DeleteOrderAsync(int id);
    Task<int> GetTodayOrderCountAsync();
    Task<int> GetTodayRevenueAsync();
    Task<List<Order>> GetRecentOrdersAsync(int count = 3);
    Task<List<(string Name, int Total)>> GetTopSellingProductsAsync(int top = 5);
}

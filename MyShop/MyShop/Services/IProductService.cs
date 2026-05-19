using MyShop.Models;

namespace MyShop.Services;

public interface IProductService
{
    Task<List<Category>> GetCategoriesAsync();
    Task<List<Product>> GetProductsAsync(int categoryId = 0, string? keyword = null,
        int? minPrice = null, int? maxPrice = null, string? sortBy = null,
        int page = 1, int pageSize = 10);
    Task<int> GetProductCountAsync(int categoryId = 0, string? keyword = null,
        int? minPrice = null, int? maxPrice = null);
    Task<Product?> GetProductByIdAsync(int id);
    Task<int> AddProductAsync(Product product);
    Task UpdateProductAsync(Product product);
    Task DeleteProductAsync(int id);
    Task<int> AddCategoryAsync(Category category);
    Task UpdateCategoryAsync(Category category);
    Task DeleteCategoryAsync(int id);
    Task ImportFromExcelAsync(string filePath);
}

using System.Text;
using DocumentFormat.OpenXml.Packaging;
using DocumentFormat.OpenXml.Spreadsheet;
using Npgsql;
using MyShop.Models;

namespace MyShop.Services;

public class ProductService : IProductService
{
    private readonly DatabaseService _dbService;

    public ProductService(DatabaseService dbService)
    {
        _dbService = dbService;
    }

    public async Task<List<Category>> GetCategoriesAsync()
    {
        var list = new List<Category>();
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "SELECT category_id, name, description FROM category ORDER BY name";
        using var reader = await cmd.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            list.Add(new Category
            {
                CategoryId = reader.GetInt32(0),
                Name = reader.GetString(1),
                Description = reader.IsDBNull(2) ? "" : reader.GetString(2)
            });
        }
        return list;
    }

    public async Task<List<Product>> GetProductsAsync(int categoryId = 0, string? keyword = null,
        int? minPrice = null, int? maxPrice = null, string? sortBy = null,
        int page = 1, int pageSize = 10)
    {
        var sql = new StringBuilder(@"
            SELECT p.product_id, p.sku, p.name, p.import_price, p.sale_price, p.count,
                   p.description, p.category_id, COALESCE(c.name, '') AS cat_name
            FROM product p
            LEFT JOIN category c ON p.category_id = c.category_id
            WHERE 1=1");
        var parameters = new List<NpgsqlParameter>();

        if (categoryId > 0)
        {
            sql.Append(" AND p.category_id = @catId");
            parameters.Add(new NpgsqlParameter("@catId", categoryId));
        }
        if (!string.IsNullOrWhiteSpace(keyword))
        {
            sql.Append(" AND (p.name ILIKE @kw OR p.sku ILIKE @kw)");
            parameters.Add(new NpgsqlParameter("@kw", $"%{keyword}%"));
        }
        if (minPrice.HasValue)
        {
            sql.Append(" AND p.sale_price >= @min");
            parameters.Add(new NpgsqlParameter("@min", minPrice.Value));
        }
        if (maxPrice.HasValue)
        {
            sql.Append(" AND p.sale_price <= @max");
            parameters.Add(new NpgsqlParameter("@max", maxPrice.Value));
        }

        sql.Append(sortBy switch
        {
            "name_asc" => " ORDER BY p.name ASC",
            "name_desc" => " ORDER BY p.name DESC",
            "price_asc" => " ORDER BY p.sale_price ASC",
            "price_desc" => " ORDER BY p.sale_price DESC",
            "count_asc" => " ORDER BY p.count ASC",
            "count_desc" => " ORDER BY p.count DESC",
            _ => " ORDER BY p.product_id DESC"
        });

        sql.Append(" LIMIT @limit OFFSET @offset");
        parameters.Add(new NpgsqlParameter("@limit", pageSize));
        parameters.Add(new NpgsqlParameter("@offset", (page - 1) * pageSize));

        var list = new List<Product>();
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = sql.ToString();
        foreach (var p in parameters)
            cmd.Parameters.Add(p);

        using var reader = await cmd.ExecuteReaderAsync();
        while (await reader.ReadAsync())
        {
            list.Add(new Product
            {
                ProductId = reader.GetInt32(0),
                Sku = reader.GetString(1),
                Name = reader.GetString(2),
                ImportPrice = reader.GetInt32(3),
                SalePrice = reader.GetInt32(4),
                Count = reader.GetInt32(5),
                Description = reader.IsDBNull(6) ? "" : reader.GetString(6),
                CategoryId = reader.IsDBNull(7) ? 0 : reader.GetInt32(7),
                CategoryName = reader.GetString(8)
            });
        }
        return list;
    }

    public async Task<int> GetProductCountAsync(int categoryId = 0, string? keyword = null,
        int? minPrice = null, int? maxPrice = null)
    {
        var sql = new StringBuilder("SELECT COUNT(*) FROM product p WHERE 1=1");
        var parameters = new List<NpgsqlParameter>();

        if (categoryId > 0)
        {
            sql.Append(" AND p.category_id = @catId");
            parameters.Add(new NpgsqlParameter("@catId", categoryId));
        }
        if (!string.IsNullOrWhiteSpace(keyword))
        {
            sql.Append(" AND (p.name ILIKE @kw OR p.sku ILIKE @kw)");
            parameters.Add(new NpgsqlParameter("@kw", $"%{keyword}%"));
        }
        if (minPrice.HasValue)
        {
            sql.Append(" AND p.sale_price >= @min");
            parameters.Add(new NpgsqlParameter("@min", minPrice.Value));
        }
        if (maxPrice.HasValue)
        {
            sql.Append(" AND p.sale_price <= @max");
            parameters.Add(new NpgsqlParameter("@max", maxPrice.Value));
        }

        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = sql.ToString();
        foreach (var p in parameters)
            cmd.Parameters.Add(p);
        return Convert.ToInt32(await cmd.ExecuteScalarAsync());
    }

    public async Task<Product?> GetProductByIdAsync(int id)
    {
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = @"
            SELECT p.product_id, p.sku, p.name, p.import_price, p.sale_price, p.count,
                   p.description, p.category_id, COALESCE(c.name, '') AS cat_name
            FROM product p
            LEFT JOIN category c ON p.category_id = c.category_id
            WHERE p.product_id = @id";
        cmd.Parameters.Add(new NpgsqlParameter("@id", id));

        using var reader = await cmd.ExecuteReaderAsync();
        if (await reader.ReadAsync())
        {
            return new Product
            {
                ProductId = reader.GetInt32(0),
                Sku = reader.GetString(1),
                Name = reader.GetString(2),
                ImportPrice = reader.GetInt32(3),
                SalePrice = reader.GetInt32(4),
                Count = reader.GetInt32(5),
                Description = reader.IsDBNull(6) ? "" : reader.GetString(6),
                CategoryId = reader.IsDBNull(7) ? 0 : reader.GetInt32(7),
                CategoryName = reader.GetString(8)
            };
        }
        return null;
    }

    public async Task<int> AddProductAsync(Product product)
    {
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = @"
            INSERT INTO product (sku, name, import_price, sale_price, count, description, category_id)
            VALUES (@sku, @name, @import, @sale, @count, @desc, @catId)
            RETURNING product_id";
        cmd.Parameters.Add(new NpgsqlParameter("@sku", product.Sku));
        cmd.Parameters.Add(new NpgsqlParameter("@name", product.Name));
        cmd.Parameters.Add(new NpgsqlParameter("@import", product.ImportPrice));
        cmd.Parameters.Add(new NpgsqlParameter("@sale", product.SalePrice));
        cmd.Parameters.Add(new NpgsqlParameter("@count", product.Count));
        cmd.Parameters.Add(new NpgsqlParameter("@desc", product.Description));
        cmd.Parameters.Add(new NpgsqlParameter("@catId", product.CategoryId > 0 ? product.CategoryId : DBNull.Value));

        return Convert.ToInt32(await cmd.ExecuteScalarAsync());
    }

    public async Task UpdateProductAsync(Product product)
    {
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = @"
            UPDATE product SET sku = @sku, name = @name, import_price = @import,
                sale_price = @sale, count = @count, description = @desc, category_id = @catId
            WHERE product_id = @id";
        cmd.Parameters.Add(new NpgsqlParameter("@id", product.ProductId));
        cmd.Parameters.Add(new NpgsqlParameter("@sku", product.Sku));
        cmd.Parameters.Add(new NpgsqlParameter("@name", product.Name));
        cmd.Parameters.Add(new NpgsqlParameter("@import", product.ImportPrice));
        cmd.Parameters.Add(new NpgsqlParameter("@sale", product.SalePrice));
        cmd.Parameters.Add(new NpgsqlParameter("@count", product.Count));
        cmd.Parameters.Add(new NpgsqlParameter("@desc", product.Description));
        cmd.Parameters.Add(new NpgsqlParameter("@catId", product.CategoryId > 0 ? product.CategoryId : DBNull.Value));
        await cmd.ExecuteNonQueryAsync();
    }

    public async Task DeleteProductAsync(int id)
    {
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "DELETE FROM product WHERE product_id = @id";
        cmd.Parameters.Add(new NpgsqlParameter("@id", id));
        await cmd.ExecuteNonQueryAsync();
    }

    public async Task<int> AddCategoryAsync(Category category)
    {
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "INSERT INTO category (name, description) VALUES (@name, @desc) RETURNING category_id";
        cmd.Parameters.Add(new NpgsqlParameter("@name", category.Name));
        cmd.Parameters.Add(new NpgsqlParameter("@desc", category.Description));
        return Convert.ToInt32(await cmd.ExecuteScalarAsync());
    }

    public async Task UpdateCategoryAsync(Category category)
    {
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "UPDATE category SET name = @name, description = @desc WHERE category_id = @id";
        cmd.Parameters.Add(new NpgsqlParameter("@id", category.CategoryId));
        cmd.Parameters.Add(new NpgsqlParameter("@name", category.Name));
        cmd.Parameters.Add(new NpgsqlParameter("@desc", category.Description));
        await cmd.ExecuteNonQueryAsync();
    }

    public async Task DeleteCategoryAsync(int id)
    {
        using var conn = await _dbService.GetConnectionAsync();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = "DELETE FROM category WHERE category_id = @id";
        cmd.Parameters.Add(new NpgsqlParameter("@id", id));
        await cmd.ExecuteNonQueryAsync();
    }

    public async Task ImportFromExcelAsync(string filePath)
    {
        using var document = SpreadsheetDocument.Open(filePath, false);
        var workbookPart = document.WorkbookPart!;
        var sheet = workbookPart.Workbook.Sheets!.GetFirstChild<Sheet>()!;
        var worksheetPart = (WorksheetPart)workbookPart.GetPartById(sheet.Id!);
        var rows = worksheetPart.Worksheet.GetFirstChild<SheetData>()!
            .Elements<Row>().Skip(1);

        using var conn = await _dbService.GetConnectionAsync();
        foreach (var row in rows)
        {
            var cells = row.Elements<Cell>().ToList();
            if (cells.Count < 7) continue;

            var sku = GetCellValue(cells[0], workbookPart);
            var name = GetCellValue(cells[1], workbookPart);
            var importStr = GetCellValue(cells[2], workbookPart);
            var saleStr = GetCellValue(cells[3], workbookPart);
            var countStr = GetCellValue(cells[4], workbookPart);
            var catName = GetCellValue(cells[5], workbookPart);
            var desc = GetCellValue(cells[6], workbookPart);

            if (!int.TryParse(importStr, out var importPrice)) importPrice = 0;
            if (!int.TryParse(saleStr, out var salePrice)) salePrice = 0;
            if (!int.TryParse(countStr, out var count)) count = 0;

            int catId = 0;
            if (!string.IsNullOrWhiteSpace(catName))
            {
                using var catCmd = conn.CreateCommand();
                catCmd.CommandText = "SELECT category_id FROM category WHERE name = @n";
                catCmd.Parameters.Add(new NpgsqlParameter("@n", catName));
                var result = await catCmd.ExecuteScalarAsync();
                if (result != null)
                {
                    catId = Convert.ToInt32(result);
                }
                else
                {
                    using var insCat = conn.CreateCommand();
                    insCat.CommandText = "INSERT INTO category (name) VALUES (@n) RETURNING category_id";
                    insCat.Parameters.Add(new NpgsqlParameter("@n", catName));
                    catId = Convert.ToInt32(await insCat.ExecuteScalarAsync());
                }
            }

            using var insCmd = conn.CreateCommand();
            insCmd.CommandText = @"
                INSERT INTO product (sku, name, import_price, sale_price, count, description, category_id)
                VALUES (@sku, @name, @import, @sale, @count, @desc, @catId)
                ON CONFLICT (sku) DO UPDATE SET
                    name = EXCLUDED.name, import_price = EXCLUDED.import_price,
                    sale_price = EXCLUDED.sale_price, count = EXCLUDED.count,
                    description = EXCLUDED.description, category_id = EXCLUDED.category_id";
            insCmd.Parameters.Add(new NpgsqlParameter("@sku", sku));
            insCmd.Parameters.Add(new NpgsqlParameter("@name", name));
            insCmd.Parameters.Add(new NpgsqlParameter("@import", importPrice));
            insCmd.Parameters.Add(new NpgsqlParameter("@sale", salePrice));
            insCmd.Parameters.Add(new NpgsqlParameter("@count", count));
            insCmd.Parameters.Add(new NpgsqlParameter("@desc", desc));
            insCmd.Parameters.Add(new NpgsqlParameter("@catId", catId > 0 ? catId : DBNull.Value));
            await insCmd.ExecuteNonQueryAsync();
        }
    }

    private static string GetCellValue(Cell cell, WorkbookPart workbookPart)
    {
        if (cell.CellValue == null) return "";
        var value = cell.CellValue.InnerText;
        if (cell.DataType != null && cell.DataType.Value == CellValues.SharedString)
        {
            var stringTable = workbookPart.SharedStringTablePart!.SharedStringTable;
            value = stringTable.ElementAt(int.Parse(value)).InnerText;
        }
        return value;
    }
}

namespace MyShop.Services;

public interface IReportService
{
    Task<List<(string Label, int Value)>> GetSalesByDayAsync(DateTime fromDate, DateTime toDate);
    Task<List<(string Label, int Value)>> GetSalesByWeekAsync(int year);
    Task<List<(string Label, int Value)>> GetSalesByMonthAsync(int year);
    Task<List<(string Label, int Value)>> GetSalesByYearAsync();
    Task<List<(string Label, int Revenue, int Profit)>> GetRevenueProfitByDayAsync(DateTime fromDate, DateTime toDate);
    Task<List<(string Label, int Revenue, int Profit)>> GetRevenueProfitByMonthAsync(int year);
}

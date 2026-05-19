using Microsoft.UI.Xaml;
using MyShop.Services;
using MyShop.ViewModels;
using MyShop.Views;

namespace MyShop;

public partial class App : Application
{
    public static DatabaseService? DatabaseService { get; private set; }
    public static IConfigService? ConfigService { get; private set; }
    public static IAuthService? AuthService { get; private set; }
    public static IProductService? ProductService { get; private set; }
    public static IOrderService? OrderService { get; private set; }
    public static IReportService? ReportService { get; private set; }
    public static MainWindow? MainWindow { get; private set; }

    public App()
    {
        InitializeComponent();
        UnhandledException += (s, e) =>
        {
            var msg = e.Exception?.ToString() ?? "unknown";
            System.IO.File.WriteAllText(
                System.IO.Path.Combine(
                    Environment.GetFolderPath(Environment.SpecialFolder.Desktop),
                    "MyShop_crash.txt"), msg);
            e.Handled = true;
        };
    }

    protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
    {
        ConfigService = new ConfigService();
        DatabaseService = new Services.DatabaseService(ConfigService);
        AuthService = new AuthService(DatabaseService);
        ProductService = new ProductService(DatabaseService);
        OrderService = new OrderService(DatabaseService);
        ReportService = new ReportService(DatabaseService);

        MainWindow = new MainWindow();
        MainWindow.Activate();
    }
}

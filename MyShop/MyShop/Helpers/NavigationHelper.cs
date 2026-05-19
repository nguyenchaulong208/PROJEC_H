using Microsoft.UI.Xaml.Controls;

namespace MyShop.Helpers;

public static class NavigationHelper
{
    public static Frame? MainFrame { get; set; }

    public static void NavigateTo(string pageKey)
    {
        var type = pageKey switch
        {
            "Dashboard" => typeof(Views.DashboardPage),
            "Products" => typeof(Views.ProductsPage),
            "Orders" => typeof(Views.OrdersPage),
            "Report" => typeof(Views.ReportPage),
            "Settings" => typeof(Views.SettingsPage),
            _ => typeof(Views.DashboardPage)
        };
        MainFrame?.Navigate(type);
    }
}

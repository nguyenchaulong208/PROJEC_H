using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using MyShop.Helpers;
using MyShop.Services;
using MyShop.ViewModels;
using MyShop.Views;

namespace MyShop.Views;

public sealed partial class ShellPage : Page
{
    public string CurrentPage { get; private set; } = "Dashboard";

    public ShellPage()
    {
        InitializeComponent();
        NavigationHelper.MainFrame = ContentFrame;
    }

    private void OnLoaded(object sender, RoutedEventArgs e)
    {
        var cfg = App.ConfigService?.LoadConfig();
        var target = cfg?.LastScreen ?? "Dashboard";
        NavigateToPage(target);
        NavView.SelectedItem = NavView.MenuItems
            .OfType<NavigationViewItem>()
            .FirstOrDefault(i => i.Tag?.ToString() == target);
    }

    private void OnNavItemInvoked(NavigationView sender, NavigationViewItemInvokedEventArgs args)
    {
        if (args.InvokedItemContainer is NavigationViewItem item)
        {
            var tag = item.Tag?.ToString();
            if (tag == "Logout")
            {
                Logout();
                return;
            }
            NavigateToPage(tag);
        }
    }

    private void NavigateToPage(string? tag)
    {
        tag ??= "Dashboard";
        CurrentPage = tag;
        var type = tag switch
        {
            "Dashboard" => typeof(DashboardPage),
            "Products" => typeof(ProductsPage),
            "Orders" => typeof(OrdersPage),
            "Report" => typeof(ReportPage),
            "Settings" => typeof(SettingsPage),
            _ => typeof(DashboardPage)
        };
        ContentFrame.Navigate(type);
    }

    private void Logout()
    {
        var mainWindow = App.MainWindow;
        if (mainWindow != null)
        {
            var mainFrame = mainWindow.RootFrame;
            NavigationHelper.MainFrame = mainFrame;
            mainFrame.Navigate(typeof(LoginPage));
        }
    }

    private void OnBackRequested(NavigationView sender, NavigationViewBackRequestedEventArgs args)
    {
        if (ContentFrame.CanGoBack)
            ContentFrame.GoBack();
    }
}

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using MyShop.Helpers;
using MyShop.Services;
using MyShop.Views;

namespace MyShop;

public sealed partial class MainWindow : Window
{
    public Frame RootFrame => MainFrame;

    public MainWindow()
    {
        InitializeComponent();
        ExtendsContentIntoTitleBar = true;
        Title = "MyShop 2025";

        NavigationHelper.MainFrame = MainFrame;
        MainFrame.Navigate(typeof(LoginPage));
    }

    private void OnClosed(object sender, WindowEventArgs args)
    {
        var cfg = App.ConfigService?.LoadConfig();
        if (cfg != null)
        {
            if (MainFrame.Content is ShellPage shell)
                cfg.LastScreen = shell.CurrentPage;
            App.ConfigService?.SaveConfig(cfg);
        }
    }
}

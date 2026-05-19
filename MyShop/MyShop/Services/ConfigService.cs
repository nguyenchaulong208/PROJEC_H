using Newtonsoft.Json;
using MyShop.Helpers;
using MyShop.Models;

namespace MyShop.Services;

public class ConfigService : IConfigService
{
    private readonly string _configPath;

    public ConfigService()
    {
        var folder = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
            "MyShop");
        Directory.CreateDirectory(folder);
        _configPath = Path.Combine(folder, "config.json");
    }

    public AppConfig LoadConfig()
    {
        if (!File.Exists(_configPath))
            return new AppConfig();

        var encrypted = File.ReadAllText(_configPath);
        try
        {
            var json = CryptoHelper.Decrypt(encrypted);
            return JsonConvert.DeserializeObject<AppConfig>(json) ?? new AppConfig();
        }
        catch
        {
            return new AppConfig();
        }
    }

    public void SaveConfig(AppConfig config)
    {
        var json = JsonConvert.SerializeObject(config);
        var encrypted = CryptoHelper.Encrypt(json);
        File.WriteAllText(_configPath, encrypted);
    }
}

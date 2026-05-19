using MyShop.Models;

namespace MyShop.Services;

public interface IConfigService
{
    AppConfig LoadConfig();
    void SaveConfig(AppConfig config);
}

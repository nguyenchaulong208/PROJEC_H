using System.Security.Cryptography;
using System.Text;

namespace MyShop.Helpers;

public static class CryptoHelper
{
    private static readonly byte[] Key = SHA256.HashData(Encoding.UTF8.GetBytes("MyShop@2025!SecretKey#WinUI"));
    private static readonly byte[] IV = Encoding.UTF8.GetBytes("MyShop2025IV1234");

    public static string Encrypt(string plainText)
    {
        using var aes = Aes.Create();
        aes.Key = Key;
        aes.IV = IV;
        using var encryptor = aes.CreateEncryptor();
        var input = Encoding.UTF8.GetBytes(plainText);
        var output = encryptor.TransformFinalBlock(input, 0, input.Length);
        return Convert.ToBase64String(output);
    }

    public static string Decrypt(string cipherText)
    {
        using var aes = Aes.Create();
        aes.Key = Key;
        aes.IV = IV;
        using var decryptor = aes.CreateDecryptor();
        var input = Convert.FromBase64String(cipherText);
        var output = decryptor.TransformFinalBlock(input, 0, input.Length);
        return Encoding.UTF8.GetString(output);
    }
}

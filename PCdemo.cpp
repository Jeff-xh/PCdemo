#include <windows.h>
#include <string>

int main(int argc, char* argv[]) {
    // 注册 URL Scheme (使用宽字符)
    HKEY hKey;
    LSTATUS result = RegCreateKeyExW( // 使用 RegCreateKeyExW，W 表示 Wide
        HKEY_CURRENT_USER,
        L"Software\\Classes\\pcdemo", // 使用 L 前缀表示宽字符串
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        NULL,
        &hKey,
        NULL
    );

    if (result == ERROR_SUCCESS) {
        const wchar_t* protocolDescription = L"URL:pcdemo Protocol"; // 使用宽字符
        RegSetValueExW(hKey, NULL, 0, REG_SZ, (const BYTE*)protocolDescription, (wcslen(protocolDescription) + 1) * sizeof(wchar_t)); // 使用 RegSetValueExW 和 wcslen
        RegSetValueExW(hKey, L"URL Protocol", 0, REG_SZ, (const BYTE*)L"", 2); // 空字符串需要 2 个字节 (包括结尾的 L'\0')

        HKEY hIconKey;
        RegCreateKeyExW(hKey, L"DefaultIcon", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hIconKey, NULL); // 使用宽字符
        std::wstring iconPath = std::wstring(argv[0], argv[0] + strlen(argv[0])) + L",0"; // 使用程序自身图标，需要转换为宽字符串
        RegSetValueExW(hIconKey, NULL, 0, REG_SZ, (const BYTE*)iconPath.c_str(), (iconPath.length() + 1) * sizeof(wchar_t));
        RegCloseKey(hIconKey);

        HKEY hCommandKey;
        RegCreateKeyExW(hKey, L"shell\\open\\command", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hCommandKey, NULL); // 使用宽字符
        std::wstring command = L"\"" + std::wstring(argv[0], argv[0] + strlen(argv[0])) + L"\" \"\%1\""; // 使用宽字符串
        RegSetValueExW(hCommandKey, NULL, 0, REG_SZ, (const BYTE*)command.c_str(), (command.length() + 1) * sizeof(wchar_t));
        RegCloseKey(hCommandKey);

        RegCloseKey(hKey);

        MessageBoxW(NULL, L"pcdemo:// 已成功注册!", L"注册成功", MB_OK | MB_ICONINFORMATION); // 使用 MessageBoxW
    }
    else {
        MessageBoxW(NULL, L"注册失败!", L"错误", MB_OK | MB_ICONERROR); // 使用 MessageBoxW
    }

    // 检查是否通过 URL Scheme 唤起 (使用宽字符)
    if (argc > 1) {
        //argv[1]的类型是char*，需要把类型转成wchar_t*才能在MessageBoxW中显示
        int num = MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, NULL, 0);
        wchar_t* wide_str = new wchar_t[num];
        MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, wide_str, num);
        std::wstring url(wide_str);
        delete[] wide_str;
        std::wstring message = L"唤起参数: " + url;
        if (url.rfind(L"pcdemo://", 0) == 0) {
            MessageBoxW(NULL, message.c_str(), L"唤起参数", MB_OK | MB_ICONINFORMATION);
        }
    }

    return 0;
}

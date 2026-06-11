#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <dwmapi.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cwctype>
#include <shlwapi.h>
#include <tlhelp32.h>
#include "resource.h"

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "shlwapi.lib")

static const wchar_t* APP_NAME = L"Wabbe Control Center";
static const UINT WM_TRAYICON = WM_APP + 1;
static const UINT TIMER_ID = 1;
static const UINT TIMER_MS = 250;
static const int HOTKEY_TOGGLE_CLEAN = 1;
static const int HOTKEY_PAUSE_ALL = 2;
static const int HOTKEY_STUDIO = 3;

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWCP_ROUND
#define DWMWCP_ROUND 2
#endif

static COLORREF CLR_BG = RGB(18, 18, 24);
static COLORREF CLR_CARD = RGB(34, 36, 46);
static COLORREF CLR_CARD_2 = RGB(44, 47, 60);
static COLORREF CLR_TEXT = RGB(238, 241, 246);
static COLORREF CLR_MUTED = RGB(165, 171, 184);
static COLORREF CLR_ACCENT = RGB(115, 180, 255);
static COLORREF CLR_ACCENT_2 = RGB(74, 130, 255);
static COLORREF CLR_OFF = RGB(83, 88, 105);

#define SWITCH_STYLE (BS_AUTOCHECKBOX | BS_OWNERDRAW)
#define CHECK_STYLE (BS_AUTOCHECKBOX | BS_OWNERDRAW)
#define BTN_STYLE (BS_PUSHBUTTON | BS_OWNERDRAW)

// Tray/menu IDs
#define ID_TRAY_OPEN_PANEL     1001
#define ID_TRAY_TOGGLE_CLEAN   1002
#define ID_TRAY_TOGGLE_ICONS   1003
#define ID_TRAY_TOGGLE_CURSOR  1004
#define ID_TRAY_TOGGLE_TASKBAR 1005
#define ID_TRAY_GAME_MODE      1006
#define ID_TRAY_STUDIO_MODE    1007
#define ID_TRAY_RESTORE        1008
#define ID_TRAY_PAUSE_ALL      1009
#define ID_TRAY_EXIT           1010

// Control IDs
#define IDC_CLEAN_ENABLED      2001
#define IDC_HIDE_ICONS         2002
#define IDC_ICONS_DELAY        2003
#define IDC_HIDE_CURSOR        2004
#define IDC_CURSOR_DELAY       2005
#define IDC_HIDE_TASKBAR       2006
#define IDC_TASKBAR_DELAY      2007
#define IDC_TASKBAR_SAFE       2008
#define IDC_ICONS_RESTORE_DESKTOP 2009
#define IDC_BLOCK_MAX          2010

#define IDC_GAME_ENABLED       2100
#define IDC_GAME_PAUSE_CLEAN   2101
#define IDC_GAME_RESTORE       2102
#define IDC_GAME_STEAM         2103
#define IDC_GAME_XBOX          2104
#define IDC_GAME_ADRENALIN     2105
#define IDC_GAME_POWER         2106

#define IDC_STUDIO_ENABLED     2200
#define IDC_STUDIO_PAUSE_CLEAN 2201
#define IDC_STUDIO_RESTORE     2202
#define IDC_STUDIO_VSCODE      2203
#define IDC_STUDIO_YTMUSIC     2204
#define IDC_STUDIO_BROWSER_EDGE   2205
#define IDC_STUDIO_BROWSER_CHROME 2206
#define IDC_STUDIO_BROWSER_BRAVE  2207
#define IDC_STUDIO_TOGGLE_BTN      2208

#define IDC_SAVE               2301
#define IDC_RESTORE_ALL        2302
#define IDC_STATUS             2303
#define IDC_TAB_CLEAN           2310
#define IDC_TAB_GAME            2311
#define IDC_TAB_STUDIO          2312
#define IDC_TAB_GENERAL         2313
#define IDC_TAB_CUSTOMIZE      2314
#define IDC_THEME_DARK          2320
#define IDC_THEME_LIGHT         2321
#define IDC_THEME_TOGGLE        2322
#define IDC_STARTUP_TOGGLE      2323
#define IDC_START_MINIMIZED     2324
#define IDC_CLOSE_TO_TRAY       2325
#define IDC_RESET_CONFIG       2326
#define IDC_OPEN_CONFIG        2327
#define IDC_SHORTCUT_LABEL     2328
#define IDC_LABEL_COLOR_TITLE  2420
#define IDC_LABEL_CLEAN_GROUP1 2421
#define IDC_LABEL_CLEAN_GROUP2 2422
#define IDC_LABEL_GAME_GROUP   2423
#define IDC_LABEL_STUDIO_GROUP 2424
#define IDC_LABEL_CUSTOM_GROUP 2425
#define IDC_COLOR_BLUE          2401
#define IDC_COLOR_PURPLE        2402
#define IDC_COLOR_SILVER        2403
#define IDC_COLOR_GREEN         2404
#define IDC_COLOR_RED           2405
#define IDC_COLOR_Y2K           2406
#define IDC_SHOW_NOTIFICATIONS  2410
#define IDC_LABEL_ICONS_DELAY   2330
#define IDC_LABEL_ICONS_S       2331
#define IDC_LABEL_CURSOR_DELAY  2332
#define IDC_LABEL_CURSOR_S      2333
#define IDC_LABEL_TASKBAR_DELAY 2334
#define IDC_LABEL_TASKBAR_S     2335
#define IDC_LABEL_YTMUSIC_WITH  2336
#define IDC_LABEL_THEME         2337
#define IDC_LABEL_GENERAL_INFO1 2338
#define IDC_LABEL_GENERAL_INFO2 2339

struct Config
{
    bool cleanEnabled = true;
    bool hideIcons = true;
    int hideIconsDelay = 5;
    bool hideCursor = true;
    int hideCursorDelay = 5;
    bool hideTaskbar = false;
    int hideTaskbarDelay = 5;
    bool taskbarSafeMode = true;
    bool iconsRestoreOnlyOnDesktop = false;
    bool blockWhenMaximized = true;
    bool darkTheme = true;
    bool startMinimized = false;
    bool closeToTray = true;
    bool showNotifications = true;
    int accentColor = 0; // 0 azul, 1 roxo, 2 prata, 3 verde, 4 vermelho, 5 y2k

    bool gameEnabled = false;
    bool gamePauseClean = true;
    bool gameRestoreDesktop = true;
    bool gameOpenSteam = false;
    bool gameOpenXbox = false;
    bool gameOpenAdrenalin = false;
    bool gameHighPerformance = false;

    bool studioEnabled = false;
    bool studioPauseClean = false;
    bool studioRestoreDesktop = false;
    bool studioOpenVSCode = false;
    bool studioOpenYouTubeMusic = true;
    std::wstring studioBrowser = L"edge";
};

HINSTANCE g_hInst = nullptr;
HWND g_hwnd = nullptr;
NOTIFYICONDATAW g_nid{};
Config g_cfg;

void UpdateTabVisibility(HWND hwnd);
bool IsStartupEnabled();
void SetStartupEnabled(bool enabled);
HWND GetDesktopListView();
bool IsDesktopEffectivelyVisible();
void SaveCurrentDelaysFromPanel(HWND hwnd);

bool g_iconsHidden = false;
bool g_cursorHidden = false;
bool g_taskbarHidden = false;
bool g_pausedAll = false;
bool g_waitFreshInputAfterBlocked = false;
DWORD g_lastInputTickDuringBlocked = 0;
DWORD g_appWindowFocusStartTick = 0;
std::vector<DWORD> g_studioPids;
HBRUSH g_bgBrush = nullptr;
HBRUSH g_cardBrush = nullptr;
HBRUSH g_editBrush = nullptr;
HFONT g_titleFont = nullptr;
HFONT g_uiFont = nullptr;
int g_activeTab = 0; // 0 Clean, 1 Jogo, 2 Estudio, 3 Personalizar

std::wstring GetExeDir()
{
    wchar_t path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::wstring s(path);
    size_t pos = s.find_last_of(L"\\/");
    return (pos == std::wstring::npos) ? L"." : s.substr(0, pos);
}

std::wstring ConfigPath()
{
    wchar_t appData[MAX_PATH]{};

    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, appData)))
    {
        std::wstring dir = std::wstring(appData) + L"\\WabbeControlCenter";
        CreateDirectoryW(dir.c_str(), nullptr);
        return dir + L"\\config.ini";
    }

    return GetExeDir() + L"\\config.ini";
}

std::wstring LegacyConfigPath()
{
    return GetExeDir() + L"\\config.ini";
}

void MigrateLegacyConfigIfNeeded()
{
    std::wstring current = ConfigPath();
    std::wstring legacy = LegacyConfigPath();

    if (current == legacy) return;

    DWORD currentAttr = GetFileAttributesW(current.c_str());
    DWORD legacyAttr = GetFileAttributesW(legacy.c_str());

    if (currentAttr == INVALID_FILE_ATTRIBUTES && legacyAttr != INVALID_FILE_ATTRIBUTES)
        CopyFileW(legacy.c_str(), current.c_str(), TRUE);
}

bool ReadBool(const wchar_t* section, const wchar_t* key, bool def)
{
    return GetPrivateProfileIntW(section, key, def ? 1 : 0, ConfigPath().c_str()) != 0;
}

int ReadInt(const wchar_t* section, const wchar_t* key, int def)
{
    int v = GetPrivateProfileIntW(section, key, def, ConfigPath().c_str());
    if (v < 1) v = 1;
    if (v > 3600) v = 3600;
    return v;
}

std::wstring ReadString(const wchar_t* section, const wchar_t* key, const wchar_t* def)
{
    wchar_t buf[256]{};
    GetPrivateProfileStringW(section, key, def, buf, 256, ConfigPath().c_str());
    return buf;
}

void WriteBool(const wchar_t* section, const wchar_t* key, bool value)
{
    WritePrivateProfileStringW(section, key, value ? L"true" : L"false", ConfigPath().c_str());
}

void WriteInt(const wchar_t* section, const wchar_t* key, int value)
{
    wchar_t buf[32]{};
    wsprintfW(buf, L"%d", value);
    WritePrivateProfileStringW(section, key, buf, ConfigPath().c_str());
}

void WriteString(const wchar_t* section, const wchar_t* key, const std::wstring& value)
{
    WritePrivateProfileStringW(section, key, value.c_str(), ConfigPath().c_str());
}

void LoadConfig()
{
    g_cfg.cleanEnabled = ReadBool(L"Clean", L"Enabled", true);
    g_cfg.hideIcons = ReadBool(L"Clean", L"HideIcons", true);
    g_cfg.hideIconsDelay = ReadInt(L"Clean", L"HideIconsDelay", 5);
    g_cfg.hideCursor = ReadBool(L"Clean", L"HideCursor", true);
    g_cfg.hideCursorDelay = ReadInt(L"Clean", L"HideCursorDelay", 5);
    g_cfg.hideTaskbar = ReadBool(L"Clean", L"HideTaskbar", false);
    g_cfg.hideTaskbarDelay = ReadInt(L"Clean", L"HideTaskbarDelay", 5);
    g_cfg.taskbarSafeMode = ReadBool(L"Clean", L"TaskbarSafeMode", true);
    g_cfg.iconsRestoreOnlyOnDesktop = ReadBool(L"Clean", L"IconsRestoreOnlyOnDesktop", false);
    g_cfg.blockWhenMaximized = true;
    g_cfg.darkTheme = ReadBool(L"General", L"DarkTheme", true);
    g_cfg.startMinimized = ReadBool(L"General", L"StartMinimized", false);
    g_cfg.closeToTray = ReadBool(L"General", L"CloseToTray", true);
    g_cfg.showNotifications = ReadBool(L"Customize", L"ShowNotifications", true);
    g_cfg.accentColor = ReadInt(L"Customize", L"AccentColor", 0);
    if (g_cfg.accentColor < 0) g_cfg.accentColor = 0;
    if (g_cfg.accentColor > 5) g_cfg.accentColor = 5;

    g_cfg.gameEnabled = ReadBool(L"Game", L"Enabled", false);
    g_cfg.gamePauseClean = ReadBool(L"Game", L"PauseClean", true);
    g_cfg.gameRestoreDesktop = ReadBool(L"Game", L"RestoreDesktop", true);
    g_cfg.gameOpenSteam = ReadBool(L"Game", L"OpenSteam", false);
    g_cfg.gameOpenXbox = ReadBool(L"Game", L"OpenXbox", false);
    g_cfg.gameOpenAdrenalin = ReadBool(L"Game", L"OpenAdrenalin", false);
    g_cfg.gameHighPerformance = ReadBool(L"Game", L"HighPerformance", false);

    g_cfg.studioEnabled = ReadBool(L"Studio", L"Enabled", false);
    g_cfg.studioPauseClean = ReadBool(L"Studio", L"PauseClean", false);
    g_cfg.studioRestoreDesktop = ReadBool(L"Studio", L"RestoreDesktop", false);
    g_cfg.studioOpenVSCode = ReadBool(L"Studio", L"OpenVSCode", false);
    g_cfg.studioOpenYouTubeMusic = ReadBool(L"Studio", L"OpenYouTubeMusic", true);
    g_cfg.studioBrowser = ReadString(L"Studio", L"YouTubeMusicBrowser", L"edge");
}

void ResetConfigToDefaults()
{
    g_cfg.cleanEnabled = true;
    g_cfg.hideIcons = true;
    g_cfg.hideIconsDelay = 5;
    g_cfg.hideCursor = true;
    g_cfg.hideCursorDelay = 5;
    g_cfg.hideTaskbar = false;
    g_cfg.hideTaskbarDelay = 5;
    g_cfg.taskbarSafeMode = true;
    g_cfg.iconsRestoreOnlyOnDesktop = false;
    g_cfg.blockWhenMaximized = true;
    g_cfg.darkTheme = true;
    g_cfg.startMinimized = false;
    g_cfg.closeToTray = true;
    g_cfg.showNotifications = true;
    g_cfg.accentColor = 0;

    g_cfg.gameEnabled = false;
    g_cfg.gamePauseClean = true;
    g_cfg.gameRestoreDesktop = true;
    g_cfg.gameOpenSteam = false;
    g_cfg.gameOpenXbox = false;
    g_cfg.gameOpenAdrenalin = false;
    g_cfg.gameHighPerformance = false;

    g_cfg.studioEnabled = false;
    g_cfg.studioPauseClean = false;
    g_cfg.studioRestoreDesktop = false;
    g_cfg.studioOpenVSCode = false;
    g_cfg.studioOpenYouTubeMusic = true;
    g_cfg.studioBrowser = L"edge";
}

void SaveConfig()
{
    WriteBool(L"Clean", L"Enabled", g_cfg.cleanEnabled);
    WriteBool(L"Clean", L"HideIcons", g_cfg.hideIcons);
    WriteInt(L"Clean", L"HideIconsDelay", g_cfg.hideIconsDelay);
    WriteBool(L"Clean", L"HideCursor", g_cfg.hideCursor);
    WriteInt(L"Clean", L"HideCursorDelay", g_cfg.hideCursorDelay);
    WriteBool(L"Clean", L"HideTaskbar", g_cfg.hideTaskbar);
    WriteInt(L"Clean", L"HideTaskbarDelay", g_cfg.hideTaskbarDelay);
    WriteBool(L"Clean", L"TaskbarSafeMode", g_cfg.taskbarSafeMode);
    WriteBool(L"Clean", L"IconsRestoreOnlyOnDesktop", g_cfg.iconsRestoreOnlyOnDesktop);
    WriteBool(L"General", L"DarkTheme", g_cfg.darkTheme);
    WriteBool(L"General", L"StartMinimized", g_cfg.startMinimized);
    WriteBool(L"General", L"CloseToTray", g_cfg.closeToTray);
    WriteBool(L"Customize", L"ShowNotifications", g_cfg.showNotifications);
    WriteInt(L"Customize", L"AccentColor", g_cfg.accentColor);

    WriteBool(L"Game", L"Enabled", g_cfg.gameEnabled);
    WriteBool(L"Game", L"PauseClean", g_cfg.gamePauseClean);
    WriteBool(L"Game", L"RestoreDesktop", g_cfg.gameRestoreDesktop);
    WriteBool(L"Game", L"OpenSteam", g_cfg.gameOpenSteam);
    WriteBool(L"Game", L"OpenXbox", g_cfg.gameOpenXbox);
    WriteBool(L"Game", L"OpenAdrenalin", g_cfg.gameOpenAdrenalin);
    WriteBool(L"Game", L"HighPerformance", g_cfg.gameHighPerformance);

    WriteBool(L"Studio", L"Enabled", g_cfg.studioEnabled);
    WriteBool(L"Studio", L"PauseClean", g_cfg.studioPauseClean);
    WriteBool(L"Studio", L"RestoreDesktop", g_cfg.studioRestoreDesktop);
    WriteBool(L"Studio", L"OpenVSCode", g_cfg.studioOpenVSCode);
    WriteBool(L"Studio", L"OpenYouTubeMusic", g_cfg.studioOpenYouTubeMusic);
    WriteString(L"Studio", L"YouTubeMusicBrowser", g_cfg.studioBrowser);
}


void ForceSaveConfig()
{
    SaveCurrentDelaysFromPanel(g_hwnd);
    SaveConfig();
    WritePrivateProfileStringW(nullptr, nullptr, nullptr, ConfigPath().c_str()); // flush do INI
}

DWORD GetLastInputTick()
{
    LASTINPUTINFO li{};
    li.cbSize = sizeof(LASTINPUTINFO);
    if (!GetLastInputInfo(&li)) return GetTickCount();
    return li.dwTime;
}

DWORD GetIdleMs()
{
    return GetTickCount() - GetLastInputTick();
}


std::wstring ToLowerCopy(std::wstring text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](wchar_t c) {
        return (wchar_t)towlower(c);
    });
    return text;
}

bool TextContainsInsensitive(const std::wstring& text, const std::wstring& needle)
{
    return ToLowerCopy(text).find(ToLowerCopy(needle)) != std::wstring::npos;
}

std::wstring GetWindowProcessPath(HWND hwnd)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return L"";

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return L"";

    wchar_t path[MAX_PATH]{};
    DWORD size = MAX_PATH;
    std::wstring result;

    if (QueryFullProcessImageNameW(process, 0, path, &size))
        result = path;

    CloseHandle(process);
    return result;
}

bool IsKnownIgnoredAppWindow(HWND hwnd)
{
    // Alguns jogos/apps criam janelas auxiliares invisíveis/estranhas.
    // Taskbar Hero é um deles: ele pode fazer o Control Center achar que
    // existe uma janela real aberta mesmo quando você voltou para o desktop.
    wchar_t title[512]{};
    GetWindowTextW(hwnd, title, 512);

    std::wstring windowTitle = title;
    std::wstring processPath = GetWindowProcessPath(hwnd);

    if (TextContainsInsensitive(windowTitle, L"taskbar hero")) return true;
    if (TextContainsInsensitive(windowTitle, L"taskbarhero")) return true;
    if (TextContainsInsensitive(processPath, L"taskbar hero")) return true;
    if (TextContainsInsensitive(processPath, L"taskbarhero")) return true;

    return false;
}


bool IsTaskbarHeroRunning()
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);

    bool found = false;

    if (Process32FirstW(snap, &pe))
    {
        do
        {
            std::wstring exeName = pe.szExeFile;
            if (TextContainsInsensitive(exeName, L"taskbarhero") ||
                TextContainsInsensitive(exeName, L"taskbar hero"))
            {
                found = true;
                break;
            }
        } while (Process32NextW(snap, &pe));
    }

    CloseHandle(snap);
    return found;
}

bool IsDesktopForeground()
{
    HWND fg = GetForegroundWindow();
    if (!fg) return false;

    wchar_t cls[256]{};
    GetClassNameW(fg, cls, 256);

    if (wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0)
        return true;

    // Às vezes o foco real fica no ListView dos ícones.
    HWND desktopList = GetDesktopListView();
    if (desktopList && fg == desktopList)
        return true;

    HWND parent = GetParent(fg);
    while (parent)
    {
        if (desktopList && parent == desktopList)
            return true;

        wchar_t parentCls[256]{};
        GetClassNameW(parent, parentCls, 256);

        if (wcscmp(parentCls, L"Progman") == 0 || wcscmp(parentCls, L"WorkerW") == 0)
            return true;

        parent = GetParent(parent);
    }

    return false;
}

bool ShouldRestoreIconsByDesktopRule()
{
    if (!g_cfg.iconsRestoreOnlyOnDesktop)
        return true;

    // Regra especial: com Taskbar Hero aberto, só restaura ao clicar/focar desktop.
    if (IsTaskbarHeroRunning())
        return IsDesktopForeground();

    return IsDesktopEffectivelyVisible();
}


bool IsIgnoredShellWindow(HWND hwnd)
{
    if (IsKnownIgnoredAppWindow(hwnd)) return true;

    wchar_t cls[256]{};
    GetClassNameW(hwnd, cls, 256);
    return wcscmp(cls, L"Progman") == 0 ||
           wcscmp(cls, L"WorkerW") == 0 ||
           wcscmp(cls, L"Shell_TrayWnd") == 0 ||
           wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
           wcscmp(cls, L"Button") == 0 ||
           wcscmp(cls, L"NotifyIconOverflowWindow") == 0 ||
           wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0 ||
           wcscmp(cls, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0;
}

bool IsRealVisibleAppWindow(HWND hwnd)
{
    if (!hwnd || !IsWindowVisible(hwnd) || IsIconic(hwnd)) return false;
    if (IsIgnoredShellWindow(hwnd)) return false;
    if (GetWindow(hwnd, GW_OWNER) != nullptr) return false;

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;

    BOOL cloaked = FALSE;
    DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked) return false;

    RECT r{};
    if (!GetWindowRect(hwnd, &r)) return false;
    int w = r.right - r.left;
    int h = r.bottom - r.top;
    if (w < 80 || h < 80) return false;

    return true;
}

bool HasVisibleAppWindow()
{
    HWND hwnd = GetTopWindow(nullptr);
    while (hwnd)
    {
        if (IsRealVisibleAppWindow(hwnd)) return true;
        hwnd = GetWindow(hwnd, GW_HWNDNEXT);
    }
    return false;
}

bool IsDesktopEffectivelyVisible()
{
    return !HasVisibleAppWindow();
}

bool IsActiveWindowMaximizedOrFullscreen()
{
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) return false;
    if (IsIgnoredShellWindow(hwnd)) return false;
    if (IsZoomed(hwnd)) return true;

    RECT wr{};
    if (!GetWindowRect(hwnd, &wr)) return false;

    HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!mon) return false;

    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(mon, &mi)) return false;

    const int tol = 20;
    return wr.left <= mi.rcMonitor.left + tol &&
           wr.top <= mi.rcMonitor.top + tol &&
           wr.right >= mi.rcMonitor.right - tol &&
           wr.bottom >= mi.rcMonitor.bottom - tol;
}

HWND GetDesktopListView()
{
    HWND progman = FindWindowW(L"Progman", nullptr);
    HWND shellView = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
    if (!shellView)
    {
        HWND worker = nullptr;
        while ((worker = FindWindowExW(nullptr, worker, L"WorkerW", nullptr)) != nullptr)
        {
            shellView = FindWindowExW(worker, nullptr, L"SHELLDLL_DefView", nullptr);
            if (shellView) break;
        }
    }
    if (!shellView) return nullptr;

    HWND listView = FindWindowExW(shellView, nullptr, L"SysListView32", L"FolderView");
    if (!listView) listView = FindWindowExW(shellView, nullptr, L"SysListView32", nullptr);
    return listView;
}

void ShowDesktopIcons()
{
    HWND h = GetDesktopListView();
    if (h) ShowWindow(h, SW_SHOW);
    g_iconsHidden = false;
}

void HideDesktopIcons()
{
    HWND h = GetDesktopListView();
    if (h) ShowWindow(h, SW_HIDE);
    g_iconsHidden = true;
}

void ShowTaskbar()
{
    HWND tb = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tb) ShowWindow(tb, SW_SHOW);
    HWND sec = nullptr;
    while ((sec = FindWindowExW(nullptr, sec, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr)
        ShowWindow(sec, SW_SHOW);
    g_taskbarHidden = false;
}

void HideTaskbar()
{
    HWND tb = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tb) ShowWindow(tb, SW_HIDE);
    HWND sec = nullptr;
    while ((sec = FindWindowExW(nullptr, sec, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr)
        ShowWindow(sec, SW_HIDE);
    g_taskbarHidden = true;
}

void RestoreCursors()
{
    SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0);
    g_cursorHidden = false;
}

void HideCursors()
{
    static const DWORD cursors[] = {
        32512, 32513, 32514, 32515, 32516, 32642, 32643,
        32644, 32645, 32646, 32648, 32649, 32650, 32651
    };

    const int width = 32;
    const int height = 32;
    BYTE andMask[128];
    BYTE xorMask[128];
    ZeroMemory(xorMask, sizeof(xorMask));
    for (BYTE& b : andMask) b = 0xFF;

    HCURSOR blank = CreateCursor(nullptr, 0, 0, width, height, andMask, xorMask);
    if (!blank) return;

    for (DWORD id : cursors)
    {
        HCURSOR copy = (HCURSOR)CopyImage(blank, IMAGE_CURSOR, 0, 0, LR_COPYFROMRESOURCE);
        if (copy) SetSystemCursor(copy, id);
    }
    DestroyCursor(blank);
    g_cursorHidden = true;
}

void RestoreEverything()
{
    ShowDesktopIcons();
    RestoreCursors();
    ShowTaskbar();
}

bool IsCleanActuallyActive()
{
    if (g_pausedAll) return false;
    if (!g_cfg.cleanEnabled) return false;
    if (g_cfg.gameEnabled && g_cfg.gamePauseClean) return false;
    if (g_cfg.studioEnabled && g_cfg.studioPauseClean) return false;
    return true;
}

void ShellRun(const std::wstring& file, const std::wstring& params = L"")
{
    ShellExecuteW(nullptr, L"open", file.c_str(), params.empty() ? nullptr : params.c_str(), nullptr, SW_SHOWNORMAL);
}

bool FileExistsW2(const std::wstring& path)
{
    DWORD attr = GetFileAttributesW(path.c_str());
    return attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

std::wstring EnvPath(const wchar_t* name)
{
    wchar_t buf[1024]{};
    DWORD n = GetEnvironmentVariableW(name, buf, 1024);
    return n ? std::wstring(buf) : L"";
}

std::wstring ResolveBrowserExe(const std::wstring& browser)
{
    std::vector<std::wstring> candidates;
    std::wstring b = browser;
    std::transform(b.begin(), b.end(), b.begin(), ::towlower);

    if (b == L"chrome")
    {
        candidates.push_back(EnvPath(L"ProgramFiles") + L"\\Google\\Chrome\\Application\\chrome.exe");
        candidates.push_back(EnvPath(L"ProgramFiles(x86)") + L"\\Google\\Chrome\\Application\\chrome.exe");
        candidates.push_back(EnvPath(L"LocalAppData") + L"\\Google\\Chrome\\Application\\chrome.exe");
        candidates.push_back(L"chrome.exe");
    }
    else if (b == L"brave")
    {
        candidates.push_back(EnvPath(L"ProgramFiles") + L"\\BraveSoftware\\Brave-Browser\\Application\\brave.exe");
        candidates.push_back(EnvPath(L"ProgramFiles(x86)") + L"\\BraveSoftware\\Brave-Browser\\Application\\brave.exe");
        candidates.push_back(EnvPath(L"LocalAppData") + L"\\BraveSoftware\\Brave-Browser\\Application\\brave.exe");
        candidates.push_back(L"brave.exe");
    }
    else
    {
        candidates.push_back(EnvPath(L"ProgramFiles(x86)") + L"\\Microsoft\\Edge\\Application\\msedge.exe");
        candidates.push_back(EnvPath(L"ProgramFiles") + L"\\Microsoft\\Edge\\Application\\msedge.exe");
        candidates.push_back(EnvPath(L"LocalAppData") + L"\\Microsoft\\Edge\\Application\\msedge.exe");
        candidates.push_back(L"msedge.exe");
    }

    for (const auto& c : candidates)
    {
        if (c.empty()) continue;
        if (c.find(L"\\") == std::wstring::npos) return c;
        if (FileExistsW2(c)) return c;
    }

    if (b == L"chrome") return L"chrome.exe";
    if (b == L"brave") return L"brave.exe";
    return L"msedge.exe";
}

std::wstring ResolveVSCodeExe()
{
    std::vector<std::wstring> candidates = {
        EnvPath(L"LocalAppData") + L"\\Programs\\Microsoft VS Code\\Code.exe",
        EnvPath(L"ProgramFiles") + L"\\Microsoft VS Code\\Code.exe",
        EnvPath(L"ProgramFiles(x86)") + L"\\Microsoft VS Code\\Code.exe",
        L"code.exe"
    };
    for (const auto& c : candidates)
    {
        if (c.find(L"\\") == std::wstring::npos) return c;
        if (FileExistsW2(c)) return c;
    }
    return L"code.exe";
}

DWORD LaunchProcess(const std::wstring& exe, const std::wstring& args)
{
    std::wstring cmd = L"\"" + exe + L"\"";
    if (!args.empty()) cmd += L" " + args;

    STARTUPINFOW si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;

    std::vector<wchar_t> buffer(cmd.begin(), cmd.end());
    buffer.push_back(L'\0');

    BOOL ok = CreateProcessW(nullptr, buffer.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
    if (ok)
    {
        DWORD pid = pi.dwProcessId;
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return pid;
    }

    // Fallback melhor para Chrome/Brave/Edge registrados no Windows.
    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"open";
    sei.lpFile = exe.c_str();
    sei.lpParameters = args.empty() ? nullptr : args.c_str();
    sei.nShow = SW_SHOWNORMAL;

    if (ShellExecuteExW(&sei) && sei.hProcess)
    {
        DWORD pid = GetProcessId(sei.hProcess);
        CloseHandle(sei.hProcess);
        return pid;
    }

    return 0;
}

void KillProcessTree(DWORD pid)
{
    if (!pid) return;
    wchar_t args[128]{};
    wsprintfW(args, L"/PID %lu /T /F", pid);

    STARTUPINFOW si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    std::wstring cmd = L"taskkill.exe ";
    cmd += args;
    std::vector<wchar_t> buffer(cmd.begin(), cmd.end());
    buffer.push_back(L'\0');

    if (CreateProcessW(nullptr, buffer.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, 1500);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}

BOOL CALLBACK CloseWindowByPidsProc(HWND hwnd, LPARAM lParam)
{
    auto* pids = reinterpret_cast<std::vector<DWORD>*>(lParam);
    if (!IsWindowVisible(hwnd)) return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid && std::find(pids->begin(), pids->end(), pid) != pids->end())
    {
        PostMessageW(hwnd, WM_CLOSE, 0, 0);
    }
    return TRUE;
}

void CloseStudioWindows()
{
    if (g_studioPids.empty()) return;
    EnumWindows(CloseWindowByPidsProc, reinterpret_cast<LPARAM>(&g_studioPids));
    Sleep(700);

    for (DWORD pid : g_studioPids)
        KillProcessTree(pid);

    g_studioPids.clear();
}

void SetGameMode(bool enabled)
{
    if (g_cfg.gameEnabled == enabled)
    {
        SaveConfig();
        return;
    }

    g_cfg.gameEnabled = enabled;

    if (g_cfg.gameEnabled)
    {
        if (g_cfg.gameRestoreDesktop) RestoreEverything();
        if (g_cfg.gameOpenSteam) ShellRun(L"steam://open/main");
        if (g_cfg.gameOpenXbox) ShellRun(L"explorer.exe", L"shell:AppsFolder\\Microsoft.GamingApp_8wekyb3d8bbwe!Microsoft.Xbox.App");
        if (g_cfg.gameOpenAdrenalin) ShellRun(L"C:\\Program Files\\AMD\\CNext\\CNext\\RadeonSoftware.exe");
        if (g_cfg.gameHighPerformance) ShellRun(L"powercfg.exe", L"/setactive SCHEME_MIN");
    }
    SaveConfig();
}

void ToggleGameMode()
{
    SetGameMode(!g_cfg.gameEnabled);
}

void SetStudioMode(bool enabled)
{
    if (g_cfg.studioEnabled == enabled)
    {
        SaveConfig();
        return;
    }

    g_cfg.studioEnabled = enabled;

    if (!g_cfg.studioEnabled)
    {
        CloseStudioWindows();
        SaveConfig();
        return;
    }

    if (g_cfg.studioRestoreDesktop) RestoreEverything();

    if (g_cfg.studioOpenVSCode)
    {
        DWORD pid = LaunchProcess(ResolveVSCodeExe(), L"");
        if (pid) g_studioPids.push_back(pid);
    }

    if (g_cfg.studioOpenYouTubeMusic)
    {
        std::wstring browser = g_cfg.studioBrowser;
        std::transform(browser.begin(), browser.end(), browser.begin(), ::towlower);
        if (browser != L"chrome" && browser != L"brave") browser = L"edge";

        std::wstring exe = ResolveBrowserExe(browser);
        std::wstring profile = GetExeDir() + L"\\studio_ytmusic_profile_" + browser;
        CreateDirectoryW(profile.c_str(), nullptr);

        std::wstring args =
            L"--app=https://music.youtube.com "
            L"--new-window "
            L"--window-size=460,760 "
            L"--window-position=70,70 "
            L"--user-data-dir=\"" + profile + L"\" "
            L"--no-first-run";

        DWORD pid = LaunchProcess(exe, args);
        if (pid) g_studioPids.push_back(pid);
    }

    SaveConfig();
}

void ToggleStudioMode()
{
    SetStudioMode(!g_cfg.studioEnabled);
}


void ApplyPersistedModesOnStartup()
{
    // Clean não precisa abrir nada: se estiver ligado no config, o timer já aplica.
    // Jogo/Estúdio têm ações, então precisam ser reaplicados quando o app inicia.

    if (g_cfg.gameEnabled)
    {
        if (g_cfg.gameRestoreDesktop) RestoreEverything();
        if (g_cfg.gameOpenSteam) ShellRun(L"steam://open/main");
        if (g_cfg.gameOpenXbox) ShellRun(L"explorer.exe", L"shell:AppsFolder\\Microsoft.GamingApp_8wekyb3d8bbwe!Microsoft.Xbox.App");
        if (g_cfg.gameOpenAdrenalin) ShellRun(L"C:\\Program Files\\AMD\\CNext\\CNext\\RadeonSoftware.exe");
        if (g_cfg.gameHighPerformance) ShellRun(L"powercfg.exe", L"/setactive SCHEME_MIN");
    }

    if (g_cfg.studioEnabled)
    {
        if (g_cfg.studioRestoreDesktop) RestoreEverything();

        if (g_cfg.studioOpenVSCode)
        {
            DWORD pid = LaunchProcess(ResolveVSCodeExe(), L"");
            if (pid) g_studioPids.push_back(pid);
        }

        if (g_cfg.studioOpenYouTubeMusic)
        {
            std::wstring browser = g_cfg.studioBrowser;
            std::transform(browser.begin(), browser.end(), browser.begin(), ::towlower);
            if (browser != L"chrome" && browser != L"brave") browser = L"edge";

            std::wstring exe = ResolveBrowserExe(browser);
            std::wstring profile = GetExeDir() + L"\\studio_ytmusic_profile_" + browser;
            CreateDirectoryW(profile.c_str(), nullptr);

            std::wstring args =
                L"--app=https://music.youtube.com "
                L"--new-window "
                L"--window-size=460,760 "
                L"--window-position=70,70 "
                L"--user-data-dir=\"" + profile + L"\" "
                L"--no-first-run";

            DWORD pid = LaunchProcess(exe, args);
            if (pid) g_studioPids.push_back(pid);
        }
    }
}


int GetEditInt(HWND hwnd, int id, int def)
{
    wchar_t buf[32]{};
    GetWindowTextW(GetDlgItem(hwnd, id), buf, 32);
    int v = _wtoi(buf);
    if (v < 1) v = def;
    if (v > 3600) v = 3600;
    return v;
}

void SetEditInt(HWND hwnd, int id, int value)
{
    wchar_t buf[32]{};
    wsprintfW(buf, L"%d", value);
    SetWindowTextW(GetDlgItem(hwnd, id), buf);
}

void SetCheck(HWND hwnd, int id, bool value)
{
    HWND c = GetDlgItem(hwnd, id);
    if (!c) return;
    SendMessageW(c, BM_SETCHECK, value ? BST_CHECKED : BST_UNCHECKED, 0);
    InvalidateRect(c, nullptr, TRUE);
}

bool GetCheck(HWND hwnd, int id)
{
    return SendMessageW(GetDlgItem(hwnd, id), BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void SaveCurrentDelaysFromPanel(HWND hwnd)
{
    if (!hwnd) return;
    g_cfg.hideIconsDelay = GetEditInt(hwnd, IDC_ICONS_DELAY, g_cfg.hideIconsDelay);
    g_cfg.hideCursorDelay = GetEditInt(hwnd, IDC_CURSOR_DELAY, g_cfg.hideCursorDelay);
    g_cfg.hideTaskbarDelay = GetEditInt(hwnd, IDC_TASKBAR_DELAY, g_cfg.hideTaskbarDelay);
    SaveConfig();
}

void UpdateStatusText()
{
    if (!g_hwnd) return;
    std::wstringstream ss;
    if (g_pausedAll) ss << L"Pausado";
    else
    {
        ss << (g_cfg.cleanEnabled ? L"Clean ON" : L"Clean OFF");
        if (g_cfg.gameEnabled) ss << L" | Jogo ON";
        if (g_cfg.studioEnabled) ss << L" | Estudio ON";
    }
    SetWindowTextW(GetDlgItem(g_hwnd, IDC_STATUS), ss.str().c_str());
}

void UpdatePanelFromConfig(HWND hwnd)
{
    SetCheck(hwnd, IDC_CLEAN_ENABLED, g_cfg.cleanEnabled);
    SetCheck(hwnd, IDC_HIDE_ICONS, g_cfg.hideIcons);
    SetEditInt(hwnd, IDC_ICONS_DELAY, g_cfg.hideIconsDelay);
    SetCheck(hwnd, IDC_HIDE_CURSOR, g_cfg.hideCursor);
    SetEditInt(hwnd, IDC_CURSOR_DELAY, g_cfg.hideCursorDelay);
    SetCheck(hwnd, IDC_HIDE_TASKBAR, g_cfg.hideTaskbar);
    SetEditInt(hwnd, IDC_TASKBAR_DELAY, g_cfg.hideTaskbarDelay);
    SetCheck(hwnd, IDC_TASKBAR_SAFE, g_cfg.taskbarSafeMode);
    SetCheck(hwnd, IDC_ICONS_RESTORE_DESKTOP, g_cfg.iconsRestoreOnlyOnDesktop);
    SetCheck(hwnd, IDC_GAME_ENABLED, g_cfg.gameEnabled);
    SetCheck(hwnd, IDC_GAME_PAUSE_CLEAN, g_cfg.gamePauseClean);
    SetCheck(hwnd, IDC_GAME_RESTORE, g_cfg.gameRestoreDesktop);
    SetCheck(hwnd, IDC_GAME_STEAM, g_cfg.gameOpenSteam);
    SetCheck(hwnd, IDC_GAME_XBOX, g_cfg.gameOpenXbox);
    SetCheck(hwnd, IDC_GAME_ADRENALIN, g_cfg.gameOpenAdrenalin);
    SetCheck(hwnd, IDC_GAME_POWER, g_cfg.gameHighPerformance);

    SetCheck(hwnd, IDC_STUDIO_ENABLED, g_cfg.studioEnabled);
    SetCheck(hwnd, IDC_STUDIO_PAUSE_CLEAN, g_cfg.studioPauseClean);
    SetCheck(hwnd, IDC_STUDIO_RESTORE, g_cfg.studioRestoreDesktop);
    SetCheck(hwnd, IDC_STUDIO_VSCODE, g_cfg.studioOpenVSCode);
    SetCheck(hwnd, IDC_STUDIO_YTMUSIC, g_cfg.studioOpenYouTubeMusic);
    SetCheck(hwnd, IDC_START_MINIMIZED, g_cfg.startMinimized);
    SetCheck(hwnd, IDC_CLOSE_TO_TRAY, g_cfg.closeToTray);
    SetCheck(hwnd, IDC_SHOW_NOTIFICATIONS, g_cfg.showNotifications);
    InvalidateRect(GetDlgItem(hwnd, IDC_STUDIO_BROWSER_EDGE), nullptr, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_STUDIO_BROWSER_CHROME), nullptr, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_STUDIO_BROWSER_BRAVE), nullptr, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_STUDIO_TOGGLE_BTN), nullptr, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_COLOR_BLUE), nullptr, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_COLOR_PURPLE), nullptr, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_COLOR_SILVER), nullptr, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_COLOR_GREEN), nullptr, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_COLOR_RED), nullptr, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_COLOR_Y2K), nullptr, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_THEME_TOGGLE), nullptr, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_TAB_CLEAN), nullptr, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_TAB_GAME), nullptr, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_TAB_STUDIO), nullptr, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_TAB_CUSTOMIZE), nullptr, TRUE);
    SetWindowTextW(GetDlgItem(hwnd, ID_TRAY_PAUSE_ALL), g_pausedAll ? L"Ativar" : L"Pausar");
    SetWindowTextW(GetDlgItem(hwnd, IDC_THEME_TOGGLE), g_cfg.darkTheme ? L"Tema claro" : L"Tema escuro");
    SetWindowTextW(GetDlgItem(hwnd, IDC_STARTUP_TOGGLE), IsStartupEnabled() ? L"Remover inicializacao" : L"Iniciar com Windows");
    SetWindowTextW(GetDlgItem(hwnd, IDC_STUDIO_TOGGLE_BTN), g_cfg.studioEnabled ? L"Desligar Modo Estudio" : L"Ativar Modo Estudio");
    UpdateTabVisibility(hwnd);
    UpdateStatusText();
}

void ReadPanelToConfig(HWND hwnd)
{
    g_cfg.cleanEnabled = GetCheck(hwnd, IDC_CLEAN_ENABLED);
    g_cfg.hideIcons = GetCheck(hwnd, IDC_HIDE_ICONS);
    g_cfg.hideIconsDelay = GetEditInt(hwnd, IDC_ICONS_DELAY, 5);
    g_cfg.hideCursor = GetCheck(hwnd, IDC_HIDE_CURSOR);
    g_cfg.hideCursorDelay = GetEditInt(hwnd, IDC_CURSOR_DELAY, 5);
    g_cfg.hideTaskbar = GetCheck(hwnd, IDC_HIDE_TASKBAR);
    g_cfg.hideTaskbarDelay = GetEditInt(hwnd, IDC_TASKBAR_DELAY, 5);
    g_cfg.taskbarSafeMode = GetCheck(hwnd, IDC_TASKBAR_SAFE);
    g_cfg.gamePauseClean = GetCheck(hwnd, IDC_GAME_PAUSE_CLEAN);
    g_cfg.gameRestoreDesktop = GetCheck(hwnd, IDC_GAME_RESTORE);
    g_cfg.gameOpenSteam = GetCheck(hwnd, IDC_GAME_STEAM);
    g_cfg.gameOpenXbox = GetCheck(hwnd, IDC_GAME_XBOX);
    g_cfg.gameOpenAdrenalin = GetCheck(hwnd, IDC_GAME_ADRENALIN);
    g_cfg.gameHighPerformance = GetCheck(hwnd, IDC_GAME_POWER);

    g_cfg.studioPauseClean = GetCheck(hwnd, IDC_STUDIO_PAUSE_CLEAN);
    g_cfg.studioRestoreDesktop = GetCheck(hwnd, IDC_STUDIO_RESTORE);
    g_cfg.studioOpenVSCode = GetCheck(hwnd, IDC_STUDIO_VSCODE);
    g_cfg.studioOpenYouTubeMusic = GetCheck(hwnd, IDC_STUDIO_YTMUSIC);

}

void ShowTrayBalloon(const wchar_t* title, const wchar_t* msg)
{
    if (!g_cfg.showNotifications) return;
    if (!g_nid.hWnd) return;
    g_nid.uFlags = NIF_INFO;
    lstrcpynW(g_nid.szInfoTitle, title, ARRAYSIZE(g_nid.szInfoTitle));
    lstrcpynW(g_nid.szInfo, msg, ARRAYSIZE(g_nid.szInfo));
    g_nid.dwInfoFlags = NIIF_INFO;
    Shell_NotifyIconW(NIM_MODIFY, &g_nid);
    g_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
}

void AddTrayIcon(HWND hwnd)
{
    ZeroMemory(&g_nid, sizeof(g_nid));
    g_nid.cbSize = sizeof(g_nid);
    g_nid.hWnd = hwnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon = LoadIconW(g_hInst, MAKEINTRESOURCEW(IDI_APP));
    if (!g_nid.hIcon) g_nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcscpy_s(g_nid.szTip, APP_NAME);
    Shell_NotifyIconW(NIM_ADD, &g_nid);
}

void RemoveTrayIcon()
{
    Shell_NotifyIconW(NIM_DELETE, &g_nid);
}

void ShowTrayMenu(HWND hwnd)
{
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, ID_TRAY_OPEN_PANEL, L"Abrir painel");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING | (g_cfg.cleanEnabled ? MF_CHECKED : 0), ID_TRAY_TOGGLE_CLEAN, L"Modo Clean");
    AppendMenuW(menu, MF_STRING | (g_cfg.hideIcons ? MF_CHECKED : 0), ID_TRAY_TOGGLE_ICONS, L"Clean: icones");
    AppendMenuW(menu, MF_STRING | (g_cfg.hideCursor ? MF_CHECKED : 0), ID_TRAY_TOGGLE_CURSOR, L"Clean: cursor");
    AppendMenuW(menu, MF_STRING | (g_cfg.hideTaskbar ? MF_CHECKED : 0), ID_TRAY_TOGGLE_TASKBAR, L"Clean: barra de tarefas");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING | (g_cfg.gameEnabled ? MF_CHECKED : 0), ID_TRAY_GAME_MODE, L"Modo Jogo");
    AppendMenuW(menu, MF_STRING | (g_cfg.studioEnabled ? MF_CHECKED : 0), ID_TRAY_STUDIO_MODE, L"Modo Estudio");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, ID_TRAY_RESTORE, L"Restaurar tudo");
    AppendMenuW(menu, MF_STRING | (g_pausedAll ? MF_CHECKED : 0), ID_TRAY_PAUSE_ALL, L"Pausar tudo");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, ID_TRAY_EXIT, L"Sair");

    POINT p{};
    GetCursorPos(&p);
    SetForegroundWindow(hwnd);
    TrackPopupMenu(menu, TPM_RIGHTBUTTON, p.x, p.y, 0, hwnd, nullptr);
    DestroyMenu(menu);
}



bool IsStartupEnabled()
{
    HKEY key{};
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &key) != ERROR_SUCCESS)
        return false;

    wchar_t value[MAX_PATH * 2]{};
    DWORD size = sizeof(value);
    LONG result = RegQueryValueExW(key, L"WabbeControlCenter", nullptr, nullptr, (LPBYTE)value, &size);
    RegCloseKey(key);

    return result == ERROR_SUCCESS;
}

void SetStartupEnabled(bool enabled)
{
    HKEY key{};
    if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, nullptr, 0, KEY_SET_VALUE, nullptr, &key, nullptr) != ERROR_SUCCESS)
        return;

    if (enabled)
    {
        wchar_t exe[MAX_PATH]{};
        GetModuleFileNameW(nullptr, exe, MAX_PATH);
        std::wstring value = L"\"" + std::wstring(exe) + L"\"";
        RegSetValueExW(key, L"WabbeControlCenter", 0, REG_SZ, (const BYTE*)value.c_str(), (DWORD)((value.size() + 1) * sizeof(wchar_t)));
    }
    else
    {
        RegDeleteValueW(key, L"WabbeControlCenter");
    }

    RegCloseKey(key);
}


void ApplyAccentColor()
{
    switch (g_cfg.accentColor)
    {
    case 1: // roxo
        CLR_ACCENT = RGB(180, 140, 255);
        CLR_ACCENT_2 = RGB(135, 88, 245);
        break;
    case 2: // prata
        CLR_ACCENT = RGB(210, 218, 230);
        CLR_ACCENT_2 = RGB(145, 158, 180);
        break;
    case 3: // verde
        CLR_ACCENT = RGB(120, 235, 185);
        CLR_ACCENT_2 = RGB(45, 180, 130);
        break;
    case 4: // vermelho
        CLR_ACCENT = RGB(255, 130, 130);
        CLR_ACCENT_2 = RGB(220, 70, 80);
        break;
    case 5: // y2k cyan
        CLR_ACCENT = RGB(120, 245, 255);
        CLR_ACCENT_2 = RGB(45, 185, 210);
        break;
    default: // azul
        CLR_ACCENT = RGB(125, 185, 255);
        CLR_ACCENT_2 = RGB(65, 135, 255);
        break;
    }
}

void RebuildBrushes()
{
    if (g_bgBrush) DeleteObject(g_bgBrush);
    if (g_cardBrush) DeleteObject(g_cardBrush);
    if (g_editBrush) DeleteObject(g_editBrush);

    g_bgBrush = CreateSolidBrush(CLR_BG);
    g_cardBrush = CreateSolidBrush(CLR_CARD);
    g_editBrush = CreateSolidBrush(CLR_CARD_2);
}

void ApplyThemeColors()
{
    if (g_cfg.darkTheme)
    {
        CLR_BG = RGB(15, 16, 22);
        CLR_CARD = RGB(32, 35, 46);
        CLR_CARD_2 = RGB(45, 49, 64);
        CLR_TEXT = RGB(242, 245, 250);
        CLR_MUTED = RGB(165, 172, 186);
        CLR_ACCENT = RGB(125, 185, 255);
        CLR_ACCENT_2 = RGB(65, 135, 255);
        CLR_OFF = RGB(83, 88, 105);
    }
    else
    {
        CLR_BG = RGB(236, 240, 247);
        CLR_CARD = RGB(255, 255, 255);
        CLR_CARD_2 = RGB(226, 232, 242);
        CLR_TEXT = RGB(22, 26, 35);
        CLR_MUTED = RGB(90, 98, 115);
        CLR_ACCENT = RGB(75, 145, 255);
        CLR_ACCENT_2 = RGB(30, 105, 230);
        CLR_OFF = RGB(180, 187, 200);
    }

    ApplyAccentColor();
    RebuildBrushes();
}

bool IsTabControl(int id)
{
    return id == IDC_TAB_CLEAN || id == IDC_TAB_GAME || id == IDC_TAB_STUDIO || id == IDC_TAB_GENERAL || id == IDC_TAB_CUSTOMIZE ||
           id == IDC_THEME_DARK || id == IDC_THEME_LIGHT || id == IDC_THEME_TOGGLE || id == IDC_STARTUP_TOGGLE;
}

bool IsModeSwitchId(int id)
{
    return id == IDC_CLEAN_ENABLED || id == IDC_GAME_ENABLED;
}

bool IsOptionCheckId(int id)
{
    switch (id)
    {
    case IDC_HIDE_ICONS:
    case IDC_HIDE_CURSOR:
    case IDC_HIDE_TASKBAR:
    case IDC_TASKBAR_SAFE:
    case IDC_ICONS_RESTORE_DESKTOP:
    case IDC_GAME_PAUSE_CLEAN:
    case IDC_GAME_RESTORE:
    case IDC_GAME_STEAM:
    case IDC_GAME_XBOX:
    case IDC_GAME_ADRENALIN:
    case IDC_GAME_POWER:
    case IDC_STUDIO_PAUSE_CLEAN:
    case IDC_STUDIO_RESTORE:
    case IDC_STUDIO_VSCODE:
    case IDC_STUDIO_YTMUSIC:
    case IDC_START_MINIMIZED:
    case IDC_CLOSE_TO_TRAY:
    case IDC_SHOW_NOTIFICATIONS:
        return true;
    default:
        return false;
    }
}

void ToggleButtonCheck(HWND hwnd, int id)
{
    HWND c = GetDlgItem(hwnd, id);
    if (!c) return;
    LRESULT current = SendMessageW(c, BM_GETCHECK, 0, 0);
    SendMessageW(c, BM_SETCHECK, current == BST_CHECKED ? BST_UNCHECKED : BST_CHECKED, 0);
    InvalidateRect(c, nullptr, TRUE);
}

void SetControlVisible(HWND hwnd, int id, bool visible)
{
    HWND c = GetDlgItem(hwnd, id);
    if (c) ShowWindow(c, visible ? SW_SHOW : SW_HIDE);
}

void UpdateTabVisibility(HWND hwnd)
{
    int cleanIds[] = { IDC_LABEL_CLEAN_GROUP1, IDC_LABEL_CLEAN_GROUP2, IDC_CLEAN_ENABLED, IDC_HIDE_ICONS, IDC_ICONS_DELAY, IDC_LABEL_ICONS_DELAY, IDC_LABEL_ICONS_S,
        IDC_HIDE_CURSOR, IDC_CURSOR_DELAY, IDC_LABEL_CURSOR_DELAY, IDC_LABEL_CURSOR_S,
        IDC_HIDE_TASKBAR, IDC_TASKBAR_DELAY, IDC_LABEL_TASKBAR_DELAY, IDC_LABEL_TASKBAR_S, IDC_TASKBAR_SAFE, IDC_ICONS_RESTORE_DESKTOP };
    int gameIds[] = { IDC_LABEL_GAME_GROUP, IDC_GAME_ENABLED, IDC_GAME_PAUSE_CLEAN, IDC_GAME_RESTORE, IDC_GAME_STEAM,
        IDC_GAME_XBOX, IDC_GAME_ADRENALIN, IDC_GAME_POWER };
    int studioIds[] = { IDC_LABEL_STUDIO_GROUP, IDC_STUDIO_TOGGLE_BTN, IDC_STUDIO_PAUSE_CLEAN, IDC_STUDIO_RESTORE, IDC_STUDIO_VSCODE,
        IDC_STUDIO_YTMUSIC, IDC_LABEL_YTMUSIC_WITH, IDC_STUDIO_BROWSER_EDGE, IDC_STUDIO_BROWSER_CHROME, IDC_STUDIO_BROWSER_BRAVE };
    int customIds[] = { IDC_LABEL_COLOR_TITLE, IDC_COLOR_BLUE, IDC_COLOR_PURPLE, IDC_COLOR_SILVER, IDC_COLOR_GREEN, IDC_COLOR_RED, IDC_COLOR_Y2K,
        IDC_SHOW_NOTIFICATIONS, IDC_OPEN_CONFIG, IDC_RESET_CONFIG };

    for (int id : cleanIds) SetControlVisible(hwnd, id, g_activeTab == 0);
    for (int id : gameIds) SetControlVisible(hwnd, id, g_activeTab == 1);
    for (int id : studioIds) SetControlVisible(hwnd, id, g_activeTab == 2);
    SetControlVisible(hwnd, IDC_START_MINIMIZED, g_activeTab == 0);
    SetControlVisible(hwnd, IDC_CLOSE_TO_TRAY, g_activeTab == 0);
    for (int id : customIds) SetControlVisible(hwnd, id, g_activeTab == 3);
    SetControlVisible(hwnd, IDC_SHORTCUT_LABEL, true);
    InvalidateRect(hwnd, nullptr, TRUE);
}


HWND MakeCtrl(const wchar_t* cls, const wchar_t* text, DWORD style, int x, int y, int w, int height, HWND parent, int id)
{
    return CreateWindowExW(0, cls, text, WS_CHILD | WS_VISIBLE | style, x, y, w, height, parent, (HMENU)(INT_PTR)id, g_hInst, nullptr);
}

HWND MakeLabel(const wchar_t* text, int x, int y, int w, int height, HWND parent, int id = -1)
{
    HWND hwndCtrl = MakeCtrl(L"STATIC", text, 0, x, y, w, height, parent, id);
    SendMessageW(hwndCtrl, WM_SETFONT, (WPARAM)g_uiFont, TRUE);
    return hwndCtrl;
}

HWND MakeSwitch(const wchar_t* text, int x, int y, int w, int height, HWND parent, int id)
{
    HWND hwndCtrl = MakeCtrl(L"BUTTON", text, SWITCH_STYLE, x, y, w, height, parent, id);
    SendMessageW(hwndCtrl, WM_SETFONT, (WPARAM)g_uiFont, TRUE);
    return hwndCtrl;
}

HWND MakeCheck(const wchar_t* text, int x, int y, int w, int height, HWND parent, int id)
{
    HWND hwndCtrl = MakeCtrl(L"BUTTON", text, CHECK_STYLE, x, y, w, height, parent, id);
    SendMessageW(hwndCtrl, WM_SETFONT, (WPARAM)g_uiFont, TRUE);
    return hwndCtrl;
}

HWND MakeButton2(const wchar_t* text, int x, int y, int w, int height, HWND parent, int id)
{
    HWND hwndCtrl = MakeCtrl(L"BUTTON", text, BTN_STYLE, x, y, w, height, parent, id);
    SendMessageW(hwndCtrl, WM_SETFONT, (WPARAM)g_uiFont, TRUE);
    return hwndCtrl;
}

HWND MakeEdit2(const wchar_t* text, int x, int y, int w, int height, HWND parent, int id)
{
    HWND hwndCtrl = MakeCtrl(L"EDIT", text, WS_BORDER | ES_NUMBER, x, y, w, height, parent, id);
    SendMessageW(hwndCtrl, WM_SETFONT, (WPARAM)g_uiFont, TRUE);
    return hwndCtrl;
}

void CreatePanelControls(HWND hwnd)
{
    g_uiFont = CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
    g_titleFont = CreateFontW(31, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI Variable Display");

    HWND title = MakeLabel(L"Wabbe Control Center", 34, 28, 410, 40, hwnd);
    SendMessageW(title, WM_SETFONT, (WPARAM)g_titleFont, TRUE);
    MakeLabel(L"Status:", 590, 40, 65, 22, hwnd);
    MakeLabel(L"", 650, 40, 230, 24, hwnd, IDC_STATUS);
    MakeLabel(L"Atalhos:  Ctrl+Alt+C Clean   •   Ctrl+Alt+P Pausar   •   Ctrl+Alt+E Estudio", 34, 86, 620, 22, hwnd, IDC_SHORTCUT_LABEL);

    MakeButton2(L"Clean", 34, 126, 126, 38, hwnd, IDC_TAB_CLEAN);
    MakeButton2(L"Jogo", 170, 126, 126, 38, hwnd, IDC_TAB_GAME);
    MakeButton2(L"Estudio", 306, 126, 132, 38, hwnd, IDC_TAB_STUDIO);
    MakeButton2(L"Personalizar", 448, 126, 154, 38, hwnd, IDC_TAB_CUSTOMIZE);
    MakeButton2(L"Tema claro", 690, 126, 126, 38, hwnd, IDC_THEME_TOGGLE);
    MakeButton2(L"Iniciar com Windows", 826, 126, 150, 38, hwnd, IDC_STARTUP_TOGGLE);

    // CLEAN
    MakeLabel(L"Automacao visual", 70, 184, 220, 24, hwnd, IDC_LABEL_CLEAN_GROUP1);
    MakeSwitch(L"Modo Clean", 62, 214, 330, 42, hwnd, IDC_CLEAN_ENABLED);
    MakeCheck(L"Esconder icones", 70, 284, 220, 28, hwnd, IDC_HIDE_ICONS);
    MakeLabel(L"Tempo", 330, 288, 48, 22, hwnd, IDC_LABEL_ICONS_DELAY);
    MakeEdit2(L"5", 388, 282, 58, 30, hwnd, IDC_ICONS_DELAY);
    MakeLabel(L"s", 456, 288, 18, 22, hwnd, IDC_LABEL_ICONS_S);

    MakeCheck(L"Esconder cursor", 70, 338, 220, 28, hwnd, IDC_HIDE_CURSOR);
    MakeLabel(L"Tempo", 330, 342, 48, 22, hwnd, IDC_LABEL_CURSOR_DELAY);
    MakeEdit2(L"5", 388, 336, 58, 30, hwnd, IDC_CURSOR_DELAY);
    MakeLabel(L"s", 456, 342, 18, 22, hwnd, IDC_LABEL_CURSOR_S);

    MakeCheck(L"Esconder barra de tarefas", 70, 392, 250, 28, hwnd, IDC_HIDE_TASKBAR);
    MakeLabel(L"Tempo", 330, 396, 48, 22, hwnd, IDC_LABEL_TASKBAR_DELAY);
    MakeEdit2(L"5", 388, 390, 58, 30, hwnd, IDC_TASKBAR_DELAY);
    MakeLabel(L"s", 456, 396, 18, 22, hwnd, IDC_LABEL_TASKBAR_S);
    MakeCheck(L"Barra: so no desktop", 70, 446, 250, 28, hwnd, IDC_TASKBAR_SAFE);
    MakeCheck(L"Icones: janela em foco / volta no desktop", 70, 494, 360, 28, hwnd, IDC_ICONS_RESTORE_DESKTOP);

    // JOGO
    MakeLabel(L"Acoes do modo", 70, 184, 220, 24, hwnd, IDC_LABEL_GAME_GROUP);
    MakeSwitch(L"Modo Jogo", 62, 214, 330, 42, hwnd, IDC_GAME_ENABLED);
    MakeCheck(L"Pausar Clean", 70, 284, 250, 28, hwnd, IDC_GAME_PAUSE_CLEAN);
    MakeCheck(L"Restaurar desktop", 70, 334, 250, 28, hwnd, IDC_GAME_RESTORE);
    MakeCheck(L"Abrir Steam", 70, 384, 250, 28, hwnd, IDC_GAME_STEAM);
    MakeCheck(L"Abrir Xbox", 70, 434, 250, 28, hwnd, IDC_GAME_XBOX);
    MakeCheck(L"Abrir AMD Adrenalin", 70, 484, 250, 28, hwnd, IDC_GAME_ADRENALIN);
    MakeCheck(L"Plano alto desempenho", 70, 534, 260, 28, hwnd, IDC_GAME_POWER);

    // ESTUDIO
    MakeLabel(L"Acoes do modo", 70, 184, 220, 24, hwnd, IDC_LABEL_STUDIO_GROUP);
    MakeButton2(L"Ativar Modo Estudio", 62, 214, 250, 42, hwnd, IDC_STUDIO_TOGGLE_BTN);
    MakeCheck(L"Pausar Clean", 70, 284, 250, 28, hwnd, IDC_STUDIO_PAUSE_CLEAN);
    MakeCheck(L"Restaurar desktop", 70, 334, 250, 28, hwnd, IDC_STUDIO_RESTORE);
    MakeCheck(L"Abrir VS Code", 70, 384, 250, 28, hwnd, IDC_STUDIO_VSCODE);
    MakeCheck(L"YouTube Music pequeno", 70, 434, 270, 28, hwnd, IDC_STUDIO_YTMUSIC);
    MakeLabel(L"Abrir YouTube Music com:", 70, 490, 260, 24, hwnd, IDC_LABEL_YTMUSIC_WITH);
    MakeButton2(L"Edge", 70, 526, 96, 38, hwnd, IDC_STUDIO_BROWSER_EDGE);
    MakeButton2(L"Chrome", 176, 526, 112, 38, hwnd, IDC_STUDIO_BROWSER_CHROME);
    MakeButton2(L"Brave", 298, 526, 102, 38, hwnd, IDC_STUDIO_BROWSER_BRAVE);

    // OPCOES GERAIS
    MakeLabel(L"Sistema", 540, 244, 180, 24, hwnd, IDC_LABEL_CLEAN_GROUP2);
    MakeCheck(L"Iniciar minimizado", 540, 284, 250, 28, hwnd, IDC_START_MINIMIZED);
    MakeCheck(L"Fechar para bandeja", 540, 334, 250, 28, hwnd, IDC_CLOSE_TO_TRAY);

    // PERSONALIZAR
    MakeLabel(L"Cor dos botoes", 70, 190, 240, 26, hwnd, IDC_LABEL_COLOR_TITLE);
    MakeButton2(L"Azul", 70, 232, 88, 38, hwnd, IDC_COLOR_BLUE);
    MakeButton2(L"Roxo", 168, 232, 88, 38, hwnd, IDC_COLOR_PURPLE);
    MakeButton2(L"Prata", 266, 232, 88, 38, hwnd, IDC_COLOR_SILVER);
    MakeButton2(L"Verde", 364, 232, 88, 38, hwnd, IDC_COLOR_GREEN);
    MakeButton2(L"Vermelho", 462, 232, 112, 38, hwnd, IDC_COLOR_RED);
    MakeButton2(L"Y2K", 584, 232, 88, 38, hwnd, IDC_COLOR_Y2K);

    MakeCheck(L"Mostrar notificacoes", 70, 318, 260, 28, hwnd, IDC_SHOW_NOTIFICATIONS);
    MakeButton2(L"Abrir config.ini", 70, 368, 160, 38, hwnd, IDC_OPEN_CONFIG);
    MakeButton2(L"Resetar config", 240, 368, 160, 38, hwnd, IDC_RESET_CONFIG);

    MakeButton2(L"Pausar", 54, 628, 146, 42, hwnd, ID_TRAY_PAUSE_ALL);
    MakeButton2(L"Sair", 212, 628, 104, 42, hwnd, ID_TRAY_EXIT);

    UpdateTabVisibility(hwnd);
}


void HandleFocusedWindowIconHide()
{
    if (!g_cfg.hideIcons || !g_cfg.iconsRestoreOnlyOnDesktop)
    {
        g_appWindowFocusStartTick = 0;
        return;
    }

    bool desktopReadyToRestore = ShouldRestoreIconsByDesktopRule();

    if (desktopReadyToRestore)
    {
        g_appWindowFocusStartTick = 0;

        if (g_iconsHidden)
            ShowDesktopIcons();

        return;
    }

    DWORD now = GetTickCount();

    if (g_appWindowFocusStartTick == 0)
        g_appWindowFocusStartTick = now;

    if (!g_iconsHidden && now - g_appWindowFocusStartTick >= 5000)
        HideDesktopIcons();
}

void RestoreAfterUserActivity()
{
    if (g_iconsHidden)
    {
        if (ShouldRestoreIconsByDesktopRule())
            ShowDesktopIcons();
    }

    if (g_cursorHidden)
        RestoreCursors();

    if (g_taskbarHidden)
        ShowTaskbar();
}

void HandleCleanTick()
{
    if (!IsCleanActuallyActive())
    {
        g_appWindowFocusStartTick = 0;
        if (g_iconsHidden || g_cursorHidden || g_taskbarHidden) RestoreEverything();
        return;
    }

    HandleFocusedWindowIconHide();

    bool blocked = IsActiveWindowMaximizedOrFullscreen();
    if (blocked)
    {
        if (g_cfg.iconsRestoreOnlyOnDesktop && g_iconsHidden)
        {
            if (g_cursorHidden) RestoreCursors();
            if (g_taskbarHidden) ShowTaskbar();
        }
        else if (g_iconsHidden || g_cursorHidden || g_taskbarHidden)
        {
            RestoreEverything();
        }

        g_waitFreshInputAfterBlocked = true;
        g_lastInputTickDuringBlocked = GetLastInputTick();
        return;
    }

    if (g_waitFreshInputAfterBlocked)
    {
        DWORD current = GetLastInputTick();
        if (current == g_lastInputTickDuringBlocked)
        {
            if (g_iconsHidden || g_cursorHidden || g_taskbarHidden) RestoreAfterUserActivity();
            return;
        }
        g_waitFreshInputAfterBlocked = false;
    }

    DWORD idle = GetIdleMs();

    if (idle < 250)
    {
        if (g_iconsHidden || g_cursorHidden || g_taskbarHidden) RestoreAfterUserActivity();
        return;
    }

    if (g_cfg.hideIcons && idle >= (DWORD)g_cfg.hideIconsDelay * 1000 && !g_iconsHidden)
        HideDesktopIcons();

    // Cursor: some apenas quando o desktop esta efetivamente visivel.
    if (g_cfg.hideCursor)
    {
        if (!IsDesktopEffectivelyVisible())
        {
            if (g_cursorHidden) RestoreCursors();
        }
        else if (idle >= (DWORD)g_cfg.hideCursorDelay * 1000 && !g_cursorHidden)
        {
            HideCursors();
        }
    }
    else if (g_cursorHidden) RestoreCursors();

    if (g_cfg.hideTaskbar)
    {
        bool canHideTaskbar = g_cfg.taskbarSafeMode ? IsDesktopEffectivelyVisible() : !IsActiveWindowMaximizedOrFullscreen();
        if (!canHideTaskbar)
        {
            if (g_taskbarHidden) ShowTaskbar();
        }
        else if (idle >= (DWORD)g_cfg.hideTaskbarDelay * 1000 && !g_taskbarHidden)
        {
            HideTaskbar();
        }
    }
    else if (g_taskbarHidden) ShowTaskbar();
}


void FillRound(HDC dc, RECT rc, COLORREF color, int radius)
{
    HBRUSH brush = CreateSolidBrush(color);
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    RoundRect(dc, rc.left, rc.top, rc.right, rc.bottom, radius, radius);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}


bool IsControlCheckedByConfig(int id)
{
    switch (id)
    {
    case IDC_CLEAN_ENABLED: return g_cfg.cleanEnabled;
    case IDC_HIDE_ICONS: return g_cfg.hideIcons;
    case IDC_HIDE_CURSOR: return g_cfg.hideCursor;
    case IDC_HIDE_TASKBAR: return g_cfg.hideTaskbar;
    case IDC_TASKBAR_SAFE: return g_cfg.taskbarSafeMode;
    case IDC_ICONS_RESTORE_DESKTOP: return g_cfg.iconsRestoreOnlyOnDesktop;

    case IDC_GAME_ENABLED: return g_cfg.gameEnabled;
    case IDC_GAME_PAUSE_CLEAN: return g_cfg.gamePauseClean;
    case IDC_GAME_RESTORE: return g_cfg.gameRestoreDesktop;
    case IDC_GAME_STEAM: return g_cfg.gameOpenSteam;
    case IDC_GAME_XBOX: return g_cfg.gameOpenXbox;
    case IDC_GAME_ADRENALIN: return g_cfg.gameOpenAdrenalin;
    case IDC_GAME_POWER: return g_cfg.gameHighPerformance;

    case IDC_STUDIO_TOGGLE_BTN: return g_cfg.studioEnabled;
    case IDC_STUDIO_PAUSE_CLEAN: return g_cfg.studioPauseClean;
    case IDC_STUDIO_RESTORE: return g_cfg.studioRestoreDesktop;
    case IDC_STUDIO_VSCODE: return g_cfg.studioOpenVSCode;
    case IDC_STUDIO_YTMUSIC: return g_cfg.studioOpenYouTubeMusic;
    case IDC_START_MINIMIZED: return g_cfg.startMinimized;
    case IDC_CLOSE_TO_TRAY: return g_cfg.closeToTray;
    case IDC_SHOW_NOTIFICATIONS: return g_cfg.showNotifications;

    default: return false;
    }
}

void ToggleConfigOptionById(int id)
{
    switch (id)
    {
    case IDC_HIDE_ICONS: g_cfg.hideIcons = !g_cfg.hideIcons; if (!g_cfg.hideIcons) ShowDesktopIcons(); break;
    case IDC_HIDE_CURSOR: g_cfg.hideCursor = !g_cfg.hideCursor; if (!g_cfg.hideCursor) RestoreCursors(); break;
    case IDC_HIDE_TASKBAR: g_cfg.hideTaskbar = !g_cfg.hideTaskbar; if (!g_cfg.hideTaskbar) ShowTaskbar(); break;
    case IDC_TASKBAR_SAFE: g_cfg.taskbarSafeMode = !g_cfg.taskbarSafeMode; break;
    case IDC_ICONS_RESTORE_DESKTOP: g_cfg.iconsRestoreOnlyOnDesktop = !g_cfg.iconsRestoreOnlyOnDesktop; break;

    case IDC_GAME_PAUSE_CLEAN: g_cfg.gamePauseClean = !g_cfg.gamePauseClean; break;
    case IDC_GAME_RESTORE: g_cfg.gameRestoreDesktop = !g_cfg.gameRestoreDesktop; break;
    case IDC_GAME_STEAM: g_cfg.gameOpenSteam = !g_cfg.gameOpenSteam; break;
    case IDC_GAME_XBOX: g_cfg.gameOpenXbox = !g_cfg.gameOpenXbox; break;
    case IDC_GAME_ADRENALIN: g_cfg.gameOpenAdrenalin = !g_cfg.gameOpenAdrenalin; break;
    case IDC_GAME_POWER: g_cfg.gameHighPerformance = !g_cfg.gameHighPerformance; break;

    case IDC_STUDIO_PAUSE_CLEAN: g_cfg.studioPauseClean = !g_cfg.studioPauseClean; break;
    case IDC_STUDIO_RESTORE: g_cfg.studioRestoreDesktop = !g_cfg.studioRestoreDesktop; break;
    case IDC_STUDIO_VSCODE: g_cfg.studioOpenVSCode = !g_cfg.studioOpenVSCode; break;
    case IDC_STUDIO_YTMUSIC: g_cfg.studioOpenYouTubeMusic = !g_cfg.studioOpenYouTubeMusic; break;
    case IDC_START_MINIMIZED: g_cfg.startMinimized = !g_cfg.startMinimized; break;
    case IDC_CLOSE_TO_TRAY: g_cfg.closeToTray = !g_cfg.closeToTray; break;
    case IDC_SHOW_NOTIFICATIONS: g_cfg.showNotifications = !g_cfg.showNotifications; break;
    }
}

void DrawSwitchControl(const DRAWITEMSTRUCT* dis)
{
    HDC dc = dis->hDC;
    RECT rc = dis->rcItem;
    bool checked = IsControlCheckedByConfig(GetDlgCtrlID(dis->hwndItem));

    SetBkMode(dc, TRANSPARENT);
    FillRound(dc, rc, CLR_CARD_2, 18);

    wchar_t text[128]{};
    GetWindowTextW(dis->hwndItem, text, 128);
    RECT textRc = rc;
    textRc.left += 14;
    textRc.right -= 74;
    SetTextColor(dc, CLR_TEXT);
    DrawTextW(dc, text, -1, &textRc, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS);

    RECT track{ rc.right - 64, rc.top + 7, rc.right - 14, rc.bottom - 7 };
    FillRound(dc, track, checked ? CLR_ACCENT_2 : CLR_OFF, 20);

    int knob = (track.bottom - track.top) - 6;
    RECT k{};
    if (checked)
        k = { track.right - knob - 3, track.top + 3, track.right - 3, track.bottom - 3 };
    else
        k = { track.left + 3, track.top + 3, track.left + knob + 3, track.bottom - 3 };
    FillRound(dc, k, RGB(250, 252, 255), knob);
}


void DrawCheckControl(const DRAWITEMSTRUCT* dis)
{
    HDC dc = dis->hDC;
    RECT rc = dis->rcItem;
    bool checked = IsControlCheckedByConfig(GetDlgCtrlID(dis->hwndItem));

    SetBkMode(dc, TRANSPARENT);
    FillRound(dc, rc, CLR_CARD, 16);

    RECT box{ rc.left + 8, rc.top + 6, rc.left + 28, rc.top + 26 };
    HBRUSH boxBrush = CreateSolidBrush(checked ? CLR_ACCENT_2 : CLR_CARD_2);
    HPEN boxPen = CreatePen(PS_SOLID, 1, checked ? CLR_ACCENT_2 : CLR_OFF);
    HGDIOBJ oldBrush = SelectObject(dc, boxBrush);
    HGDIOBJ oldPen = SelectObject(dc, boxPen);
    RoundRect(dc, box.left, box.top, box.right, box.bottom, 5, 5);

    if (checked)
    {
        HPEN checkPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
        SelectObject(dc, checkPen);
        MoveToEx(dc, box.left + 5, box.top + 10, nullptr);
        LineTo(dc, box.left + 9, box.top + 14);
        LineTo(dc, box.left + 16, box.top + 6);
        DeleteObject(checkPen);
    }

    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(boxBrush);
    DeleteObject(boxPen);

    wchar_t text[160]{};
    GetWindowTextW(dis->hwndItem, text, 160);
    RECT textRc = rc;
    textRc.left += 38;
    SetTextColor(dc, CLR_TEXT);
    DrawTextW(dc, text, -1, &textRc, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS);
}

void DrawButtonControl(const DRAWITEMSTRUCT* dis)
{
    HDC dc = dis->hDC;
    RECT rc = dis->rcItem;
    bool down = (dis->itemState & ODS_SELECTED) != 0;
    int id = GetDlgCtrlID(dis->hwndItem);

    bool isBrowser =
        id == IDC_STUDIO_BROWSER_EDGE ||
        id == IDC_STUDIO_BROWSER_CHROME ||
        id == IDC_STUDIO_BROWSER_BRAVE;

    bool selectedBrowser =
        (id == IDC_STUDIO_BROWSER_EDGE && g_cfg.studioBrowser == L"edge") ||
        (id == IDC_STUDIO_BROWSER_CHROME && g_cfg.studioBrowser == L"chrome") ||
        (id == IDC_STUDIO_BROWSER_BRAVE && g_cfg.studioBrowser == L"brave");

    bool selectedTab =
        (id == IDC_TAB_CLEAN && g_activeTab == 0) ||
        (id == IDC_TAB_GAME && g_activeTab == 1) ||
        (id == IDC_TAB_STUDIO && g_activeTab == 2) ||
        (id == IDC_TAB_CUSTOMIZE && g_activeTab == 3) ||
        (id == IDC_TAB_GENERAL && g_activeTab == 3);

    bool selectedTheme = (id == IDC_THEME_TOGGLE);
    bool selectedStartup = (id == IDC_STARTUP_TOGGLE && IsStartupEnabled());

    bool selectedColor =
        (id == IDC_COLOR_BLUE && g_cfg.accentColor == 0) ||
        (id == IDC_COLOR_PURPLE && g_cfg.accentColor == 1) ||
        (id == IDC_COLOR_SILVER && g_cfg.accentColor == 2) ||
        (id == IDC_COLOR_GREEN && g_cfg.accentColor == 3) ||
        (id == IDC_COLOR_RED && g_cfg.accentColor == 4) ||
        (id == IDC_COLOR_Y2K && g_cfg.accentColor == 5);


    bool accentButton =
        id == IDC_SAVE ||
        id == ID_TRAY_PAUSE_ALL ||
        id == ID_TRAY_EXIT ||
        id == IDC_STUDIO_TOGGLE_BTN ||
        id == IDC_OPEN_CONFIG ||
        id == IDC_RESET_CONFIG;

    COLORREF fill = CLR_CARD_2;
    COLORREF border = CLR_CARD_2;
    COLORREF textColor = CLR_TEXT;

    if (isBrowser)
    {
        if (selectedBrowser)
        {
            fill = CLR_ACCENT_2;
            border = CLR_ACCENT_2;
            textColor = RGB(255, 255, 255);
        }
        else
        {
            fill = CLR_CARD;
            border = CLR_ACCENT_2;
            textColor = CLR_TEXT;
        }
    }
    else if (selectedTab || selectedTheme || selectedStartup || selectedColor || accentButton)
    {
        fill = CLR_ACCENT_2;
        border = CLR_ACCENT_2;
        textColor = RGB(255, 255, 255);
    }

    if (down)
        fill = RGB(65, 95, 150);

    HBRUSH brush = CreateSolidBrush(fill);
    HPEN pen = CreatePen(PS_SOLID, 1, border);
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    HGDIOBJ oldPen = SelectObject(dc, pen);

    RoundRect(dc, rc.left, rc.top, rc.right, rc.bottom, 18, 18);

    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);

    wchar_t text[128]{};
    GetWindowTextW(dis->hwndItem, text, 128);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, textColor);
    DrawTextW(dc, text, -1, &rc, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void ApplyGlassWindowStyle(HWND hwnd)
{
    BOOL dark = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

    int corner = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        ApplyGlassWindowStyle(hwnd);
        CreatePanelControls(hwnd);
        UpdatePanelFromConfig(hwnd);
        AddTrayIcon(hwnd);
        ApplyPersistedModesOnStartup();
        RegisterHotKey(hwnd, HOTKEY_TOGGLE_CLEAN, MOD_CONTROL | MOD_ALT, 'C');
        RegisterHotKey(hwnd, HOTKEY_PAUSE_ALL, MOD_CONTROL | MOD_ALT, 'P');
        RegisterHotKey(hwnd, HOTKEY_STUDIO, MOD_CONTROL | MOD_ALT, 'E');
        SetTimer(hwnd, TIMER_ID, TIMER_MS, nullptr);
        return 0;

    case WM_TIMER:
        HandleCleanTick();
        UpdateStatusText();
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps{};
        HDC dc = BeginPaint(hwnd, &ps);
        RECT rc{}; GetClientRect(hwnd, &rc);
        FillRect(dc, &rc, g_bgBrush);
        RECT cardMain{22, 178, 958, 602};
        RECT footer{22, 616, 650, 680};
        FillRound(dc, cardMain, CLR_CARD, 28);
        FillRound(dc, footer, CLR_CARD, 24);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_CTLCOLORSTATIC:
    {
        HDC dc = (HDC)wParam;
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, CLR_TEXT);
        return (LRESULT)GetStockObject(HOLLOW_BRUSH);
    }



    case WM_CTLCOLORBTN:
    {
        HDC dc = (HDC)wParam;
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, CLR_TEXT);
        return (LRESULT)g_cardBrush;
    }

    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    {
        HDC dc = (HDC)wParam;
        SetBkColor(dc, CLR_CARD_2);
        SetTextColor(dc, CLR_TEXT);
        return (LRESULT)g_editBrush;
    }

    case WM_DRAWITEM:
    {
        auto* dis = (DRAWITEMSTRUCT*)lParam;
        wchar_t cls[32]{};
        GetClassNameW(dis->hwndItem, cls, 32);
        int drawId = GetDlgCtrlID(dis->hwndItem);

        if (IsModeSwitchId(drawId))
            DrawSwitchControl(dis);
        else if (IsOptionCheckId(drawId))
            DrawCheckControl(dis);
        else
            DrawButtonControl(dis);
        return TRUE;
    }

    case WM_CLOSE:
        ForceSaveConfig();
        if (g_cfg.closeToTray)
        {
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        }
        RestoreEverything();
        RemoveTrayIcon();
        DestroyWindow(hwnd);
        return 0;


    case WM_HOTKEY:
        if (wParam == HOTKEY_TOGGLE_CLEAN)
        {
            g_cfg.cleanEnabled = !g_cfg.cleanEnabled;
            SaveConfig();
            UpdatePanelFromConfig(hwnd);
            ShowTrayBalloon(APP_NAME, g_cfg.cleanEnabled ? L"Modo Clean ativado." : L"Modo Clean desligado.");
            return 0;
        }
        if (wParam == HOTKEY_PAUSE_ALL)
        {
            g_pausedAll = !g_pausedAll;
            if (g_pausedAll) RestoreEverything();
            UpdatePanelFromConfig(hwnd);
            ShowTrayBalloon(APP_NAME, g_pausedAll ? L"App pausado." : L"App ativado.");
            return 0;
        }
        if (wParam == HOTKEY_STUDIO)
        {
            ToggleStudioMode();
            UpdatePanelFromConfig(hwnd);
            ShowTrayBalloon(APP_NAME, g_cfg.studioEnabled ? L"Modo Estudio ativado." : L"Modo Estudio desligado.");
            return 0;
        }
        return 0;

    case WM_COMMAND:
        if ((LOWORD(wParam) == IDC_ICONS_DELAY || LOWORD(wParam) == IDC_CURSOR_DELAY || LOWORD(wParam) == IDC_TASKBAR_DELAY)
            && HIWORD(wParam) == EN_KILLFOCUS)
        {
            SaveCurrentDelaysFromPanel(hwnd);
            UpdatePanelFromConfig(hwnd);
            return 0;
        }

        switch (LOWORD(wParam))
        {
        case IDC_SAVE:
            SaveCurrentDelaysFromPanel(hwnd);
            UpdatePanelFromConfig(hwnd);
            return 0;
        case IDC_RESET_CONFIG:
            if (MessageBoxW(hwnd, L"Resetar todas as configuracoes para o padrao?", APP_NAME, MB_YESNO | MB_ICONQUESTION) == IDYES)
            {
                RestoreEverything();
                CloseStudioWindows();
                ResetConfigToDefaults();
                ApplyThemeColors();
                SaveConfig();
                UpdatePanelFromConfig(hwnd);
                ShowTrayBalloon(APP_NAME, L"Configuracoes resetadas.");
            }
            return 0;
        case IDC_OPEN_CONFIG:
            ShellExecuteW(nullptr, L"open", ConfigPath().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            return 0;
        case IDC_RESTORE_ALL:
        case ID_TRAY_RESTORE:
            RestoreEverything();
            UpdatePanelFromConfig(hwnd);
            return 0;
        case ID_TRAY_OPEN_PANEL:
            ShowWindow(hwnd, SW_SHOW);
            SetForegroundWindow(hwnd);
            return 0;
        case IDC_CLEAN_ENABLED:
            g_cfg.cleanEnabled = !g_cfg.cleanEnabled;
            SaveConfig();
            UpdatePanelFromConfig(hwnd);
            return 0;
        case ID_TRAY_TOGGLE_CLEAN:
            g_cfg.cleanEnabled = !g_cfg.cleanEnabled;
            SaveConfig();
            UpdatePanelFromConfig(hwnd);
            return 0;
        case ID_TRAY_TOGGLE_ICONS:
            g_cfg.hideIcons = !g_cfg.hideIcons;
            if (!g_cfg.hideIcons) ShowDesktopIcons();
            SaveConfig();
            UpdatePanelFromConfig(hwnd);
            return 0;
        case ID_TRAY_TOGGLE_CURSOR:
            g_cfg.hideCursor = !g_cfg.hideCursor;
            if (!g_cfg.hideCursor) RestoreCursors();
            SaveConfig();
            UpdatePanelFromConfig(hwnd);
            return 0;
        case ID_TRAY_TOGGLE_TASKBAR:
            g_cfg.hideTaskbar = !g_cfg.hideTaskbar;
            if (!g_cfg.hideTaskbar) ShowTaskbar();
            SaveConfig();
            UpdatePanelFromConfig(hwnd);
            return 0;

        case IDC_HIDE_ICONS:
        case IDC_HIDE_CURSOR:
        case IDC_HIDE_TASKBAR:
        case IDC_TASKBAR_SAFE:
        case IDC_ICONS_RESTORE_DESKTOP:
        case IDC_GAME_PAUSE_CLEAN:
        case IDC_GAME_RESTORE:
        case IDC_GAME_STEAM:
        case IDC_GAME_XBOX:
        case IDC_GAME_ADRENALIN:
        case IDC_GAME_POWER:
        case IDC_STUDIO_PAUSE_CLEAN:
        case IDC_STUDIO_RESTORE:
        case IDC_STUDIO_VSCODE:
        case IDC_STUDIO_YTMUSIC:
        case IDC_START_MINIMIZED:
        case IDC_CLOSE_TO_TRAY:
        case IDC_SHOW_NOTIFICATIONS:
                    ToggleConfigOptionById(LOWORD(wParam));
            SaveConfig();
            UpdatePanelFromConfig(hwnd);
            return 0;
        case IDC_GAME_ENABLED:
        {
            SetGameMode(!g_cfg.gameEnabled);
            UpdatePanelFromConfig(hwnd);
            return 0;
        }
        case ID_TRAY_GAME_MODE:
            ToggleGameMode();
            UpdatePanelFromConfig(hwnd);
            return 0;
        case IDC_STUDIO_TOGGLE_BTN:
        {
            SetStudioMode(!g_cfg.studioEnabled);
            UpdatePanelFromConfig(hwnd);
            return 0;
        }
        case ID_TRAY_STUDIO_MODE:
            ToggleStudioMode();
            UpdatePanelFromConfig(hwnd);
            return 0;
        case IDC_STUDIO_BROWSER_EDGE:
            g_cfg.studioBrowser = L"edge";
            SaveConfig();
            UpdatePanelFromConfig(hwnd);
            return 0;
        case IDC_STUDIO_BROWSER_CHROME:
            g_cfg.studioBrowser = L"chrome";
            SaveConfig();
            UpdatePanelFromConfig(hwnd);
            return 0;
        case IDC_STUDIO_BROWSER_BRAVE:
            g_cfg.studioBrowser = L"brave";
            SaveConfig();
            UpdatePanelFromConfig(hwnd);
            return 0;
        case IDC_TAB_CLEAN:
            g_activeTab = 0;
            UpdatePanelFromConfig(hwnd);
            return 0;
        case IDC_TAB_GAME:
            g_activeTab = 1;
            UpdatePanelFromConfig(hwnd);
            return 0;
        case IDC_TAB_STUDIO:
            g_activeTab = 2;
            UpdatePanelFromConfig(hwnd);
            return 0;
        case IDC_TAB_GENERAL:
        case IDC_TAB_CUSTOMIZE:
            g_activeTab = 3;
            UpdatePanelFromConfig(hwnd);
            return 0;
        case IDC_THEME_TOGGLE:
            g_cfg.darkTheme = !g_cfg.darkTheme;
            ApplyThemeColors();
            SaveConfig();
            UpdatePanelFromConfig(hwnd);
            return 0;
        case IDC_STARTUP_TOGGLE:
            SetStartupEnabled(!IsStartupEnabled());
            UpdatePanelFromConfig(hwnd);
            ShowTrayBalloon(APP_NAME, IsStartupEnabled() ? L"Inicializacao com Windows ativada." : L"Inicializacao removida.");
            return 0;

        case IDC_COLOR_BLUE:
        case IDC_COLOR_PURPLE:
        case IDC_COLOR_SILVER:
        case IDC_COLOR_GREEN:
        case IDC_COLOR_RED:
        case IDC_COLOR_Y2K:
        {
            int id = LOWORD(wParam);
            if (id == IDC_COLOR_BLUE) g_cfg.accentColor = 0;
            if (id == IDC_COLOR_PURPLE) g_cfg.accentColor = 1;
            if (id == IDC_COLOR_SILVER) g_cfg.accentColor = 2;
            if (id == IDC_COLOR_GREEN) g_cfg.accentColor = 3;
            if (id == IDC_COLOR_RED) g_cfg.accentColor = 4;
            if (id == IDC_COLOR_Y2K) g_cfg.accentColor = 5;
            ApplyThemeColors();
            SaveConfig();
            UpdatePanelFromConfig(hwnd);
            return 0;
        }
        case ID_TRAY_PAUSE_ALL:
            g_pausedAll = !g_pausedAll;
            if (g_pausedAll) RestoreEverything();
            UpdatePanelFromConfig(hwnd);
            ShowTrayBalloon(APP_NAME, g_pausedAll ? L"App pausado." : L"App ativado.");
            return 0;
        case ID_TRAY_EXIT:
            ForceSaveConfig();
            RestoreEverything();
            RemoveTrayIcon();
            DestroyWindow(hwnd);
            return 0;
        }
        break;

    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP)
            ShowTrayMenu(hwnd);
        else if (lParam == WM_LBUTTONDBLCLK)
        {
            ShowWindow(hwnd, SW_SHOW);
            SetForegroundWindow(hwnd);
        }
        return 0;


    case WM_QUERYENDSESSION:
        ForceSaveConfig();
        return TRUE;

    case WM_ENDSESSION:
        if (wParam)
            ForceSaveConfig();
        return 0;

    case WM_DESTROY:
        ForceSaveConfig();
        UnregisterHotKey(hwnd, HOTKEY_TOGGLE_CLEAN);
        UnregisterHotKey(hwnd, HOTKEY_PAUSE_ALL);
        UnregisterHotKey(hwnd, HOTKEY_STUDIO);
        CloseStudioWindows();
        RestoreEverything();
        RemoveTrayIcon();
        if (g_bgBrush) DeleteObject(g_bgBrush);
        if (g_cardBrush) DeleteObject(g_cardBrush);
        if (g_editBrush) DeleteObject(g_editBrush);
        if (g_titleFont) DeleteObject(g_titleFont);
        if (g_uiFont) DeleteObject(g_uiFont);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool FocusExistingInstance()
{
    HWND existing = FindWindowW(L"WabbeControlCenterWindow", nullptr);
    if (!existing)
        return false;

    ShowWindow(existing, SW_SHOW);
    ShowWindow(existing, SW_RESTORE);
    SetForegroundWindow(existing);
    return true;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    HANDLE singleInstanceMutex = CreateMutexW(nullptr, TRUE, L"WabbeControlCenter_SingleInstance_Mutex");

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        FocusExistingInstance();
        if (singleInstanceMutex) CloseHandle(singleInstanceMutex);
        return 0;
    }

    g_hInst = hInstance;
    MigrateLegacyConfigIfNeeded();
    LoadConfig();
    SaveConfig();
    ApplyThemeColors();

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"WabbeControlCenterWindow";
    wc.hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_APP));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = g_bgBrush;

    RegisterClassW(&wc);

    g_hwnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        APP_NAME,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1000,
        750,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!g_hwnd) return 1;

    bool startMinimized = g_cfg.startMinimized;
    if (!startMinimized)
        ShowWindow(g_hwnd, SW_SHOW);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (singleInstanceMutex) CloseHandle(singleInstanceMutex);
    return 0;
}

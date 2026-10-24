"""Camada de Windows (ctypes). Só funciona no Windows.

Tudo que mexe no sistema fica aqui: ícones do desktop, barra de tarefas,
cursor, janelas, inicialização com o Windows e atalhos globais.
"""
import ctypes
import os
import subprocess
import sys
import winreg
from ctypes import wintypes

user32 = ctypes.WinDLL("user32", use_last_error=True)
kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
dwmapi = ctypes.WinDLL("dwmapi", use_last_error=True)

HWND, LPARAM, BOOL, DWORD, UINT = wintypes.HWND, wintypes.LPARAM, wintypes.BOOL, wintypes.DWORD, wintypes.UINT
WNDENUMPROC = ctypes.WINFUNCTYPE(BOOL, HWND, LPARAM)

SW_HIDE, SW_SHOW = 0, 5
GW_OWNER = 4
GWL_EXSTYLE = -20
WS_EX_TOOLWINDOW = 0x00000080
DWMWA_CLOAKED = 14
SPI_SETCURSORS = 0x0057
IMAGE_CURSOR = 2
LR_COPYFROMRESOURCE = 0x4000
MONITOR_DEFAULTTONEAREST = 2
WM_CLOSE, WM_HOTKEY, WM_QUIT = 0x0010, 0x0312, 0x0012
PROCESS_QUERY_LIMITED_INFORMATION = 0x1000
ERROR_ALREADY_EXISTS = 183
CREATE_NO_WINDOW = 0x08000000


class LASTINPUTINFO(ctypes.Structure):
    _fields_ = [("cbSize", UINT), ("dwTime", DWORD)]


class MONITORINFO(ctypes.Structure):
    _fields_ = [("cbSize", DWORD), ("rcMonitor", wintypes.RECT),
                ("rcWork", wintypes.RECT), ("dwFlags", DWORD)]


def _sig(func, restype, *argtypes):
    func.restype = restype
    func.argtypes = list(argtypes)


_sig(user32.FindWindowW, HWND, wintypes.LPCWSTR, wintypes.LPCWSTR)
_sig(user32.FindWindowExW, HWND, HWND, HWND, wintypes.LPCWSTR, wintypes.LPCWSTR)
_sig(user32.ShowWindow, BOOL, HWND, ctypes.c_int)
_sig(user32.GetForegroundWindow, HWND)
_sig(user32.GetClassNameW, ctypes.c_int, HWND, wintypes.LPWSTR, ctypes.c_int)
_sig(user32.GetWindowTextW, ctypes.c_int, HWND, wintypes.LPWSTR, ctypes.c_int)
_sig(user32.GetLastInputInfo, BOOL, ctypes.POINTER(LASTINPUTINFO))
_sig(user32.IsWindowVisible, BOOL, HWND)
_sig(user32.IsIconic, BOOL, HWND)
_sig(user32.IsZoomed, BOOL, HWND)
_sig(user32.GetWindow, HWND, HWND, UINT)
_sig(user32.GetWindowRect, BOOL, HWND, ctypes.POINTER(wintypes.RECT))
_sig(user32.GetWindowThreadProcessId, DWORD, HWND, ctypes.POINTER(DWORD))
_sig(user32.EnumWindows, BOOL, WNDENUMPROC, LPARAM)
_sig(user32.PostMessageW, BOOL, HWND, UINT, wintypes.WPARAM, LPARAM)
_sig(user32.MonitorFromWindow, wintypes.HANDLE, HWND, DWORD)
_sig(user32.GetMonitorInfoW, BOOL, wintypes.HANDLE, ctypes.POINTER(MONITORINFO))
_sig(user32.SystemParametersInfoW, BOOL, UINT, UINT, ctypes.c_void_p, UINT)
_sig(user32.CreateCursor, wintypes.HANDLE, wintypes.HANDLE, ctypes.c_int, ctypes.c_int,
     ctypes.c_int, ctypes.c_int, ctypes.c_void_p, ctypes.c_void_p)
_sig(user32.CopyImage, wintypes.HANDLE, wintypes.HANDLE, UINT, ctypes.c_int, ctypes.c_int, UINT)
_sig(user32.SetSystemCursor, BOOL, wintypes.HANDLE, DWORD)
_sig(user32.DestroyCursor, BOOL, wintypes.HANDLE)
_sig(user32.RegisterHotKey, BOOL, HWND, ctypes.c_int, UINT, UINT)
_sig(user32.GetMessageW, ctypes.c_int, ctypes.POINTER(wintypes.MSG), HWND, UINT, UINT)
_sig(user32.PostThreadMessageW, BOOL, DWORD, UINT, wintypes.WPARAM, LPARAM)
_sig(user32.MessageBoxW, ctypes.c_int, HWND, wintypes.LPCWSTR, wintypes.LPCWSTR, UINT)
_sig(kernel32.GetTickCount, DWORD)
_sig(kernel32.GetCurrentThreadId, DWORD)
_sig(kernel32.OpenProcess, wintypes.HANDLE, DWORD, BOOL, DWORD)
_sig(kernel32.CloseHandle, BOOL, wintypes.HANDLE)
_sig(kernel32.QueryFullProcessImageNameW, BOOL, wintypes.HANDLE, DWORD, wintypes.LPWSTR, ctypes.POINTER(DWORD))
_sig(kernel32.CreateMutexW, wintypes.HANDLE, ctypes.c_void_p, BOOL, wintypes.LPCWSTR)
_sig(dwmapi.DwmGetWindowAttribute, ctypes.c_long, HWND, DWORD, ctypes.c_void_p, DWORD)

_get_style = getattr(user32, "GetWindowLongPtrW", None) or user32.GetWindowLongW
_get_style.restype = ctypes.c_ssize_t
_get_style.argtypes = [HWND, ctypes.c_int]

_OWN_PID = os.getpid()
_IGNORED_TEXT = ("taskbar hero", "taskbarhero")
_IGNORED_CLASSES = {
    "Progman", "WorkerW", "Shell_TrayWnd", "Shell_SecondaryTrayWnd", "Button",
    "NotifyIconOverflowWindow", "Windows.UI.Core.CoreWindow",
    "Windows.UI.Composition.DesktopWindowContentBridge",
}
_CURSOR_IDS = (32512, 32513, 32514, 32515, 32516, 32642, 32643,
               32644, 32645, 32646, 32648, 32649, 32650, 32651)
_path_cache = {}


# ---------------------------------------------------------------- helpers
def _class_name(hwnd):
    buf = ctypes.create_unicode_buffer(256)
    user32.GetClassNameW(hwnd, buf, 256)
    return buf.value


def _window_text(hwnd):
    buf = ctypes.create_unicode_buffer(512)
    user32.GetWindowTextW(hwnd, buf, 512)
    return buf.value


def _pid_of(hwnd):
    pid = DWORD(0)
    user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
    return pid.value


def _process_path(pid):
    if pid in _path_cache:
        return _path_cache[pid]
    path = ""
    h = kernel32.OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, False, pid)
    if h:
        buf = ctypes.create_unicode_buffer(1024)
        size = DWORD(1024)
        if kernel32.QueryFullProcessImageNameW(h, 0, buf, ctypes.byref(size)):
            path = buf.value.lower()
        kernel32.CloseHandle(h)
    if len(_path_cache) > 500:
        _path_cache.clear()
    _path_cache[pid] = path
    return path


def _is_ignored(hwnd):
    if _class_name(hwnd) in _IGNORED_CLASSES:
        return True
    pid = _pid_of(hwnd)
    if pid == _OWN_PID:  # o próprio painel do Wabbe não conta como "janela aberta"
        return True
    text = _window_text(hwnd).lower()
    path = _process_path(pid)
    return any(t in text or t in path for t in _IGNORED_TEXT)


def _is_real_app_window(hwnd):
    if not user32.IsWindowVisible(hwnd) or user32.IsIconic(hwnd):
        return False
    if _is_ignored(hwnd):
        return False
    if user32.GetWindow(hwnd, GW_OWNER):
        return False
    if _get_style(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW:
        return False
    cloaked = BOOL(0)
    dwmapi.DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, ctypes.byref(cloaked), ctypes.sizeof(cloaked))
    if cloaked.value:
        return False
    r = wintypes.RECT()
    if not user32.GetWindowRect(hwnd, ctypes.byref(r)):
        return False
    return (r.right - r.left) >= 80 and (r.bottom - r.top) >= 80


def has_visible_app_window():
    found = []

    def cb(hwnd, _):
        if _is_real_app_window(hwnd):
            found.append(hwnd)
            return False  # para a enumeração
        return True

    user32.EnumWindows(WNDENUMPROC(cb), 0)
    return bool(found)


def active_window_maximized_or_fullscreen():
    hwnd = user32.GetForegroundWindow()
    if not hwnd or _is_ignored(hwnd):
        return False
    if user32.IsZoomed(hwnd):
        return True
    r = wintypes.RECT()
    if not user32.GetWindowRect(hwnd, ctypes.byref(r)):
        return False
    mon = user32.MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST)
    if not mon:
        return False
    mi = MONITORINFO()
    mi.cbSize = ctypes.sizeof(MONITORINFO)
    if not user32.GetMonitorInfoW(mon, ctypes.byref(mi)):
        return False
    m, tol = mi.rcMonitor, 20
    return (r.left <= m.left + tol and r.top <= m.top + tol and
            r.right >= m.right - tol and r.bottom >= m.bottom - tol)


def last_input_tick():
    li = LASTINPUTINFO()
    li.cbSize = ctypes.sizeof(LASTINPUTINFO)
    if not user32.GetLastInputInfo(ctypes.byref(li)):
        return kernel32.GetTickCount()
    return li.dwTime


def idle_ms():
    return (kernel32.GetTickCount() - last_input_tick()) & 0xFFFFFFFF


def _desktop_listview():
    progman = user32.FindWindowW("Progman", None)
    shell = user32.FindWindowExW(progman, None, "SHELLDLL_DefView", None) if progman else None
    if not shell:
        worker = None
        while True:
            worker = user32.FindWindowExW(None, worker, "WorkerW", None)
            if not worker:
                break
            shell = user32.FindWindowExW(worker, None, "SHELLDLL_DefView", None)
            if shell:
                break
    if not shell:
        return None
    return (user32.FindWindowExW(shell, None, "SysListView32", "FolderView")
            or user32.FindWindowExW(shell, None, "SysListView32", None))


def _taskbars():
    bars = []
    main = user32.FindWindowW("Shell_TrayWnd", None)
    if main:
        bars.append(main)
    sec = None
    while True:
        sec = user32.FindWindowExW(None, sec, "Shell_SecondaryTrayWnd", None)
        if not sec:
            break
        bars.append(sec)
    return bars


class WinApi:
    """Implementa a interface que o CleanEngine espera."""

    def show_icons(self):
        h = _desktop_listview()
        if h:
            user32.ShowWindow(h, SW_SHOW)

    def hide_icons(self):
        h = _desktop_listview()
        if h:
            user32.ShowWindow(h, SW_HIDE)

    def show_taskbar(self):
        for h in _taskbars():
            user32.ShowWindow(h, SW_SHOW)

    def hide_taskbar(self):
        for h in _taskbars():
            user32.ShowWindow(h, SW_HIDE)

    def restore_cursors(self):
        user32.SystemParametersInfoW(SPI_SETCURSORS, 0, None, 0)

    def hide_cursors(self):
        and_mask = (ctypes.c_ubyte * 128)(*([0xFF] * 128))
        xor_mask = (ctypes.c_ubyte * 128)()
        blank = user32.CreateCursor(None, 0, 0, 32, 32, and_mask, xor_mask)
        if not blank:
            return False
        for cid in _CURSOR_IDS:  # SetSystemCursor consome o handle: precisa de uma cópia por cursor
            copy = user32.CopyImage(blank, IMAGE_CURSOR, 0, 0, LR_COPYFROMRESOURCE)
            if copy:
                user32.SetSystemCursor(copy, cid)
        user32.DestroyCursor(blank)
        return True

    def desktop_visible(self):
        return not has_visible_app_window()

    def active_window_blocked(self):
        return active_window_maximized_or_fullscreen()

    def last_input_tick(self):
        return last_input_tick()

    def idle_ms(self):
        return idle_ms()

    def now_ms(self):
        return kernel32.GetTickCount()


# ------------------------------------------------------------ sistema
def acquire_single_instance(name="Local\\WabbeControlCenterPy"):
    """Devolve (handle, ok). Guarde o handle até o programa fechar."""
    h = kernel32.CreateMutexW(None, True, name)
    return h, ctypes.get_last_error() != ERROR_ALREADY_EXISTS


def message_box(text, title="Wabbe Control Center"):
    user32.MessageBoxW(None, text, title, 0x40)


_RUN_KEY = r"Software\Microsoft\Windows\CurrentVersion\Run"
_RUN_NAME = "WabbeControlCenterPy"


def startup_command(script_path):
    if getattr(sys, "frozen", False):
        return f'"{sys.executable}" --autostart'
    pyw = os.path.join(os.path.dirname(sys.executable), "pythonw.exe")
    exe = pyw if os.path.exists(pyw) else sys.executable
    return f'"{exe}" "{os.path.abspath(script_path)}" --autostart'


def is_startup_enabled():
    try:
        with winreg.OpenKey(winreg.HKEY_CURRENT_USER, _RUN_KEY, 0, winreg.KEY_READ) as k:
            winreg.QueryValueEx(k, _RUN_NAME)
            return True
    except OSError:
        return False


def set_startup(enabled, script_path):
    with winreg.CreateKeyEx(winreg.HKEY_CURRENT_USER, _RUN_KEY, 0, winreg.KEY_SET_VALUE) as k:
        if enabled:
            winreg.SetValueEx(k, _RUN_NAME, 0, winreg.REG_SZ, startup_command(script_path))
        else:
            try:
                winreg.DeleteValue(k, _RUN_NAME)
            except FileNotFoundError:
                pass


def hidden_run(args):
    """Roda um comando sem abrir janela de console."""
    try:
        subprocess.Popen(args, creationflags=CREATE_NO_WINDOW)
        return True
    except OSError:
        return False


def request_close_windows_of(pids):
    """Pede (educadamente) para fechar as janelas dos PIDs. Não força nada."""
    pids = set(pids)

    def cb(hwnd, _):
        if user32.IsWindowVisible(hwnd) and _pid_of(hwnd) in pids:
            user32.PostMessageW(hwnd, WM_CLOSE, 0, 0)
        return True

    user32.EnumWindows(WNDENUMPROC(cb), 0)


def run_hotkey_loop(hotkeys, callback):
    """Bloqueia numa thread: registra atalhos globais e chama callback(nome).

    hotkeys: {id: (modificadores, vk, nome)}
    """
    for hid, (mods, vk, _name) in hotkeys.items():
        user32.RegisterHotKey(None, hid, mods, vk)
    msg = wintypes.MSG()
    while user32.GetMessageW(ctypes.byref(msg), None, 0, 0) > 0:
        if msg.message == WM_HOTKEY and msg.wParam in hotkeys:
            callback(hotkeys[msg.wParam][2])


# ---------------------------------------------- reabrir a janela existente
_sig(kernel32.CreateEventW, wintypes.HANDLE, ctypes.c_void_p, BOOL, BOOL, wintypes.LPCWSTR)
_sig(kernel32.OpenEventW, wintypes.HANDLE, DWORD, BOOL, wintypes.LPCWSTR)
_sig(kernel32.SetEvent, BOOL, wintypes.HANDLE)
_sig(kernel32.WaitForSingleObject, DWORD, wintypes.HANDLE, DWORD)
_SHOW_EVENT = "Local\\WabbeControlCenterPy_Show"
EVENT_MODIFY_STATE = 0x0002


def create_show_event():
    return kernel32.CreateEventW(None, False, False, _SHOW_EVENT)


def signal_show_event():
    """Avisa a instância que já está rodando para mostrar a janela."""
    h = kernel32.OpenEventW(EVENT_MODIFY_STATE, False, _SHOW_EVENT)
    if not h:
        return False
    kernel32.SetEvent(h)
    kernel32.CloseHandle(h)
    return True


def show_event_signaled(handle):
    return bool(handle) and kernel32.WaitForSingleObject(handle, 0) == 0

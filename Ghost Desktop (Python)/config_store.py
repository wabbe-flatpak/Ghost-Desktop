"""Configuração do Wabbe Control Center.

- Salva em %APPDATA%\\WabbeControlCenter\\config.json
- Escrita atômica (arquivo temporário + os.replace): nunca fica meio gravado.
- Valores inválidos voltam ao padrão em vez de quebrar o programa.
- Importa o config.ini da versão antiga (C++) na primeira execução.
"""
import configparser
import json
import os
from pathlib import Path

APP_DIR_NAME = "WabbeControlCenter"

DEFAULTS = {
    "clean_enabled": True,
    "hide_icons": True,
    "hide_icons_delay": 5,
    "hide_cursor": True,
    "hide_cursor_delay": 5,
    "hide_taskbar": False,
    "hide_taskbar_delay": 5,
    "taskbar_safe_mode": True,
    "icons_restore_only_on_desktop": False,
    "icons_keep_with_windows": True,

    "game_enabled": False,
    "game_pause_clean": True,
    "game_restore_desktop": True,
    "game_open_steam": False,
    "game_open_xbox": False,
    "game_open_adrenalin": False,
    "game_high_performance": False,

    "studio_enabled": False,
    "studio_pause_clean": False,
    "studio_restore_desktop": False,
    "studio_open_vscode": False,
    "studio_open_ytmusic": True,
    "studio_browser": "edge",

    "close_to_tray": True,
    "start_minimized": False,
    "dark_theme": True,
    "show_notifications": True,
    "accent": "blue",

    "org_enabled": True,
    "org_downloads": True,
    "org_desktop": True,
    "org_documents": True,
    "org_installer_cleanup": True,
    "org_installer_days": 7,
    "org_desktop_keep": "Claude;Visual Studio Code;Discord;Wabbe CC;Ghost Desktop",
}

ACCENTS = ["blue", "purple", "silver", "green", "red", "y2k"]  # mesma ordem do AccentColor antigo
BROWSERS = ["edge", "chrome", "brave"]
DELAY_MIN, DELAY_MAX = 1, 3600

# (seção, chave) do config.ini antigo -> chave nova
INI_MAP = {
    ("Clean", "Enabled"): "clean_enabled",
    ("Clean", "HideIcons"): "hide_icons",
    ("Clean", "HideIconsDelay"): "hide_icons_delay",
    ("Clean", "HideCursor"): "hide_cursor",
    ("Clean", "HideCursorDelay"): "hide_cursor_delay",
    ("Clean", "HideTaskbar"): "hide_taskbar",
    ("Clean", "HideTaskbarDelay"): "hide_taskbar_delay",
    ("Clean", "TaskbarSafeMode"): "taskbar_safe_mode",
    ("Clean", "IconsRestoreOnlyOnDesktop"): "icons_restore_only_on_desktop",
    ("Game", "Enabled"): "game_enabled",
    ("Game", "PauseClean"): "game_pause_clean",
    ("Game", "RestoreDesktop"): "game_restore_desktop",
    ("Game", "OpenSteam"): "game_open_steam",
    ("Game", "OpenXbox"): "game_open_xbox",
    ("Game", "OpenAdrenalin"): "game_open_adrenalin",
    ("Game", "HighPerformance"): "game_high_performance",
    ("Studio", "Enabled"): "studio_enabled",
    ("Studio", "PauseClean"): "studio_pause_clean",
    ("Studio", "RestoreDesktop"): "studio_restore_desktop",
    ("Studio", "OpenVSCode"): "studio_open_vscode",
    ("Studio", "OpenYouTubeMusic"): "studio_open_ytmusic",
    ("Studio", "YouTubeMusicBrowser"): "studio_browser",
    ("General", "CloseToTray"): "close_to_tray",
    ("General", "StartMinimized"): "start_minimized",
    ("General", "DarkTheme"): "dark_theme",
    ("Customize", "ShowNotifications"): "show_notifications",
    ("Customize", "AccentColor"): "accent",
}


def config_dir() -> Path:
    base = os.environ.get("APPDATA") or str(Path.home())
    d = Path(base) / APP_DIR_NAME
    d.mkdir(parents=True, exist_ok=True)
    return d


def config_path() -> Path:
    return config_dir() / "config.json"


def _coerce(key, value):
    """Converte/valida um valor. Se não der, devolve o padrão."""
    default = DEFAULTS[key]

    if isinstance(default, bool):  # bool ANTES de int (bool é subclasse de int)
        if isinstance(value, bool):
            return value
        if isinstance(value, (int, float)):
            return bool(value)
        if isinstance(value, str):
            v = value.strip().lower()
            if v in ("1", "true", "yes", "on", "sim"):
                return True
            if v in ("0", "false", "no", "off", "nao", "não"):
                return False
        return default

    if isinstance(default, int):
        try:
            n = int(value)
        except (TypeError, ValueError):
            return default
        return min(max(n, DELAY_MIN), DELAY_MAX)

    if key == "accent":
        if isinstance(value, str) and value.strip().lower() in ACCENTS:
            return value.strip().lower()
        try:  # índice numérico do config.ini antigo (0 = azul)
            idx = int(value)
            if 0 <= idx < len(ACCENTS):
                return ACCENTS[idx]
        except (TypeError, ValueError):
            pass
        return default

    if key == "org_desktop_keep":
        return value.strip() if isinstance(value, str) else default

    if key == "studio_browser":
        if isinstance(value, str) and value.strip().lower() in BROWSERS:
            return value.strip().lower()
        return default

    return default


def import_ini(candidates):
    """Lê o primeiro config.ini existente da lista. Devolve dict (pode ser vazio)."""
    for path in candidates:
        try:
            p = Path(path)
            if not p.is_file():
                continue
            parser = configparser.ConfigParser(interpolation=None)
            parser.read(p, encoding="utf-8")
            data = {}
            for (section, option), new_key in INI_MAP.items():
                if parser.has_option(section, option):
                    data[new_key] = parser.get(section, option)
            if data:
                return data
        except Exception:
            continue
    return {}


def load(path=None, legacy_candidates=()):
    path = Path(path) if path else config_path()
    cfg = dict(DEFAULTS)
    data = None

    if path.is_file():
        try:
            data = json.loads(path.read_text(encoding="utf-8"))
        except Exception:
            try:  # guarda o arquivo corrompido para você poder olhar depois
                path.replace(path.with_name(path.name + ".bad"))
            except Exception:
                pass
            data = None
    else:
        data = import_ini(legacy_candidates)

    if isinstance(data, dict):
        for key, value in data.items():
            if key in DEFAULTS:
                cfg[key] = _coerce(key, value)
    return cfg


def save(cfg, path=None) -> bool:
    path = Path(path) if path else config_path()
    clean = {k: _coerce(k, cfg.get(k, DEFAULTS[k])) for k in DEFAULTS}
    tmp = path.with_name(path.name + ".tmp")
    try:
        path.parent.mkdir(parents=True, exist_ok=True)
        tmp.write_text(json.dumps(clean, indent=2, ensure_ascii=False), encoding="utf-8")
        os.replace(tmp, path)
        return True
    except Exception:
        return False

"""Organizador automático de arquivos (Downloads, Área de Trabalho, Documentos).

Regras de segurança:
- NUNCA apaga nada, só move.
- Se já existir um arquivo com o mesmo nome no destino, salva como "nome (2).ext".
- Só mexe em arquivos (nunca em pastas).
- Espera o arquivo ficar parado por 2 minutos antes de mover (download terminando).
- Em Documentos só move tipos "seguros" (nunca .sav, .ini, .cfg, .json... que podem ser de jogos).

Só usa a biblioteca padrão do Python, então quase não gasta memória.
"""
import ctypes
import os
import re
import struct
import time
from datetime import datetime
from pathlib import Path

MIN_AGE_S = 120  # 2 minutos parado antes de mover

# ---------------------------------------------------------------- regras
DOWNLOADS_RULES = {
    "Imagens": {".jpg", ".jpeg", ".png", ".gif", ".webp", ".bmp", ".svg", ".ico", ".heic", ".tif", ".tiff"},
    "Videos": {".mp4", ".mkv", ".mov", ".avi", ".webm", ".wmv"},
    "Audio": {".mp3", ".wav", ".ogg", ".flac", ".m4a", ".aac"},
    "Documentos": {".pdf", ".doc", ".docx", ".txt", ".odt", ".rtf", ".xls", ".xlsx", ".csv", ".ppt", ".pptx", ".md"},
    "Instaladores": {".exe", ".msi", ".msix", ".msixbundle", ".appx", ".iso"},
    "Minecraft": {".mcpack", ".mcworld", ".mcaddon", ".mctemplate", ".jar", ".mrpack", ".bbmodel",
                  ".cpmproject", ".litematic", ".schem", ".schematic"},
    "Compactados": {".zip", ".rar", ".7z", ".tar", ".gz"},
    "Codigo": {".py", ".cpp", ".c", ".h", ".hpp", ".java", ".js", ".ts", ".html", ".css", ".json", ".ipynb"},
    "Projetos de design": {".af", ".afphoto", ".afdesign", ".psd", ".kra", ".xcf", ".blend", ".clip"},
}

DOCUMENTS_RULES = {
    "Projetos de design": {".af", ".afphoto", ".afdesign", ".psd", ".kra", ".xcf", ".blend", ".clip"},
    "Imagens": {".jpg", ".jpeg", ".png", ".gif", ".webp", ".bmp", ".heic"},
    "Videos": {".mp4", ".mkv", ".mov", ".avi", ".webm"},
    "Audio": {".mp3", ".wav", ".flac", ".m4a"},
    "Textos e PDFs": {".pdf", ".doc", ".docx", ".odt", ".xlsx", ".pptx"},
    "Compactados": {".zip", ".rar", ".7z"},
}

DOWNLOADING_EXTS = {".crdownload", ".part", ".partial", ".tmp", ".download", ".opdownload", ".!ut"}
SKIP_NAMES = {"desktop.ini", "thumbs.db"}

GAME_WORDS = ("steam", "epic games", "riot games", "xboxgames", "roblox", "minecraft", "gog galaxy",
              "ubisoft", "ea games", "battle.net", "\\games\\", "hydra")
GAME_URL_PREFIXES = ("steam://", "com.epicgames.launcher://", "uplay://", "origin://")

FILE_ATTRIBUTE_HIDDEN = 0x2
FILE_ATTRIBUTE_SYSTEM = 0x4


def _last_touch(st):
    """Mais recente entre 'criado' e 'alterado'. (No Windows st_ctime = data de criação.)"""
    created = getattr(st, "st_birthtime", st.st_ctime if os.name == "nt" else st.st_mtime)
    return max(st.st_mtime, created)


def _category(rules, ext):
    for folder, exts in rules.items():
        if ext in exts:
            return folder
    return None


# ---------------------------------------------------------------- pastas
def downloads_dir():
    """Pasta Downloads de verdade (mesmo se você mudou de lugar no Windows)."""
    try:
        import winreg
        key = r"Software\Microsoft\Windows\CurrentVersion\Explorer\User Shell Folders"
        with winreg.OpenKey(winreg.HKEY_CURRENT_USER, key) as k:
            value, _ = winreg.QueryValueEx(k, "{374DE290-123F-4565-9164-39C4925E467B}")
            return Path(os.path.expandvars(value))
    except OSError:
        return Path.home() / "Downloads"


def _shell_folder(csidl, fallback):
    try:
        buf = ctypes.create_unicode_buffer(260)
        if ctypes.windll.shell32.SHGetFolderPathW(None, csidl, None, 0, buf) == 0:
            return Path(buf.value)
    except Exception:
        pass
    return fallback


def desktop_dir():
    return _shell_folder(0x10, Path.home() / "Desktop")  # CSIDL_DESKTOPDIRECTORY


def documents_dir():
    return _shell_folder(0x05, Path.home() / "Documents")  # CSIDL_PERSONAL


# ---------------------------------------------------------------- utilidades
class Organizer:
    def __init__(self, log_path):
        self.log_path = Path(log_path)

    def log(self, text):
        try:
            if self.log_path.exists() and self.log_path.stat().st_size > 1024 * 1024:
                self.log_path.unlink()  # passou de 1 MB: começa de novo
            stamp = datetime.now().strftime("%Y-%m-%d %H:%M")
            with open(self.log_path, "a", encoding="utf-8") as f:
                f.write(f"{stamp}  {text}\n")
        except OSError:
            pass

    @staticmethod
    def _skip(entry, now):
        """True = não mexer nesse arquivo agora."""
        try:
            if not entry.is_file(follow_symlinks=False):
                return True
            name = entry.name.lower()
            if name in SKIP_NAMES or name.startswith("~$"):  # ~$ = temporário do Office
                return True
            if os.path.splitext(name)[1] in DOWNLOADING_EXTS:
                return True
            st = entry.stat(follow_symlinks=False)
            attrs = getattr(st, "st_file_attributes", 0)
            if attrs & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM):
                return True
            if now - _last_touch(st) < MIN_AGE_S:
                return True
        except OSError:
            return True
        return False

    def move_unique(self, src: Path, dest_dir: Path):
        try:
            dest_dir.mkdir(parents=True, exist_ok=True)
            target = dest_dir / src.name
            n = 2
            while target.exists():
                if n > 99:
                    return False
                target = dest_dir / f"{src.stem} ({n}){src.suffix}"
                n += 1
            os.rename(src, target)  # mesmo disco: instantâneo; nunca sobrescreve
        except OSError as e:
            self.log(f"ERRO {src}: {e}")
            return False
        self.log(f"{src}  ->  {target}")
        return True

    # ------------------------------------------------------------ pastas
    def sort_folder(self, folder: Path, rules):
        moved = 0
        now = time.time()
        try:
            entries = list(os.scandir(folder))
        except OSError:
            return 0
        for e in entries:
            if self._skip(e, now):
                continue
            cat = _category(rules, os.path.splitext(e.name)[1].lower())
            if cat and self.move_unique(Path(e.path), folder / cat):
                moved += 1
        return moved

    def age_installers(self, downloads: Path, days: int):
        inst = downloads / "Instaladores"
        limit = days * 24 * 3600
        now = time.time()
        moved = 0
        try:
            entries = list(os.scandir(inst))
        except OSError:
            return 0
        for e in entries:
            try:
                if not e.is_file(follow_symlinks=False):
                    continue
                st = e.stat()
                if now - _last_touch(st) > limit:
                    if self.move_unique(Path(e.path), downloads / "_Para apagar"):
                        moved += 1
            except OSError:
                continue
        return moved

    def sort_desktop(self, desktop: Path, keep: str):
        keep_names = {k.strip().lower() for k in keep.split(";") if k.strip()}
        moved = 0
        now = time.time()
        try:
            entries = list(os.scandir(desktop))
        except OSError:
            return 0
        for e in entries:
            if self._skip(e, now):
                continue
            stem, ext = os.path.splitext(e.name)
            ext = ext.lower()
            if stem.lower() in keep_names:
                continue
            folder = None
            if ext == ".url":
                url = _read_url(e.path).lower()
                if url.startswith(GAME_URL_PREFIXES):
                    folder = "Jogos"
            elif ext == ".lnk":
                target = lnk_target(e.path).lower()
                if not target.endswith(".exe"):
                    continue  # atalho de pasta/arquivo: deixa onde está
                folder = "Jogos" if any(w in target for w in GAME_WORDS) else "Apps"
            if folder and self.move_unique(Path(e.path), desktop / folder):
                moved += 1
        return moved

    def run(self, cfg):
        """Roda uma vez. Devolve quantos arquivos foram movidos."""
        moved = 0
        downloads = downloads_dir()
        if cfg["org_downloads"]:
            moved += self.sort_folder(downloads, DOWNLOADS_RULES)
            if cfg["org_installer_cleanup"]:
                moved += self.age_installers(downloads, cfg["org_installer_days"])
        if cfg["org_desktop"]:
            moved += self.sort_desktop(desktop_dir(), cfg["org_desktop_keep"])
        if cfg["org_documents"]:
            moved += self.sort_folder(documents_dir(), DOCUMENTS_RULES)
        return moved


# ---------------------------------------------------------------- atalhos
def _read_url(path):
    try:
        with open(path, "r", encoding="utf-8", errors="ignore") as f:
            for line in f:
                if line.lower().startswith("url="):
                    return line[4:].strip()
    except OSError:
        pass
    return ""


_EXE_RE_W = re.compile(r"[A-Za-z]:\\[^\x00<>\"|?*]+?\.exe", re.IGNORECASE)


def lnk_target(path):
    """Lê o caminho do programa de um atalho .lnk (sem precisar de bibliotecas extras)."""
    try:
        data = Path(path).read_bytes()
    except OSError:
        return ""
    try:
        if len(data) < 0x4C or data[:4] != b"\x4c\x00\x00\x00":
            raise ValueError
        flags = struct.unpack_from("<I", data, 0x14)[0]
        pos = 0x4C
        if flags & 0x01:  # HasLinkTargetIDList
            pos += 2 + struct.unpack_from("<H", data, pos)[0]
        if flags & 0x02:  # HasLinkInfo
            info = pos
            header_size = struct.unpack_from("<I", data, info + 4)[0]
            if header_size >= 0x24:  # tem caminho em Unicode
                off = struct.unpack_from("<I", data, info + 28)[0]
                if off:
                    start = info + off
                    end = data.find(b"\x00\x00", start)
                    while end != -1 and (end - start) % 2:
                        end = data.find(b"\x00\x00", end + 1)
                    if end > start:
                        return data[start:end].decode("utf-16-le", errors="ignore")
            off = struct.unpack_from("<I", data, info + 16)[0]  # LocalBasePathOffset
            if off:
                start = info + off
                end = data.find(b"\x00", start)
                if end > start:
                    return data[start:end].decode("mbcs" if os.name == "nt" else "latin-1", errors="ignore")
    except (ValueError, struct.error, LookupError):
        pass
    # plano B: procura qualquer "X:\...\algo.exe" dentro do arquivo
    for text in (data.decode("utf-16-le", errors="ignore"), data.decode("latin-1", errors="ignore")):
        m = _EXE_RE_W.search(text)
        if m:
            return m.group(0)
    return ""

"""Wabbe Control Center (versão Python / CustomTkinter)."""
import logging
import os
import queue
import subprocess
import sys
import threading
import time
from pathlib import Path
from tkinter import messagebox

import customtkinter as ctk
import pystray
from PIL import Image, ImageDraw

import config_store
import organizer
import winfx
from clean_engine import CleanEngine

APP_NAME = "Wabbe Control Center"
HERE = Path(__file__).resolve().parent
ICON_PATH = HERE / "app.ico"
TICK_MS = 250
ORG_INTERVAL_S = 30  # organizador verifica a cada 30 s

# (cor principal, cor de hover) – mesma paleta do programa antigo
ACCENTS = {
    "blue":   ("#4187FF", "#7DB9FF", "Azul"),
    "purple": ("#8758F5", "#B48CFF", "Roxo"),
    "silver": ("#919EB4", "#D2DAE6", "Prata"),
    "green":  ("#2DB482", "#78EBB9", "Verde"),
    "red":    ("#DC4650", "#FF8282", "Vermelho"),
    "y2k":    ("#2DB9D2", "#78F5FF", "Y2K"),
}
# (claro, escuro)
BG = ("#ECF0F7", "#0F1016")
SIDEBAR = ("#E0E5EF", "#15161E")
CARD = ("#FFFFFF", "#20232E")
CARD_2 = ("#E2E8F2", "#2D3140")
TEXT = ("#161A23", "#F2F5FA")
MUTED = ("#5A6273", "#A5ACBA")

logging.basicConfig(
    filename=str(config_store.config_dir() / "wabbe.log"),
    level=logging.WARNING,
    format="%(asctime)s %(levelname)s %(message)s",
)


def make_tray_image(accent_hex):
    if ICON_PATH.exists():
        try:
            return Image.open(ICON_PATH).convert("RGBA")
        except Exception:
            pass
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rounded_rectangle((4, 4, 60, 60), radius=16, fill=accent_hex)
    d.polygon([(32, 14), (50, 32), (32, 50), (14, 32)], fill="white")
    return img


class App(ctk.CTk):
    def __init__(self):
        super().__init__()
        legacy = [config_store.config_dir() / "config.ini", HERE / "config.ini"]
        self.cfg = config_store.load(legacy_candidates=legacy)
        config_store.save(self.cfg)  # garante que o arquivo exista já na primeira execução

        self.engine = CleanEngine(winfx.WinApi())
        self.events = queue.Queue()
        self.paused = False
        self.studio_procs = []
        self.vars = {}
        self.delay_widgets = {}
        self.accent_widgets = []
        self.pages = []
        self.nav_buttons = []
        self.swatches = {}
        self._save_job = None
        self._closing = False
        self.autostart = "--autostart" in sys.argv
        self.show_event = winfx.create_show_event()
        self.org = organizer.Organizer(config_store.config_dir() / "organizador.log")
        self.org_lock = threading.Lock()
        self.org_status = "Ainda não rodou."
        self.org_last_moved = 0
        self.org_manual = False

        ctk.set_appearance_mode("dark" if self.cfg["dark_theme"] else "light")
        self.title(APP_NAME)
        self.geometry("920x660")
        self.minsize(880, 640)
        self.configure(fg_color=BG)
        if ICON_PATH.exists():
            self.after(250, lambda: self._set_icon())

        self.f_title = ctk.CTkFont(size=22, weight="bold")
        self.f_h2 = ctk.CTkFont(size=15, weight="bold")
        self.f_body = ctk.CTkFont(size=14)
        self.f_small = ctk.CTkFont(size=12)

        self.build_ui()
        self.select_page(0)
        self.apply_accent()
        self.refresh_ui()

        self.protocol("WM_DELETE_WINDOW", self.on_close)
        self.tray = None
        self.start_tray()
        self.start_hotkeys()
        self.apply_persisted_modes()

        if self.autostart and self.cfg["start_minimized"]:
            self.after(150, self.withdraw)  # só esconde quando abriu junto com o Windows
        else:
            self.after(200, lambda: self.handle_event("show"))
        self.after(TICK_MS, self.tick)
        threading.Thread(target=self._org_loop, daemon=True).start()

    # ------------------------------------------------------------ config
    def _set_icon(self):
        try:
            self.iconbitmap(str(ICON_PATH))
        except Exception:
            pass

    def save_now(self):
        if self._save_job is not None:
            self.after_cancel(self._save_job)
            self._save_job = None
        if not config_store.save(self.cfg):
            logging.warning("Não consegui salvar a configuração")

    def schedule_save(self):
        if self._save_job is not None:
            self.after_cancel(self._save_job)
        self._save_job = self.after(400, self.save_now)

    def notify(self, text):
        if self.cfg["show_notifications"] and self.tray is not None:
            try:
                self.tray.notify(text, APP_NAME)
            except Exception:
                pass

    # --------------------------------------------------------------- UI
    def build_ui(self):
        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(0, weight=1)

        side = ctk.CTkFrame(self, width=210, corner_radius=0, fg_color=SIDEBAR)
        side.grid(row=0, column=0, sticky="nsew")
        side.pack_propagate(False)
        ctk.CTkLabel(side, text="Wabbe", font=self.f_title, text_color=TEXT, anchor="w").pack(
            fill="x", padx=22, pady=(26, 0))
        ctk.CTkLabel(side, text="Control Center", font=self.f_small, text_color=MUTED, anchor="w").pack(
            fill="x", padx=22, pady=(0, 22))

        for i, name in enumerate(["Clean", "Jogo", "Estudio", "Organizar", "Personalizar"]):
            b = ctk.CTkButton(side, text=name, anchor="w", height=40, corner_radius=10, font=self.f_body,
                              fg_color="transparent", text_color=TEXT, hover_color=CARD_2,
                              command=lambda n=i: self.select_page(n))
            b.pack(fill="x", padx=14, pady=3)
            self.nav_buttons.append(b)

        bottom = ctk.CTkFrame(side, fg_color="transparent")
        bottom.pack(side="bottom", fill="x", padx=14, pady=16)
        self.status_label = ctk.CTkLabel(bottom, text="", font=self.f_small, text_color=MUTED, anchor="w",
                                         justify="left", wraplength=170)
        self.status_label.pack(fill="x", padx=6, pady=(0, 10))
        self.pause_btn = ctk.CTkButton(bottom, text="Pausar tudo", height=36, corner_radius=10,
                                       command=self.toggle_pause)
        self.pause_btn.pack(fill="x", pady=3)
        self.accent_widgets.append(("button", self.pause_btn))
        b2 = ctk.CTkButton(bottom, text="Restaurar tudo", height=36, corner_radius=10, fg_color=CARD_2,
                           text_color=TEXT, hover_color=("#D0D7E5", "#3A3F52"), command=self.restore_all)
        b2.pack(fill="x", pady=3)
        b3 = ctk.CTkButton(bottom, text="Sair", height=36, corner_radius=10, fg_color=CARD_2,
                           text_color=TEXT, hover_color=("#D0D7E5", "#3A3F52"), command=self.quit_app)
        b3.pack(fill="x", pady=3)

        self.content = ctk.CTkFrame(self, fg_color="transparent")
        self.content.grid(row=0, column=1, sticky="nsew", padx=26, pady=22)
        self.content.grid_columnconfigure(0, weight=1)
        self.content.grid_rowconfigure(0, weight=1)

        self.pages = [self.page_clean(), self.page_game(), self.page_studio(), self.page_organize(), self.page_custom()]

    def new_page(self, title, subtitle):
        page = ctk.CTkFrame(self.content, fg_color="transparent")
        page.grid(row=0, column=0, sticky="nsew")
        ctk.CTkLabel(page, text=title, font=self.f_title, text_color=TEXT, anchor="w").pack(fill="x")
        ctk.CTkLabel(page, text=subtitle, font=self.f_small, text_color=MUTED, anchor="w").pack(
            fill="x", pady=(0, 14))
        return page

    def card(self, parent, title=None):
        f = ctk.CTkFrame(parent, corner_radius=16, fg_color=CARD)
        f.pack(fill="x", pady=(0, 14))
        if title:
            ctk.CTkLabel(f, text=title, font=self.f_h2, text_color=TEXT, anchor="w").pack(
                fill="x", padx=20, pady=(14, 2))
        return f

    def switch(self, parent, text, key, hint=None, on_change=None, command=None):
        row = ctk.CTkFrame(parent, fg_color="transparent")
        row.pack(fill="x", padx=20, pady=(8, 6))
        initial = winfx.is_startup_enabled() if key == "_startup" else bool(self.cfg[key])
        var = ctk.BooleanVar(value=initial)
        self.vars[key] = var

        def cb():
            if command is not None:
                command(bool(var.get()))
                return
            self.cfg[key] = bool(var.get())
            if on_change:
                on_change(self.cfg[key])
            self.save_now()
            self.refresh_status()

        sw = ctk.CTkSwitch(row, text=text, variable=var, command=cb, font=self.f_body, text_color=TEXT,
                           switch_width=44, switch_height=22)
        sw.pack(anchor="w")
        self.accent_widgets.append(("switch", sw))
        if hint:
            ctk.CTkLabel(row, text=hint, font=self.f_small, text_color=MUTED, anchor="w").pack(
                anchor="w", padx=(54, 0))
        return sw

    def switch_delay(self, parent, text, key, delay_key, on_change=None):
        row = ctk.CTkFrame(parent, fg_color="transparent")
        row.pack(fill="x", padx=20, pady=6)
        row.grid_columnconfigure(0, weight=1)
        var = ctk.BooleanVar(value=bool(self.cfg[key]))
        self.vars[key] = var

        def cb():
            self.cfg[key] = bool(var.get())
            if on_change:
                on_change(self.cfg[key])
            self.save_now()

        sw = ctk.CTkSwitch(row, text=text, variable=var, command=cb, font=self.f_body, text_color=TEXT,
                           switch_width=44, switch_height=22)
        sw.grid(row=0, column=0, sticky="w")
        self.accent_widgets.append(("switch", sw))

        box = self.number_box(row, delay_key, ["1", "2", "3", "5", "10", "15", "30", "60", "120", "300"], "segundos")
        box.grid(row=0, column=1, padx=(10, 0))
        self.delay_widgets[delay_key] = box

    def number_box(self, parent, key, options, unit):
        """Caixinha onde dá para DIGITAR o número ou ESCOLHER na lista. Aceita de 1 a 3600."""
        frame = ctk.CTkFrame(parent, fg_color="transparent")
        box = ctk.CTkComboBox(frame, values=options, width=96, height=32, corner_radius=8, font=self.f_body,
                              justify="center")
        box.set(str(self.cfg[key]))
        box.pack(side="left")
        ctk.CTkLabel(frame, text=unit, font=self.f_small, text_color=MUTED).pack(side="left", padx=(8, 0))

        def apply(_=None):
            text = box.get().strip()
            try:
                n = min(max(int(text), config_store.DELAY_MIN), config_store.DELAY_MAX)
            except ValueError:
                n = self.cfg[key]  # digitou algo que não é número: volta ao valor anterior
            self.cfg[key] = n
            box.set(str(n))
            self.save_now()

        box.configure(command=apply)  # escolheu na lista
        box.bind("<Return>", apply)   # digitou e apertou Enter
        box.bind("<FocusOut>", apply)  # digitou e clicou fora
        try:
            box._entry.bind("<Return>", apply)
            box._entry.bind("<FocusOut>", apply)
        except AttributeError:
            pass
        self.accent_widgets.append(("combo", box))
        frame.box = box
        return frame

    def page_clean(self):
        p = self.new_page("Clean", "Esconde o que atrapalha quando você para de mexer no PC.")
        c = self.card(p, "Automação visual")
        self.switch(c, "Modo Clean ligado", "clean_enabled", "Atalho: Ctrl+Alt+C")
        c2 = self.card(p, "O que esconder e depois de quanto tempo parado")
        self.switch_delay(c2, "Ícones da área de trabalho", "hide_icons", "hide_icons_delay",
                          on_change=lambda v: None if v else self.engine.show_icons())
        self.switch_delay(c2, "Cursor do mouse", "hide_cursor", "hide_cursor_delay",
                          on_change=lambda v: None if v else self.engine.restore_cursor())
        self.switch_delay(c2, "Barra de tarefas", "hide_taskbar", "hide_taskbar_delay",
                          on_change=lambda v: None if v else self.engine.show_taskbar())
        ctk.CTkFrame(c2, height=6, fg_color="transparent").pack()
        c3 = self.card(p, "Regras")
        self.switch(c3, "Barra e cursor só somem com o desktop livre", "taskbar_safe_mode",
                    "Se desligado, a barra some mesmo com janelas abertas (menos em tela cheia).")
        self.switch(c3, "Ícones não somem com app aberto em janela", "icons_keep_with_windows",
                    "Só somem com tudo minimizado ou com o app maximizado/em tela cheia.")
        self.switch(c3, "Ícones: esconder com janela em foco, voltar só no desktop", "icons_restore_only_on_desktop",
                    "Só vale com a opção de cima desligada.")
        ctk.CTkFrame(c3, height=6, fg_color="transparent").pack()
        return p

    def page_game(self):
        p = self.new_page("Jogo", "Prepara o PC para jogar.")
        c = self.card(p, "Modo Jogo")
        self.switch(c, "Modo Jogo ligado", "game_enabled", command=self.set_game_mode)
        c2 = self.card(p, "Ao ligar")
        self.switch(c2, "Pausar o Clean enquanto joga", "game_pause_clean")
        self.switch(c2, "Restaurar ícones, cursor e barra", "game_restore_desktop")
        self.switch(c2, "Abrir Steam", "game_open_steam")
        self.switch(c2, "Abrir Xbox", "game_open_xbox")
        self.switch(c2, "Abrir AMD Adrenalin", "game_open_adrenalin")
        self.switch(c2, "Plano de energia: alto desempenho", "game_high_performance")
        ctk.CTkFrame(c2, height=6, fg_color="transparent").pack()
        return p

    def page_studio(self):
        p = self.new_page("Estudio", "Abre suas ferramentas de estudo/trabalho de uma vez.")
        c = self.card(p, "Modo Estudio")
        self.switch(c, "Modo Estudio ligado", "studio_enabled", "Atalho: Ctrl+Alt+E", command=self.set_studio_mode)
        c2 = self.card(p, "Ao ligar")
        self.switch(c2, "Pausar o Clean", "studio_pause_clean")
        self.switch(c2, "Restaurar ícones, cursor e barra", "studio_restore_desktop")
        self.switch(c2, "Abrir VS Code", "studio_open_vscode")
        self.switch(c2, "Abrir YouTube Music (janela pequena)", "studio_open_ytmusic")
        row = ctk.CTkFrame(c2, fg_color="transparent")
        row.pack(fill="x", padx=20, pady=(8, 16))
        ctk.CTkLabel(row, text="Navegador:", font=self.f_body, text_color=TEXT).pack(side="left", padx=(0, 12))
        self.browser_seg = ctk.CTkSegmentedButton(row, values=["Edge", "Chrome", "Brave"],
                                                  command=self.on_browser)
        self.browser_seg.pack(side="left")
        self.accent_widgets.append(("segmented", self.browser_seg))
        return p

    def page_organize(self):
        p = self.new_page("Organizar", "Arruma Downloads, Área de Trabalho e Documentos sozinho. Nunca apaga nada, só move.")
        c = self.card(p, "Organizador automático")
        self.switch(c, "Organizar sozinho", "org_enabled", "Verifica a cada 30 s. Arquivos novos esperam 2 min.")
        row = ctk.CTkFrame(c, fg_color="transparent")
        row.pack(fill="x", padx=20, pady=(6, 4))
        b1 = ctk.CTkButton(row, text="Organizar agora", height=36, corner_radius=10, command=self.organize_now)
        b1.pack(side="left", padx=(0, 10))
        self.accent_widgets.append(("button", b1))
        ctk.CTkButton(row, text="Ver log", height=36, corner_radius=10, fg_color=CARD_2, text_color=TEXT,
                      hover_color=("#D0D7E5", "#3A3F52"), command=self.open_org_log).pack(side="left")
        self.org_status_label = ctk.CTkLabel(c, text=self.org_status, font=self.f_small, text_color=MUTED, anchor="w")
        self.org_status_label.pack(fill="x", padx=20, pady=(4, 14))

        c2 = self.card(p, "Onde organizar")
        self.switch(c2, "Downloads", "org_downloads", "Separa por tipo: Imagens, Videos, Instaladores, Minecraft...")
        self.switch(c2, "Área de Trabalho", "org_desktop", "Atalhos novos: jogos vão para Jogos, programas para Apps.")
        self.switch(c2, "Documentos", "org_documents", "Só arquivos soltos. Pastas e saves de jogos não são tocados.")

        row = ctk.CTkFrame(c2, fg_color="transparent")
        row.pack(fill="x", padx=20, pady=(8, 16))
        row.grid_columnconfigure(0, weight=1)
        var = ctk.BooleanVar(value=bool(self.cfg["org_installer_cleanup"]))
        self.vars["org_installer_cleanup"] = var

        def on_toggle():
            self.cfg["org_installer_cleanup"] = bool(var.get())
            self.save_now()

        sw = ctk.CTkSwitch(row, text="Instaladores velhos vão para _Para apagar", variable=var, command=on_toggle,
                           font=self.f_body, text_color=TEXT, switch_width=44, switch_height=22)
        sw.grid(row=0, column=0, sticky="w")
        self.accent_widgets.append(("switch", sw))
        days = self.number_box(row, "org_installer_days", ["1", "3", "7", "14", "30", "60"], "dias")
        days.grid(row=0, column=1, padx=(10, 0))
        self.org_days_widgets = days
        return p

    def page_custom(self):
        p = self.new_page("Personalizar", "Aparência e comportamento do programa.")
        c = self.card(p, "Cor de destaque")
        row = ctk.CTkFrame(c, fg_color="transparent")
        row.pack(fill="x", padx=20, pady=(8, 16))
        for name, (main, hover, label) in ACCENTS.items():
            col = ctk.CTkFrame(row, fg_color="transparent")
            col.pack(side="left", padx=(0, 14))
            b = ctk.CTkButton(col, text="", width=44, height=44, corner_radius=22, fg_color=main,
                              hover_color=hover, border_width=0, command=lambda n=name: self.set_accent(n))
            b.pack()
            ctk.CTkLabel(col, text=label, font=self.f_small, text_color=MUTED).pack(pady=(4, 0))
            self.swatches[name] = b
        c2 = self.card(p, "Programa")
        self.switch(c2, "Tema escuro", "dark_theme", command=self.set_dark)
        self.switch(c2, "Iniciar com o Windows", "_startup", command=self.set_startup)
        self.switch(c2, "Iniciar minimizado (só quando abrir com o Windows)", "start_minimized")
        self.switch(c2, "Fechar para a bandeja", "close_to_tray")
        self.switch(c2, "Mostrar notificações", "show_notifications")
        row2 = ctk.CTkFrame(c2, fg_color="transparent")
        row2.pack(fill="x", padx=20, pady=(8, 16))
        b1 = ctk.CTkButton(row2, text="Abrir pasta de configuração", height=36, corner_radius=10,
                           command=lambda: os.startfile(str(config_store.config_dir())))
        b1.pack(side="left", padx=(0, 10))
        b2 = ctk.CTkButton(row2, text="Resetar configurações", height=36, corner_radius=10,
                           fg_color=CARD_2, text_color=TEXT, hover_color=("#D0D7E5", "#3A3F52"),
                           command=self.reset_config)
        b2.pack(side="left")
        self.accent_widgets.append(("button", b1))
        return p

    # ---------------------------------------------------- aparência/estado
    def select_page(self, idx):
        for p in self.pages:
            p.grid_remove()
        self.pages[idx].grid()
        self.current_page = idx
        self.apply_accent()

    def apply_accent(self):
        main, hover, _ = ACCENTS.get(self.cfg["accent"], ACCENTS["blue"])
        for kind, w in self.accent_widgets:
            try:
                if kind == "switch":
                    w.configure(progress_color=main)
                elif kind == "slider":
                    w.configure(progress_color=main, button_color=main, button_hover_color=hover)
                elif kind == "button":
                    w.configure(fg_color=main, hover_color=hover, text_color="#FFFFFF")
                elif kind == "combo":
                    w.configure(border_color=main, button_color=main, button_hover_color=hover)
                elif kind == "segmented":
                    w.configure(selected_color=main, selected_hover_color=hover)
            except Exception:
                logging.exception("apply_accent")
        for i, b in enumerate(self.nav_buttons):
            if i == getattr(self, "current_page", 0):
                b.configure(fg_color=main, text_color="#FFFFFF", hover_color=hover)
            else:
                b.configure(fg_color="transparent", text_color=TEXT, hover_color=CARD_2)
        for name, b in self.swatches.items():
            selected = name == self.cfg["accent"]
            b.configure(border_width=3 if selected else 0, border_color=TEXT)

    def refresh_ui(self):
        for key, var in self.vars.items():
            if key == "_startup":
                var.set(winfx.is_startup_enabled())
            elif key in self.cfg:
                var.set(bool(self.cfg[key]))
        for key, frame in self.delay_widgets.items():
            frame.box.set(str(self.cfg[key]))
        self.browser_seg.set(self.cfg["studio_browser"].capitalize())
        self.org_days_widgets.box.set(str(self.cfg["org_installer_days"]))
        self.pause_btn.configure(text="Retomar" if self.paused else "Pausar tudo")
        self.refresh_status()

    def refresh_status(self):
        if self.paused:
            text = "Pausado"
        else:
            parts = ["Clean ON" if self.cfg["clean_enabled"] else "Clean OFF"]
            if self.cfg["game_enabled"]:
                parts.append("Jogo ON")
            if self.cfg["studio_enabled"]:
                parts.append("Estudio ON")
            if self.cfg["org_enabled"]:
                parts.append("Org ON")
            text = "  |  ".join(parts)
        self.status_label.configure(text=text)

    def set_accent(self, name):
        self.cfg["accent"] = name
        self.save_now()
        self.apply_accent()
        if self.tray is not None:
            self.tray.icon = make_tray_image(ACCENTS[name][0])

    def set_dark(self, value):
        self.cfg["dark_theme"] = value
        ctk.set_appearance_mode("dark" if value else "light")
        self.save_now()

    def set_startup(self, value):
        try:
            winfx.set_startup(value, __file__)
        except OSError:
            logging.exception("startup")
            messagebox.showerror(APP_NAME, "Não consegui mexer na inicialização do Windows.")
        self.vars["_startup"].set(winfx.is_startup_enabled())

    def on_browser(self, value):
        self.cfg["studio_browser"] = value.lower()
        self.save_now()

    def reset_config(self):
        if not messagebox.askyesno(APP_NAME, "Resetar todas as configurações para o padrão?"):
            return
        self.restore_all()
        self.close_studio()
        self.cfg = dict(config_store.DEFAULTS)
        ctk.set_appearance_mode("dark")
        self.save_now()
        self.refresh_ui()
        self.apply_accent()
        self.notify("Configurações resetadas.")

    # -------------------------------------------------------------- modos
    def clean_active(self):
        c = self.cfg
        if self.paused or not c["clean_enabled"]:
            return False
        if c["game_enabled"] and c["game_pause_clean"]:
            return False
        if c["studio_enabled"] and c["studio_pause_clean"]:
            return False
        return True

    def restore_all(self):
        self.engine.restore_everything()

    def toggle_pause(self):
        self.paused = not self.paused
        if self.paused:
            self.restore_all()
        self.refresh_ui()
        self.notify("App pausado." if self.paused else "App ativado.")

    def toggle_clean(self):
        self.cfg["clean_enabled"] = not self.cfg["clean_enabled"]
        self.save_now()
        self.refresh_ui()
        self.notify("Modo Clean ativado." if self.cfg["clean_enabled"] else "Modo Clean desligado.")

    def run_game_actions(self):
        c = self.cfg
        if c["game_restore_desktop"]:
            self.restore_all()
        if c["game_open_steam"]:
            self._startfile("steam://open/main")
        if c["game_open_xbox"]:
            winfx.hidden_run(["explorer.exe", r"shell:AppsFolder\Microsoft.GamingApp_8wekyb3d8bbwe!Microsoft.Xbox.App"])
        if c["game_open_adrenalin"]:
            self._startfile(r"C:\Program Files\AMD\CNext\CNext\RadeonSoftware.exe")
        if c["game_high_performance"]:
            winfx.hidden_run(["powercfg.exe", "/setactive", "SCHEME_MIN"])

    def set_game_mode(self, enabled):
        self.cfg["game_enabled"] = enabled
        if enabled:
            self.run_game_actions()
        self.save_now()
        self.refresh_ui()

    def run_studio_actions(self):
        c = self.cfg
        if c["studio_restore_desktop"]:
            self.restore_all()
        if c["studio_open_vscode"]:
            self._launch(self._vscode_exe(), [], "VS Code")
        if c["studio_open_ytmusic"]:
            browser = c["studio_browser"]
            profile = config_store.config_dir() / f"ytmusic_profile_{browser}"
            profile.mkdir(exist_ok=True)
            args = ["--app=https://music.youtube.com", "--new-window", "--window-size=460,760",
                    "--window-position=70,70", f"--user-data-dir={profile}", "--no-first-run"]
            self._launch(self._browser_exe(browser), args, "YouTube Music")

    def set_studio_mode(self, enabled):
        self.cfg["studio_enabled"] = enabled
        if enabled:
            self.run_studio_actions()
        else:
            self.close_studio()
        self.save_now()
        self.refresh_ui()

    def toggle_studio(self):
        self.set_studio_mode(not self.cfg["studio_enabled"])
        self.notify("Modo Estudio ativado." if self.cfg["studio_enabled"] else "Modo Estudio desligado.")

    def apply_persisted_modes(self):
        if self.cfg["game_enabled"]:
            self.run_game_actions()
        if self.cfg["studio_enabled"]:
            self.run_studio_actions()

    def _startfile(self, target):
        try:
            os.startfile(target)
        except OSError:
            logging.warning("não consegui abrir %s", target)

    def _launch(self, exe, args, label):
        try:
            self.studio_procs.append(subprocess.Popen([exe, *args]))
        except OSError:
            logging.warning("não consegui abrir %s (%s)", label, exe)
            self.notify(f"Não encontrei {label}.")

    @staticmethod
    def _first_existing(paths, fallback):
        for p in paths:
            if p and os.path.isfile(p):
                return p
        return fallback

    def _vscode_exe(self):
        env = os.environ.get
        return self._first_existing([
            os.path.join(env("LOCALAPPDATA", ""), "Programs", "Microsoft VS Code", "Code.exe"),
            os.path.join(env("ProgramFiles", ""), "Microsoft VS Code", "Code.exe"),
            os.path.join(env("ProgramFiles(x86)", ""), "Microsoft VS Code", "Code.exe"),
        ], "code.exe")

    def _browser_exe(self, browser):
        env = os.environ.get
        pf, pf86, la = env("ProgramFiles", ""), env("ProgramFiles(x86)", ""), env("LOCALAPPDATA", "")
        if browser == "chrome":
            c = [os.path.join(b, "Google", "Chrome", "Application", "chrome.exe") for b in (pf, pf86, la)]
            return self._first_existing(c, "chrome.exe")
        if browser == "brave":
            c = [os.path.join(b, "BraveSoftware", "Brave-Browser", "Application", "brave.exe") for b in (pf, pf86, la)]
            return self._first_existing(c, "brave.exe")
        c = [os.path.join(b, "Microsoft", "Edge", "Application", "msedge.exe") for b in (pf86, pf, la)]
        return self._first_existing(c, "msedge.exe")

    def close_studio(self):
        """Pede para fechar. Não força: se o VS Code perguntar se quer salvar, nada é perdido."""
        procs, self.studio_procs = self.studio_procs, []
        pids = [p.pid for p in procs if p.poll() is None]
        if pids:
            winfx.request_close_windows_of(pids)

    # ---------------------------------------------------------- organizador
    def _org_loop(self):
        """Thread separada: mexer em arquivo nunca trava a janela."""
        time.sleep(10)
        while not self._closing:
            if self.org_manual:
                self.org_manual = False
                self._org_run(manual=True)
            elif self.cfg["org_enabled"] and not self.paused:
                self._org_run(manual=False)
            for _ in range(ORG_INTERVAL_S):
                if self._closing or self.org_manual:
                    break
                time.sleep(1)

    def _org_run(self, manual):
        if not self.org_lock.acquire(blocking=False):
            return
        try:
            moved = self.org.run(dict(self.cfg))
        except Exception:
            logging.exception("organizador")
            moved = 0
        finally:
            self.org_lock.release()
        self.org_last_moved = moved
        self.org_status = f"Última verificação {time.strftime('%H:%M')} - {moved} arquivo(s) movido(s)."
        self.post("org_done_manual" if manual else "org_done")

    def organize_now(self):
        self.org_status_label.configure(text="Organizando...")
        self.org_manual = True

    def open_org_log(self):
        path = config_store.config_dir() / "organizador.log"
        if not path.exists():
            self.org.log("Log do organizador criado.")
        try:
            os.startfile(str(path))
        except OSError:
            logging.warning("não consegui abrir o log do organizador")

    # --------------------------------------------------- bandeja / atalhos
    def post(self, name):
        self.events.put(name)

    def start_tray(self):
        menu = pystray.Menu(
            pystray.MenuItem("Abrir painel", lambda i, it: self.post("show"), default=True),
            pystray.Menu.SEPARATOR,
            pystray.MenuItem("Modo Clean", lambda i, it: self.post("toggle_clean"),
                             checked=lambda it: self.cfg["clean_enabled"]),
            pystray.MenuItem("Modo Jogo", lambda i, it: self.post("toggle_game"),
                             checked=lambda it: self.cfg["game_enabled"]),
            pystray.MenuItem("Modo Estudio", lambda i, it: self.post("toggle_studio"),
                             checked=lambda it: self.cfg["studio_enabled"]),
            pystray.MenuItem("Organizar agora", lambda i, it: self.post("organize")),
            pystray.Menu.SEPARATOR,
            pystray.MenuItem("Restaurar tudo", lambda i, it: self.post("restore")),
            pystray.MenuItem("Pausar tudo", lambda i, it: self.post("pause"), checked=lambda it: self.paused),
            pystray.Menu.SEPARATOR,
            pystray.MenuItem("Sair", lambda i, it: self.post("quit")),
        )
        self.tray = pystray.Icon("wabbe", make_tray_image(ACCENTS[self.cfg["accent"]][0]), APP_NAME, menu)
        self.tray.run_detached()

    def start_hotkeys(self):
        MOD_ALT, MOD_CONTROL, MOD_NOREPEAT = 0x1, 0x2, 0x4000
        keys = {
            1: (MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, ord("C"), "toggle_clean"),
            2: (MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, ord("P"), "pause"),
            3: (MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, ord("E"), "toggle_studio"),
        }
        threading.Thread(target=winfx.run_hotkey_loop, args=(keys, self.post), daemon=True).start()

    def handle_event(self, name):
        if name == "show":
            self.deiconify()
            self.lift()
            self.focus_force()
        elif name == "toggle_clean":
            self.toggle_clean()
        elif name == "toggle_game":
            self.set_game_mode(not self.cfg["game_enabled"])
        elif name == "toggle_studio":
            self.toggle_studio()
        elif name == "restore":
            self.restore_all()
        elif name == "pause":
            self.toggle_pause()
        elif name == "quit":
            self.quit_app()
        elif name == "organize":
            self.organize_now()
        elif name in ("org_done", "org_done_manual"):
            self.org_status_label.configure(text=self.org_status)
            if self.org_last_moved:
                self.notify(f"Organizei {self.org_last_moved} arquivo(s).")
            elif name == "org_done_manual":
                self.notify("Tudo já estava organizado.")

    # ---------------------------------------------------------- ciclo
    def tick(self):
        try:
            while True:
                try:
                    name = self.events.get_nowait()
                except queue.Empty:
                    break
                self.handle_event(name)
            if winfx.show_event_signaled(self.show_event):
                self.handle_event("show")
            self.engine.tick(self.cfg, self.clean_active())
        except Exception:
            logging.exception("tick")
        finally:
            if not self._closing:
                self.after(TICK_MS, self.tick)

    def on_close(self):
        self.save_now()
        if self.cfg["close_to_tray"]:
            self.withdraw()
        else:
            self.quit_app()

    def quit_app(self):
        self._closing = True
        self.save_now()
        try:
            self.restore_all()
            self.close_studio()
        finally:
            if self.tray is not None:
                self.tray.stop()
            self.destroy()


def main():
    handle, ok = winfx.acquire_single_instance()
    if not ok:
        if not winfx.signal_show_event():  # pede para a janela existente aparecer
            winfx.message_box("O Wabbe já está rodando. Procure o ícone na bandeja do Windows.")
        return
    app = App()
    import atexit
    atexit.register(app.engine.restore_everything)  # rede de segurança: nunca deixar o cursor sumido
    app.mainloop()
    del handle


if __name__ == "__main__":
    main()

"""Lógica do modo Clean (esconder ícones, cursor e barra de tarefas).

Não chama o Windows diretamente: usa um objeto `api` (veja winfx.WinApi).
Isso deixa a lógica testável e fácil de ler.
"""


class CleanEngine:
    def __init__(self, api):
        self.api = api
        self.icons_hidden = False
        self.cursor_hidden = False
        self.taskbar_hidden = False
        self._wait_fresh_input = False
        self._last_input_blocked = 0
        self._focus_start = None

    # ---- ações básicas -------------------------------------------------
    def any_hidden(self):
        return self.icons_hidden or self.cursor_hidden or self.taskbar_hidden

    def show_icons(self):
        self.api.show_icons()
        self.icons_hidden = False

    def hide_icons(self):
        self.api.hide_icons()
        self.icons_hidden = True

    def show_taskbar(self):
        self.api.show_taskbar()
        self.taskbar_hidden = False

    def hide_taskbar(self):
        self.api.hide_taskbar()
        self.taskbar_hidden = True

    def restore_cursor(self):
        self.api.restore_cursors()
        self.cursor_hidden = False

    def hide_cursor(self):
        if self.api.hide_cursors():
            self.cursor_hidden = True

    def restore_everything(self):
        """Sempre restaura de verdade (não confia só nas flags)."""
        self.show_icons()
        self.restore_cursor()
        self.show_taskbar()
        self._focus_start = None

    # ---- regras --------------------------------------------------------
    def _should_restore_icons(self, cfg, desktop_visible):
        if not cfg["icons_restore_only_on_desktop"]:
            return True
        return desktop_visible()

    def _restore_after_activity(self, cfg, desktop_visible):
        if self.icons_hidden and self._should_restore_icons(cfg, desktop_visible):
            self.show_icons()
        if self.cursor_hidden:
            self.restore_cursor()
        if self.taskbar_hidden:
            self.show_taskbar()

    def _focused_window_icon_hide(self, cfg, desktop_visible):
        """Modo 'ícones: janela em foco / volta no desktop'."""
        if not (cfg["hide_icons"] and cfg["icons_restore_only_on_desktop"]):
            self._focus_start = None
            return
        if desktop_visible():
            self._focus_start = None
            if self.icons_hidden:
                self.show_icons()
            return
        now = self.api.now_ms()
        if self._focus_start is None:
            self._focus_start = now
        if not self.icons_hidden and now - self._focus_start >= 5000:
            self.hide_icons()

    # ---- chamado a cada 250 ms ----------------------------------------
    def tick(self, cfg, active):
        a = self.api
        memo = {}

        def desktop_visible():  # consulta cara: calcula no máximo 1x por tick
            if "v" not in memo:
                memo["v"] = a.desktop_visible()
            return memo["v"]

        if not active:
            self._focus_start = None
            if self.any_hidden():
                self.restore_everything()
            return

        keep_with_windows = cfg.get("icons_keep_with_windows", True)
        blocked = a.active_window_blocked()  # janela em foco maximizada ou tela cheia

        if keep_with_windows:
            self._focus_start = None
            # App aberto em janela (nem minimizado, nem maximizado/tela cheia): ícones ficam na tela.
            windowed_app_open = not blocked and not desktop_visible()
            if windowed_app_open and self.icons_hidden:
                self.show_icons()
        else:
            windowed_app_open = False
            self._focused_window_icon_hide(cfg, desktop_visible)

        if blocked:
            # Maximizado/tela cheia: cursor e barra voltam (o usuário está usando o app)...
            if self.cursor_hidden:
                self.restore_cursor()
            if self.taskbar_hidden:
                self.show_taskbar()
            if keep_with_windows:
                # ...mas os ícones podem sumir, porque a área de trabalho está coberta.
                if cfg["hide_icons"] and not self.icons_hidden and a.idle_ms() >= cfg["hide_icons_delay"] * 1000:
                    self.hide_icons()
            elif not cfg["icons_restore_only_on_desktop"] and self.icons_hidden:
                self.show_icons()
            self._wait_fresh_input = True
            self._last_input_blocked = a.last_input_tick()
            return

        if self._wait_fresh_input:
            if a.last_input_tick() == self._last_input_blocked:
                if self.any_hidden():
                    self._restore_after_activity(cfg, desktop_visible)
                return
            self._wait_fresh_input = False

        idle = a.idle_ms()
        if idle < 250:
            if self.any_hidden():
                self._restore_after_activity(cfg, desktop_visible)
            return

        # ícones
        if cfg["hide_icons"]:
            if (not windowed_app_open and idle >= cfg["hide_icons_delay"] * 1000
                    and not self.icons_hidden):
                self.hide_icons()
        elif self.icons_hidden:
            self.show_icons()

        # cursor: só some com o desktop visível
        if cfg["hide_cursor"]:
            if not desktop_visible():
                if self.cursor_hidden:
                    self.restore_cursor()
            elif idle >= cfg["hide_cursor_delay"] * 1000 and not self.cursor_hidden:
                self.hide_cursor()
        elif self.cursor_hidden:
            self.restore_cursor()

        # barra de tarefas
        if cfg["hide_taskbar"]:
            can_hide = desktop_visible() if cfg["taskbar_safe_mode"] else not a.active_window_blocked()
            if not can_hide:
                if self.taskbar_hidden:
                    self.show_taskbar()
            elif idle >= cfg["hide_taskbar_delay"] * 1000 and not self.taskbar_hidden:
                self.hide_taskbar()
        elif self.taskbar_hidden:
            self.show_taskbar()

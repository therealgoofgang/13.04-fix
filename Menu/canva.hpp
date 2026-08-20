#pragma once
#pragma once
#include <Windows.h>
#include <math.h>
#include <algorithm>

wchar_t* s2wc(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);
	return wc;
}

namespace menu
{
	uobject* font;

	// ── Temel renk dönüşümleri ──────────────────────────────────────────────
	flinearcolor RGBtoFLC(float r, float g, float b, float a = 1.0f)
	{
		float gamma = 1.8f;
		return { powf(r / 255.f,gamma), powf(g / 255.f,gamma), powf(b / 255.f,gamma), a };
	}
	flinearcolor RGBtoFLC2(float r, float g, float b, float a = 1.0f)
	{
		return { r / 255.f, g / 255.f, b / 255.f, a };
	}

	// ── HSV → RGB ──────────────────────────────────────────────────────────
	flinearcolor HSVtoRGB(float h, float s, float v)
	{
		float r, g, b;
		int i = (int)(h * 6); float f = h * 6 - i;
		float p = v * (1 - s), q = v * (1 - f * s), t = v * (1 - (1 - f) * s);
		switch (i % 6) {
		case 0:r = v;g = t;b = p;break; case 1:r = q;g = v;b = p;break;
		case 2:r = p;g = v;b = t;break; case 3:r = p;g = q;b = v;break;
		case 4:r = t;g = p;b = v;break; default:r = v;g = p;b = q;break;
		}
		return { r,g,b,1.f };
	}

	// ── Input sistemi ──────────────────────────────────────────────────────
	namespace input
	{
		bool mouseDown[5];
		bool mouseDownAlready[1024];
		bool keysDown[256];
		bool keysDownAlready[256];

		bool is_any_mouse_down()
		{
			for (int i = 0;i < 5;i++) if (mouseDown[i]) return true;
			return false;
		}
		bool is_mouse_clicked(int button, int element_id, bool repeat)
		{
			if (element_id < 0 || element_id >= 1024) return false;
			if (mouseDown[button]) {
				if (!mouseDownAlready[element_id]) { mouseDownAlready[element_id] = true; return true; }
				if (repeat) return true;
			}
			else { mouseDownAlready[element_id] = false; }
			return false;
		}
		bool is_key_pressed(int key, bool repeat)
		{
			if (keysDown[key]) {
				if (!keysDownAlready[key]) { keysDownAlready[key] = true; return true; }
				if (repeat) return true;
			}
			else { keysDownAlready[key] = false; }
			return false;
		}
		void handle()
		{
			mouseDown[0] = (GetAsyncKeyState(0x01) & 0x8000) != 0;
		}
	}

	// ── Tema sistemi ───────────────────────────────────────────────────────
	inline int g_guiTheme = 0; // 0=Siyah 1=Beyaz
	inline int g_guiVariant = 0; // 0=Acik 1=Sik

	struct ThemeCache {
		flinearcolor BG, Header, Border, OuterBorder, Section, SectionMid, SectionOut;
		flinearcolor Text, SubText, CheckOff, CheckBorder, SliderBg, SliderIn;
		flinearcolor ComboBg, ComboBorder, ButtonBg, ButtonHov;
	};
	inline ThemeCache g_themeCache;
	inline int g_themeCacheKey = -1;

	inline void RebuildThemeCache()
	{
		int key = g_guiTheme * 10 + g_guiVariant;
		if (key == g_themeCacheKey) return;
		g_themeCacheKey = key;
		if (g_guiTheme == 0) { // Siyah
			g_themeCache.BG = { 0.004f,0.004f,0.004f,1.f };
			g_themeCache.Header = { 0.010f,0.010f,0.010f,1.f };
			g_themeCache.Border = { 0.016f,0.016f,0.016f,1.f };
			g_themeCache.OuterBorder = { 0.000f,0.000f,0.000f,1.f };
			g_themeCache.Section = { 0.008f,0.008f,0.008f,1.f };
			g_themeCache.SectionMid = { 0.012f,0.012f,0.012f,1.f };
			g_themeCache.SectionOut = { 0.003f,0.003f,0.003f,1.f };
			g_themeCache.Text = { 1.000f,1.000f,1.000f,1.f };
			g_themeCache.SubText = { 0.780f,0.780f,0.780f,1.f };
			g_themeCache.CheckOff = { 0.006f,0.006f,0.006f,1.f };
			g_themeCache.CheckBorder = { 0.022f,0.022f,0.022f,1.f };
			g_themeCache.SliderBg = { 0.018f,0.018f,0.018f,1.f };
			g_themeCache.SliderIn = { 0.008f,0.008f,0.008f,1.f };
			g_themeCache.ComboBg = { 0.007f,0.007f,0.007f,1.f };
			g_themeCache.ComboBorder = { 0.017f,0.017f,0.017f,1.f };
			g_themeCache.ButtonBg = { 0.025f,0.025f,0.025f,1.f };
			g_themeCache.ButtonHov = { 0.010f,0.010f,0.010f,1.f };
		}
		else if (g_guiVariant == 0) { // Acik Beyaz
			g_themeCache.BG = { 0.960f,0.960f,0.980f,1.f };
			g_themeCache.Header = { 0.843f,0.843f,0.870f,1.f };
			g_themeCache.Border = { 0.647f,0.647f,0.686f,1.f };
			g_themeCache.OuterBorder = { 0.470f,0.470f,0.529f,1.f };
			g_themeCache.Section = { 0.784f,0.784f,0.823f,1.f };
			g_themeCache.SectionMid = { 0.870f,0.870f,0.902f,1.f };
			g_themeCache.SectionOut = { 0.686f,0.686f,0.725f,1.f };
			g_themeCache.Text = { 0.004f,0.004f,0.005f,1.f };
			g_themeCache.SubText = { 0.070f,0.070f,0.090f,1.f };
			g_themeCache.CheckOff = { 0.725f,0.725f,0.765f,1.f };
			g_themeCache.CheckBorder = { 0.529f,0.529f,0.580f,1.f };
			g_themeCache.SliderBg = { 0.776f,0.776f,0.815f,1.f };
			g_themeCache.SliderIn = { 0.843f,0.843f,0.882f,1.f };
			g_themeCache.ComboBg = { 0.803f,0.803f,0.843f,1.f };
			g_themeCache.ComboBorder = { 0.568f,0.568f,0.612f,1.f };
			g_themeCache.ButtonBg = { 0.752f,0.752f,0.803f,1.f };
			g_themeCache.ButtonHov = { 0.831f,0.831f,0.870f,1.f };
		}
		else { // Sik Beyaz
			g_themeCache.BG = { 0.843f,0.843f,0.862f,1.f };
			g_themeCache.Header = { 0.686f,0.686f,0.725f,1.f };
			g_themeCache.Border = { 0.431f,0.431f,0.490f,1.f };
			g_themeCache.OuterBorder = { 0.274f,0.274f,0.333f,1.f };
			g_themeCache.Section = { 0.647f,0.647f,0.698f,1.f };
			g_themeCache.SectionMid = { 0.725f,0.725f,0.764f,1.f };
			g_themeCache.SectionOut = { 0.549f,0.549f,0.607f,1.f };
			g_themeCache.Text = { 0.002f,0.002f,0.003f,1.f };
			g_themeCache.SubText = { 0.035f,0.035f,0.055f,1.f };
			g_themeCache.CheckOff = { 0.607f,0.607f,0.658f,1.f };
			g_themeCache.CheckBorder = { 0.392f,0.392f,0.450f,1.f };
			g_themeCache.SliderBg = { 0.627f,0.627f,0.674f,1.f };
			g_themeCache.SliderIn = { 0.705f,0.705f,0.752f,1.f };
			g_themeCache.ComboBg = { 0.666f,0.666f,0.713f,1.f };
			g_themeCache.ComboBorder = { 0.439f,0.439f,0.501f,1.f };
			g_themeCache.ButtonBg = { 0.619f,0.619f,0.674f,1.f };
			g_themeCache.ButtonHov = { 0.698f,0.698f,0.745f,1.f };
		}
	}

	inline flinearcolor ThemeBG() { RebuildThemeCache(); return g_themeCache.BG; }
	inline flinearcolor ThemeHeader() { RebuildThemeCache(); return g_themeCache.Header; }
	inline flinearcolor ThemeBorder() { RebuildThemeCache(); return g_themeCache.Border; }
	inline flinearcolor ThemeOuterBorder() { RebuildThemeCache(); return g_themeCache.OuterBorder; }
	inline flinearcolor ThemeSection() { RebuildThemeCache(); return g_themeCache.Section; }
	inline flinearcolor ThemeSectionMid() { RebuildThemeCache(); return g_themeCache.SectionMid; }
	inline flinearcolor ThemeSectionOut() { RebuildThemeCache(); return g_themeCache.SectionOut; }
	inline flinearcolor ThemeText() { RebuildThemeCache(); return g_themeCache.Text; }
	inline flinearcolor ThemeSubText() { RebuildThemeCache(); return g_themeCache.SubText; }
	inline flinearcolor ThemeCheckOff() { RebuildThemeCache(); return g_themeCache.CheckOff; }
	inline flinearcolor ThemeCheckBorder() { RebuildThemeCache(); return g_themeCache.CheckBorder; }
	inline flinearcolor ThemeSliderBg() { RebuildThemeCache(); return g_themeCache.SliderBg; }
	inline flinearcolor ThemeSliderIn() { RebuildThemeCache(); return g_themeCache.SliderIn; }
	inline flinearcolor ThemeComboBg() { RebuildThemeCache(); return g_themeCache.ComboBg; }
	inline flinearcolor ThemeComboBorder() { RebuildThemeCache(); return g_themeCache.ComboBorder; }
	inline flinearcolor ThemeButtonBg() { RebuildThemeCache(); return g_themeCache.ButtonBg; }
	inline flinearcolor ThemeButtonHov() { RebuildThemeCache(); return g_themeCache.ButtonHov; }

	// ── Kisisellestime ──────────────────────────────────────────────────────
	inline bool g_cursorDot = true;
	inline bool g_showAccentLine = true;

	// ── Dil sistemi ────────────────────────────────────────────────────────
	inline int  g_lang = 0;
	inline bool g_showWelcome = true;
	inline fvector2d g_welcomePos = { 0.f,0.f };
	inline bool g_welcomePositioned = false;
	inline float g_welcome_anim = 0.f;
	inline float g_btn1_hover_a = 0.f;
	inline float g_btn2_hover_a = 0.f;
	inline float g_btn3_hover_a = 0.f;

	// ── GUI Settings pencere state ─────────────────────────────────────────
	inline bool     g_guiSettingsOpen = false;
	inline fvector2d g_guiSettingsPos = { 100.f,100.f };
	inline int      gs_elements_count = 0;
	inline int      gs_current_element = -1;
	inline bool     gs_hover_element = false;
	inline fvector2d gs_dragPos = { 0.f,0.f };
	inline fvector2d gs_menu_pos = { 0.f,0.f };
	inline float    gs_offset_x = 0.f;
	inline float    gs_offset_y = 0.f;
	inline fvector2d gs_last_pos = { 0.f,0.f };
	inline fvector2d gs_last_size = { 0.f,0.f };

	// ── Multi-config veri ──────────────────────────────────────────────────
	inline const int CFG_MAX = 16;
	inline const int CFG_NAME_LEN = 48;
	struct ConfigEntry { char name[48] = {}; bool exists = false; };
	inline ConfigEntry g_configList[16];
	inline int         g_configCount = 0;
	inline int         g_configSelected = -1;
	inline char        g_configNewName[48] = {};

	// ── Dikey tab animasyon state ──────────────────────────────────────────
	inline float g_tab_anim_y = 0.f;
	inline float g_tab_anim_target = 0.f;
	inline float g_tab_hover_alpha[8] = {};
	inline float g_tab_active_w[8] = {};

	// ── PostRenderer ───────────────────────────────────────────────────────
	namespace PostRenderer
	{
		struct DrawList {
			int type = -1;
			fvector2d pos, size, from, to, scale, shadow_offset;
			flinearcolor color, outline_color;
			const wchar_t* name;
			bool outline, center;
			int thickness;
			float width, height;
		};
		DrawList drawlist[128];

		void drawFilledRect(fvector2d pos, float w, float h, flinearcolor color) {
			for (int i = 0;i < 128;i++) if (drawlist[i].type == -1) {
				drawlist[i] = {};drawlist[i].type = 1;drawlist[i].pos = pos;
				drawlist[i].size = { w,h };drawlist[i].color = color;return;
			}
		}
		void FilledRect(fvector2d pos, float w, float h, flinearcolor color) {
			for (int i = 0;i < 128;i++) if (drawlist[i].type == -1) {
				drawlist[i] = {};drawlist[i].type = 6;drawlist[i].pos = pos;
				drawlist[i].width = w;drawlist[i].height = h;drawlist[i].color = color;return;
			}
		}
		void TextCenter(const wchar_t* name, fvector2d pos, flinearcolor color, bool outline) {
			for (int i = 0;i < 128;i++) if (drawlist[i].type == -1) {
				drawlist[i] = {};drawlist[i].type = 3;drawlist[i].name = name;
				drawlist[i].pos = pos;drawlist[i].outline = outline;drawlist[i].color = color;return;
			}
		}
		void Draw_Line(fvector2d from, fvector2d to, int thickness, flinearcolor color) {
			for (int i = 0;i < 128;i++) if (drawlist[i].type == -1) {
				drawlist[i] = {};drawlist[i].type = 4;drawlist[i].from = from;
				drawlist[i].to = to;drawlist[i].thickness = thickness;drawlist[i].color = color;return;
			}
		}
		void Text(const wchar_t* text, fvector2d pos, fvector2d scale, flinearcolor color,
			float angle, flinearcolor shadow_color, fvector2d shadow_offset,
			bool center, bool outline, flinearcolor outline_color) {
			for (int i = 0;i < 128;i++) if (drawlist[i].type == -1) {
				drawlist[i] = {};drawlist[i].type = 5;drawlist[i].name = text;
				drawlist[i].pos = pos;drawlist[i].scale = scale;drawlist[i].color = color;
				drawlist[i].outline_color = outline_color;drawlist[i].shadow_offset = shadow_offset;
				drawlist[i].center = center;drawlist[i].outline = outline;return;
			}
		}
	}

	namespace Colors
	{
		flinearcolor Text = { 0.75f,0.75f,0.75f,1.f };
		flinearcolor Text_Shadow = { 0.f,0.f,0.f,1.f };
		flinearcolor Text_Outline = { 0.f,0.f,0.f,1.f };
	}

	ucanvas* canvas;
	bool hover_element = false;
	fvector2d menu_pos = { 0,0 };
	float offset_x = 0, offset_y = 0;
	fvector2d first_element_pos = { 0,0 }, last_element_pos = { 0,0 }, last_element_size = { 0,0 };
	fvector2d current_element_pos = { 0,0 }, current_element_size = { 0,0 };
	int current_element = -1, elements_count = 0;
	bool sameLine = false, pushY = false;
	float pushYvalue = 0;
	int active_textbox = -1;
	fvector2d dragPos = { 0,0 };

	// ── Ekran boyutu cache ────────────────────────────────────────────────
	float g_screen_w = 0.f;
	float g_screen_h = 0.f;

	void UpdateScreenSize()
	{
		static float last_w = 0.f;
		float w = (float)GetSystemMetrics(SM_CXSCREEN);
		if (w != last_w) {
			last_w = w;
			g_screen_w = w;
			g_screen_h = (float)GetSystemMetrics(SM_CYSCREEN);
		}
	}

	void SetupCanvas(ucanvas* c) { canvas = c; UpdateScreenSize(); }

	// ── GetBlinkAccentColor ────────────────────────────────────────────────
	// Cached accent color for performance
	inline flinearcolor g_cachedAccentColor = { 0.f,0.f,0.f,0.f };
	inline DWORD64 g_lastAccentColorTick = 0;

	flinearcolor GetBlinkAccentColor()
	{
		DWORD64 currentTick = GetTickCount64();
		// Cache for 16ms (about 60 FPS)
		if (currentTick - g_lastAccentColorTick > 16) {
			float t = (float)(currentTick % 3000) / 3000.f;
			g_cachedAccentColor = HSVtoRGB(t, 0.85f, 1.f);
			g_lastAccentColorTick = currentTick;
		}
		return g_cachedAccentColor;
	}

	// ── Temel cizim ───────────────────────────────────────────────────────
	void Draw_Line(fvector2d from, fvector2d to, int thickness, flinearcolor color)
	{
		canvas->k2_drawline({ from.x,from.y }, { to.x,to.y }, (float)thickness, color);
	}
	void drawFilledRect(fvector2d pos, float w, float h, flinearcolor color)
	{
		// Optimized: Draw thicker lines for taller rectangles to reduce draw calls
		if (h <= 1.f) {
			canvas->k2_drawline({ pos.x,pos.y }, { pos.x + w,pos.y }, 1.f, color);
			return;
		}

		// For small rectangles, use the original method
		if (h <= 10.f) {
			for (float i = 0;i < h;i += 1.f)
				canvas->k2_drawline({ pos.x,pos.y + i }, { pos.x + w,pos.y + i }, 1.f, color);
			return;
		}

		// For medium rectangles, draw every 2px
		if (h <= 30.f) {
			for (float i = 0;i < h;i += 2.f)
				canvas->k2_drawline({ pos.x,pos.y + i }, { pos.x + w,pos.y + i }, 2.f, color);
			return;
		}

		// For large rectangles, draw every 3px
		if (h <= 60.f) {
			for (float i = 0;i < h;i += 3.f)
				canvas->k2_drawline({ pos.x,pos.y + i }, { pos.x + w,pos.y + i }, 3.f, color);
			return;
		}

		// For very large rectangles, draw every 4px
		for (float i = 0;i < h;i += 4.f)
			canvas->k2_drawline({ pos.x,pos.y + i }, { pos.x + w,pos.y + i }, 4.f, color);
	}
	// Cached mouse position for performance
	inline fvector2d g_cachedCursorPos = { 0.f, 0.f };
	inline DWORD64 g_lastCursorPosTick = 0;

	fvector2d CursorPos()
	{
		DWORD64 currentTick = GetTickCount64();
		// Update cached position every 8ms (about 120Hz polling)
		if (currentTick - g_lastCursorPosTick > 8) {
			POINT p = {}; GetCursorPos(&p);
			g_cachedCursorPos = { (float)p.x,(float)p.y };
			g_lastCursorPosTick = currentTick;
		}
		return g_cachedCursorPos;
	}
	bool MouseInZone(fvector2d pos, fvector2d size)
	{
		fvector2d c = CursorPos();
		return c.x > pos.x && c.y > pos.y && c.x < pos.x + size.x && c.y < pos.y + size.y;
	}
	void drawCircle(fvector2d center, float radius, flinearcolor color, int thickness = 1, int segments = 32)
	{
		float step = 6.2831853f / segments;
		for (int i = 0;i < segments;i++) {
			float a1 = i * step, a2 = (i + 1) * step;
			Draw_Line({ center.x + cosf(a1) * radius,center.y + sinf(a1) * radius },
				{ center.x + cosf(a2) * radius,center.y + sinf(a2) * radius }, thickness, color);
		}
	}

	// ── Layout yardimcilari ────────────────────────────────────────────────
	void SameLine() { sameLine = true; }
	void PushNextElementY(float y, bool from_last = true) {
		pushY = true;
		pushYvalue = from_last ? last_element_pos.y + last_element_size.y + y : y;
	}
	void NextColumn(float x) {
		offset_x = x;
		PushNextElementY(first_element_pos.y, false);
	}
	void ClearFirstPos() { first_element_pos = { 0,0 }; }

	// ── VirtualKeyCodeToString ────────────────────────────────────────────
	std::string VirtualKeyCodeToString(int vk)
	{
		if (vk == 0 || vk == 255) return "";
		switch (vk) {
		case VK_LBUTTON:return"MB1";case VK_RBUTTON:return"MB2";
		case VK_MBUTTON:return"MB3";case VK_XBUTTON1:return"MB4";
		case VK_XBUTTON2:return"MB5";case VK_INSERT:return"INS";
		case VK_DELETE:return"DEL";case VK_HOME:return"HOME";
		case VK_END:return"END";case VK_PRIOR:return"PGUP";
		case VK_NEXT:return"PGDN";case VK_LEFT:return"LEFT";
		case VK_RIGHT:return"RIGHT";case VK_UP:return"UP";
		case VK_DOWN:return"DOWN";case VK_SPACE:return"SPACE";
		case VK_RETURN:return"ENTER";case VK_ESCAPE:return"ESC";
		case VK_BACK:return"BACK";case VK_TAB:return"TAB";
		case VK_SHIFT:return"SHIFT";case VK_CONTROL:return"CTRL";
		case VK_MENU:return"ALT";
		}
		if (vk >= VK_F1 && vk <= VK_F24) return "F" + std::to_string(vk - VK_F1 + 1);
		if (vk >= 'A' && vk <= 'Z') return std::string(1, (char)vk);
		if (vk >= '0' && vk <= '9') return std::string(1, (char)vk);
		if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) return "NUM" + std::to_string(vk - VK_NUMPAD0);
		UINT sc = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
		char buf[64] = {};
		GetKeyNameTextA(sc << 16, buf, 64);
		std::string r = buf;
		for (auto& c : r) c = toupper(c);
		return r;
	}

	int active_hotkey = -1, active_hotkey_labeled = -1, active_hotkey2 = -1;
	bool already_pressed = false, already_pressed_labeled = false, already_pressed2 = false;


	// ── Menü açılış animasyonu state ─────────────────────────────────────
	inline float g_menu_open_anim = 0.0f; // 0=kapali 1=acik
	inline bool  g_menu_was_open = false;

	// ── Window fonksiyonu ─────────────────────────────────────────────────
	bool Window(const wchar_t* name, fvector2d* pos, fvector2d size, bool isOpen)
	{
		elements_count = 0;

		// Açılış/kapanış animasyonu
		float anim_target = isOpen ? 1.0f : 0.0f;
		g_menu_open_anim += (anim_target - g_menu_open_anim) * 0.16f;
		if (!isOpen && g_menu_open_anim < 0.01f) return false;
		if (!isOpen && !g_menu_was_open) return false;
		g_menu_was_open = isOpen;

		float alpha = g_menu_open_anim;
		float scale = 0.82f + g_menu_open_anim * 0.18f; // 0.82→1.0
		fvector2d anim_size = { size.x * scale, size.y * scale };
		fvector2d anim_pos = {
			pos->x + (size.x - anim_size.x) / 2.0f,
			pos->y + (size.y - anim_size.y) / 2.0f
		};

		if (!isOpen) {
			// Kapanma animasyonu — sadece çiz, input alma
			flinearcolor ac2 = GetBlinkAccentColor();
			auto drawA = [&](fvector2d p, float w, float h, flinearcolor c) {
				c.a *= alpha; drawFilledRect(p, w, h, c);};
			drawA(anim_pos, anim_size.x, anim_size.y, ThemeOuterBorder());
			drawA({ anim_pos.x + 1,anim_pos.y + 1 }, anim_size.x - 2, anim_size.y - 2, ThemeBorder());
			drawA({ anim_pos.x + 2,anim_pos.y + 2 }, anim_size.x - 4, 23.f, ThemeHeader());
			return false;
		}
		float sw = g_screen_w;
		float sh = g_screen_h;
		bool isHovered = MouseInZone(*pos, { size.x,23.f });
		if (current_element != -1 && !((GetAsyncKeyState(0x01) & 0x8000) != 0))
			current_element = -1;
		if ((isHovered || dragPos.x != 0) && !hover_element) {
			if (input::is_mouse_clicked(0, elements_count, true)) {
				fvector2d cur = CursorPos(); cur.x -= size.x; cur.y -= size.y;
				if (dragPos.x == 0) { dragPos.x = cur.x - pos->x;dragPos.y = cur.y - pos->y; }
				float nx = cur.x - dragPos.x, ny = cur.y - dragPos.y;
				if (nx < 0)nx = 0;if (ny < 0)ny = 0;
				if (nx + size.x > sw)nx = sw - size.x;
				if (ny + size.y > sh)ny = sh - size.y;
				pos->x = nx;pos->y = ny;
			}
			else { dragPos = { 0,0 }; }
		}
		else { hover_element = false; }

		offset_x = 8.f;offset_y = 2.f;
		menu_pos = *pos;
		first_element_pos = { 0,0 };
		current_element_pos = { 0,0 };
		current_element_size = { 0,0 };

		flinearcolor ac = GetBlinkAccentColor();

		drawFilledRect(*pos, size.x, size.y, ThemeOuterBorder());
		drawFilledRect({ pos->x + 1,pos->y + 1 }, size.x - 2, size.y - 2, ThemeBorder());
		drawFilledRect({ pos->x + 2,pos->y + 2 }, size.x - 4, 23.f, ThemeHeader());
		if (g_showAccentLine)
			drawFilledRect({ pos->x + 2,pos->y + 25 }, size.x - 4, 1.f, ac);
		drawFilledRect({ pos->x + 2,pos->y + 26 }, size.x - 4, 23.f, ThemeHeader());
		if (g_showAccentLine)
			drawFilledRect({ pos->x + 2,pos->y + 49 }, size.x - 4, 1.f, ac);
		drawFilledRect({ pos->x + 2,pos->y + 50 }, size.x - 4, size.y - 52 - 23, ThemeBG());
		drawFilledRect({ pos->x + 2,pos->y + size.y - (23 + 2) }, size.x - 4, 23.f, ThemeHeader());
		if (g_showAccentLine)
			drawFilledRect({ pos->x + 2,pos->y + size.y - (23 + 2) }, size.x - 4, 1.f, ac);
		// Alt sol
		canvas->k2_drawtext(font, L"Scarlet Valorant Free Internal Cheat",
			{ pos->x + 8,pos->y + size.y - ((23 / 2) + 2) }, { 0.90f,0.83f }, ac, 0.f,
			RGBtoFLC(0, 0, 0), { 0,0 }, false, true, false, RGBtoFLC(0, 0, 0));
		// Alt sag
		canvas->k2_drawtext(font, L"Made By larpingthebands (Liam)",
			{ pos->x + (size.x - 8) - 80,pos->y + size.y - ((23 / 2) + 2) }, { 0.90f,0.83f }, ac, 0.f,
			RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, RGBtoFLC(0, 0, 0));
		return true;
	}

	// ── ButtonTab2 ────────────────────────────────────────────────────────
	bool ButtonTab2(const wchar_t* name, fvector2d size, bool active)
	{
		elements_count++;
		fvector2d pos = { menu_pos.x + offset_x,menu_pos.y + offset_y };
		if (sameLine) { pos.x = last_element_pos.x + last_element_size.x;pos.y = last_element_pos.y; }
		if (pushY) { pos.y = pushYvalue;pushY = false;pushYvalue = 0;offset_y = pos.y - menu_pos.y; }
		bool hov = MouseInZone(pos, size);
		if (!sameLine) offset_y += size.y;
		fvector2d tp = { pos.x + size.x / 2,pos.y + size.y / 2 };
		flinearcolor tc = active ? GetBlinkAccentColor() : (hov ? ThemeText() : ThemeSubText());
		if (active) {
			flinearcolor ac = GetBlinkAccentColor();
			canvas->k2_drawtext(font, name, { tp.x + 1,tp.y + 1 }, { 0.91f,0.84f },
				{ ac.r,ac.g,ac.b,0.25f }, 0.f, RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, RGBtoFLC(0, 0, 0));
		}
		canvas->k2_drawtext(font, name, tp, { 0.91f,0.84f }, tc, 0.f,
			RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, RGBtoFLC(0, 0, 0));
		sameLine = false;last_element_pos = pos;last_element_size = size;
		if (first_element_pos.x == 0.f) first_element_pos = pos;
		if (hov && input::is_mouse_clicked(0, elements_count, false)) return true;
		return false;
	}

	// ── SectionWrapper ────────────────────────────────────────────────────
	void SectionWrapper(const wchar_t* name, fvector2d size)
	{
		fvector2d pos = { menu_pos.x + offset_x,menu_pos.y + offset_y };
		drawFilledRect(pos, size.x, size.y, ThemeSectionOut());
		drawFilledRect({ pos.x + 1,pos.y + 1 }, size.x - 2, size.y - 2, ThemeSectionMid());
		drawFilledRect({ pos.x + 2,pos.y + 2 }, size.x - 4, size.y - 4, ThemeSection());
		canvas->k2_drawtext(font, name, { pos.x + 6,pos.y + 2 }, { 0.90f,0.83f }, ThemeText(), 0.f,
			RGBtoFLC(0, 0, 0), { 0,0 }, false, false, false, RGBtoFLC(0, 0, 0));
		offset_y += 27;sameLine = false;
	}


	// ── Checkbox ─────────────────────────────────────────────────────────
	void Checkbox(const wchar_t* name, bool* value, bool risky = false)
	{
		elements_count++;
		fvector2d pad = { 13,11 };
		fvector2d pos = { menu_pos.x + pad.x + offset_x,menu_pos.y + pad.y + offset_y };
		if (sameLine) { pos.x = last_element_pos.x + last_element_size.x + pad.x;pos.y = last_element_pos.y; }
		if (pushY) { pos.y = pushYvalue;pushY = false;pushYvalue = 0;offset_y = pos.y - menu_pos.y; }
		bool hov = MouseInZone(pos, { 9,9 });
		fvector2d tp = { pos.x + 9 + 11,pos.y - 4.5f };
		drawFilledRect(pos, 9, 9, ThemeCheckBorder());
		drawFilledRect({ pos.x + 1,pos.y + 1 }, 7, 7, ThemeCheckOff());
		if (*value) {
			if (risky) drawFilledRect({ pos.x + 1,pos.y + 1 }, 7, 7, RGBtoFLC(170, 41, 41));
			else      drawFilledRect({ pos.x + 1,pos.y + 1 }, 7, 7, GetBlinkAccentColor());
		}
		else { drawFilledRect({ pos.x + 2,pos.y + 2 }, 5, 5, ThemeCheckOff()); }
		flinearcolor tc = risky ? RGBtoFLC(170, 41, 41, 1.2f) : ThemeText();
		canvas->k2_drawtext(font, name, tp, { 0.88f,0.82f }, tc, 0.f,
			RGBtoFLC(0, 0, 0), { 0,0 }, false, false, false, Colors::Text_Outline);
		if (!sameLine) offset_y += 9 + pad.y;
		sameLine = false;last_element_pos = pos;last_element_size = { 9,9 };
		if (first_element_pos.x == 0.f) first_element_pos = pos;
		if (hov && input::is_mouse_clicked(0, elements_count, false)) *value = !*value;
	}

	// ── Button ────────────────────────────────────────────────────────────
	bool Button(const wchar_t* name)
	{
		elements_count++;
		float bw = 176, bh = 20;
		fvector2d pos = { menu_pos.x + offset_x + (335.f / 2.f) - bw / 2.f,menu_pos.y + 14 + offset_y };
		if (sameLine) { pos.x = last_element_pos.x + last_element_size.x;pos.y = last_element_pos.y; }
		if (pushY) { pos.y = pushYvalue;pushY = false;pushYvalue = 0;offset_y = pos.y - menu_pos.y; }
		bool hov = MouseInZone(pos, { bw,bh });
		static DWORD64 clickTime = 0; static bool isAnim = false;
		float fade = 0;
		if (isAnim) { DWORD64 el = GetTickCount64() - clickTime;if (el < 1000)fade = 1.f - (el / 1000.f);else isAnim = false; }
		drawFilledRect(pos, bw, bh, hov ? ThemeButtonHov() : ThemeButtonBg());
		drawFilledRect({ pos.x + 1,pos.y + 1 }, bw - 2, bh - 2, hov ? ThemeHeader() : ThemeButtonBg());
		if (!sameLine) offset_y += bh + 14;
		canvas->k2_drawtext(font, name, { pos.x + bw / 2,pos.y + bh / 2 }, { 0.90f,0.83f }, ThemeSubText(), 0.f,
			Colors::Text_Shadow, { 0,0 }, true, true, false, Colors::Text_Outline);
		sameLine = false;last_element_pos = pos;last_element_size = { bw,bh };
		if (first_element_pos.x == 0.f) first_element_pos = pos;
		if (hov && input::is_mouse_clicked(0, elements_count, false)) { isAnim = true;clickTime = GetTickCount64();return true; }
		return false;
	}

	// ── SliderFloat ───────────────────────────────────────────────────────
	void SliderFloat(const wchar_t* name, float* value, float mn, float mx, const char* fmt = "%.0f")
	{
		elements_count++;
		float ow = 185, oh = 8, iw = 183, ih = 6;
		fvector2d size = { ow,38 };
		fvector2d pad = { 38,11 };
		fvector2d pos = { menu_pos.x + pad.x + offset_x,menu_pos.y + pad.y + offset_y };
		if (sameLine) { pos.x = last_element_pos.x + last_element_size.x + pad.x;pos.y = last_element_pos.y; }
		if (pushY) { pos.y = pushYvalue;pushY = false;pushYvalue = 0;offset_y = pos.y - menu_pos.y; }
		float sy = pos.y + pad.y + 5;
		float ix = pos.x + 1 + 2, iy = sy + 1;
		bool hov = MouseInZone({ pos.x + 2,sy - 20 }, { ow,oh + 30 });
		if (!sameLine) offset_y += size.y + pad.y;
		// minus
		float mx2 = (pos.x + 2) - 10 - 2.5f;
		drawFilledRect({ mx2 - 2.5f,sy + oh / 2 - .5f }, 5, 1, RGBtoFLC(166, 166, 166));
		if (MouseInZone({ mx2 - 2.5f,sy + oh / 2 - 5 }, { 10,10 }) && input::is_mouse_clicked(0, elements_count, false)) { *value -= 1;if (*value < mn)*value = mn; }
		// track
		drawFilledRect({ pos.x + 2,sy }, ow, oh, ThemeSliderBg());
		drawFilledRect({ ix,iy }, iw, ih, ThemeSliderIn());
		float fp = (*value - mn) / (mx - mn);
		float fw = iw * fp;
		if (fw > 0) drawFilledRect({ ix,iy }, fw, ih, GetBlinkAccentColor());
		// drag
		if (hov || current_element == elements_count) {
			if (input::is_mouse_clicked(0, elements_count, true)) {
				current_element = elements_count;
				float rx = CursorPos().x - ix;
				*value = (rx / iw) * (mx - mn) + mn;
				if (*value < mn)*value = mn;if (*value > mx)*value = mx;
			}
			hover_element = true;
		}
		// value text
		char buf[32]; sprintf_s(buf, fmt, *value);
		canvas->k2_drawtext(font, s2wc(buf), { ix + fw,sy + oh + 1 }, { 0.82f,0.76f }, ThemeSubText(), 0.f,
			Colors::Text_Shadow, { 0,0 }, false, false, false, Colors::Text_Outline);
		// plus
		float px2 = (pos.x + 2) + ow + 10;
		drawFilledRect({ px2 - 2.5f,sy + oh / 2 - .5f }, 5, 1, RGBtoFLC(166, 166, 166));
		drawFilledRect({ px2 - .5f,sy + oh / 2 - 2.5f }, 1, 5, RGBtoFLC(166, 166, 166));
		if (MouseInZone({ px2 - 2.5f,sy + oh / 2 - 5 }, { 10,10 }) && input::is_mouse_clicked(0, elements_count, false)) { *value += 1;if (*value > mx)*value = mx; }
		// name
		canvas->k2_drawtext(font, name, { pos.x,pos.y + 1 }, { 0.86f,0.80f }, ThemeText(), 0.f,
			RGBtoFLC(0, 0, 0), { 0,0 }, false, false, false, Colors::Text_Outline);
		sameLine = false;last_element_pos = pos;last_element_size = size;
		if (first_element_pos.x == 0.f) first_element_pos = pos;
	}


	// ── SliderInt ─────────────────────────────────────────────────────────
	void SliderInt(const wchar_t* name, int* value, int mn, int mx, const char* fmt = "%d")
	{
		float fv = (float)*value;
		SliderFloat(name, &fv, (float)mn, (float)mx, fmt);
		*value = (int)roundf(fv);
	}

	// ── Combobox ──────────────────────────────────────────────────────────
	bool checkbox_enabled[256];
	void Combobox(fvector2d size, int* value, const wchar_t* arg, ...)
	{
		elements_count++;
		fvector2d pad = { 38,11 };
		fvector2d pos = { menu_pos.x + pad.x + offset_x,menu_pos.y + pad.y + offset_y };
		if (sameLine) { pos.x = last_element_pos.x + last_element_size.x + pad.x;pos.y = last_element_pos.y; }
		if (pushY) { pos.y = pushYvalue;pushY = false;pushYvalue = 0;offset_y = pos.y - menu_pos.y; }
		bool hov = MouseInZone(pos, size);
		float cw = size.x, ch = size.y, ci = size.y - 2;
		if (!sameLine) offset_y += ch + pad.y;
		if (!checkbox_enabled[elements_count]) {
			drawFilledRect(pos, cw, ch, ThemeComboBorder());
			drawFilledRect({ pos.x + 1,pos.y + 1 }, cw - 2, ch - 2, ThemeComboBg());
			va_list a;va_start(a, arg);const wchar_t* ca = arg;int cn = 0;
			while (ca != NULL && cn <= *value) {
				if (cn == *value) canvas->k2_drawtext(font, ca, { pos.x + 5,pos.y + ch / 2 }, { 0.88f,0.82f }, ThemeSubText(), 0.f, RGBtoFLC(0, 0, 0), { 0,0 }, false, true, false, Colors::Text_Outline);
				cn++;ca = va_arg(a, const wchar_t*);
			}
			va_end(a);
			float ax = pos.x + cw - 15, ay = pos.y + ch / 2;
			drawFilledRect({ ax,ay - 1 }, 5, 1, RGBtoFLC(179, 179, 179));
			drawFilledRect({ ax + 1,ay }, 3, 1, RGBtoFLC(179, 179, 179));
			drawFilledRect({ ax + 2,ay + 1 }, 1, 1, RGBtoFLC(179, 179, 179));
		}
		else {
			hover_element = true;
			drawFilledRect(pos, cw, ch, ThemeComboBorder());
			drawFilledRect({ pos.x + 1,pos.y + 1 }, cw - 2, ch - 2, ThemeComboBg());
			va_list a2;va_start(a2, arg);const wchar_t* ca2 = arg;int cn2 = 0;
			while (ca2 != NULL && cn2 <= *value) {
				if (cn2 == *value) canvas->k2_drawtext(font, ca2, { pos.x + 5,pos.y + ch / 2 }, { 0.88f,0.82f }, ThemeSubText(), 0.f, RGBtoFLC(0, 0, 0), { 0,0 }, false, true, false, Colors::Text_Outline);
				cn2++;ca2 = va_arg(a2, const wchar_t*);
			}
			va_end(a2);
			float ax = pos.x + cw - 15, ay = pos.y + ch / 2;
			drawFilledRect({ ax,ay + 1 }, 5, 1, RGBtoFLC(179, 179, 179));
			drawFilledRect({ ax + 1,ay }, 3, 1, RGBtoFLC(179, 179, 179));
			drawFilledRect({ ax + 2,ay - 1 }, 1, 1, RGBtoFLC(179, 179, 179));
			fvector2d ip = { pos.x,pos.y + ch };
			va_list cnt;va_start(cnt, arg);const wchar_t* carg = arg;int ic = 0;
			while (carg != NULL) { ic++;carg = va_arg(cnt, const wchar_t*); }
			va_end(cnt);
			va_list draw;va_start(draw, arg);int num = 0;
			for (const wchar_t* ca3 = arg;ca3 != NULL;ca3 = va_arg(draw, const wchar_t*)) {
				bool ih = MouseInZone({ ip.x,ip.y }, { cw,ci });
				if (ih && input::is_mouse_clicked(0, elements_count, false)) { *value = num;checkbox_enabled[elements_count] = false; }
				PostRenderer::drawFilledRect({ ip.x + 1,ip.y + (num == 0 ? 1 : 0) }, cw - 2, ci - (num == 0 ? 1 : 0) - (num == ic - 1 ? 1 : 0), num == *value ? ThemeSection() : ThemeComboBg());
				PostRenderer::Text(ca3, { ip.x + 5,ip.y + ci / 2 }, { 0.88f,0.82f }, ThemeSubText(), 0.f, RGBtoFLC(0, 0, 0), { 0,0 }, false, false, Colors::Text_Outline);
				ip.y += ci;num++;
			}
			va_end(draw);
			if (checkbox_enabled[elements_count]) { current_element_pos = pos;current_element_size = { cw,ip.y - pos.y }; }
		}
		sameLine = false;last_element_pos = pos;last_element_size = { cw,ch };
		if (first_element_pos.x == 0.f) first_element_pos = pos;
		if (hov && input::is_mouse_clicked(0, elements_count, false)) checkbox_enabled[elements_count] = !checkbox_enabled[elements_count];
		if (!hov && checkbox_enabled[elements_count] && input::is_mouse_clicked(0, elements_count, false)) checkbox_enabled[elements_count] = false;
	}

	// ── InputField ────────────────────────────────────────────────────────
	void InputField(const wchar_t* label, std::string* value, size_t max_len)
	{
		elements_count++;
		float ow = 185, oh = 20, iw = 183, ih = 18;
		fvector2d pad = { 38,11 };
		fvector2d pos = { menu_pos.x + pad.x + offset_x,menu_pos.y + pad.y + offset_y };
		if (sameLine) { pos.x = last_element_pos.x + last_element_size.x + pad.x;pos.y = last_element_pos.y; }
		if (pushY) { pos.y = pushYvalue;pushY = false;pushYvalue = 0;offset_y = pos.y - menu_pos.y; }
		bool hov = MouseInZone(pos, { ow,oh });
		if (!sameLine) offset_y += oh + pad.y + 4;
		canvas->k2_drawtext(font, label, { pos.x,pos.y - 12 }, { 0.82f,0.76f }, ThemeSubText(), 0.f, Colors::Text_Shadow, { 0,0 }, false, false, false, Colors::Text_Outline);
		bool act = (active_textbox == elements_count);
		drawFilledRect(pos, ow, oh, act ? GetBlinkAccentColor() : ThemeComboBorder());
		drawFilledRect({ pos.x + 1,pos.y + 1 }, iw, ih, act ? ThemeHeader() : ThemeComboBg());
		if (value->size() > max_len - 1) value->resize(max_len - 1);
		std::wstring wv(value->begin(), value->end());
		canvas->k2_drawtext(font, wv.c_str(), { pos.x + 5,pos.y + ih / 2 }, { 0.86f,0.80f }, ThemeText(), 0.f, Colors::Text_Shadow, { 0,0 }, false, true, false, Colors::Text_Outline);
		if (act && (int)(GetTickCount64() / 400) % 2)
			canvas->k2_drawtext(font, L"|", { pos.x + 5 + (float)wv.size() * 7.5f,pos.y + ih / 2 }, { 0.82f,0.76f }, GetBlinkAccentColor(), 0.f, Colors::Text_Shadow, { 0,0 }, false, true, false, Colors::Text_Outline);
		if (hov && input::is_mouse_clicked(0, elements_count, false)) active_textbox = elements_count;
		else if (!hov && input::is_mouse_clicked(0, elements_count, false) && act) active_textbox = -1;
		if (act) {
			BYTE kb[256];GetKeyboardState(kb);
			for (int vk = 0;vk < 255;vk++) {
				if (!(GetAsyncKeyState(vk) & 0x1)) continue;
				if (vk == VK_BACK && !value->empty()) value->pop_back();
				else if (vk == VK_RETURN) active_textbox = -1;
				else if (value->size() < max_len - 1) {
					WCHAR wc[4] = {};
					int r = ToUnicode(vk, MapVirtualKey(vk, MAPVK_VK_TO_VSC), kb, wc, 4, 0);
					if (r > 0 && iswprint(wc[0])) { char mb[4] = {};WideCharToMultiByte(CP_UTF8, 0, wc, r, mb, 3, nullptr, nullptr);value->append(mb); }
				}
			}
		}
		sameLine = false;last_element_pos = pos;last_element_size = { ow,oh };
		if (first_element_pos.x == 0.f) first_element_pos = pos;
	}


	// ── Hotkey ────────────────────────────────────────────────────────────
	void Hotkey(const char* name, fvector2d size, int* key)
	{
		elements_count++;
		fvector2d pad = { 195,-2 };
		fvector2d pos = { menu_pos.x + pad.x + offset_x,menu_pos.y + pad.y + offset_y };
		if (sameLine) { pos.x = last_element_pos.x + last_element_size.x + pad.x;pos.y = last_element_pos.y + (last_element_size.y / 2) - size.y / 2; }
		if (pushY) { pos.y = pushYvalue;pushY = false;pushYvalue = 0;offset_y = pos.y - menu_pos.y; }
		bool hov = MouseInZone(pos, size);
		if (!sameLine) offset_y += size.y + pad.y;
		fvector2d tp = { pos.x + size.x / 2,pos.y + size.y / 2 };
		if (active_hotkey == elements_count) {
			canvas->k2_drawtext(font, s2wc("[ ... ]"), tp, { 0.78f,0.72f }, RGBtoFLC(120, 120, 120, 1.2f), 0.f, Colors::Text_Shadow, { 0,0 }, true, true, true, RGBtoFLC(0, 0, 0, 0.1f));
			if (!input::is_any_mouse_down()) already_pressed = false;
			if (!already_pressed) for (int c = 0;c < 255;c++) if (GetAsyncKeyState(c)) { *key = c;active_hotkey = -1; }
		}
		else {
			std::string dt = (*key == 0 || *key == -1) ? "[ - ]" : "[ " + VirtualKeyCodeToString(*key) + " ]";
			canvas->k2_drawtext(font, s2wc(dt.c_str()), tp, { 0.78f,0.72f }, RGBtoFLC(120, 120, 120, 1.2f), 0.f, RGBtoFLC(0, 0, 0), { 0,0 }, true, true, true, RGBtoFLC(0, 0, 0, 0.1f));
			if (hov && input::is_mouse_clicked(0, elements_count, false)) { already_pressed = true;active_hotkey = elements_count; }
			else if (!hov && input::is_mouse_clicked(0, elements_count, false)) active_hotkey = -1;
		}
		sameLine = false;last_element_pos = pos;last_element_size = size;
		if (first_element_pos.x == 0.f) first_element_pos = pos;
	}

	void HotkeyLabeled(const char* name, int* key)
	{
		elements_count++;
		fvector2d pad = { 195,-2 };
		fvector2d size = { 9,18 };
		fvector2d pos = { menu_pos.x + pad.x + offset_x,menu_pos.y + pad.y + offset_y };
		if (sameLine) { pos.x = last_element_pos.x + last_element_size.x + pad.x;pos.y = last_element_pos.y + (last_element_size.y / 2) - size.y / 2; }
		if (pushY) { pos.y = pushYvalue;pushY = false;pushYvalue = 0;offset_y = pos.y - menu_pos.y; }
		bool hov = MouseInZone(pos, size);
		if (!sameLine) offset_y += size.y + pad.y;
		fvector2d tp = { pos.x + size.x / 2,pos.y + size.y / 2 };
		canvas->k2_drawtext(font, s2wc(name), { pos.x - 10,pos.y + size.y / 2 }, { 0.78f,0.72f }, RGBtoFLC(120, 120, 120, 1.2f), 0.f, RGBtoFLC(0, 0, 0), { 0,0 }, true, true, true, RGBtoFLC(0, 0, 0, 0.1f));
		if (active_hotkey_labeled == elements_count) {
			canvas->k2_drawtext(font, s2wc("[ ... ]"), tp, { 0.78f,0.72f }, RGBtoFLC(120, 120, 120, 1.2f), 0.f, Colors::Text_Shadow, { 0,0 }, true, true, true, RGBtoFLC(0, 0, 0, 0.1f));
			if (!input::is_any_mouse_down()) already_pressed_labeled = false;
			if (!already_pressed_labeled) for (int c = 0;c < 255;c++) if (GetAsyncKeyState(c)) { *key = c;active_hotkey_labeled = -1; }
		}
		else {
			std::string dt = (*key == 0 || *key == -1) ? "[ - ]" : "[ " + VirtualKeyCodeToString(*key) + " ]";
			canvas->k2_drawtext(font, s2wc(dt.c_str()), tp, { 0.78f,0.72f }, RGBtoFLC(120, 120, 120, 1.2f), 0.f, RGBtoFLC(0, 0, 0), { 0,0 }, true, true, true, RGBtoFLC(0, 0, 0, 0.1f));
			if (hov && input::is_mouse_clicked(0, elements_count, false)) { already_pressed_labeled = true;active_hotkey_labeled = elements_count; }
			else if (!hov && input::is_mouse_clicked(0, elements_count, false)) active_hotkey_labeled = -1;
		}
		sameLine = false;last_element_pos = pos;last_element_size = size;
		if (first_element_pos.x == 0.f) first_element_pos = pos;
	}

	// ── GS_ yardimcilari (MenuAyarlari icin) ──────────────────────────────
	void GS_Checkbox(const wchar_t* name, bool* value)
	{
		gs_elements_count++;
		float box = 9;
		fvector2d pos = { gs_menu_pos.x + gs_offset_x + 14,gs_menu_pos.y + gs_offset_y + 10 };
		bool hov = MouseInZone(pos, { box,box });
		drawFilledRect(pos, box, box, ThemeCheckBorder());
		drawFilledRect({ pos.x + 1,pos.y + 1 }, box - 2, box - 2, ThemeCheckOff());
		if (*value) drawFilledRect({ pos.x + 1,pos.y + 1 }, box - 2, box - 2, GetBlinkAccentColor());
		canvas->k2_drawtext(font, name, { pos.x + box + 8,pos.y - 4.5f }, { 0.86f,0.80f }, ThemeText(), 0.f, RGBtoFLC(0, 0, 0), { 0,0 }, false, false, false, RGBtoFLC(0, 0, 0));
		gs_offset_y += box + 11;
		gs_last_pos = pos;gs_last_size = { box,box };
		if (hov && input::is_mouse_clicked(0, gs_elements_count + 700, false)) *value = !*value;
	}
	void GS_Radio(const wchar_t* name, int* current, int myVal)
	{
		gs_elements_count++;
		float box = 9;
		fvector2d pos = { gs_menu_pos.x + gs_offset_x + 14,gs_menu_pos.y + gs_offset_y + 10 };
		bool hov = MouseInZone(pos, { box,box });
		bool act = (*current == myVal);
		drawFilledRect(pos, box, box, ThemeCheckBorder());
		drawFilledRect({ pos.x + 1,pos.y + 1 }, box - 2, box - 2, ThemeCheckOff());
		if (act) drawFilledRect({ pos.x + 2,pos.y + 2 }, box - 4, box - 4, GetBlinkAccentColor());
		canvas->k2_drawtext(font, name, { pos.x + box + 8,pos.y - 4.5f }, { 0.86f,0.80f }, ThemeText(), 0.f, RGBtoFLC(0, 0, 0), { 0,0 }, false, false, false, RGBtoFLC(0, 0, 0));
		gs_offset_y += box + 11;
		if (hov && input::is_mouse_clicked(0, gs_elements_count + 800, false)) *current = myVal;
	}
	void GS_Section(const wchar_t* title, float w)
	{
		fvector2d pos = { gs_menu_pos.x + gs_offset_x + 8,gs_menu_pos.y + gs_offset_y + 6 };
		drawFilledRect({ gs_menu_pos.x + gs_offset_x + 8,pos.y + 7 }, w - 16, 1, GetBlinkAccentColor());
		canvas->k2_drawtext(font, title, pos, { 0.82f,0.76f }, GetBlinkAccentColor(), 0.f, RGBtoFLC(0, 0, 0), { 0,0 }, false, false, false, RGBtoFLC(0, 0, 0));
		gs_offset_y += 18;
	}


	// ── MenuAyarlariWindow ────────────────────────────────────────────────
	void MenuAyarlariWindow()
	{
		if (!g_guiSettingsOpen) return;
		if (!canvas || !font) return;
		const float W = 360, H = 390;
		fvector2d* pos = &g_guiSettingsPos;
		float sw = g_screen_w, sh = g_screen_h;
		if (g_guiSettingsPos.x == 0 && g_guiSettingsPos.y == 0)
			g_guiSettingsPos = { sw / 2 - W / 2,sh / 2 - H / 2 };
		gs_elements_count = 0;
		bool th = MouseInZone(*pos, { W,24 });
		if (gs_current_element != -1 && !((GetAsyncKeyState(0x01) & 0x8000) != 0)) gs_current_element = -1;
		if ((th || gs_dragPos.x != 0) && !gs_hover_element) {
			if (input::is_mouse_clicked(0, 249, true)) {
				fvector2d cur = CursorPos();cur.x -= W;cur.y -= H;
				if (gs_dragPos.x == 0) { gs_dragPos.x = cur.x - pos->x;gs_dragPos.y = cur.y - pos->y; }
				float nx = cur.x - gs_dragPos.x, ny = cur.y - gs_dragPos.y;
				if (nx < 0)nx = 0;if (ny < 0)ny = 0;
				if (nx + W > sw)nx = sw - W;if (ny + H > sh)ny = sh - H;
				pos->x = nx;pos->y = ny;
			}
			else { gs_dragPos = { 0,0 }; }
		}
		else if (!th) gs_hover_element = false;
		gs_menu_pos = *pos;gs_offset_x = 0;gs_offset_y = 0;
		flinearcolor ac = GetBlinkAccentColor();
		drawFilledRect({ pos->x + 4,pos->y + 4 }, W, H, { 0,0,0,0.35f });
		drawFilledRect(*pos, W, H, ThemeOuterBorder());
		drawFilledRect({ pos->x + 1,pos->y + 1 }, W - 2, H - 2, ThemeBorder());
		drawFilledRect({ pos->x + 2,pos->y + 2 }, W - 4, H - 4, ThemeBG());
		drawFilledRect({ pos->x + 2,pos->y + 2 }, W - 4, 24, ThemeHeader());
		drawFilledRect({ pos->x + 2,pos->y + 26 }, W - 4, 1, ac);
		canvas->k2_drawtext(font, g_lang == 0 ? L"Menu Ayarlari" : L"Menu Settings", { pos->x + 10,pos->y + 13 }, { 0.92f,0.85f }, ThemeText(), 0.f, RGBtoFLC(0, 0, 0), { 0,0 }, false, true, false, RGBtoFLC(0, 0, 0));
		canvas->k2_drawtext(font, L"Made By Larpingthebands (Liam)", { pos->x + W - 8,pos->y + 13 }, { 0.80f,0.74f }, ac, 0.f, RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, RGBtoFLC(0, 0, 0));
		drawFilledRect({ pos->x + 2,pos->y + H - 22 }, W - 4, 20, ThemeHeader());
		drawFilledRect({ pos->x + 2,pos->y + H - 22 }, W - 4, 1, ac);
		drawFilledRect({ pos->x + W / 2,pos->y + 28 }, 1, H - 50, ThemeBorder());
		// Sol: Tema + Dil
		gs_offset_x = 0;gs_offset_y = 30;
		GS_Section(g_lang == 0 ? L"Tema" : L"Theme", W / 2);
		GS_Radio(g_lang == 0 ? L"Siyah" : L"Dark", &g_guiTheme, 0);
		GS_Radio(g_lang == 0 ? L"Beyaz" : L"White", &g_guiTheme, 1);
		if (g_guiTheme == 1) {
			gs_offset_y += 3;GS_Section(g_lang == 0 ? L"Beyaz Stil" : L"White Style", W / 2);
			GS_Radio(g_lang == 0 ? L"Acik Beyaz" : L"Light White", &g_guiVariant, 0);
			GS_Radio(g_lang == 0 ? L"Sik Beyaz" : L"Dense White", &g_guiVariant, 1);
		}
		gs_offset_y += 5;
		GS_Section(g_lang == 0 ? L"Dil" : L"Language", W / 2);
		GS_Radio(L"Turkce", &g_lang, 0);
		// Mini TR bayragi
		{
			float fx = pos->x + W / 2 - 28, fy = pos->y + gs_offset_y - 18, fw = 22, fh = 15;
			flinearcolor tr = { 0.90f,0.08f,0.12f,1 }, tw = { 1,1,1,1 };
			drawFilledRect({ fx,fy }, fw, fh, tr);
			float cx = fx + fw * 0.38f, cy = fy + fh * 0.50f, R = fh * 0.30f;
			for (float dy = -R;dy <= R;dy += 1)for (float dx = -R;dx <= R;dx += 1)if (dx * dx + dy * dy <= R * R)drawFilledRect({ cx + dx,cy + dy }, 1, 1, tw);
			float ox2 = fx + fw * 0.47f, r2 = R * 0.74f;
			for (float dy = -r2;dy <= r2;dy += 1)for (float dx = -r2;dx <= r2;dx += 1)if (dx * dx + dy * dy <= r2 * r2)drawFilledRect({ ox2 + dx,cy + dy }, 1, 1, tr);
		}
		GS_Radio(L"English", &g_lang, 1);
		// Mini ABD bayragi
		{
			float fx = pos->x + W / 2 - 28, fy = pos->y + gs_offset_y - 18, fw = 22, fh = 15;
			flinearcolor ar = { 0.80f,0.06f,0.06f,1 }, aw = { 1,1,1,1 }, ab = { 0.07f,0.17f,0.55f,1 };
			float sh2 = fh / 13;
			for (int s = 0;s < 13;s++) drawFilledRect({ fx,fy + s * sh2 }, fw, sh2, (s % 2 == 0) ? ar : aw);
			float kw = fw * 0.40f, kh = sh2 * 7;
			drawFilledRect({ fx,fy }, kw, kh, ab);
			for (int row = 0;row < 3;row++)for (int col = 0;col < 4;col++)
				drawFilledRect({ fx + 1.5f + col * (kw / 4.5f),fy + 1.5f + row * (kh / 3.5f) }, 1.5f, 1.5f, aw);
		}
		// Sag: Efektler
		gs_offset_x = W / 2;gs_offset_y = 30;
		GS_Section(g_lang == 0 ? L"Efektler" : L"Effects", W / 2);
		GS_Checkbox(g_lang == 0 ? L"Imle\x00e7 Nokta" : L"Cursor Dot", &g_cursorDot);
		GS_Checkbox(g_lang == 0 ? L"Accent Cizgiler" : L"Accent Lines", &g_showAccentLine);
		GS_Checkbox(g_lang == 0 ? L"Filigran" : L"Watermark", &globals::Watermark);
		gs_hover_element = false;
	}
	void GUISettingsWindow() { MenuAyarlariWindow(); }


	// ── DrawVerticalTabs (animasyonlu sidebar) ────────────────────────────
	int DrawVerticalTabs(const wchar_t** names, int count, int cur,
		float sx, float sy, float sw2, float th2, fvector2d origin)
	{
		if (!canvas || !font) return -1;
		if (count > 8)count = 8;
		int clicked = -1;
		flinearcolor ac = GetBlinkAccentColor();
		drawFilledRect({ sx,sy }, sw2, th2 * count, ThemeHeader());
		drawFilledRect({ sx,sy }, 1, th2 * count, ThemeBorder());
		g_tab_anim_target = sy + cur * th2;
		float dist = fabsf(g_tab_anim_target - g_tab_anim_y);
		float speed = 0.12f + dist * 0.003f; if (speed > 0.35f)speed = 0.35f;
		g_tab_anim_y += (g_tab_anim_target - g_tab_anim_y) * speed;
		float stretch = 1 + dist * 0.008f; if (stretch > 1.4f)stretch = 1.4f;
		float bh2 = th2 * stretch, by = g_tab_anim_y + (th2 - bh2) / 2;
		drawFilledRect({ sx,by }, 5, bh2, { ac.r * 0.4f,ac.g * 0.4f,ac.b * 0.4f,0.5f });
		drawFilledRect({ sx,by }, 3, bh2, ac);
		float acy = sy + cur * th2;
		g_tab_active_w[cur] += (sw2 - 3 - g_tab_active_w[cur]) * 0.15f;
		drawFilledRect({ sx + 3,acy }, g_tab_active_w[cur], th2,
			g_guiTheme == 1 ? flinearcolor{ 0.78f,0.78f,0.83f,1 } : flinearcolor{ 0.055f,0.055f,0.065f,1 });
		for (int i = 0;i < count;i++) if (i != cur) g_tab_active_w[i] *= 0.80f;
		drawFilledRect({ sx + sw2,sy }, 1, th2 * count, ThemeBorder());
		for (int i = 0;i < count;i++) {
			float ty = sy + i * th2;
			bool hov = MouseInZone({ sx,ty }, { sw2,th2 });
			bool act = (i == cur);
			float ta = (hov ? 1.f : 0.f);
			g_tab_hover_alpha[i] += (ta - g_tab_hover_alpha[i]) * 0.18f;
			if (g_tab_hover_alpha[i] > 0.01f && !act)
				drawFilledRect({ sx + 3,ty }, sw2 - 3, th2,
					g_guiTheme == 1 ? flinearcolor{ 0.82f,0.82f,0.88f,g_tab_hover_alpha[i] * 0.85f }
			: flinearcolor{ 0.08f,0.08f,0.10f,g_tab_hover_alpha[i] * 0.85f });
			if (g_tab_hover_alpha[i] > 0.01f && !act)
				drawFilledRect({ sx,ty }, 2, th2, { ac.r,ac.g,ac.b,g_tab_hover_alpha[i] * 0.45f });
			if (i > 0) drawFilledRect({ sx + 8,ty }, sw2 - 16, 1, { ThemeBorder().r,ThemeBorder().g,ThemeBorder().b,act ? 0.f : 0.5f });
			float ds = act ? 5.f : (hov ? 4.f : 3.f);
			float dx = sx + 10, dy = ty + th2 / 2 - ds / 2;
			flinearcolor dc = act ? ac : (hov ? ThemeText() : ThemeSubText());
			if (act) drawFilledRect({ dx - 1,dy - 1 }, ds + 2, ds + 2, { ac.r,ac.g,ac.b,0.30f });
			drawFilledRect({ dx,dy }, ds, ds, dc);
			flinearcolor tc = act ? ac : (hov ? ThemeText() : ThemeSubText());
			float tx = sx + sw2 / 2 + 4, tty = ty + th2 / 2;
			fvector2d ts = act ? fvector2d{ 0.88f,0.82f } : fvector2d{ 0.84f,0.78f };
			if (act) canvas->k2_drawtext(font, names[i], { tx + 1,tty + 1 }, ts, { ac.r,ac.g,ac.b,0.25f }, 0, RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, RGBtoFLC(0, 0, 0));
			canvas->k2_drawtext(font, names[i], { tx,tty }, ts, tc, 0, RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, RGBtoFLC(0, 0, 0));
			elements_count++;
			if (hov && input::is_mouse_clicked(0, elements_count, false)) clicked = i;
		}
		return clicked;
	}

	// ── DrawESPPreview ────────────────────────────────────────────────────
	void DrawESPPreview(float px, float py, float pw, float ph,
		bool box2d, bool box3d, bool corner, bool skel, bool snap, bool hpbar, bool head, bool visc, bool water = false)
	{
		if (!canvas || !font) return;
		flinearcolor ac = GetBlinkAccentColor();
		flinearcolor green = { 0.15f,0.85f,0.25f,1 }, red = { 0.90f,0.15f,0.15f,1 }, yellow = { 0.95f,0.85f,0.10f,1 };
		drawFilledRect({ px,py }, pw, ph, ThemeOuterBorder());
		drawFilledRect({ px + 1,py + 1 }, pw - 2, ph - 2, ThemeBorder());
		drawFilledRect({ px + 2,py + 2 }, pw - 4, ph - 4, ThemeBG());
		drawFilledRect({ px + 2,py + 2 }, pw - 4, 18, ThemeHeader());
		canvas->k2_drawtext(font, g_lang == 0 ? L"ESP Onizleme" : L"ESP Preview", { px + pw / 2,py + 10 }, { 0.80f,0.74f }, ac, 0, RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, RGBtoFLC(0, 0, 0));
		float t = (float)(GetTickCount64()) / 1000.f;
		float sway = sinf(t * 1.2f) * 2;
		float ccx = px + pw * 0.40f + sway, ctop = py + 28, ch = ph - 36, cw = ch * 0.35f;
		float hr = ch * 0.10f, hcy = ctop + hr, btop = hcy + hr, bbot = ctop + ch * 0.62f, fty = ctop + ch;
		Draw_Line({ px + 2,fty + 1 }, { px + pw - 2,fty + 1 }, 1, ThemeBorder());
		if (box2d) {
			float bx = ccx - cw / 2 - 3, by = ctop - 2, bw = cw + 6, bh2 = ch + 2;
			Draw_Line({ bx,by }, { bx + bw,by }, 1, ac);Draw_Line({ bx + bw,by }, { bx + bw,by + bh2 }, 1, ac);
			Draw_Line({ bx + bw,by + bh2 }, { bx,by + bh2 }, 1, ac);Draw_Line({ bx,by + bh2 }, { bx,by }, 1, ac);
		}
		if (corner) {
			float bx = ccx - cw / 2 - 3, by = ctop - 2, bw = cw + 6, bh2 = ch + 2, cl = bw * 0.25f;
			Draw_Line({ bx,by }, { bx + cl,by }, 1, ac);Draw_Line({ bx,by }, { bx,by + cl }, 1, ac);
			Draw_Line({ bx + bw,by }, { bx + bw - cl,by }, 1, ac);Draw_Line({ bx + bw,by }, { bx + bw,by + cl }, 1, ac);
			Draw_Line({ bx,by + bh2 }, { bx + cl,by + bh2 }, 1, ac);Draw_Line({ bx,by + bh2 }, { bx,by + bh2 - cl }, 1, ac);
			Draw_Line({ bx + bw,by + bh2 }, { bx + bw - cl,by + bh2 }, 1, ac);Draw_Line({ bx + bw,by + bh2 }, { bx + bw,by + bh2 - cl }, 1, ac);
		}
		if (skel) {
			flinearcolor sc = ThemeText();
			float ny = btop + ch * 0.03f, sy2 = btop + ch * 0.12f, ely = sy2 + ch * 0.15f, hy2 = ely + ch * 0.13f;
			float slx = ccx - cw * 0.48f, srx = ccx + cw * 0.48f, kny = bbot + ch * 0.18f;
			Draw_Line({ ccx,ny }, { ccx,bbot }, 1, sc);
			Draw_Line({ ccx,sy2 }, { slx,sy2 }, 1, sc);Draw_Line({ slx,sy2 }, { slx - cw * 0.08f,ely }, 1, sc);Draw_Line({ slx - cw * 0.08f,ely }, { slx - cw * 0.04f,hy2 }, 1, sc);
			Draw_Line({ ccx,sy2 }, { srx,sy2 }, 1, sc);Draw_Line({ srx,sy2 }, { srx + cw * 0.08f,ely }, 1, sc);Draw_Line({ srx + cw * 0.08f,ely }, { srx + cw * 0.04f,hy2 }, 1, sc);
			Draw_Line({ ccx,bbot }, { ccx - cw * 0.22f,kny }, 1, sc);Draw_Line({ ccx - cw * 0.22f,kny }, { ccx - cw * 0.14f,fty }, 1, sc);
			Draw_Line({ ccx,bbot }, { ccx + cw * 0.22f,kny }, 1, sc);Draw_Line({ ccx + cw * 0.22f,kny }, { ccx + cw * 0.14f,fty }, 1, sc);
		}
		{ flinearcolor hc = head ? yellow : ThemeSubText(); drawCircle({ ccx,hcy }, hr, hc, 1, 16); }
		if (hpbar) {
			float bx = ccx - cw / 2 - 8, hp = 0.65f + sinf(t * 0.5f) * 0.15f;
			drawFilledRect({ bx,ctop }, 4, ch, ThemeBorder());
			flinearcolor hpc = hp > 0.5f ? green : (hp > 0.25f ? yellow : red);
			drawFilledRect({ bx,ctop + ch * (1 - hp) }, 4, ch * hp, hpc);
		}
		if (snap) { float bcx = px + pw / 2, bcy = py + ph - 4;Draw_Line({ bcx,bcy }, { ccx,fty }, 1, ac); }
		if (visc) { bool vis = (int)(t * 0.8f) % 2 == 0;drawFilledRect({ ccx - 3,hcy - hr - 4 }, 6, 3, vis ? green : red); }
		float lx = px + pw * 0.72f, ly = py + 24, ls = 11;
		struct { const wchar_t* n;bool a; }fs[] = {
			{g_lang == 0 ? L"2D Kutu" : L"2D Box",box2d},{g_lang == 0 ? L"Kose" : L"Corner",corner},
			{g_lang == 0 ? L"Iskelet" : L"Skel",skel},{g_lang == 0 ? L"Saglik" : L"Health",hpbar},
			{g_lang == 0 ? L"Kafa" : L"Head",head},{L"Snapline",snap},{g_lang == 0 ? L"Gorunur" : L"Vis",visc},
			{g_lang == 0 ? L"Su" : L"Water",water} };
		for (int i = 0;i < 8;i++) {
			drawFilledRect({ lx,ly + i * ls + 3.5f }, 4, 4, fs[i].a ? green : ThemeBorder());
			canvas->k2_drawtext(font, fs[i].n, { lx + 7,ly + i * ls + ls / 2 }, { 0.70f,0.65f }, fs[i].a ? ThemeText() : ThemeSubText(), 0, RGBtoFLC(0, 0, 0), { 0,0 }, false, true, false, RGBtoFLC(0, 0, 0));
		}
	}


	// ── DrawBlurOverlay ───────────────────────────────────────────────────
	void DrawBlurOverlay()
	{
		if (!g_showWelcome || !canvas) return;
		flinearcolor d = { 0,0,0,0.72f };
		for (float y = 0;y < g_screen_h;y += 6)
			canvas->k2_drawline({ 0,y }, { g_screen_w,y }, 1, d);
	}

	// ── WelcomeWindow (acilis animasyonu + bayraklar + Skip butonu) ───────
	void WelcomeWindow()
	{
		if (!g_showWelcome || !canvas || !font) return;
		const float W = 380, H = 295;
		if (!g_welcomePositioned) {
			g_welcomePos = { g_screen_w / 2 - W / 2,g_screen_h / 2 - H / 2 };
			g_welcomePositioned = true;
		}
		// Acilis animasyonu
		g_welcome_anim += (1.f - g_welcome_anim) * 0.14f;
		float alpha = g_welcome_anim, scale = 0.60f + g_welcome_anim * 0.40f;
		float aw = W * scale, ah = H * scale;
		float ox = g_welcomePos.x + (W - aw) / 2, oy = g_welcomePos.y + (H - ah) / 2;
		flinearcolor ac = GetBlinkAccentColor();
		// Golge
		drawFilledRect({ ox + 5,oy + 5 }, aw, ah, { 0,0,0,0.40f * alpha });
		// Cerceve
		drawFilledRect({ ox,oy }, aw, ah, { ThemeOuterBorder().r,ThemeOuterBorder().g,ThemeOuterBorder().b,alpha });
		drawFilledRect({ ox + 1,oy + 1 }, aw - 2, ah - 2, { ThemeBorder().r,ThemeBorder().g,ThemeBorder().b,alpha });
		drawFilledRect({ ox + 2,oy + 2 }, aw - 4, ah - 4, { ThemeBG().r,ThemeBG().g,ThemeBG().b,alpha });
		// Baslik
		drawFilledRect({ ox + 2,oy + 2 }, aw - 4, 26, { ThemeHeader().r,ThemeHeader().g,ThemeHeader().b,alpha });
		drawFilledRect({ ox + 2,oy + 28 }, aw - 4, 1, { ac.r,ac.g,ac.b,alpha });
		canvas->k2_drawtext(font, L"Hos Geldiniz / Welcome", { ox + aw / 2,oy + 14 }, { 0.92f,0.85f }, { ThemeText().r,ThemeText().g,ThemeText().b,alpha }, 0, RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, RGBtoFLC(0, 0, 0));
		canvas->k2_drawtext(font, L"Scarlet Valorant Internal Free", { ox + aw - 8,oy + 14 }, { 0.72f,0.66f }, { ac.r,ac.g,ac.b,alpha }, 0, RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, RGBtoFLC(0, 0, 0));
		// Aciklama
		canvas->k2_drawtext(font, L"Lutfen dilinizi secin:", { ox + aw / 2,oy + 46 }, { 0.88f,0.82f }, { ThemeText().r,ThemeText().g,ThemeText().b,alpha }, 0, RGBtoFLC(0, 0, 0), { 0,0 }, true, false, false, RGBtoFLC(0, 0, 0));
		canvas->k2_drawtext(font, L"Please select your language:", { ox + aw / 2,oy + 61 }, { 0.84f,0.78f }, { ThemeSubText().r,ThemeSubText().g,ThemeSubText().b,alpha }, 0, RGBtoFLC(0, 0, 0), { 0,0 }, true, false, false, RGBtoFLC(0, 0, 0));
		// Butonlar
		float bW = 128, bH = 34, bY = oy + 86;
		float b1X = ox + aw / 2 - bW - 14, b2X = ox + aw / 2 + 14;
		bool hov1 = MouseInZone({ b1X,bY }, { bW,bH }), hov2 = MouseInZone({ b2X,bY }, { bW,bH });
		g_btn1_hover_a += ((hov1 ? 1.f : 0.f) - g_btn1_hover_a) * 0.20f;
		g_btn2_hover_a += ((hov2 ? 1.f : 0.f) - g_btn2_hover_a) * 0.20f;
		auto DrawBtn = [&](float bx, float by, float bw2, float bh2, const wchar_t* lbl, float ha) {
			float gr2 = ha * 2;
			float rx = bx - gr2 / 2, ry = by - gr2 / 2, rw = bw2 + gr2, rh = bh2 + gr2;
			if (ha > 0.01f) drawFilledRect({ rx - 2,ry - 2 }, rw + 4, rh + 4, { ac.r,ac.g,ac.b,ha * 0.25f * alpha });
			flinearcolor bd = ha > 0.5f ? ac : ThemeBorder();
			drawFilledRect({ rx,ry }, rw, rh, { bd.r,bd.g,bd.b,alpha });
			flinearcolor bg2 = ha > 0.5f ? ThemeHeader() : ThemeBG();
			drawFilledRect({ rx + 1,ry + 1 }, rw - 2, rh - 2, { bg2.r,bg2.g,bg2.b,alpha });
			flinearcolor tc = ha > 0.3f ? ac : ThemeText();
			canvas->k2_drawtext(font, lbl, { rx + rw / 2,ry + rh / 2 }, { 0.88f,0.82f }, { tc.r,tc.g,tc.b,alpha }, 0, RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, RGBtoFLC(0, 0, 0));
			};
		DrawBtn(b1X, bY, bW, bH, L"Turkce", g_btn1_hover_a);
		DrawBtn(b2X, bY, bW, bH, L"English", g_btn2_hover_a);
		// TR Bayragi
		{
			float fX = b1X + (bW - 40) / 2, fY = bY + bH + 5, fw = 40, fh = 26;
			flinearcolor tr = { 0.90f,0.08f,0.12f,alpha }, tw = { 1,1,1,alpha };
			drawFilledRect({ fX,fY }, fw, fh, tr);
			float cx = fX + fw * 0.38f, cy = fY + fh * 0.50f, R = fh * 0.30f;
			for (float dy = -R;dy <= R;dy += 1)for (float dx = -R;dx <= R;dx += 1)if (dx * dx + dy * dy <= R * R)drawFilledRect({ cx + dx,cy + dy }, 1, 1, tw);
			float ox2 = fX + fw * 0.47f, r2 = R * 0.74f;
			for (float dy = -r2;dy <= r2;dy += 1)for (float dx = -r2;dx <= r2;dx += 1)if (dx * dx + dy * dy <= r2 * r2)drawFilledRect({ ox2 + dx,cy + dy }, 1, 1, tr);
		}
		// ABD Bayragi
		{
			float fX = b2X + (bW - 40) / 2, fY = bY + bH + 5, fw = 40, fh = 26;
			flinearcolor r = { 0.80f,0.06f,0.06f,alpha }, w = { 1,1,1,alpha }, b = { 0.07f,0.17f,0.55f,alpha };
			float sh2 = fh / 13;
			for (int s = 0;s < 13;s++) drawFilledRect({ fX,fY + s * sh2 }, fw, sh2, (s % 2 == 0) ? r : w);
			float kw = fw * 0.40f, kh = sh2 * 7;
			drawFilledRect({ fX,fY }, kw, kh, b);
			for (int row = 0;row < 3;row++)for (int col = 0;col < 4;col++)
				drawFilledRect({ fX + 1.5f + col * (kw / 4.5f),fY + 1.5f + row * (kh / 3.5f) }, 1.5f, 1.5f, w);
		}
		// Skip Tutorial butonu (ortada)
		{
			float skW = 130, skH = 26, skX = ox + aw / 2 - skW / 2, skY = bY + bH + 42;
			bool hs = MouseInZone({ skX,skY }, { skW,skH });
			g_btn3_hover_a += ((hs ? 1.f : 0.f) - g_btn3_hover_a) * 0.20f;
			flinearcolor sb = { ThemeSubText().r,ThemeSubText().g,ThemeSubText().b,(0.45f + g_btn3_hover_a * 0.40f) * alpha };
			drawFilledRect({ skX,skY }, skW, skH, sb);
			drawFilledRect({ skX + 1,skY + 1 }, skW - 2, skH - 2, { ThemeBG().r,ThemeBG().g,ThemeBG().b,alpha });
			flinearcolor st = g_btn3_hover_a > 0.4f ? ac : ThemeSubText();
			canvas->k2_drawtext(font, L"Atla / Skip Tutorial", { skX + skW / 2,skY + skH / 2 }, { 0.78f,0.72f }, { st.r,st.g,st.b,alpha }, 0, RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, RGBtoFLC(0, 0, 0));
			elements_count++;
			if (hs && input::is_mouse_clicked(0, 903, false)) { g_lang = 0;g_showWelcome = false; }
		}
		// Alt not
		canvas->k2_drawtext(font, L"Bu ayari Menu Ayarlari'ndan degistirebilirsiniz.", { ox + aw / 2,oy + H * scale - 14 }, { 0.70f,0.65f }, { ThemeSubText().r,ThemeSubText().g,ThemeSubText().b,alpha * 0.75f }, 0, RGBtoFLC(0, 0, 0), { 0,0 }, true, false, false, RGBtoFLC(0, 0, 0));
		elements_count++;if (hov1 && input::is_mouse_clicked(0, 901, false)) { g_lang = 0;g_showWelcome = false; }
		elements_count++;if (hov2 && input::is_mouse_clicked(0, 902, false)) { g_lang = 1;g_showWelcome = false; }
	}


	// ── current_element_pos/size (layout yardimcilari) ───────────────────
	// (global olarak yukarıda tanımlı)

	// ── DrawSpectatorList — radar'ın altında sizi izleyenleri gösterir ─────
	void DrawSpectatorList()
	{
		if (!globals::visuals::show_spectators) return;
		if (!canvas || !font) return;
		const auto& specs = globals::visuals::spectator_names;

		// Radar: pRadar = {0,0,0} → sol üst köşe (0,0), çap 270px
		// Spectator paneli radar'ın tam altına gelsin
		const float RADAR_X = 0.0f;    // pRadar.x
		const float RADAR_Y = 0.0f;    // pRadar.y
		const float RADAR_D = 270.0f;  // radar çapı
		const float GAP = 6.0f;

		float pw = RADAR_D;             // panel genişliği = radar genişliği
		float lh = 16.0f;
		int   cnt = (int)specs.size();

		// Hiç izleyen yok bile başlığı göster (boş liste mesajıyla)
		float ph = 20.0f + (cnt > 0 ? cnt * lh : lh) + 4.0f;
		float px2 = RADAR_X;
		float py2 = RADAR_Y + RADAR_D + GAP;

		flinearcolor ac = GetBlinkAccentColor();

		// Gölge
		drawFilledRect({ px2 + 2, py2 + 2 }, pw, ph, { 0,0,0,0.35f });
		// Çerçeve
		drawFilledRect({ px2,   py2 }, pw, ph, ThemeOuterBorder());
		drawFilledRect({ px2 + 1, py2 + 1 }, pw - 2, ph - 2, ThemeBorder());
		drawFilledRect({ px2 + 2, py2 + 2 }, pw - 4, ph - 4, ThemeBG());
		// Başlık çubuğu
		drawFilledRect({ px2 + 2, py2 + 2 }, pw - 4, 18.0f, ThemeHeader());
		drawFilledRect({ px2 + 2, py2 + 20 }, pw - 4, 1.0f, ac);

		const wchar_t* title = (g_lang == 0) ? L"Sizi Izleyenler" : L"Spectators";
		canvas->k2_drawtext(font, title,
			{ px2 + pw / 2.0f, py2 + 10.0f },
			{ 0.80f, 0.74f }, ac, 0.0f,
			RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, RGBtoFLC(0, 0, 0));

		if (cnt == 0) {
			// Kimse izlemiyor
			const wchar_t* empty = (g_lang == 0) ? L"Kimse izlemiyor" : L"No spectators";
			canvas->k2_drawtext(font, empty,
				{ px2 + pw / 2.0f, py2 + 22.0f + lh / 2.0f },
				{ 0.76f, 0.70f }, ThemeSubText(), 0.0f,
				RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, RGBtoFLC(0, 0, 0));
			return;
		}

		// İzleyici satırları
		for (int i = 0; i < cnt; i++) {
			float iy = py2 + 22.0f + i * lh;
			// Alternatif satır arka planı
			if (i % 2 == 0)
				drawFilledRect({ px2 + 2, iy }, pw - 4, lh, { ac.r,ac.g,ac.b,0.06f });
			// Göz ikonu
			drawFilledRect({ px2 + 6, iy + lh / 2.0f - 2 }, 4, 4, ac);
			// İsim
			canvas->k2_drawtext(font, specs[i].c_str(),
				{ px2 + 14.0f, iy + lh / 2.0f },
				{ 0.78f, 0.72f }, ThemeText(), 0.0f,
				RGBtoFLC(0, 0, 0), { 0,0 }, false, true, false, RGBtoFLC(0, 0, 0));
		}
	}

	// ── Render ────────────────────────────────────────────────────────────
	void Render()
	{
		for (int i = 0;i < 128;i++) {
			if (PostRenderer::drawlist[i].type == -1) continue;
			auto& d = PostRenderer::drawlist[i];
			if (d.type == 1 || d.type == 6) {
				float w = d.type == 1 ? d.size.x : d.width, h = d.type == 1 ? d.size.y : d.height;
				drawFilledRect(d.pos, w, h, d.color);
			}
			else if (d.type == 2) {
				canvas->k2_drawtext(font, d.name, d.pos, { 0.98f,0.98f }, d.color, 0.f, Colors::Text_Shadow, { 0,0 }, false, false, d.outline, Colors::Text_Outline);
			}
			else if (d.type == 3) {
				canvas->k2_drawtext(font, d.name, d.pos, { 0.98f,0.98f }, d.color, 0.f, Colors::Text_Shadow, { 0,0 }, true, false, d.outline, Colors::Text_Outline);
			}
			else if (d.type == 4) {
				Draw_Line(d.from, d.to, d.thickness, d.color);
			}
			else if (d.type == 5) {
				canvas->k2_drawtext(font, d.name, d.pos, d.scale, d.color, 0.f, d.outline_color, d.shadow_offset, d.center, true, d.outline, Colors::Text_Outline);
			}
			d.type = -1;
		}
	}

} // namespace menu
#pragma once
#include "canva.hpp"
#include "config.h"
#include "../mfa_bypass.hpp"
#include <ShlObj.h>
#include <filesystem>

// ─── MULTI-CONFIG FONKSIYONLARI ──────────────────────────────────────────────
// Config nesnesi ve std::filesystem buradan erisilebiliyor.

inline void ScanConfigs()
{
    menu::g_configCount = 0;
    menu::g_configSelected = -1;

    std::string dir = "C:/MagicBullet";
    if (!std::filesystem::exists(dir)) return;

    for (auto& entry : std::filesystem::directory_iterator(dir))
    {
        if (menu::g_configCount >= menu::CFG_MAX) break;
        if (entry.path().extension() != ".json") continue;

        std::string stem = entry.path().stem().string();
        menu::ConfigEntry ce;
        strncpy_s(ce.name, stem.c_str(), menu::CFG_NAME_LEN - 1);
        ce.exists = true;
        menu::g_configList[menu::g_configCount++] = ce;
    }
}

inline void DrawConfigPanel(bool TR, float panel_x, float panel_y, float panel_w, float panel_h)
{
    if (!menu::canvas || !menu::font) return;

    flinearcolor accent = menu::GetBlinkAccentColor();

    // panel_x / panel_y zaten menu_pos'a göre — direkt kullan
    float x = menu::menu_pos.x + panel_x;
    float y = menu::menu_pos.y + panel_y;
    float input_w = panel_w - 4.0f;
    float input_h = 20.0f;
    float input_x = x + 2.0f;
    float input_y = y + 14.0f;   // label için yukarıda yer bırak

    // Label
    menu::canvas->k2_drawtext(menu::font,
        TR ? L"Yeni konfig adi:" : L"New config name:",
        { input_x, input_y - 11.0f },
        { 0.85f, 0.80f }, menu::ThemeSubText(), 0.0f,
        menu::RGBtoFLC(0, 0, 0), { 0,0 }, false, false, false, menu::RGBtoFLC(0, 0, 0));

    bool inputActive = (menu::active_textbox == 450);
    menu::drawFilledRect({ input_x, input_y }, input_w, input_h,
        inputActive ? accent : menu::ThemeComboBorder());
    menu::drawFilledRect({ input_x + 1, input_y + 1 }, input_w - 2, input_h - 2,
        inputActive ? menu::ThemeHeader() : menu::ThemeComboBg());

    std::wstring wname(menu::g_configNewName,
        menu::g_configNewName + strlen(menu::g_configNewName));
    menu::canvas->k2_drawtext(menu::font, wname.c_str(),
        { input_x + 5.0f, input_y + input_h / 2.0f },
        { 0.88f, 0.82f }, menu::ThemeText(), 0.0f,
        menu::RGBtoFLC(0, 0, 0), { 0,0 }, false, true, false, menu::RGBtoFLC(0, 0, 0));

    if (inputActive && (int)(GetTickCount64() / 400) % 2)
        menu::canvas->k2_drawtext(menu::font, L"_",
            { input_x + 5.0f + (float)wcslen(wname.c_str()) * 7.5f, input_y + input_h / 2.0f },
            { 0.85f, 0.80f }, accent, 0.0f,
            menu::RGBtoFLC(0, 0, 0), { 0,0 }, false, true, false, menu::RGBtoFLC(0, 0, 0));

    bool hovInput = menu::MouseInZone({ input_x, input_y }, { input_w, input_h });
    menu::elements_count++;
    if (hovInput && menu::input::is_mouse_clicked(0, menu::elements_count, false))
        menu::active_textbox = 450;
    else if (!hovInput && menu::input::is_mouse_clicked(0, menu::elements_count, false))
        if (menu::active_textbox == 450) menu::active_textbox = -1;

    if (menu::active_textbox == 450)
    {
        BYTE kb[256]; GetKeyboardState(kb);
        size_t cur_len = strlen(menu::g_configNewName);
        for (int vk = 0; vk < 255; vk++) {
            if (!(GetAsyncKeyState(vk) & 0x1)) continue;
            if (vk == VK_BACK && cur_len > 0) {
                menu::g_configNewName[cur_len - 1] = '\0';
                cur_len--;
            }
            else if (vk == VK_RETURN) {
                menu::active_textbox = -1;
            }
            else if (cur_len < (size_t)menu::CFG_NAME_LEN - 1) {
                WCHAR wc[4] = {};
                int r = ToUnicode(vk, MapVirtualKey(vk, MAPVK_VK_TO_VSC), kb, wc, 4, 0);
                if (r > 0 && iswprint(wc[0]) && wc[0] != L'/' && wc[0] != L'\\' && wc[0] != L':') {
                    char mb[4] = {};
                    WideCharToMultiByte(CP_UTF8, 0, wc, r, mb, 3, nullptr, nullptr);
                    strncat_s(menu::g_configNewName, mb, menu::CFG_NAME_LEN - cur_len - 1);
                }
            }
        }
    }

    // Kaydet + Yenile butonlari
    float saveY = input_y + input_h + 6.0f;
    float saveBtnW = (input_w - 4.0f) / 2.0f;
    float saveBtnH = 18.0f;

    bool hovSave = menu::MouseInZone({ input_x, saveY }, { saveBtnW, saveBtnH });
    menu::drawFilledRect({ input_x, saveY }, saveBtnW, saveBtnH, hovSave ? accent : menu::ThemeBorder());
    menu::drawFilledRect({ input_x + 1, saveY + 1 }, saveBtnW - 2, saveBtnH - 2, hovSave ? menu::ThemeHeader() : menu::ThemeBG());
    menu::canvas->k2_drawtext(menu::font, TR ? L"Kaydet" : L"Save",
        { input_x + saveBtnW / 2.0f, saveY + saveBtnH / 2.0f },
        { 0.88f, 0.82f }, menu::ThemeText(), 0.0f,
        menu::RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, menu::RGBtoFLC(0, 0, 0));

    menu::elements_count++;
    if (hovSave && menu::input::is_mouse_clicked(0, menu::elements_count, false)) {
        if (strlen(menu::g_configNewName) > 0) {
            std::filesystem::create_directories("C:/MagicBullet");
            std::string path = std::string("C:/MagicBullet/") + menu::g_configNewName + ".json";
            Config->SaveSettings(path);
            ScanConfigs();
        }
    }

    float scanX = input_x + saveBtnW + 4.0f;
    bool hovScan = menu::MouseInZone({ scanX, saveY }, { saveBtnW, saveBtnH });
    menu::drawFilledRect({ scanX, saveY }, saveBtnW, saveBtnH, hovScan ? accent : menu::ThemeBorder());
    menu::drawFilledRect({ scanX + 1, saveY + 1 }, saveBtnW - 2, saveBtnH - 2, hovScan ? menu::ThemeHeader() : menu::ThemeBG());
    menu::canvas->k2_drawtext(menu::font, TR ? L"Yenile" : L"Scan",
        { scanX + saveBtnW / 2.0f, saveY + saveBtnH / 2.0f },
        { 0.88f, 0.82f }, menu::ThemeText(), 0.0f,
        menu::RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, menu::RGBtoFLC(0, 0, 0));

    menu::elements_count++;
    if (hovScan && menu::input::is_mouse_clicked(0, menu::elements_count, false))
        ScanConfigs();

    // Config listesi
    float listX = input_x;
    float listY = saveY + saveBtnH + 8.0f;
    float listW = input_w;
    float itemH = 20.0f;
    float loadDelW = 42.0f;
    float nameW = listW - loadDelW * 2.0f - 6.0f;
    float maxY = menu::menu_pos.y + panel_y + panel_h - 4.0f;

    if (menu::g_configCount == 0) {
        menu::canvas->k2_drawtext(menu::font,
            TR ? L"Hic konfig bulunamadi." : L"No configs found.",
            { listX + listW / 2.0f, listY + 10.0f },
            { 0.82f, 0.76f }, menu::ThemeSubText(), 0.0f,
            menu::RGBtoFLC(0, 0, 0), { 0,0 }, true, false, false, menu::RGBtoFLC(0, 0, 0));
        return;
    }

    for (int i = 0; i < menu::g_configCount; i++)
    {
        float iy = listY + i * (itemH + 2.0f);
        if (iy + itemH > maxY) break;

        bool selected = (menu::g_configSelected == i);
        bool hovRow = menu::MouseInZone({ listX, iy }, { nameW, itemH });

        menu::drawFilledRect({ listX, iy }, nameW, itemH,
            selected ? accent : (hovRow ? menu::ThemeBorder() : menu::ThemeComboBorder()));
        menu::drawFilledRect({ listX + 1, iy + 1 }, nameW - 2, itemH - 2,
            selected ? menu::ThemeHeader() : menu::ThemeComboBg());

        std::wstring wn(menu::g_configList[i].name,
            menu::g_configList[i].name + strlen(menu::g_configList[i].name));
        menu::canvas->k2_drawtext(menu::font, wn.c_str(),
            { listX + 5.0f, iy + itemH / 2.0f },
            { 0.85f, 0.80f }, selected ? accent : menu::ThemeText(), 0.0f,
            menu::RGBtoFLC(0, 0, 0), { 0,0 }, false, true, false, menu::RGBtoFLC(0, 0, 0));

        menu::elements_count++;
        if (hovRow && menu::input::is_mouse_clicked(0, menu::elements_count, false))
            menu::g_configSelected = i;

        // Yukle
        float lbX = listX + nameW + 2.0f;
        bool hovLoad = menu::MouseInZone({ lbX, iy }, { loadDelW, itemH });
        menu::drawFilledRect({ lbX, iy }, loadDelW, itemH, hovLoad ? accent : menu::ThemeBorder());
        menu::drawFilledRect({ lbX + 1, iy + 1 }, loadDelW - 2, itemH - 2, hovLoad ? menu::ThemeHeader() : menu::ThemeBG());
        menu::canvas->k2_drawtext(menu::font, TR ? L"Yukle" : L"Load",
            { lbX + loadDelW / 2.0f, iy + itemH / 2.0f },
            { 0.82f, 0.76f }, menu::ThemeText(), 0.0f,
            menu::RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, menu::RGBtoFLC(0, 0, 0));

        menu::elements_count++;
        if (hovLoad && menu::input::is_mouse_clicked(0, menu::elements_count, false)) {
            std::string path = std::string("C:/MagicBullet/") + menu::g_configList[i].name + ".json";
            if (std::filesystem::exists(path))
                Config->LoadSettings(path);
        }

        // Sil
        float dbX = lbX + loadDelW + 2.0f;
        bool hovDel = menu::MouseInZone({ dbX, iy }, { loadDelW, itemH });
        flinearcolor delC = hovDel
            ? flinearcolor{ 0.80f, 0.10f, 0.10f, 1.0f }
        : menu::ThemeBorder();
        menu::drawFilledRect({ dbX, iy }, loadDelW, itemH, delC);
        menu::drawFilledRect({ dbX + 1, iy + 1 }, loadDelW - 2, itemH - 2,
            hovDel ? flinearcolor{ 0.30f,0.04f,0.04f,1.0f } : menu::ThemeBG());
        menu::canvas->k2_drawtext(menu::font, TR ? L"Sil" : L"Del",
            { dbX + loadDelW / 2.0f, iy + itemH / 2.0f },
            { 0.82f, 0.76f }, menu::ThemeText(), 0.0f,
            menu::RGBtoFLC(0, 0, 0), { 0,0 }, true, true, false, menu::RGBtoFLC(0, 0, 0));

        menu::elements_count++;
        if (hovDel && menu::input::is_mouse_clicked(0, menu::elements_count, false)) {
            std::string path = std::string("C:/MagicBullet/") + menu::g_configList[i].name + ".json";
            if (std::filesystem::exists(path))
                std::filesystem::remove(path);
            ScanConfigs();
        }
    }
}

//
//void SaveVandalTextTransforms() {
//    std::string desktopPath = std::getenv("USERPROFILE");
//    desktopPath += "\\Desktop\\weapon_transforms.txt";
//    std::ofstream file(desktopPath);
//    if (!file.is_open()) { printf("[-] Failed to save\n"); return; }
//
//    file << "=== VANDAL TEXT ===" << std::endl;
//    file << "pos_x=" << globals::misc::text_pos_x << std::endl;
//    file << "pos_y=" << globals::misc::text_pos_y << std::endl;
//    file << "pos_z=" << globals::misc::text_pos_z << std::endl;
//    file << "rot_pitch=" << globals::misc::text_rot_pitch << std::endl;
//    file << "rot_yaw=" << globals::misc::text_rot_yaw << std::endl;
//    file << "rot_roll=" << globals::misc::text_rot_roll << std::endl;
//    file << "scale_x=" << globals::misc::text_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::text_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::text_scale_z << std::endl;
//
//    file << "=== FRENZY TEXT ===" << std::endl;
//    file << "pos_x=" << globals::misc::frenzy_text_pos_x << std::endl;
//    file << "pos_y=" << globals::misc::frenzy_text_pos_y << std::endl;
//    file << "pos_z=" << globals::misc::frenzy_text_pos_z << std::endl;
//    file << "rot_pitch=" << globals::misc::frenzy_text_rot_pitch << std::endl;
//    file << "rot_yaw=" << globals::misc::frenzy_text_rot_yaw << std::endl;
//    file << "rot_roll=" << globals::misc::frenzy_text_rot_roll << std::endl;
//    file << "scale_x=" << globals::misc::frenzy_text_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::frenzy_text_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::frenzy_text_scale_z << std::endl;
//
//    file << "=== GHOST TEXT ===" << std::endl;
//    file << "pos_x=" << globals::misc::ghost_text_pos_x << std::endl;
//    file << "pos_y=" << globals::misc::ghost_text_pos_y << std::endl;
//    file << "pos_z=" << globals::misc::ghost_text_pos_z << std::endl;
//    file << "rot_pitch=" << globals::misc::ghost_text_rot_pitch << std::endl;
//    file << "rot_yaw=" << globals::misc::ghost_text_rot_yaw << std::endl;
//    file << "rot_roll=" << globals::misc::ghost_text_rot_roll << std::endl;
//    file << "scale_x=" << globals::misc::ghost_text_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::ghost_text_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::ghost_text_scale_z << std::endl;
//
//    file << "=== PHANTOM TEXT ===" << std::endl;
//    file << "pos_x=" << globals::misc::phantom_text_pos_x << std::endl;
//    file << "pos_y=" << globals::misc::phantom_text_pos_y << std::endl;
//    file << "pos_z=" << globals::misc::phantom_text_pos_z << std::endl;
//    file << "rot_pitch=" << globals::misc::phantom_text_rot_pitch << std::endl;
//    file << "rot_yaw=" << globals::misc::phantom_text_rot_yaw << std::endl;
//    file << "rot_roll=" << globals::misc::phantom_text_rot_roll << std::endl;
//    file << "scale_x=" << globals::misc::phantom_text_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::phantom_text_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::phantom_text_scale_z << std::endl;
//
//    file << "=== VANDAL MESH ===" << std::endl;
//    file << "scale_x=" << globals::misc::vandal_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::vandal_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::vandal_scale_z << std::endl;
//
//    file << "=== FRENZY MESH ===" << std::endl;
//    file << "scale_x=" << globals::misc::frenzy_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::frenzy_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::frenzy_scale_z << std::endl;
//
//    file << "=== GHOST MESH ===" << std::endl;
//    file << "scale_x=" << globals::misc::ghost_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::ghost_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::ghost_scale_z << std::endl;
//
//    file << "=== SPECTRE TEXT ===" << std::endl;
//    file << "pos_x=" << globals::misc::spectre_text_pos_x << std::endl;
//    file << "pos_y=" << globals::misc::spectre_text_pos_y << std::endl;
//    file << "pos_z=" << globals::misc::spectre_text_pos_z << std::endl;
//    file << "rot_pitch=" << globals::misc::spectre_text_rot_pitch << std::endl;
//    file << "rot_yaw=" << globals::misc::spectre_text_rot_yaw << std::endl;
//    file << "rot_roll=" << globals::misc::spectre_text_rot_roll << std::endl;
//    file << "scale_x=" << globals::misc::spectre_text_scale_x << std::endl;
//    file << "scale_y=" << globals::misc::spectre_text_scale_y << std::endl;
//    file << "scale_z=" << globals::misc::spectre_text_scale_z << std::endl;
//
//    file.close();
//    printf("[+] Saved weapon_transforms.txt\n");
//}
//
//
//namespace burat {
//
//    fvector2d pos = { 960 - (490 / 2), 540 - (420 / 2) };
//    bool menu_open = true;
//    static flinearcolor fovcolor = { 255.0f, 255.0f, 255.0f, 1.0f };
//
//    void Menu(ucanvas* canvas)
//    {
//        menu::SetupCanvas(canvas);
//        menu::input::handle();
//
//        if (GetAsyncKeyState(VK_DELETE) & 1) menu_open = !menu_open;
//
//        if (menu::Window(L"Magicbullet", &pos, fvector2d(490, 420), menu_open))
//        {
//            static int tab = 0;
//
//            if (menu::ButtonTab2(L"Aimbot", fvector2d(50, 23), tab == 0)) tab = 0;
//            menu::SameLine();
//            if (menu::ButtonTab2(L"Visuals", fvector2d(50, 23), tab == 1)) tab = 1;
//            menu::SameLine();
//            if (menu::ButtonTab2(L"Chams", fvector2d(45, 23), tab == 2)) tab = 2;
//            menu::SameLine();
//            if (menu::ButtonTab2(L"Exploit", fvector2d(50, 23), tab == 3)) tab = 3;
//            menu::SameLine();
//            if (menu::ButtonTab2(L"Misc", fvector2d(35, 23), tab == 4)) tab = 4;
//
//           
//            if (tab == 0)
//            {
//                static int sub_tab = 1;
//
//                menu::offset_x = 82.0f;
//                menu::offset_y = 2.0f + 23.0f;
//
//                if (menu::ButtonTab2(L"Main", fvector2d(40, 23), sub_tab == 1)) sub_tab = 1;
//                menu::SameLine();
//                if (menu::ButtonTab2(L"Auto Shot", fvector2d(65, 23), sub_tab == 2)) sub_tab = 2;
//
//                if (sub_tab == 1)
//                {
//                    menu::offset_x = 92;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Aimbot", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Enabled", &globals::aimbot::a1mbot);
//                    menu::SameLine();
//                    menu::Hotkey("Key", fvector2d(9, 18), &globals::aimbot::a1m_k3y);
//                    menu::Checkbox(L"Silent Aim", &globals::aimbot::silent);
//                    menu::Checkbox(L"Nospread", &globals::aimbot::nospread);
//                    menu::Checkbox(L"Autowall", &globals::aimbot::auto_wall);
//                    menu::Checkbox(L"Visible Check", &globals::aimbot::v1sh_ch3ck);
//                    menu::Checkbox(L"Recoil Control", &globals::aimbot::reco1l_contr0l);
//                    menu::Checkbox(L"Draw FOV", &globals::aimbot::draw_f0v);
//                    menu::Checkbox(L"360 FOV", &globals::aimbot::enable_360_fov);
//                    menu::SliderFloat(L"FOV", &globals::aimbot::a1m_f0v, 5.0f, 2000.0f, "%.1f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Smooth", &globals::aimbot::a1m_sm00th, 1.0f, 20.0f, "%.1f");
//                    menu::Combobox(fvector2d(180, 23), &globals::aimbot::a1m_b0ne,
//                        L"Head", L"Neck", L"Chest", NULL);
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Settings", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    if (globals::aimbot::nospread) {
//                        globals::aimbot::reco1l_contr0l = false;
//                    }
//                }
//                else if (sub_tab == 2)
//                {
//                    menu::offset_x = 92;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Auto Shot", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Auto Shot", &globals::aimbot::auto_shot);
//                    menu::Checkbox(L"Hold Aim Key", &globals::aimbot::auto_shot_hold_key);
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Options", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//                }
//            }
//
//            else if (tab == 1)
//            {
//                static int sub_tab = 1;
//
//                menu::offset_x = 82.0f;
//                menu::offset_y = 2.0f + 23.0f;
//
//                if (menu::ButtonTab2(L"ESP", fvector2d(35, 23), sub_tab == 1)) sub_tab = 1;
//                menu::SameLine();
//                if (menu::ButtonTab2(L"World", fvector2d(45, 23), sub_tab == 2)) sub_tab = 2;
//                menu::SameLine();
//                if (menu::ButtonTab2(L"Materials", fvector2d(60, 23), sub_tab == 3)) sub_tab = 3;
//
//                if (sub_tab == 1)
//                {
//                    menu::offset_x = 92;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Player ESP", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"2D Box", &globals::visuals::box2d);
//                    menu::Checkbox(L"3D Box", &globals::visuals::box3d);
//                    menu::Checkbox(L"Corner Box", &globals::visuals::corner);
//                    menu::Checkbox(L"Vis Check", &globals::visuals::vischeck);
//                    menu::Checkbox(L"Head ESP", &globals::visuals::headb0x);
//                    menu::Checkbox(L"Health Bar", &globals::visuals::h3althbar);
//                    menu::Checkbox(L"Skeleton", &globals::visuals::sk3let0n);
//                    menu::Checkbox(L"Snapline", &globals::visuals::snapl1ne);
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Info ESP", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Agent Icon", &globals::visuals::b00ms);
//                    menu::Checkbox(L"Weapon Info", &globals::visuals::b11ms);
//                    menu::Checkbox(L"Distance", &globals::visuals::dstc);
//                    menu::Checkbox(L"Spike Info", &globals::visuals::spike);
//                    menu::Checkbox(L"China Hat", &globals::visuals::chinahat);
//                    menu::Checkbox(L"China Hat Self", &globals::visuals::partyhat_self);
//
//                    if (globals::visuals::corner) {
//                        globals::visuals::box2d = false;
//                        globals::visuals::box3d = false;
//                    }
//                    if (globals::visuals::box3d) {
//                        globals::visuals::corner = false;
//                        globals::visuals::box2d = false;
//                    }
//                    if (globals::visuals::box2d) {
//                        globals::visuals::corner = false;
//                        globals::visuals::box3d = false;
//                    }
//                }
//                else if (sub_tab == 2)
//                {
//                    menu::offset_x = 92;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"World", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"No Smoke", &globals::misc::no_smoke);
//                    menu::Checkbox(L"Skybox", &globals::misc::skybox);
//                    menu::Checkbox(L"Skybox RGB", &globals::misc::skyboxrgb);
//                    menu::Checkbox(L"Bullet Tracer", &globals::misc::bullet_tracers);
//
//                    menu::SliderFloat(L"Stars", &globals::misc::StarsBrightness, 0.f, 2500.f, "%.0f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Cloud Speed", &globals::misc::CloudSpeed, 0.f, 50.f, "%.1f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Cloud Opacity", &globals::misc::CloudOpacity, 0.f, 10000.f, "%.0f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Cloud op multi", &globals::misc::CloudOpacity1, 0.f, 100000.f, "%.0f");
//                    menu::offset_y -= 16;
//
//                    menu::SliderFloat(L"Sky R", &globals::misc::SkySharedR, 0.f, 1.f, "%.2f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Sky G", &globals::misc::SkySharedG, 0.f, 1.f, "%.2f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Sky B", &globals::misc::SkySharedB, 0.f, 1.f, "%.2f");
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Wireframe", fvector2d(325, 175));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Enemy Wire", &globals::misc::Wireframe);
//                    menu::Checkbox(L"Self Wire", &globals::misc::self_wireframe);
//                    menu::Checkbox(L"Hand Wire", &globals::misc::HandWire);
//                    menu::Checkbox(L"Gun Wire", &globals::misc::WireframeGun);
//                    menu::Checkbox(L"Gun 3P Wire", &globals::misc::gun_3p_wireframe);
//
//                    // Fog section
//                    menu::offset_x = 250;
//                    menu::offset_y = 215;
//                    menu::SectionWrapper(L"Fog", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Fog", &globals::misc::Fog);
//                    menu::Checkbox(L"Fog", &globals::misc::FogRGB);
//                    menu::Checkbox(L"Volumetric Fog", &globals::misc::bEnableVolumetricFog);
//                    menu::SliderFloat(L"Density", &globals::misc::FogDensity, 0.f, 1.f, "%.2f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Falloff", &globals::misc::FogHeightFalloff, 0.f, 1.f, "%.2f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Opacity", &globals::misc::FogMaxOpacity, 0.f, 1.f, "%.2f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Start Dist", &globals::misc::FogStartDistance, 0.f, 50000.f, "%.0f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Cutoff", &globals::misc::FogCutoffDistance, 0.f, 50000.f, "%.0f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Vol Dist", &globals::misc::VolumetricFogDistance, 0.f, 20000.f, "%.0f");
//                    menu::offset_y -= 16;
//                    // Fog color sliders
//                    menu::SliderFloat(L"Fog R", &globals::misc::FogColor.r, 0.f, 1.f, "%.2f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Fog G", &globals::misc::FogColor.g, 0.f, 1.f, "%.2f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Fog B", &globals::misc::FogColor.b, 0.f, 1.f, "%.2f");
//                }
//                else if (sub_tab == 3)
//                {
//                    menu::offset_x = 92;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Materials", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Material Hand", &globals::visuals::hand_with_material);
//                    menu::Checkbox(L"Material 1P Gun", &globals::visuals::gunmaterial1p);
//                    menu::Checkbox(L"Material 3P Gun", &globals::visuals::gunmaterial3p);
//                    menu::Checkbox(L"Material Skin 3P", &globals::misc::playerchamsself);
//                    menu::Combobox(fvector2d(180, 23), &globals::visuals::typehand,
//                        L"Black", L"Reyna", L"Green", L"Glass", L"Pink", L"Yellow", L"Red", L"Blue", NULL);
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Gun Types", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Combobox(fvector2d(180, 23), &globals::visuals::typegun1p,
//                        L"Glass", L"Red", L"Blue", L"Yellow", NULL);
//                    menu::Combobox(fvector2d(180, 23), &globals::visuals::typegun3d,
//                        L"Glass", L"Red", L"Blue", L"Yellow", NULL);
//                    menu::Combobox(fvector2d(180, 23), &globals::misc::chams_material_index,
//                        L"Ninja", L"Cyber", L"Soviet", L"Sea", NULL);
//                }
//            }
//
//            // ==================== CHAMS TAB ====================
//            else if (tab == 2)
//            {
//                static int sub_tab = 1;
//
//                menu::offset_x = 82.0f;
//                menu::offset_y = 2.0f + 23.0f;
//
//                if (menu::ButtonTab2(L"Outline", fvector2d(50, 23), sub_tab == 1)) sub_tab = 1;
//                menu::SameLine();
//                if (menu::ButtonTab2(L"Glow", fvector2d(40, 23), sub_tab == 2)) sub_tab = 2;
//                menu::SameLine();
//                if (menu::ButtonTab2(L"Galaxy", fvector2d(50, 23), sub_tab == 3)) sub_tab = 3;
//
//                if (sub_tab == 1)
//                {
//                    menu::offset_x = 92;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Outline Chams", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Outline Chams", &globals::chams::outline_enabled);
//                    menu::Checkbox(L"Hand Outline", &globals::chams::hand_outline_enabled);
//                    menu::Checkbox(L"Self Chams", &globals::chams::self_chams);
//                    menu::Checkbox(L"Gun 1P Chams", &globals::chams::gun_outline1P_enabled);
//                    menu::Checkbox(L"Gun 3P Chams", &globals::chams::gun_outline3P_enabled);
//                    menu::SliderFloat(L"Vis Intensity", &globals::chams::intensityvisibleoutline, 10, 100, "%.0f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Invis Intensity", &globals::chams::intensityinvisbleoutline, 10, 100, "%.0f");
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Presets", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Combobox(fvector2d(180, 23), &globals::chams::outlinetype,
//                        L"Always", L"Behind", NULL);
//                    menu::Combobox(fvector2d(180, 23), &globals::chams::visible_outline_preset,
//                        L"Galaxy", L"Blue", L"Green", L"Orange", L"Pink", L"White", NULL);
//                    menu::Combobox(fvector2d(180, 23), &globals::chams::invisible_outline_preset,
//                        L"Red", L"Orange", L"Yellow", L"Green", L"Pink", L"Gray", NULL);
//                }
//                else if (sub_tab == 2)
//                {
//                    menu::offset_x = 92;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Visible Chams", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Visible Chams", &globals::chams::rchamsespall);
//                    menu::SliderFloat(L"Intensity", &globals::chams::Glow, 0.1f, 10.0f, "%.1f");
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Invisible Chams", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Invisible Chams", &globals::chams::rchamsesp);
//                    menu::SliderFloat(L"Intensity", &globals::chams::Glowvni, 0.1f, 10.0f, "%.1f");
//                }
//                else if (sub_tab == 3)
//                {
//                    menu::offset_x = 92;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Galaxy Chams", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Hand Galaxy", &globals::chams::hand_galaxy_enabled);
//                    menu::Checkbox(L"Gun 1P Galaxy", &globals::chams::gun1p_galaxy_enabled);
//                    menu::Checkbox(L"Gun 3P Galaxy", &globals::chams::gun3p_galaxy_enabled);
//                    menu::Checkbox(L"Self Galaxy", &globals::chams::self_galaxy_enabled);
//                    menu::Checkbox(L"Enemy Galaxy", &globals::chams::enemy_galaxy_enabled);
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Crystal Preset", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Combobox(fvector2d(180, 23), &globals::visuals::crystal_chams_preset,
//                        L"Custom", L"Red", L"Green", L"Blue", L"Orange", L"Pink", L"Purple", NULL);
//                }
//            }
//
//            
//            else if (tab == 3)
//            {
//                static int sub_tab = 1;
//
//                menu::offset_x = 82.0f;
//                menu::offset_y = 2.0f + 23.0f;
//
//                if (menu::ButtonTab2(L"Main", fvector2d(40, 23), sub_tab == 1)) sub_tab = 1;
//                menu::SameLine();
//                if (menu::ButtonTab2(L"Skins", fvector2d(40, 23), sub_tab == 2)) sub_tab = 2;
//                menu::SameLine();
//                if (menu::ButtonTab2(L"AA", fvector2d(25, 23), sub_tab == 3)) sub_tab = 3;
//
//               
//                if (sub_tab == 1)
//                {
//                    menu::offset_x = 92;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Movement", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Bunny Hop", &globals::misc::bhop);
//                    menu::Checkbox(L"Fast Crouch", &globals::misc::fastcrouch);
//                    menu::Checkbox(L"Anti Flash", &globals::misc::antiflash);
//                    menu::Checkbox(L"Third Person", &globals::misc::tperson);
//                    menu::SliderFloat(L"TP Distance", &globals::misc::PlayerDistance, 100, 1000, "%.0f");
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"View", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"FOV Changer", &globals::misc::fovchanger);
//                    menu::SliderFloat(L"FOV", &globals::misc::fovchangur, 0, 170, "%.0f");
//                    menu::Checkbox(L"Aspect Ratio", &globals::misc::aspectratio);
//                    menu::Checkbox(L"View Model", &globals::misc::ViewModelChanger);
//                    if (menu::Button(L"Skip Tutorial")) globals::misc::sk1ptut0rial = true;
//                    if (menu::Button(L"Unlock ALL"))    globals::misc::sk1n_chang3r = true;
//                }
//
//                else if (tab == 3)
//                {
//                    static int sub_tab = 1;
//
//                    menu::offset_x = 82.0f;
//                    menu::offset_y = 2.0f + 23.0f;
//
//                    if (menu::ButtonTab2(L"Main", fvector2d(40, 23), sub_tab == 1)) sub_tab = 1;
//                    menu::SameLine();
//                    if (menu::ButtonTab2(L"Skins", fvector2d(40, 23), sub_tab == 2)) sub_tab = 2;
//                    menu::SameLine();
//                    if (menu::ButtonTab2(L"AA", fvector2d(25, 23), sub_tab == 3)) sub_tab = 3;
//
//                    // ══ MAIN ══════════════════════════════════════════════════
//                    if (sub_tab == 1)
//                    {
//                        menu::offset_x = 92;
//                        menu::offset_y = 60;
//                        menu::SectionWrapper(L"Movement", fvector2d(335, 430));
//                        menu::offset_y -= 11;
//
//                        menu::Checkbox(L"Bunny Hop", &globals::misc::bhop);
//                        menu::Checkbox(L"Fast Crouch", &globals::misc::fastcrouch);
//                        menu::Checkbox(L"Anti Flash", &globals::misc::antiflash);
//                        menu::Checkbox(L"Third Person", &globals::misc::tperson);
//                        menu::SliderFloat(L"TP Distance", &globals::misc::PlayerDistance, 100, 1000, "%.0f");
//
//                        menu::offset_x = 250;
//                        menu::offset_y = 60;
//                        menu::SectionWrapper(L"View", fvector2d(335, 430));
//                        menu::offset_y -= 11;
//
//                        menu::Checkbox(L"FOV Changer", &globals::misc::fovchanger);
//                        menu::SliderFloat(L"FOV", &globals::misc::fovchangur, 0, 170, "%.0f");
//                        menu::Checkbox(L"Aspect Ratio", &globals::misc::aspectratio);
//                        menu::Checkbox(L"View Model", &globals::misc::ViewModelChanger);
//                        if (menu::Button(L"Skip Tutorial")) globals::misc::sk1ptut0rial = true;
//                        if (menu::Button(L"Unlock ALL"))    globals::misc::sk1n_chang3r = true;
//                    }
//
//                    else if (sub_tab == 2)
//                    {
//                        menu::offset_x = 92;
//                        menu::offset_y = 60;
//                        menu::SectionWrapper(L"Skins", fvector2d(335, 430));
//                        menu::offset_y -= 11;
//
//                        menu::Checkbox(L"Custom Skin", &globals::misc::custom_vandal_enabled);
//                        globals::misc::custom_text_enabled = globals::misc::custom_vandal_enabled;
//                        menu::Checkbox(L"Custom Text", &globals::misc::custom_text_enabled);
//
//                        menu::offset_y -= 8;
//                        menu::SliderFloat(L"Sc X", &globals::misc::vandal_scale_x, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sc Y", &globals::misc::vandal_scale_y, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sc Z", &globals::misc::vandal_scale_z, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//
//                        menu::SliderFloat(L"Fr Sc X", &globals::misc::frenzy_scale_x, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//                        menu::SliderFloat(L"Fr Sc Y", &globals::misc::frenzy_scale_y, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//                        menu::SliderFloat(L"Fr Sc Z", &globals::misc::frenzy_scale_z, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//
//                        menu::SliderFloat(L"Gh Sc X", &globals::misc::ghost_scale_x, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//                        menu::SliderFloat(L"Gh Sc Y", &globals::misc::ghost_scale_y, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//                        menu::SliderFloat(L"Gh Sc Z", &globals::misc::ghost_scale_z, 0.1f, 5.0f, "%.2f");
//                        menu::offset_y -= 16;
//
//                        menu::SliderFloat(L"Sp Rot P", &globals::misc::spectre_rot_pitch, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Rot Y", &globals::misc::spectre_rot_yaw, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Rot R", &globals::misc::spectre_rot_roll, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Pos X", &globals::misc::spectre_pos_x, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Pos Y", &globals::misc::spectre_pos_y, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Pos Z", &globals::misc::spectre_pos_z, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//
//
//
//                        if (menu::Button(L"Save All")) SaveVandalTextTransforms();
//
//                        menu::offset_x = 250;
//                        menu::offset_y = 60;
//                        menu::SectionWrapper(L"Text Transform", fvector2d(335, 430));
//                        menu::offset_y -= 11;
//
//                        menu::SliderFloat(L"V Pos X", &globals::misc::text_pos_x, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Pos Y", &globals::misc::text_pos_y, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Pos Z", &globals::misc::text_pos_z, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Rot P", &globals::misc::text_rot_pitch, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Rot Y", &globals::misc::text_rot_yaw, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Rot R", &globals::misc::text_rot_roll, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Sc X", &globals::misc::text_scale_x, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Sc Y", &globals::misc::text_scale_y, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"V Sc Z", &globals::misc::text_scale_z, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//
//                        menu::SliderFloat(L"F Pos X", &globals::misc::frenzy_text_pos_x, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Pos Y", &globals::misc::frenzy_text_pos_y, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Pos Z", &globals::misc::frenzy_text_pos_z, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Rot P", &globals::misc::frenzy_text_rot_pitch, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Rot Y", &globals::misc::frenzy_text_rot_yaw, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Rot R", &globals::misc::frenzy_text_rot_roll, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Sc X", &globals::misc::frenzy_text_scale_x, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Sc Y", &globals::misc::frenzy_text_scale_y, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"F Sc Z", &globals::misc::frenzy_text_scale_z, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//
//                        menu::SliderFloat(L"G Pos X", &globals::misc::ghost_text_pos_x, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Pos Y", &globals::misc::ghost_text_pos_y, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Pos Z", &globals::misc::ghost_text_pos_z, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Rot P", &globals::misc::ghost_text_rot_pitch, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Rot Y", &globals::misc::ghost_text_rot_yaw, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Rot R", &globals::misc::ghost_text_rot_roll, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Sc X", &globals::misc::ghost_text_scale_x, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Sc Y", &globals::misc::ghost_text_scale_y, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"G Sc Z", &globals::misc::ghost_text_scale_z, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//
//                        menu::SliderFloat(L"Ph Pos X", &globals::misc::phantom_text_pos_x, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Pos Y", &globals::misc::phantom_text_pos_y, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Pos Z", &globals::misc::phantom_text_pos_z, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Rot P", &globals::misc::phantom_text_rot_pitch, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Rot Y", &globals::misc::phantom_text_rot_yaw, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Rot R", &globals::misc::phantom_text_rot_roll, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Sc X", &globals::misc::phantom_text_scale_x, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Sc Y", &globals::misc::phantom_text_scale_y, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Ph Sc Z", &globals::misc::phantom_text_scale_z, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//
//
//                        menu::SliderFloat(L"Sp Pos X", &globals::misc::spectre_text_pos_x, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Pos Y", &globals::misc::spectre_text_pos_y, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Pos Z", &globals::misc::spectre_text_pos_z, -50.0f, 50.0f, "%.3f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Rot P", &globals::misc::spectre_text_rot_pitch, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Rot Y", &globals::misc::spectre_text_rot_yaw, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Rot R", &globals::misc::spectre_text_rot_roll, -180.0f, 180.0f, "%.1f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Sc X", &globals::misc::spectre_text_scale_x, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Sc Y", &globals::misc::spectre_text_scale_y, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                        menu::SliderFloat(L"Sp Sc Z", &globals::misc::spectre_text_scale_z, 0.1f, 5.0f, "%.2f"); menu::offset_y -= 16;
//                    }
//                    // ══ ANTI-AIM ══════════════════════════════════════════════
//                    else if (sub_tab == 3)
//                    {
//                        menu::offset_x = 92;
//                        menu::offset_y = 60;
//                        menu::SectionWrapper(L"Anti-Aim", fvector2d(335, 430));
//                        menu::offset_y -= 11;
//
//                        menu::Checkbox(L"Enable (F1)", &globals::misc::SpinBot);
//                        menu::Combobox(fvector2d(180, 23), &globals::misc::pitch_mode,
//                            L"None", L"Up", L"Down", L"Custom", NULL);
//                        menu::Checkbox(L"Spin", &globals::misc::aa_spin);
//                        menu::Checkbox(L"Jitter", &globals::misc::aa_jitter);
//                        menu::Checkbox(L"Desync", &globals::misc::aa_desync);
//                        menu::Checkbox(L"Backwards", &globals::misc::aa_backwards);
//                        menu::Checkbox(L"Atomic AA", &globals::misc::aa_atomic);
//                        menu::SliderFloat(L"Spin Speed", &globals::misc::spinvalue, 1.0f, 50.0f, "%.1f");
//                        menu::offset_y -= 16;
//                        menu::SliderFloat(L"Yaw Offset", &globals::misc::yaw_add, -180.0f, 180.0f, "%.1f");
//
//                        menu::offset_x = 250;
//                        menu::offset_y = 60;
//                        menu::SectionWrapper(L"Snap Keys", fvector2d(335, 430));
//                        menu::offset_y -= 11;
//
//                        menu::Checkbox(L"Three-Way", &globals::misc::aa_threeway);
//                        menu::Checkbox(L"Pred Breaker", &globals::misc::aa_prediction_breaker);
//                        menu::Hotkey("Left", fvector2d(9, 18), &globals::misc::snap_left_key);
//                        menu::Hotkey("Right", fvector2d(9, 18), &globals::misc::snap_right_key);
//                        menu::Hotkey("Back", fvector2d(9, 18), &globals::misc::snap_back_key);
//                    }
//                }
//
//
//                // ══ ANTI-AIM ════════════════════════════════════════════════
//                else if (sub_tab == 3)
//                {
//                    menu::offset_x = 92;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Anti-Aim", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Enable (F1)", &globals::misc::SpinBot);
//                    menu::Combobox(fvector2d(180, 23), &globals::misc::pitch_mode,
//                        L"None", L"Up", L"Down", L"Custom", NULL);
//                    menu::Checkbox(L"Spin", &globals::misc::aa_spin);
//                    menu::Checkbox(L"Jitter", &globals::misc::aa_jitter);
//                    menu::Checkbox(L"Desync", &globals::misc::aa_desync);
//                    menu::Checkbox(L"Backwards", &globals::misc::aa_backwards);
//                    menu::Checkbox(L"Atomic AA", &globals::misc::aa_atomic);
//                    menu::SliderFloat(L"Spin Speed", &globals::misc::spinvalue, 1.0f, 50.0f, "%.1f");
//                    menu::offset_y -= 16;
//                    menu::SliderFloat(L"Yaw Offset", &globals::misc::yaw_add, -180.0f, 180.0f, "%.1f");
//
//                    menu::offset_x = 250;
//                    menu::offset_y = 60;
//                    menu::SectionWrapper(L"Snap Keys", fvector2d(335, 430));
//                    menu::offset_y -= 11;
//
//                    menu::Checkbox(L"Three-Way", &globals::misc::aa_threeway);
//                    menu::Checkbox(L"Pred Breaker", &globals::misc::aa_prediction_breaker);
//                    menu::Hotkey("Left", fvector2d(9, 18), &globals::misc::snap_left_key);
//                    menu::Hotkey("Right", fvector2d(9, 18), &globals::misc::snap_right_key);
//                    menu::Hotkey("Back", fvector2d(9, 18), &globals::misc::snap_back_key);
//                }
//            }
//
//           
//      else if (tab == 4)
//      {
//          static int sub_tab = 1;
//          menu::offset_x = 82.0f;
//          menu::offset_y = 2.0f + 23.0f;
//          if (menu::ButtonTab2(L"Config", fvector2d(45, 23), sub_tab == 1)) sub_tab = 1;
//          menu::SameLine();
//          if (menu::ButtonTab2(L"Sounds", fvector2d(50, 23), sub_tab == 2)) sub_tab = 2;
//          menu::SameLine();
//          if (menu::ButtonTab2(L"Misc", fvector2d(40, 23), sub_tab == 3)) sub_tab = 3;
//
//          // ── Config ────────────────────────────────────────────────────────
//          if (sub_tab == 1)
//          {
//              menu::offset_x = 92;
//              menu::offset_y = 60;
//              menu::SectionWrapper(L"Config", fvector2d(335, 430));
//              menu::offset_y -= 11;
//              if (menu::Button(L"Save Config")) {
//                  std::filesystem::path configDir = "C:/MagicBullet";
//                  std::filesystem::path configPath = configDir / "config.json";
//                  if (!std::filesystem::exists(configDir))
//                      std::filesystem::create_directories(configDir);
//                  Config->SaveSettings(configPath.string());
//              }
//              if (menu::Button(L"Load Config"))
//                  Config->LoadSettings("C:/MagicBullet/config.json");
//              if (menu::Button(L"Discord"))
//                  system("start https://discord.gg/magicbullet");
//              menu::Checkbox(L"Watermark", &globals::Watermark);
//
//              menu::offset_x = 250;
//              menu::offset_y = 60;
//              menu::SectionWrapper(L"Gun Buddy", fvector2d(335, 430));
//              menu::offset_y -= 11;
//              menu::Checkbox(L"Gun Buddy", &globals::buddy::enabled);
//              menu::SliderInt(L"Buddy ID", &globals::buddy::index, 0, 658, "%d");
//          }
//
//          // ── Sounds ────────────────────────────────────────────────────────
//          else if (sub_tab == 2)
//          {
//              menu::offset_x = 92;
//              menu::offset_y = 60;
//              menu::SectionWrapper(L"Kill Sound", fvector2d(335, 430));
//              menu::offset_y -= 11;
//              menu::Checkbox(L"Kill Sound", &globals::misc::killsound);
//
//              menu::offset_x = 250;
//              menu::offset_y = 60;
//              menu::SectionWrapper(L"Kill Say", fvector2d(335, 430));
//              menu::offset_y -= 11;
//              menu::Checkbox(L"Kill Say", &globals::misc::killsays);
//
//             
//              menu::InputField(L"Chat message", &globals::misc::chat_message, 256);
//          }
//
//         
//          else if (sub_tab == 3)
//          {
//              menu::offset_x = 92;
//              menu::offset_y = 60;
//              menu::SectionWrapper(L"Finisher", fvector2d(335, 430));
//              menu::offset_y -= 11;
//              menu::Checkbox(L"Finisher", &globals::misc::finisher);
//              menu::Checkbox(L"Last Kill Only", &globals::misc::only_last_kill);
//  
//          }
//}
//            fvector2d cursorPos = menu::CursorPos();
//            menu::drawFilledRect(fvector2d(cursorPos.x - 2, cursorPos.y - 2), 4, 4, flinearcolor(1, 1, 1, 1));
//        }
//
//        menu::Render();
//    }
//}

namespace burat {

    fvector2d pos = { 960 - (780 / 2), 540 - (520 / 2) };
    bool menu_open = true;
    static flinearcolor fovcolor = { 255.0f, 255.0f, 255.0f, 1.0f };

    void Menu(ucanvas* canvas)
    {
        menu::SetupCanvas(canvas);
        menu::input::handle();

        if (GetAsyncKeyState(VK_INSERT) & 1) menu_open = !menu_open;

        if (menu::Window(L"Magicbullet", &pos, fvector2d(780, 520), menu_open))
        {
            // Dile gore tab isimleri
            bool TR = (menu::g_lang == 0);

            // ── DİKEY SIDEBAR TAB SİSTEMİ ─────────────────────────────────
            // Sol kenardan 2px içeride, header'ların altında başlar
            // Sidebar genişliği 80px, her tab 40px yükseklik
            static int tab = 0;
            {
                const wchar_t* tab_names[] = {
                    TR ? L"Nisan" : L"Aimbot",
                    TR ? L"Gorsel" : L"Visuals",
                    TR ? L"Renk" : L"Chams",
                    TR ? L"Diger" : L"Misc",
                    TR ? L"Ayar" : L"Settings",
                    TR ? L"Spam" : L"Spam",
                };
                int n = 6;
                float sw = 82.0f;   // sidebar genişliği
                float th = 66.0f;   // tab yüksekliği (520-52-2)/6 ~ 77, biraz daha sık
                float sx = pos.x + 2.0f;
                float sy = pos.y + 52.0f; // iki header sonrası

                int clicked = menu::DrawVerticalTabs(tab_names, n, tab, sx, sy, sw, th, pos);
                if (clicked >= 0) tab = clicked;

                // Content area: sidebar'ın sağından başlar
                menu::offset_x = 82.0f + 10.0f; // sidebar + boşluk
            }

            // ════════════════════════════════════════════════════
            // TAB 0 — NİSAN / AIMBOT
            // ════════════════════════════════════════════════════
            if (tab == 0)
            {
                static int sub_tab = 1;
                menu::offset_x = 82.0f;
                menu::offset_y = 2.0f + 23.0f;

                if (menu::ButtonTab2(TR ? L"Ana" : L"Main", fvector2d(35, 23), sub_tab == 1)) sub_tab = 1;
                menu::SameLine();
                if (menu::ButtonTab2(TR ? L"Oto Atis" : L"Auto Shot", fvector2d(65, 23), sub_tab == 2)) sub_tab = 2;

                if (sub_tab == 1)
                {
                    menu::offset_x = 92;
                    menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Nisan Botu" : L"Aimbot", fvector2d(335, 430));
                    menu::offset_y -= 13;

                    menu::Checkbox(TR ? L"Aktif" : L"Enabled", &globals::aimbot::a1mbot);
                    menu::SameLine();
                    menu::Hotkey(TR ? "Tus" : "Key", fvector2d(9, 18), &globals::aimbot::a1m_k3y);
                    menu::Checkbox(TR ? L"Sessiz Nisan" : L"Silent Aim", &globals::aimbot::silent);
                    menu::Checkbox(TR ? L"Yayilma Yok" : L"Nospread", &globals::aimbot::nospread);
                    menu::Checkbox(TR ? L"Duvar Delme" : L"Autowall", &globals::aimbot::auto_wall);
                    menu::Checkbox(TR ? L"Gorunurluk" : L"Visible Check", &globals::aimbot::v1sh_ch3ck);
                    menu::Checkbox(TR ? L"Tepki Kontrolu" : L"Recoil Control", &globals::aimbot::reco1l_contr0l);
                    menu::Checkbox(TR ? L"FOV Goster" : L"Draw FOV", &globals::aimbot::draw_f0v);
                    menu::Checkbox(TR ? L"360 FOV" : L"360 FOV", &globals::aimbot::enable_360_fov);

                    if (globals::aimbot::nospread)
                        globals::aimbot::reco1l_contr0l = false;

                    menu::offset_x = 440;
                    menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Ayarlar" : L"Settings", fvector2d(335, 430));
                    menu::offset_y -= 13;

                    menu::SliderFloat(L"FOV", &globals::aimbot::a1m_f0v, 5.0f, 2000.0f, "%.1f");
                    menu::offset_y -= 16;
                    menu::SliderFloat(TR ? L"Yumusatma" : L"Smooth", &globals::aimbot::a1m_sm00th, 1.0f, 20.0f, "%.1f");
                    menu::Combobox(fvector2d(180, 23), &globals::aimbot::a1m_b0ne,
                        TR ? L"Kafa" : L"Head",
                        TR ? L"Boyun" : L"Neck",
                        TR ? L"Gogus" : L"Chest",
                        NULL);
                }
                else if (sub_tab == 2)
                {
                    menu::offset_x = 92;
                    menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Otomatik Atis" : L"Auto Shot", fvector2d(335, 430));
                    menu::offset_y -= 13;

                    menu::Checkbox(TR ? L"Oto Atis" : L"Auto Shot", &globals::aimbot::auto_shot);
                    menu::Checkbox(TR ? L"Tus Basili Tut" : L"Hold Aim Key", &globals::aimbot::auto_shot_hold_key);

                    menu::offset_x = 440;
                    menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Secenekler" : L"Options", fvector2d(335, 430));
                    menu::offset_y -= 13;

                    menu::Checkbox(TR ? L"Tahmin" : L"Prediction", &globals::aimbot::prediction);
                }
            }

            // ════════════════════════════════════════════════════
            // TAB 1 — GÖRSEL / VISUALS
            // ════════════════════════════════════════════════════
            else if (tab == 1)
            {
                static int sub_tab = 1;
                menu::offset_x = 82.0f;
                menu::offset_y = 2.0f + 23.0f;

                if (menu::ButtonTab2(TR ? L"ESP" : L"ESP", fvector2d(35, 23), sub_tab == 1)) sub_tab = 1;
                menu::SameLine();
                if (menu::ButtonTab2(TR ? L"Dunya" : L"World", fvector2d(45, 23), sub_tab == 2)) sub_tab = 2;
                menu::SameLine();
                if (menu::ButtonTab2(TR ? L"Materyel" : L"Materials", fvector2d(65, 23), sub_tab == 3)) sub_tab = 3;

                if (sub_tab == 1)
                {
                    menu::offset_x = 92;
                    menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Oyuncu ESP" : L"Player ESP", fvector2d(335, 430));
                    menu::offset_y -= 13;

                    menu::Checkbox(TR ? L"2D Kutu" : L"2D Box", &globals::visuals::box2d);
                    menu::Checkbox(TR ? L"3D Kutu" : L"3D Box", &globals::visuals::box3d);
                    menu::Checkbox(TR ? L"Kose Kutu" : L"Corner Box", &globals::visuals::corner);
                    menu::Checkbox(TR ? L"Gorunurluk" : L"Vis Check", &globals::visuals::vischeck);
                    menu::Checkbox(TR ? L"Kafa ESP" : L"Head ESP", &globals::visuals::headb0x);
                    menu::Checkbox(TR ? L"Saglik Bari" : L"Health Bar", &globals::visuals::h3althbar);
                    menu::Checkbox(TR ? L"Iskelet" : L"Skeleton", &globals::visuals::sk3let0n);
                    menu::Checkbox(TR ? L"Snap Cizgi" : L"Snapline", &globals::visuals::snapl1ne);
                    menu::Checkbox(TR ? L"Su ESP" : L"Water ESP", &globals::visuals::w4ter_esp);

                    if (globals::visuals::corner) { globals::visuals::box2d = false; globals::visuals::box3d = false; }
                    if (globals::visuals::box3d) { globals::visuals::corner = false; globals::visuals::box2d = false; }
                    if (globals::visuals::box2d) { globals::visuals::corner = false; globals::visuals::box3d = false; }

                    menu::offset_x = 440;
                    menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Bilgi ESP" : L"Info ESP", fvector2d(325, 175));
                    menu::offset_y -= 13;

                    menu::Checkbox(TR ? L"Karakter Ikonu" : L"Agent Icon", &globals::visuals::b00ms);
                    menu::Checkbox(TR ? L"Silah Bilgisi" : L"Weapon Info", &globals::visuals::b11ms);
                    menu::Checkbox(TR ? L"Mesafe" : L"Distance", &globals::visuals::dstc);
                    menu::Checkbox(TR ? L"Spike Bilgisi" : L"Spike Info", &globals::visuals::spike);
                    menu::Checkbox(TR ? L"Sapka" : L"China Hat", &globals::visuals::chinahat);
                    menu::Checkbox(TR ? L"Kendi Sapkasi" : L"China Hat Self", &globals::visuals::partyhat_self);
                    menu::Checkbox(TR ? L"Izleyenler" : L"Spectators", &globals::visuals::show_spectators);

                    menu::offset_x = 440;
                    menu::offset_y = 215;
                    menu::SectionWrapper(TR ? L"Tel Kafes" : L"Wireframe", fvector2d(325, 215));
                    menu::offset_y -= 13;

                    menu::Checkbox(TR ? L"Dusmanim Wire" : L"Enemy Wire", &globals::misc::Wireframe);
                    menu::Checkbox(TR ? L"Kendi Wire" : L"Self Wire", &globals::misc::self_wireframe);
                    menu::Checkbox(TR ? L"El Wire" : L"Hand Wire", &globals::misc::HandWire);
                    menu::Checkbox(TR ? L"Silah Wire" : L"Gun Wire", &globals::misc::WireframeGun);
                    menu::Checkbox(TR ? L"3P Silah Wire" : L"Gun 3P Wire", &globals::misc::gun_3p_wireframe);
                    menu::Checkbox(TR ? L"Mermi Izi" : L"Bullet Tracer", &globals::misc::bullet_tracers);

                    // ── ESP PREVIEW ───────────────────────────────────────
                    {
                        float pvX = menu::menu_pos.x + 92.0f + 335.0f + 6.0f;
                        float pvY = menu::menu_pos.y + 52.0f;
                        float pvW = 780.0f - 92.0f - 335.0f - 8.0f - 2.0f;
                        float pvH = 520.0f - 52.0f - 25.0f - 2.0f;
                        menu::DrawESPPreview(pvX, pvY, pvW, pvH,
                            globals::visuals::box2d,
                            globals::visuals::box3d,
                            globals::visuals::corner,
                            globals::visuals::sk3let0n,
                            globals::visuals::snapl1ne,
                            globals::visuals::h3althbar,
                            globals::visuals::headb0x,
                            globals::visuals::vischeck,
                            globals::visuals::w4ter_esp);
                    }
                }
                else if (sub_tab == 2)
                {
                    menu::offset_x = 92;
                    menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Gokyuzu" : L"Skybox", fvector2d(335, 430));
                    menu::offset_y -= 13;

                    menu::Checkbox(TR ? L"Duman Yok" : L"No Smoke", &globals::misc::no_smoke);
                    menu::Checkbox(TR ? L"Gokyuzu" : L"Skybox", &globals::misc::skybox);
                    menu::Checkbox(TR ? L"Gokyuzu RGB" : L"Skybox RGB", &globals::misc::skyboxrgb);

                    menu::SliderFloat(TR ? L"Yildizlar" : L"Stars", &globals::misc::StarsBrightness, 0.f, 2500.f, "%.0f"); menu::offset_y -= 16;
                    menu::SliderFloat(TR ? L"Bulut Hizi" : L"Cloud Speed", &globals::misc::CloudSpeed, 0.f, 50.f, "%.1f");        menu::offset_y -= 16;
                    menu::SliderFloat(TR ? L"Bulut Say." : L"Cloud Opacity", &globals::misc::CloudOpacity, 0.f, 10000.f, "%.0f");   menu::offset_y -= 16;
                    menu::SliderFloat(L"Sky R", &globals::misc::SkySharedR, 0.f, 1.f, "%.2f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Sky G", &globals::misc::SkySharedG, 0.f, 1.f, "%.2f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Sky B", &globals::misc::SkySharedB, 0.f, 1.f, "%.2f");

                    menu::offset_x = 440;
                    menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Sis" : L"Fog", fvector2d(335, 430));
                    menu::offset_y -= 13;

                    menu::Checkbox(TR ? L"Sis" : L"Fog", &globals::misc::Fog);
                    menu::Checkbox(TR ? L"Sis RGB" : L"Fog RGB", &globals::misc::FogRGB);
                    menu::Checkbox(TR ? L"Hacimsel Sis" : L"Volumetric Fog", &globals::misc::bEnableVolumetricFog);

                    menu::SliderFloat(TR ? L"Yogunluk" : L"Density", &globals::misc::FogDensity, 0.f, 1.f, "%.2f");         menu::offset_y -= 16;
                    menu::SliderFloat(TR ? L"Dusum" : L"Falloff", &globals::misc::FogHeightFalloff, 0.f, 1.f, "%.2f");   menu::offset_y -= 16;
                    menu::SliderFloat(TR ? L"Opasite" : L"Opacity", &globals::misc::FogMaxOpacity, 0.f, 1.f, "%.2f");      menu::offset_y -= 16;
                    menu::SliderFloat(TR ? L"Basl. Mes." : L"Start Dist", &globals::misc::FogStartDistance, 0.f, 50000.f, "%.0f"); menu::offset_y -= 16;
                    menu::SliderFloat(TR ? L"Kesim" : L"Cutoff", &globals::misc::FogCutoffDistance, 0.f, 50000.f, "%.0f"); menu::offset_y -= 16;
                    menu::SliderFloat(TR ? L"Hac. Mes." : L"Vol Dist", &globals::misc::VolumetricFogDistance, 0.f, 20000.f, "%.0f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Sis R", &globals::misc::FogColor.r, 0.f, 1.f, "%.2f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Sis G", &globals::misc::FogColor.g, 0.f, 1.f, "%.2f"); menu::offset_y -= 16;
                    menu::SliderFloat(L"Sis B", &globals::misc::FogColor.b, 0.f, 1.f, "%.2f");
                }
                else if (sub_tab == 3)
                {
                    menu::offset_x = 92;
                    menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Materyeller" : L"Materials", fvector2d(335, 430));
                    menu::offset_y -= 13;

                    menu::Checkbox(TR ? L"El Materyeli" : L"Material Hand", &globals::visuals::hand_with_material);
                    menu::Checkbox(TR ? L"1P Silah Mat." : L"Material 1P Gun", &globals::visuals::gunmaterial1p);
                    menu::Checkbox(TR ? L"3P Silah Mat." : L"Material 3P Gun", &globals::visuals::gunmaterial3p);
                    menu::Checkbox(TR ? L"3P Deri Mat." : L"Material Skin 3P", &globals::misc::playerchamsself);
                    menu::Combobox(fvector2d(180, 23), &globals::visuals::typehand,
                        TR ? L"Siyah" : L"Black", L"Reyna",
                        TR ? L"Yesil" : L"Green",
                        TR ? L"Cam" : L"Glass",
                        TR ? L"Pembe" : L"Pink",
                        TR ? L"Sari" : L"Yellow",
                        TR ? L"Kirmizi" : L"Red",
                        TR ? L"Mavi" : L"Blue", NULL);

                    menu::offset_x = 440;
                    menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Silah Tipleri" : L"Gun Types", fvector2d(335, 430));
                    menu::offset_y -= 13;

                    menu::Combobox(fvector2d(180, 23), &globals::visuals::typegun1p,
                        TR ? L"Cam" : L"Glass", TR ? L"Kirmizi" : L"Red",
                        TR ? L"Mavi" : L"Blue", TR ? L"Sari" : L"Yellow", NULL);
                    menu::Combobox(fvector2d(180, 23), &globals::visuals::typegun3d,
                        TR ? L"Cam" : L"Glass", TR ? L"Kirmizi" : L"Red",
                        TR ? L"Mavi" : L"Blue", TR ? L"Sari" : L"Yellow", NULL);
                    menu::Combobox(fvector2d(180, 23), &globals::misc::chams_material_index,
                        L"Ninja", L"Cyber", L"Soviet", L"Sea", NULL);
                }
            }

            // ════════════════════════════════════════════════════
            // TAB 2 — RENK / CHAMS
            // ════════════════════════════════════════════════════
            else if (tab == 2)
            {
                static int sub_tab = 1;
                menu::offset_x = 82.0f;
                menu::offset_y = 2.0f + 23.0f;

                if (menu::ButtonTab2(TR ? L"Kontur" : L"Outline", fvector2d(50, 23), sub_tab == 1)) sub_tab = 1;
                menu::SameLine();
                if (menu::ButtonTab2(TR ? L"Parlama" : L"Glow", fvector2d(50, 23), sub_tab == 2)) sub_tab = 2;
                menu::SameLine();
                if (menu::ButtonTab2(TR ? L"Galaksi" : L"Galaxy", fvector2d(50, 23), sub_tab == 3)) sub_tab = 3;

                if (sub_tab == 1)
                {
                    menu::offset_x = 92;
                    menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Kontur Chams" : L"Outline Chams", fvector2d(335, 430));
                    menu::offset_y -= 13;

                    menu::Checkbox(TR ? L"Kontur Renk" : L"Outline Chams", &globals::chams::outline_enabled);
                    menu::Checkbox(TR ? L"El Kontur" : L"Hand Outline", &globals::chams::hand_outline_enabled);
                    menu::Checkbox(TR ? L"Kendi Rengi" : L"Self Chams", &globals::chams::self_chams);
                    menu::Checkbox(TR ? L"1P Silah Renk" : L"Gun 1P Chams", &globals::chams::gun_outline1P_enabled);
                    menu::Checkbox(TR ? L"3P Silah Renk" : L"Gun 3P Chams", &globals::chams::gun_outline3P_enabled);
                    menu::SliderFloat(TR ? L"Gorunen Yog." : L"Vis Intensity", &globals::chams::intensityvisibleoutline, 10, 100, "%.0f"); menu::offset_y -= 16;
                    menu::SliderFloat(TR ? L"Gizli Yog." : L"Invis Intensity", &globals::chams::intensityinvisbleoutline, 10, 100, "%.0f");

                    menu::offset_x = 440;
                    menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Onayarlar" : L"Presets", fvector2d(335, 430));
                    menu::offset_y -= 13;

                    menu::Combobox(fvector2d(180, 23), &globals::chams::outlinetype,
                        TR ? L"Her Zaman" : L"Always", TR ? L"Arkada" : L"Behind", NULL);
                    menu::Combobox(fvector2d(180, 23), &globals::chams::visible_outline_preset,
                        L"Galaxy", TR ? L"Mavi" : L"Blue", TR ? L"Yesil" : L"Green",
                        TR ? L"Turuncu" : L"Orange", TR ? L"Pembe" : L"Pink", TR ? L"Beyaz" : L"White", NULL);
                    menu::Combobox(fvector2d(180, 23), &globals::chams::invisible_outline_preset,
                        TR ? L"Kirmizi" : L"Red", TR ? L"Turuncu" : L"Orange",
                        TR ? L"Sari" : L"Yellow", TR ? L"Yesil" : L"Green",
                        TR ? L"Pembe" : L"Pink", TR ? L"Gri" : L"Gray", NULL);
                }
                else if (sub_tab == 2)
                {
                    menu::offset_x = 92;
                    menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Gorunen Renk" : L"Visible Chams", fvector2d(335, 430));
                    menu::offset_y -= 13;
                    menu::Checkbox(TR ? L"Gorunen Renk" : L"Visible Chams", &globals::chams::rchamsespall);
                    menu::SliderFloat(TR ? L"Yogunluk" : L"Intensity", &globals::chams::Glow, 0.1f, 10.0f, "%.1f");
                    menu::offset_x = 440; menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Gizli Renk" : L"Invisible Chams", fvector2d(335, 430));
                    menu::offset_y -= 13;
                    menu::Checkbox(TR ? L"Gizli Renk" : L"Invisible Chams", &globals::chams::rchamsesp);
                    menu::SliderFloat(TR ? L"Yogunluk" : L"Intensity", &globals::chams::Glowvni, 0.1f, 10.0f, "%.1f");
                }
                else if (sub_tab == 3)
                {
                    menu::offset_x = 92; menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Galaksi Renk" : L"Galaxy Chams", fvector2d(335, 430));
                    menu::offset_y -= 13;
                    menu::Checkbox(TR ? L"El Galaksi" : L"Hand Galaxy", &globals::chams::hand_galaxy_enabled);
                    menu::Checkbox(TR ? L"1P Silah Galaksi" : L"Gun 1P Galaxy", &globals::chams::gun1p_galaxy_enabled);
                    menu::Checkbox(TR ? L"3P Silah Galaksi" : L"Gun 3P Galaxy", &globals::chams::gun3p_galaxy_enabled);
                    menu::Checkbox(TR ? L"Kendi Galaksi" : L"Self Galaxy", &globals::chams::self_galaxy_enabled);
                    menu::Checkbox(TR ? L"Dusman Galaksi" : L"Enemy Galaxy", &globals::chams::enemy_galaxy_enabled);
                    menu::offset_x = 440; menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Kristal Onayar" : L"Crystal Preset", fvector2d(335, 430));
                    menu::offset_y -= 13;
                    menu::Combobox(fvector2d(180, 23), &globals::visuals::crystal_chams_preset,
                        TR ? L"Ozel" : L"Custom", TR ? L"Kirmizi" : L"Red", TR ? L"Yesil" : L"Green",
                        TR ? L"Mavi" : L"Blue", TR ? L"Turuncu" : L"Orange",
                        TR ? L"Pembe" : L"Pink", TR ? L"Mor" : L"Purple", NULL);
                }
            }

            // ════════════════════════════════════════════════════
            // TAB 3 — DIGER / MISC
            // ════════════════════════════════════════════════════
            else if (tab == 3)
            {
                static int sub_tab = 1;
                menu::offset_x = 82.0f;
                menu::offset_y = 2.0f + 23.0f;

                if (menu::ButtonTab2(TR ? L"Ana" : L"Main", fvector2d(35, 23), sub_tab == 1)) sub_tab = 1;
                menu::SameLine();
                if (menu::ButtonTab2(L"AA", fvector2d(25, 23), sub_tab == 2)) sub_tab = 2;
                menu::SameLine();
                if (menu::ButtonTab2(TR ? L"Konfig" : L"Config", fvector2d(45, 23), sub_tab == 3)) sub_tab = 3;

                // ── Ana ──────────────────────────────────────────────────
                if (sub_tab == 1)
                {
                    menu::offset_x = 92; menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Oyuncu" : L"Player", fvector2d(335, 430));
                    menu::offset_y -= 13;
                    menu::Checkbox(TR ? L"Bunny Hop" : L"Bunny Hop", &globals::misc::bhop);
                    menu::Checkbox(TR ? L"Hizli Cok" : L"Fast Crouch", &globals::misc::fastcrouch);
                    menu::Checkbox(TR ? L"Flash Engel" : L"Anti Flash", &globals::misc::antiflash);
                    menu::Checkbox(TR ? L"Ucuncu Sahis" : L"Third Person", &globals::misc::tperson);
                    menu::SliderFloat(TR ? L"3S Mesafesi" : L"TP Distance", &globals::misc::PlayerDistance, 100, 1000, "%.0f");
                    menu::offset_y -= 8;
                    menu::Checkbox(TR ? L"FOV Degistir" : L"FOV Changer", &globals::misc::fovchanger);
                    menu::SliderFloat(L"FOV", &globals::misc::fovchangur, 0, 170, "%.0f");
                    menu::Checkbox(TR ? L"En-Boy Orani" : L"Aspect Ratio", &globals::misc::aspectratio);
                    menu::Checkbox(TR ? L"El Modeli" : L"View Model", &globals::misc::ViewModelChanger);
                    menu::Checkbox(TR ? L"Filigran" : L"Watermark", &globals::Watermark);
                    menu::offset_y += 5;
                    menu::Checkbox(TR ? L"Hiza Yardimcisi" : L"Lineup Helper", &globals::lineup::enabled);
                    if (globals::lineup::enabled) {
                        menu::Checkbox(TR ? L"Rehber Goster" : L"Show Guides", &globals::lineup::show_guides);
                        menu::Checkbox(TR ? L"Oto Nisan" : L"Auto Aim", &globals::lineup::auto_aim);
                        menu::offset_y -= 8;
                        menu::SliderFloat(TR ? L"Hiz" : L"Velocity", &globals::lineup::projectile_velocity, 1000.0f, 5000.0f, "%.0f"); menu::offset_y -= 16;
                        menu::SliderFloat(TR ? L"Yerc.Kuvv." : L"Gravity", &globals::lineup::gravity_scale, 0.1f, 5.0f, "%.1f");             menu::offset_y -= 16;
                        menu::SliderFloat(TR ? L"Render Mes." : L"Render Dist.", &globals::lineup::render_distance, 100.0f, 10000.0f, "%.0f");
                    }

                    // Ping System
                    menu::offset_y += 5;
                    menu::SectionWrapper(TR ? L"Harita Isaretleme" : L"Map Pinging", fvector2d(335, 260));
                    menu::offset_y -= 13;
                    menu::Hotkey(TR ? "Manuel Isaret Tusu" : "Manual Ping Key", fvector2d(9, 18), &globals::ping::manual_ping_key);
                    menu::Checkbox(TR ? L"Oto Isaretleme" : L"Auto Ping", &globals::ping::auto_ping_enabled);
                    menu::Hotkey(TR ? "Oto Degistir Tusu" : "Auto Toggle Key", fvector2d(9, 18), &globals::ping::auto_ping_key);
                    menu::SliderInt(TR ? L"Max Grup Sayisi" : L"Max Per Burst", &globals::ping::max_per_burst, 1, 5, "%d");
                    menu::SliderInt(TR ? L"Isaret Araligi" : L"Ping Gap (ms)", &globals::ping::ping_gap, 50, 1000, "%d");
                    menu::SliderInt(TR ? L"Bekleme Suresi" : L"Cooldown (ms)", &globals::ping::cooldown, 1000, 30000, "%d");
                    menu::SliderInt(TR ? L"Tekrar Isaretleme" : L"Auto Re-ping (ms)", &globals::ping::auto_reping, 5000, 60000, "%d");
                    menu::offset_x = 440; menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Ekstralar" : L"Extras", fvector2d(335, 430));
                    menu::offset_y -= 13;
                    menu::Checkbox(TR ? L"Oto Cikis" : L"Auto Peek", &globals::autopeek::enabled);
                    if (globals::autopeek::enabled) {
                        menu::Hotkey(TR ? "Cikis Tusu" : "Peek Key", fvector2d(9, 18), &globals::autopeek::peek_key);
                        menu::Checkbox(TR ? L"Isaret Goster" : L"Draw Marker", &globals::autopeek::draw_position);
                        menu::offset_y += 5;
                    }
                    menu::Checkbox(TR ? L"Ozel Skin" : L"Custom Skin", &globals::misc::custom_vandal_enabled);
                    if (globals::misc::custom_vandal_enabled) { globals::misc::custom_text_enabled = true; globals::misc::ViewModelChanger = true; }
                    menu::Checkbox(TR ? L"Bitirici" : L"Finisher", &globals::misc::finisher);
                    menu::Checkbox(TR ? L"Son Oldurme" : L"Last Kill Only", &globals::misc::only_last_kill);
                    menu::Checkbox(TR ? L"Hellfire" : L"Hellfire", &globals::misc::hellfiremode);
                    menu::Checkbox(TR ? L"Yildirim" : L"Lightning", &globals::misc::lightningmode);
                    menu::Checkbox(TR ? L"Silah Arkadas" : L"Gun Buddy", &globals::buddy::enabled);
                    menu::SliderInt(TR ? L"Arkadas ID" : L"Buddy ID", &globals::buddy::index, 0, 658, "%d");
                    menu::offset_y -= 8;
                    if (menu::Button(TR ? L"Egitimi Atla" : L"Skip Tutorial")) globals::misc::sk1ptut0rial = true;
                    if (menu::Button(TR ? L"Tum Skinleri Ac" : L"Unlock ALL"))    globals::misc::sk1n_chang3r = true;

                    // MFA Bypass
                    menu::Checkbox(TR ? L"MFA Bypass" : L"MFA Bypass", &globals::misc::mfa_bypass_enabled);
                    if (menu::Button(TR ? L"MFA Bypass Calistir" : L"Run MFA Bypass")) {
                        mfa::on_msg = [](int type, const char* msg) {
                            // Burada MFA bypass mesajlarını işleyebilirsiniz
                            // type: 1=ok, 2=fail, 3=info
                            // msg: mesaj metni
                            };
                        mfa::RequestRun();
                    }
                }

                // ── AA ────────────────────────────────────────────────────
                else if (sub_tab == 2)
                {
                    menu::offset_x = 92; menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Anti-Aim" : L"Anti-Aim", fvector2d(335, 430));
                    menu::offset_y -= 13;
                    menu::Checkbox(TR ? L"Aktif (F1)" : L"Enable (F1)", &globals::misc::SpinBot);
                    menu::Combobox(fvector2d(180, 23), &globals::misc::pitch_mode,
                        TR ? L"Yok" : L"None", TR ? L"Yukari" : L"Up",
                        TR ? L"Asagi" : L"Down", TR ? L"Ozel" : L"Custom", NULL);
                    menu::Checkbox(TR ? L"Spin" : L"Spin", &globals::misc::aa_spin);
                    menu::Checkbox(TR ? L"Titreme" : L"Jitter", &globals::misc::aa_jitter);
                    menu::Checkbox(TR ? L"Desync" : L"Desync", &globals::misc::aa_desync);
                    menu::Checkbox(TR ? L"Geri Don" : L"Backwards", &globals::misc::aa_backwards);
                    menu::Checkbox(TR ? L"Atomik AA" : L"Atomic AA", &globals::misc::aa_atomic);
                    menu::SliderFloat(TR ? L"Spin Hizi" : L"Spin Speed", &globals::misc::spinvalue, 1.0f, 50.0f, "%.1f"); menu::offset_y -= 16;
                    menu::SliderFloat(TR ? L"Yaw Offset" : L"Yaw Offset", &globals::misc::yaw_add, -180.0f, 180.0f, "%.1f");
                    menu::offset_x = 440; menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Snap Tuslari" : L"Snap Keys", fvector2d(335, 430));
                    menu::offset_y -= 13;
                    menu::Checkbox(TR ? L"Uc Yonlu" : L"Three-Way", &globals::misc::aa_threeway);
                    menu::Checkbox(TR ? L"Tahmin Kirici" : L"Pred Breaker", &globals::misc::aa_prediction_breaker);
                    menu::HotkeyLabeled(TR ? "Sol Snap" : "Snap Left", &globals::misc::snap_left_key);
                    menu::HotkeyLabeled(TR ? "Sag Snap" : "Snap Right", &globals::misc::snap_right_key);
                    menu::HotkeyLabeled(TR ? "Geri Snap" : "Snap Back", &globals::misc::snap_back_key);
                }

                // ── Konfig / Config ───────────────────────────────────────
                else if (sub_tab == 3)
                {
                    // Config listesi ilk acilista tara
                    static bool cfg_scanned = false;
                    if (!cfg_scanned) { ScanConfigs(); cfg_scanned = true; }

                    // Sol panel: Config yönetimi
                    menu::offset_x = 92; menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Konfigurasyon" : L"Config Manager", fvector2d(335, 430));

                    // DrawConfigPanel: menu_pos.x + panel_x = sol section'ın içi
                    // offset_x=92 → section x = menu_pos.x+92
                    // section iç padding: +8 sol, içerik y: menu_pos.y+87
                    DrawConfigPanel(TR,
                        /*panel_x*/ 100.0f,   // menu_pos.x + 100  (section x=92 + 8px padding)
                        /*panel_y*/ 87.0f,    // menu_pos.y + 87   (header 60 + SectionWrapper 27)
                        /*panel_w*/ 319.0f,   // section genisligi 335 - 16px kenar
                        /*panel_h*/ 406.0f);  // section yuksekligi 430 - 24px

                    // Sag panel: Genel ayarlar
                    menu::offset_x = 440; menu::offset_y = 60;
                    menu::SectionWrapper(TR ? L"Genel" : L"General", fvector2d(325, 175));
                    menu::offset_y -= 13;
                    menu::Checkbox(TR ? L"Filigran" : L"Watermark", &globals::Watermark);
                    menu::Checkbox(TR ? L"Izleyenler" : L"Show Spectators", &globals::visuals::show_spectators);

                    menu::offset_x = 440; menu::offset_y = 215;
                    menu::SectionWrapper(TR ? L"Hizli Kaydet" : L"Quick Save", fvector2d(325, 130));
                    menu::offset_y -= 13;
                    if (menu::Button(TR ? L"Varsayilana Kaydet" : L"Save Default")) {
                        std::filesystem::create_directories("C:/MagicBullet");
                        Config->SaveSettings("C:/MagicBullet/default.json");
                        ScanConfigs();
                        cfg_scanned = false;
                    }
                    if (menu::Button(TR ? L"Varsayilani Yukle" : L"Load Default")) {
                        if (std::filesystem::exists("C:/MagicBullet/default.json"))
                            Config->LoadSettings("C:/MagicBullet/default.json");
                    }
                }
            }

            // ════════════════════════════════════════════════════
            // TAB 4 — MENU AYARLARI / MENU SETTINGS
            // ════════════════════════════════════════════════════
            else if (tab == 4)
            {
                menu::offset_x = 92; menu::offset_y = 60;
                menu::SectionWrapper(TR ? L"Tema" : L"Theme", fvector2d(335, 430));
                menu::offset_y -= 13;

                static bool t_siyah = (menu::g_guiTheme == 0);
                static bool t_beyaz = (menu::g_guiTheme == 1);
                bool ps = t_siyah, pb = t_beyaz;
                menu::Checkbox(TR ? L"Siyah Tema" : L"Dark Theme", &t_siyah);
                if (t_siyah && !ps) { menu::g_guiTheme = 0; t_beyaz = false; }
                menu::Checkbox(TR ? L"Beyaz Tema" : L"White Theme", &t_beyaz);
                if (t_beyaz && !pb) { menu::g_guiTheme = 1; t_siyah = false; }
                t_siyah = (menu::g_guiTheme == 0);
                t_beyaz = (menu::g_guiTheme == 1);

                if (menu::g_guiTheme == 1) {
                    static bool v_acik = (menu::g_guiVariant == 0);
                    static bool v_sik = (menu::g_guiVariant == 1);
                    bool pa = v_acik, pk = v_sik;
                    menu::Checkbox(TR ? L"Acik Beyaz" : L"Light White", &v_acik);
                    if (v_acik && !pa) { menu::g_guiVariant = 0; v_sik = false; }
                    menu::Checkbox(TR ? L"Sik (Koyu) Beyaz" : L"Dense White", &v_sik);
                    if (v_sik && !pk) { menu::g_guiVariant = 1; v_acik = false; }
                    v_acik = (menu::g_guiVariant == 0);
                    v_sik = (menu::g_guiVariant == 1);
                }

                menu::offset_x = 440; menu::offset_y = 60;
                menu::SectionWrapper(TR ? L"Dil" : L"Language", fvector2d(335, 430));
                menu::offset_y -= 13;

                static bool l_tr = (menu::g_lang == 0);
                static bool l_en = (menu::g_lang == 1);
                bool plt = l_tr, ple = l_en;
                menu::Checkbox(L"Turkce", &l_tr);
                if (l_tr && !plt) { menu::g_lang = 0; l_en = false; }
                menu::Checkbox(L"English", &l_en);
                if (l_en && !ple) { menu::g_lang = 1; l_tr = false; }
                l_tr = (menu::g_lang == 0);
                l_en = (menu::g_lang == 1);

                menu::offset_y += 8;
                menu::Checkbox(TR ? L"Filigran" : L"Watermark", &globals::Watermark);
            }

            // ════════════════════════════════════════════════════
            // TAB 5 — CHAT SPAMMER
            // ════════════════════════════════════════════════════
            else if (tab == 5)
            {
                // ── Sol panel: Spam Ayarları ──────────────────────────────
                menu::offset_x = 92; menu::offset_y = 60;
                menu::SectionWrapper(TR ? L"Spam Ayarlari" : L"Spam Settings", fvector2d(335, 215));
                menu::offset_y -= 13;

                menu::Checkbox(TR ? L"Spam Aktif" : L"Spam Enabled", &globals::misc::chat_spammer);

                // Mod: Otomatik zamanlayıcı mı, tuş mu?
                menu::Checkbox(TR ? L"Otomatik Mod" : L"Auto Mode", &globals::misc::spam_auto);

                // Otomatik moddaysa interval göster
                if (globals::misc::spam_auto) {
                    menu::offset_y -= 8;
                    menu::SliderInt(TR ? L"Aralik (ms)" : L"Interval (ms)",
                        &globals::misc::spam_interval, 2000, 15000, "%d");
                    // 2000ms altı crash/ban riski — slider min 2000ms
                }

                // Tuş modu — her zaman göster
                menu::SameLine();
                menu::Hotkey(TR ? "Spam Tus" : "Spam Key",
                    fvector2d(9, 18), &globals::misc::spam_key);

                menu::Checkbox(TR ? L"Kill Say" : L"Kill Say", &globals::misc::killsays);
                menu::Checkbox(TR ? L"Kill Sound" : L"Kill Sound", &globals::misc::killsound);

                menu::offset_y += 4;
                menu::InputField(TR ? L"Mesaj" : L"Message",
                    &globals::misc::chat_message, 256);

                // ── Sol panel: FPS Boost ──────────────────────────────────
                menu::offset_x = 92; menu::offset_y = 290;
                menu::SectionWrapper(TR ? L"FPS Arttirici" : L"FPS Booster", fvector2d(335, 200));
                menu::offset_y -= 13;

                menu::Checkbox(TR ? L"FPS Boost Aktif" : L"FPS Boost", &globals::misc::fps_boost);

                if (globals::misc::fps_boost) {
                    menu::offset_y -= 4;
                    menu::Combobox(fvector2d(200, 23), &globals::misc::fps_boost_level,
                        TR ? L"Dusuk  (Normal+)" : L"Low  (Normal+)",
                        TR ? L"Orta   (Yuksek)" : L"Mid  (High)",
                        TR ? L"Maks   (Realtime)" : L"Max  (Realtime)",
                        NULL);
                    menu::offset_y += 4;

                    // Aktif seviyeyi renkli göster
                    static const wchar_t* level_descs_tr[] = {
                        L"Hafif hiz artisi, guvenli",
                        L"Belirgin etki, tavsiye edilen",
                        L"Maksimum — diger uygulamalar yavaslayabilir"
                    };
                    static const wchar_t* level_descs_en[] = {
                        L"Slight boost, safe",
                        L"Noticeable gain, recommended",
                        L"Maximum — other apps may slow down"
                    };
                    int lvl = globals::misc::fps_boost_level;
                    if (lvl < 0) lvl = 0;
                    if (lvl > 2) lvl = 2;

                    // Açıklama satırı — sadece drawtext ile, element olmadan
                    flinearcolor ac = menu::GetBlinkAccentColor();
                    fvector2d dp = {
                        menu::menu_pos.x + menu::offset_x + 13,
                        menu::menu_pos.y + menu::offset_y + 10
                    };
                    menu::canvas->k2_drawtext(menu::font,
                        TR ? level_descs_tr[lvl] : level_descs_en[lvl],
                        dp, { 0.76f, 0.70f }, ac, 0.f,
                        menu::RGBtoFLC(0, 0, 0), { 0,0 }, false, false, false, menu::RGBtoFLC(0, 0, 0));
                    menu::offset_y += 16;
                }

                // ── Sağ panel: Bilgi + Sıfırla ───────────────────────────
                menu::offset_x = 440; menu::offset_y = 60;
                menu::SectionWrapper(TR ? L"Bilgi" : L"Info", fvector2d(325, 215));
                menu::offset_y -= 13;

                if (menu::Button(TR ? L"Varsayilana Sifirla" : L"Reset to Default"))
                    globals::misc::chat_message = "https://discord.gg/4mGZBD2nc";

                if (menu::Button(TR ? L"Spami Durdur" : L"Stop Spam")) {
                    globals::misc::chat_spammer = false;
                    globals::misc::spam_auto = false;
                }

                // Durum göstergesi
                {
                    fvector2d sp = {
                        menu::menu_pos.x + 440 + 13,
                        menu::menu_pos.y + menu::offset_y + 8
                    };
                    bool active = globals::misc::chat_spammer;
                    flinearcolor sc = active
                        ? flinearcolor{ 0.10f,0.85f,0.20f,1.f }
                    : flinearcolor{ 0.60f,0.60f,0.60f,1.f };
                    menu::drawFilledRect({ sp.x, sp.y + 3 }, 6, 6, sc);
                    menu::canvas->k2_drawtext(menu::font,
                        active ? (TR ? L"Spam calisiyor" : L"Spam running")
                        : (TR ? L"Spam durdu" : L"Spam stopped"),
                        { sp.x + 10, sp.y }, { 0.80f, 0.74f }, sc, 0.f,
                        menu::RGBtoFLC(0, 0, 0), { 0,0 }, false, false, false, menu::RGBtoFLC(0, 0, 0));
                    menu::offset_y += 20;
                }

                // ── Sağ panel: FPS Boost Bilgi ────────────────────────────
                menu::offset_x = 440; menu::offset_y = 290;
                menu::SectionWrapper(TR ? L"FPS Bilgi" : L"FPS Info", fvector2d(325, 200));
                menu::offset_y -= 13;

                {
                    bool factive = globals::misc::fps_boost;
                    flinearcolor fc = factive
                        ? flinearcolor{ 0.10f,0.85f,0.20f,1.f }
                    : flinearcolor{ 0.60f,0.60f,0.60f,1.f };

                    fvector2d fp2 = {
                        menu::menu_pos.x + 440 + 13,
                        menu::menu_pos.y + menu::offset_y + 8
                    };
                    menu::drawFilledRect({ fp2.x, fp2.y + 3 }, 6, 6, fc);
                    menu::canvas->k2_drawtext(menu::font,
                        factive ? (TR ? L"Boost aktif" : L"Boost active")
                        : (TR ? L"Boost kapali" : L"Boost off"),
                        { fp2.x + 10, fp2.y }, { 0.80f, 0.74f }, fc, 0.f,
                        menu::RGBtoFLC(0, 0, 0), { 0,0 }, false, false, false, menu::RGBtoFLC(0, 0, 0));
                    menu::offset_y += 22;

                    // Uyarı notu
                    fvector2d np = {
                        menu::menu_pos.x + 440 + 13,
                        menu::menu_pos.y + menu::offset_y + 6
                    };
                    menu::canvas->k2_drawtext(menu::font,
                        TR ? L"Not: Maks seviye diger" : L"Note: Max level may",
                        np, { 0.72f, 0.66f }, menu::ThemeSubText(), 0.f,
                        menu::RGBtoFLC(0, 0, 0), { 0,0 }, false, false, false, menu::RGBtoFLC(0, 0, 0));
                    np.y += 13;
                    menu::canvas->k2_drawtext(menu::font,
                        TR ? L"uygulamalari yavaslatir." : L"slow other apps.",
                        np, { 0.72f, 0.66f }, menu::ThemeSubText(), 0.f,
                        menu::RGBtoFLC(0, 0, 0), { 0,0 }, false, false, false, menu::RGBtoFLC(0, 0, 0));
                }
            }

            fvector2d cursorPos = menu::CursorPos();
            if (menu::g_cursorDot)
                menu::drawFilledRect(fvector2d(cursorPos.x - 2, cursorPos.y - 2), 4, 4, flinearcolor(1, 1, 1, 1));
        }

        // Overlay: dil secilmemisse menunun uzerini kaart
        menu::DrawBlurOverlay();

        // Spectator listesi (her zaman gorunur, show_spectators aktifse)
        menu::DrawSpectatorList();

        menu::GUISettingsWindow();
        menu::Render();

        // WelcomeWindow EN USTTE — her seyin uzerinde cizilir
        menu::WelcomeWindow();
    }
}
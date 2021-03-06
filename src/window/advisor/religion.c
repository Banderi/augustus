#include <scenario/building.h>
#include <building/menu.h>
#include "religion.h"

#include "building/count.h"
#include "city/gods.h"
#include "city/houses.h"
#include "city/festival.h"
#include "game/settings.h"
#include "graphics/generic_button.h"
#include "graphics/image.h"
#include "graphics/lang_text.h"
#include "graphics/panel.h"
#include "graphics/text.h"
#include "window/hold_festival.h"

static void button_hold_festival(int param1, int param2);

static generic_button hold_festival_button[] = {
        {102, 280 + 68, 300, 20, button_hold_festival, button_none, 0, 0},
};

static int focus_button_id;

static int get_religion_advice(void) {
    int least_happy = city_god_least_happy();
    const house_demands *demands = city_houses_demands();
    if (least_happy >= 0 && city_god_wrath_bolts(least_happy) > 4)
        return 6 + least_happy;
    else if (demands->religion == 1)
        return demands->requiring.religion ? 1 : 0;
    else if (demands->religion == 2)
        return 2;
    else if (demands->religion == 3)
        return 3;
    else if (!demands->requiring.religion)
        return 4;
    else if (least_happy >= 0)
        return 6 + least_happy;
    else
        return 5;
}
static int get_festival_advice(void) {
    int months_since_festival = city_festival_months_since_last();
    if (months_since_festival <= 1)
        return 0;
    else if (months_since_festival <= 6)
        return 1;
    else if (months_since_festival <= 12)
        return 2;
    else if (months_since_festival <= 18)
        return 3;
    else if (months_since_festival <= 24)
        return 4;
    else if (months_since_festival <= 30)
        return 5;
    else {
        return 6;
    }
}

static void draw_festival_info(int y_offset) {
    inner_panel_draw(48, 252 + y_offset, 34, 6);
    image_draw(image_id_from_group(GROUP_PANEL_WINDOWS) + 15, 460, 255 + y_offset);
//    lang_text_draw(58, 17, 52, 224 + y_offset, FONT_LARGE_BLACK);

    int width = lang_text_draw_amount(8, 4, city_festival_months_since_last(), 112, 260 + y_offset, FONT_NORMAL_WHITE);
    lang_text_draw(58, 15, 112 + width, 260 + y_offset, FONT_NORMAL_WHITE);
    if (city_festival_is_planned())
        lang_text_draw_centered(58, 34, 102, 284 + y_offset, 300, FONT_NORMAL_WHITE);
    else
        lang_text_draw_centered(58, 16, 102, 284 + y_offset, 300, FONT_NORMAL_WHITE);
    lang_text_draw_multiline(58, 18 + get_festival_advice(), 56, 305 + y_offset, 400, FONT_NORMAL_WHITE);
}
static void draw_god_row(int god, int y_offset, int small_temple, int large_temple, int shrine) {
//    lang_text_draw(59, 11 + god, 40, y_offset, FONT_NORMAL_WHITE);
//    lang_text_draw(59, 16 + god, 120, y_offset + 1, FONT_SMALL_PLAIN);

    int is_known = is_god_known(god);
    font_t font = FONT_NORMAL_WHITE;
    if (is_known == GOD_STATUS_UNKNOWN)
        font = FONT_NORMAL_RED;
//    else if (is_known == GOD_STATUS_PATRON)
//        font = FONT_NORMAL_RED;

    lang_text_draw(157, god, 40, y_offset, font); // god name
    lang_text_draw(187, is_known, 100, y_offset, font); // unknown/known deity?
    lang_text_draw(158, god, 40, y_offset + 20, FONT_NORMAL_GREEN); // god description

    int width = 0;
    if (is_known == GOD_STATUS_UNKNOWN) {
        lang_text_draw_centered(59, 37, 200, y_offset, 50, font);
        lang_text_draw_centered(59, 37, 265, y_offset, 50, font);
        lang_text_draw_centered(59, 37, 330, y_offset, 50, font);
        lang_text_draw_centered(59, 37, 390, y_offset, 50, font);
        lang_text_draw(59, 37, 460, y_offset, font);
    }
    else {
        if (is_building_enabled(BUILDING_TEMPLE_COMPLEX_OSIRIS + god))
            text_draw_number_centered(building_count_total(large_temple), 200, y_offset, 50, font);
        else
            lang_text_draw_centered(59, 37, 200, y_offset, 50, font);
        text_draw_number_centered(building_count_total(small_temple), 265, y_offset, 50, font);
        text_draw_number_centered(building_count_total(shrine), 330, y_offset, 50, font);
        text_draw_number_centered(city_god_months_since_festival(god), 390, y_offset, 50, font);
        width = lang_text_draw(59, 20 + city_god_happiness(god) / 10, 460, y_offset, font); //32
    }

    int bolts_ount = city_god_wrath_bolts(god);
    for (int i = 0; i < bolts_ount / 10; i++)
        image_draw(image_id_from_group(GROUP_GOD_BOLT), 10 * i + width + 460, y_offset - 4);
}

static int draw_background(void) {
    int height_blocks;
    if (setting_gods_enabled()) {
        height_blocks = 27; //17
        outer_panel_draw(0, 0, 40, height_blocks);
    } else {
        height_blocks = 27; //20
        outer_panel_draw(0, 0, 40, height_blocks);
        lang_text_draw_multiline(59, 43, 60, 256, 520, FONT_NORMAL_BLACK);
    }

    image_draw(image_id_from_group(GROUP_ADVISOR_ICONS) + 9, 10, 10);

    lang_text_draw(59, 0, 60, 12, FONT_LARGE_BLACK);

    // table header
    lang_text_draw(59, 5, 180, 32, FONT_NORMAL_BLACK); // temple
    lang_text_draw(59, 2, 170, 46, FONT_NORMAL_BLACK); // complexes
    lang_text_draw(59, 1, 250, 46, FONT_NORMAL_BLACK); // tempes
    lang_text_draw(28, 150, 320, 46, FONT_NORMAL_BLACK); // shrines
    lang_text_draw(59, 6, 390, 18, FONT_NORMAL_BLACK); // months
    lang_text_draw(59, 8, 400, 32, FONT_NORMAL_BLACK); // since
    lang_text_draw(59, 7, 390, 46, FONT_NORMAL_BLACK); // festival
    lang_text_draw(59, 3, 460, 46, FONT_NORMAL_BLACK); // appeasement

    inner_panel_draw(32, 60, 36, 13);

    // todo: god description when clicking on god name
    // god rows
    draw_god_row(GOD_CERES, 66, BUILDING_SMALL_TEMPLE_CERES, BUILDING_LARGE_TEMPLE_CERES, BUILDING_SHRINE_OSIRIS);
    draw_god_row(GOD_NEPTUNE, 106, BUILDING_SMALL_TEMPLE_NEPTUNE, BUILDING_LARGE_TEMPLE_NEPTUNE, BUILDING_SHRINE_RA);
    draw_god_row(GOD_MERCURY, 146, BUILDING_SMALL_TEMPLE_MERCURY, BUILDING_LARGE_TEMPLE_MERCURY, BUILDING_SHRINE_PTAH);
    draw_god_row(GOD_MARS, 186, BUILDING_SMALL_TEMPLE_MARS, BUILDING_LARGE_TEMPLE_MARS, BUILDING_SHRINE_SETH);
    draw_god_row(GOD_VENUS, 226, BUILDING_SMALL_TEMPLE_VENUS, BUILDING_LARGE_TEMPLE_VENUS, BUILDING_SHRINE_BAST);

//    // oracles
//    lang_text_draw(59, 8, 40, 166, FONT_NORMAL_WHITE);
//    text_draw_number_centered(building_count_total(BUILDING_ORACLE), 230, 166, 50, FONT_NORMAL_WHITE);

    city_gods_calculate_least_happy();

    lang_text_draw_multiline(59, 9 + get_religion_advice(), 60, 273, 512, FONT_NORMAL_BLACK); // 21

    draw_festival_info(68);

    return height_blocks;
}
static void draw_foreground(void) {
    if (!city_festival_is_planned())
        button_border_draw(102, 280 + 68, 300, 20, focus_button_id == 1);
}

static void button_hold_festival(int param1, int param2) {
    // todo: need festival square
    if (!city_festival_is_planned())
        window_hold_festival_show();
}

static int handle_mouse(const mouse *m) {
    return generic_buttons_handle_mouse(m, 0, 0, hold_festival_button, 1, &focus_button_id);
}
static int get_tooltip_text(void) {
    if (focus_button_id)
        return 112;
    else {
        return 0;
    }
}

const advisor_window_type *window_advisor_religion(void) {
    static const advisor_window_type window = {
            draw_background,
            draw_foreground,
            handle_mouse,
            get_tooltip_text
    };
    return &window;
}

#include "health.h"
#include "building/count.h"
#include "city/culture.h"
#include "city/population.h"
#include "graphics/image.h"
#include "graphics/lang_text.h"
#include "graphics/panel.h"
#include "graphics/text.h"

#include "Data/CityInfo.h"

#define ADVISOR_HEIGHT 18

static int get_health_advice()
{
    switch (Data_CityInfo.healthDemand) {
        case 1:
            return Data_CityInfo.housesRequiringBathhouse ? 1 : 0;
        case 2:
            return Data_CityInfo.housesRequiringBarber ? 3 : 2;
        case 3:
            return Data_CityInfo.housesRequiringClinic ? 5 : 4;
        case 4:
            return 6;
        default:
            return 7;
    }
}

static int draw_background()
{
    outer_panel_draw(0, 0, 40, ADVISOR_HEIGHT);
    image_draw(image_group(GROUP_ADVISOR_ICONS) + 6, 10, 10);

    lang_text_draw(56, 0, 60, 12, FONT_LARGE_BLACK);
    if (city_population() >= 200) {
        lang_text_draw_multiline(56, Data_CityInfo.healthRate / 10 + 16, 60, 46, 512, FONT_NORMAL_BLACK);
    } else {
        lang_text_draw_multiline(56, 15, 60, 46, 512, FONT_NORMAL_BLACK);
    }
    lang_text_draw(56, 3, 180, 94, FONT_SMALL_PLAIN);
    lang_text_draw(56, 4, 290, 94, FONT_SMALL_PLAIN);
    lang_text_draw(56, 5, 478, 94, FONT_SMALL_PLAIN);

    inner_panel_draw(32, 108, 36, 5);

    // bathhouses
    lang_text_draw_amount(8, 24, building_count_total(BUILDING_BATHHOUSE), 40, 112, FONT_NORMAL_GREEN);
    text_draw_number_centered(building_count_active(BUILDING_BATHHOUSE), 150, 112, 100, FONT_NORMAL_GREEN);
    lang_text_draw(56, 2, 300, 112, FONT_NORMAL_GREEN);
    lang_text_draw(56, 2, 504, 112, FONT_NORMAL_GREEN);

    // barbers
    lang_text_draw_amount(8, 26, building_count_total(BUILDING_BARBER), 40, 132, FONT_NORMAL_GREEN);
    text_draw_number_centered(building_count_active(BUILDING_BARBER), 150, 132, 100, FONT_NORMAL_GREEN);
    lang_text_draw(56, 2, 300, 132, FONT_NORMAL_GREEN);
    lang_text_draw(56, 2, 504, 132, FONT_NORMAL_GREEN);

    // clinics
    lang_text_draw_amount(8, 28, building_count_total(BUILDING_DOCTOR), 40, 152, FONT_NORMAL_GREEN);
    text_draw_number_centered(building_count_active(BUILDING_DOCTOR), 150, 152, 100, FONT_NORMAL_GREEN);
    lang_text_draw(56, 2, 300, 152, FONT_NORMAL_GREEN);
    lang_text_draw(56, 2, 504, 152, FONT_NORMAL_GREEN);

    // hospitals
    lang_text_draw_amount(8, 30, building_count_total(BUILDING_HOSPITAL), 40, 172, FONT_NORMAL_GREEN);
    text_draw_number_centered(building_count_active(BUILDING_HOSPITAL), 150, 172, 100, FONT_NORMAL_GREEN);

    int width = text_draw_number(1000 * building_count_active(BUILDING_HOSPITAL), '@', " ", 280, 172, FONT_NORMAL_GREEN);
    lang_text_draw(56, 6, 280 + width, 172, FONT_NORMAL_GREEN);

    int pct_hospital = city_culture_coverage_hospital();
    if (pct_hospital == 0) {
        lang_text_draw_centered(57, 10, 420, 172, 200, FONT_NORMAL_GREEN);
    } else if (pct_hospital < 100) {
        lang_text_draw_centered(57, pct_hospital / 10 + 11, 420, 172, 200, FONT_NORMAL_GREEN);
    } else {
        lang_text_draw_centered(57, 21, 420, 172, 200, FONT_NORMAL_GREEN);
    }

    lang_text_draw_multiline(56, 7 + get_health_advice(), 60, 194, 512, FONT_NORMAL_BLACK);

    return ADVISOR_HEIGHT;
}

const advisor_window_type *window_advisor_health()
{
    static const advisor_window_type window = {
        draw_background,
        0,
        0,
        0
    };
    return &window;
}

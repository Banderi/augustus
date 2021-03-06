#ifndef EMPIRE_TYPE_H
#define EMPIRE_TYPE_H

enum {
    EMPIRE_OBJECT_ORNAMENT = 0,
    EMPIRE_OBJECT_CITY = 1,
    EMPIRE_OBJECT_TEXT = 2,
    EMPIRE_OBJECT_BATTLE_ICON = 3,
    EMPIRE_OBJECT_LAND_TRADE_ROUTE = 4,
    EMPIRE_OBJECT_SEA_TRADE_ROUTE = 5,
    EMPIRE_OBJECT_ROMAN_ARMY = 6,
    EMPIRE_OBJECT_ENEMY_ARMY = 7,
};

enum {
    EMPIRE_CITY_DISTANT_ROMAN = 0,      EMPIRE_CITY_PH_OURS = 0,
    EMPIRE_CITY_OURS = 1,               EMPIRE_CITY_PH_PHARAOH_TRADE = 1,
    EMPIRE_CITY_TRADE = 2,              EMPIRE_CITY_PH_PHARAOH = 2,
    EMPIRE_CITY_FUTURE_TRADE = 3,       EMPIRE_CITY_PH_EGYPT_TRADE = 3,
    EMPIRE_CITY_DISTANT_FOREIGN = 4,    EMPIRE_CITY_PH_EGYPT = 4,
    EMPIRE_CITY_VULNERABLE_ROMAN = 5,   EMPIRE_CITY_PH_FOREIGN_TRADE = 5,
    EMPIRE_CITY_FUTURE_ROMAN = 6,       EMPIRE_CITY_PH_FOREIGN = 6
};

#endif // EMPIRE_TYPE_H
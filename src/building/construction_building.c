//#include "construction_building.h"
//
//#include "building/building.h"
//#include "building/construction.h"
//#include "building/construction_warning.h"
//#include "building/count.h"
//#include "building/dock.h"
//#include "building/menu.h"
//#include "building/properties.h"
//#include "building/rotation.h"
//#include "building/storage.h"
//#include "city/buildings.h"
//#include "city/view.h"
//#include "city/warning.h"
//#include "core/config.h"
//#include "core/image.h"
//#include "core/mods.h"
//#include "core/random.h"
//#include "figure/formation_legion.h"
//#include "game/undo.h"
//#include "map/building_tiles.h"
//#include "map/grid.h"
//#include "map/orientation.h"
//#include "map/routing.h"
//#include "map/routing_terrain.h"
//#include "map/terrain.h"
//#include "map/tiles.h"
//#include "map/water.h"
//
//#include "industry.h"
//
//const int CROPS_OFFSETS[2] = {5, 6};
//
//static void add_fort(int type, building *fort) {
//    fort->prev_part_building_id = 0;
//    map_building_tiles_add(fort->id, fort->x, fort->y, fort->size, image_id_from_group(GROUP_BUILDING_FORT),
//                           TERRAIN_BUILDING);
//    if (type == BUILDING_FORT_LEGIONARIES)
//        fort->subtype.fort_figure_type = FIGURE_FORT_LEGIONARY;
//    else if (type == BUILDING_FORT_JAVELIN)
//        fort->subtype.fort_figure_type = FIGURE_FORT_JAVELIN;
//    else if (type == BUILDING_FORT_MOUNTED)
//        fort->subtype.fort_figure_type = FIGURE_FORT_MOUNTED;
//
//
//    // create parade ground
//    const int offsets_x[] = {3, -1, -4, 0};
//    const int offsets_y[] = {-1, -4, 0, 3};
//    building *ground = building_create(BUILDING_FORT_GROUND, fort->x + offsets_x[building_rotation_get_rotation()],
//                                       fort->y + offsets_y[building_rotation_get_rotation()]);
//    game_undo_add_building(ground);
//    ground->prev_part_building_id = fort->id;
//    fort->next_part_building_id = ground->id;
//    ground->next_part_building_id = 0;
//    map_building_tiles_add(ground->id, fort->x + offsets_x[building_rotation_get_rotation()],
//                           fort->y + offsets_y[building_rotation_get_rotation()], 4,
//                           image_id_from_group(GROUP_BUILDING_FORT) + 1, TERRAIN_BUILDING);
//
//    fort->formation_id = formation_legion_create_for_fort(fort);
//    ground->formation_id = fort->formation_id;
//}
//static void add_hippodrome(building *b) {
//    int image1 = image_id_from_group(GROUP_BUILDING_HIPPODROME_1);
//    int image2 = image_id_from_group(GROUP_BUILDING_HIPPODROME_2);
//    city_buildings_add_hippodrome();
//
//    building_rotation_force_two_orientations();
//    int orientation = building_rotation_get_building_orientation(building_rotation_get_rotation());
//    building *part1 = b;
//
//    part1->prev_part_building_id = 0;
//    int image_id;
//    switch (orientation) {
//        case DIR_0_TOP_RIGHT:
//            image_id = image2;
//            break;
//        case DIR_2_BOTTOM_RIGHT:
//            image_id = image1 + 4;
//            break;
//        case DIR_4_BOTTOM_LEFT:
//            image_id = image2 + 4;
//            break;
//        case DIR_6_TOP_LEFT:
//            image_id = image1;
//            break;
//        default:
//            return;
//    }
//    map_building_tiles_add(b->id, b->x, b->y, b->size, image_id, TERRAIN_BUILDING);
//    int x_offset, y_offset;
//    building_rotation_get_offset_with_rotation(5, building_rotation_get_rotation(), &x_offset, &y_offset);
//    building *part2 = building_create(BUILDING_HIPPODROME, b->x + x_offset, b->y + y_offset);
//    game_undo_add_building(part2);
//
//    part2->prev_part_building_id = part1->id;
//    part1->next_part_building_id = part2->id;
//    part2->next_part_building_id = 0;
//    switch (orientation) {
//        case DIR_0_TOP_RIGHT:
//        case DIR_4_BOTTOM_LEFT:
//            image_id = image2 + 2;
//            break;
//        case DIR_2_BOTTOM_RIGHT:
//        case DIR_6_TOP_LEFT:
//            image_id = image1 + 2;
//            break;
//    }
//    map_building_tiles_add(part2->id, b->x + x_offset, b->y + y_offset, b->size, image_id, TERRAIN_BUILDING);
//
//    building_rotation_get_offset_with_rotation(10, building_rotation_get_rotation(), &x_offset, &y_offset);
//    building *part3 = building_create(BUILDING_HIPPODROME, b->x + x_offset, b->y + y_offset);
//    game_undo_add_building(part3);
//
//    part3->prev_part_building_id = part2->id;
//    part2->next_part_building_id = part3->id;
//    part3->next_part_building_id = 0;
//    switch (orientation) {
//        case DIR_0_TOP_RIGHT:
//            image_id = image2 + 4;
//            break;
//        case DIR_2_BOTTOM_RIGHT:
//            image_id = image1;
//            break;
//        case DIR_4_BOTTOM_LEFT:
//            image_id = image2;
//            break;
//        case DIR_6_TOP_LEFT:
//            image_id = image1 + 4;
//            break;
//    }
//    map_building_tiles_add(part3->id, b->x + x_offset, b->y + y_offset, b->size, image_id, TERRAIN_BUILDING);
//}
//static building *add_warehouse_space(int x, int y, building *prev) {
//    building *b = building_create(BUILDING_WAREHOUSE_SPACE, x, y);
//    game_undo_add_building(b);
//    b->prev_part_building_id = prev->id;
//    prev->next_part_building_id = b->id;
//    map_building_tiles_add(b->id, x, y, 1,
//                           image_id_from_group(GROUP_BUILDING_WAREHOUSE_STORAGE_EMPTY), TERRAIN_BUILDING);
//    return b;
//}
//static void add_warehouse(building *b) {
//    int x_offset[9] = {0, 0, 1, 1, 0, 2, 1, 2, 2};
//    int y_offset[9] = {0, 1, 0, 1, 2, 0, 2, 1, 2};
//    int corner = building_rotation_get_corner(2 * building_rotation_get_rotation());
//
//    b->storage_id = building_storage_create();
//    if (config_get(CONFIG_GP_CH_WAREHOUSES_DONT_ACCEPT))
//        building_storage_accept_none(b->storage_id);
//
//    b->prev_part_building_id = 0;
//    map_building_tiles_add(b->id, b->x + x_offset[corner], b->y + y_offset[corner], 1,
//                           image_id_from_group(GROUP_BUILDING_WAREHOUSE), TERRAIN_BUILDING);
//
//    building *prev = b;
//    for (int i = 0; i < 9; i++) {
//        if (i == corner) {
//            continue;
//        }
//        prev = add_warehouse_space(b->x + x_offset[i], b->y + y_offset[i], prev);
//    }
//    // adjust BUILDING_WAREHOUSE
//    b->x = b->x + x_offset[corner];
//    b->y = b->y + y_offset[corner];
//    b->grid_offset = map_grid_offset(b->x, b->y);
//    game_undo_adjust_building(b);
//
//    prev->next_part_building_id = 0;
//}
//static void add_building_tiles_image(building *b, int image_id) {
//    map_building_tiles_add(b->id, b->x, b->y, b->size, image_id, TERRAIN_BUILDING);
//}
//static void add_to_map(int type, building *b, int size, int orientation, int waterside_orientation_abs, int waterside_orientation_rel) {
//    switch (type) {
//        // houses
//        case BUILDING_HOUSE_LARGE_TENT:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_TENT) + 2);
//            break;
//        case BUILDING_HOUSE_SMALL_SHACK:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_SHACK));
//            break;
//        case BUILDING_HOUSE_LARGE_SHACK:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_SHACK) + 2);
//            break;
//        case BUILDING_HOUSE_SMALL_HOVEL:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_HOVEL));
//            break;
//        case BUILDING_HOUSE_LARGE_HOVEL:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_HOVEL) + 2);
//            break;
//        case BUILDING_HOUSE_SMALL_CASA:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_CASA));
//            break;
//        case BUILDING_HOUSE_LARGE_CASA:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_CASA) + 2);
//            break;
//        case BUILDING_HOUSE_SMALL_INSULA:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_INSULA_1));
//            break;
//        case BUILDING_HOUSE_MEDIUM_INSULA:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_INSULA_1) + 2);
//            break;
//        case BUILDING_HOUSE_LARGE_INSULA:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_INSULA_2));
//            break;
//        case BUILDING_HOUSE_GRAND_INSULA:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_INSULA_2) + 2);
//            break;
//        case BUILDING_HOUSE_SMALL_VILLA:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_VILLA_1));
//            break;
//        case BUILDING_HOUSE_MEDIUM_VILLA:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_VILLA_1) + 2);
//            break;
//        case BUILDING_HOUSE_LARGE_VILLA:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_VILLA_2));
//            break;
//        case BUILDING_HOUSE_GRAND_VILLA:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_VILLA_2) + 1);
//            break;
//        case BUILDING_HOUSE_SMALL_PALACE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_PALACE_1));
//            break;
//        case BUILDING_HOUSE_MEDIUM_PALACE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_PALACE_1) + 1);
//            break;
//        case BUILDING_HOUSE_LARGE_PALACE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_PALACE_2));
//            break;
//        case BUILDING_HOUSE_LUXURY_PALACE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOUSE_PALACE_2) + 1);
//            break;
//            // entertainment
//        case BUILDING_AMPHITHEATER:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_AMPHITHEATER));
//            break;
//        case BUILDING_THEATER:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_THEATER));
//            break;
//        case BUILDING_COLOSSEUM:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_COLOSSEUM));
//            break;
//        case BUILDING_GLADIATOR_SCHOOL:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_GLADIATOR_SCHOOL));
//            break;
//        case BUILDING_LION_HOUSE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_LION_HOUSE));
//            break;
//        case BUILDING_ACTOR_COLONY:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_ACTOR_COLONY));
//            break;
//        case BUILDING_CHARIOT_MAKER:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_CHARIOT_MAKER));
//            break;
//            // statues
//        case BUILDING_SMALL_STATUE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_STATUE));
//            break;
//        case BUILDING_MEDIUM_STATUE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_STATUE) + 1);
//            break;
//        case BUILDING_LARGE_STATUE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_STATUE) + 2);
//            break;
//            // health
//        case BUILDING_DOCTOR:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_DOCTOR));
//            break;
//        case BUILDING_HOSPITAL:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_HOSPITAL));
//            break;
//        case BUILDING_BATHHOUSE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_BATHHOUSE_NO_WATER));
//            break;
//        case BUILDING_BARBER:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_BARBER));
//            break;
//            // education
//        case BUILDING_SCHOOL:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_SCHOOL));
//            break;
//        case BUILDING_ACADEMY:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_ACADEMY));
//            break;
//        case BUILDING_LIBRARY:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_LIBRARY));
//            break;
//            // security
//        case BUILDING_PREFECTURE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_PREFECTURE));
//            break;
//            // farms
//        case BUILDING_WHEAT_FARM:
//            map_building_tiles_add_farm(b->id, b->x, b->y, 0, 0);
//            break;
//        case BUILDING_VEGETABLE_FARM:
//            map_building_tiles_add_farm(b->id, b->x, b->y, CROPS_OFFSETS[GAME_ENV], 0);
//            break;
//        case BUILDING_FRUIT_FARM:
//            map_building_tiles_add_farm(b->id, b->x, b->y, CROPS_OFFSETS[GAME_ENV] * 2, 0);
//            break;
//        case BUILDING_OLIVE_FARM:
//            map_building_tiles_add_farm(b->id, b->x, b->y, CROPS_OFFSETS[GAME_ENV] * 3, 0);
//            break;
//        case BUILDING_VINES_FARM:
//            map_building_tiles_add_farm(b->id, b->x, b->y, CROPS_OFFSETS[GAME_ENV] * 4, 0);
//            break;
//        case BUILDING_PIG_FARM:
//            map_building_tiles_add_farm(b->id, b->x, b->y, CROPS_OFFSETS[GAME_ENV] * 5, 0);
//            break;
//            // industry
//        case BUILDING_MARBLE_QUARRY:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_MARBLE_QUARRY));
//            break;
//        case BUILDING_IRON_MINE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_IRON_MINE));
//            break;
//        case BUILDING_TIMBER_YARD:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_TIMBER_YARD));
//            break;
//        case BUILDING_CLAY_PIT:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_CLAY_PIT));
//            break;
//            // workshops
//        case BUILDING_WINE_WORKSHOP:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_WINE_WORKSHOP));
//            break;
//        case BUILDING_OIL_WORKSHOP:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_OIL_WORKSHOP));
//            break;
//        case BUILDING_WEAPONS_WORKSHOP:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_WEAPONS_WORKSHOP));
//            break;
//        case BUILDING_FURNITURE_WORKSHOP:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_FURNITURE_WORKSHOP));
//            break;
//        case BUILDING_POTTERY_WORKSHOP:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_POTTERY_WORKSHOP));
//            break;
//            // distribution
//        case BUILDING_GRANARY:
//            b->storage_id = building_storage_create();
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_GRANARY));
//            map_tiles_update_area_roads(b->x, b->y, 5);
//            break;
//        case BUILDING_MARKET:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_MARKET));
//            break;
//            // government
//        case BUILDING_GOVERNORS_HOUSE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_GOVERNORS_HOUSE));
//            break;
//        case BUILDING_GOVERNORS_VILLA:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_GOVERNORS_VILLA));
//            break;
//        case BUILDING_GOVERNORS_PALACE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_GOVERNORS_PALACE));
//            break;
//        case BUILDING_MISSION_POST:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_MISSION_POST));
//            break;
//        case BUILDING_ENGINEERS_POST:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_ENGINEERS_POST));
//            break;
//        case BUILDING_FORUM:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_FORUM));
//            break;
//            // water
//        case BUILDING_FOUNTAIN:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_FOUNTAIN_1));
//            break;
//        case BUILDING_WELL:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_WELL));
//            break;
//            // military
//        case BUILDING_MILITARY_ACADEMY:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_MILITARY_ACADEMY));
//            break;
//            // religion
//        case BUILDING_SMALL_TEMPLE_CERES:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_TEMPLE_CERES));
//            break;
//        case BUILDING_SMALL_TEMPLE_NEPTUNE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_TEMPLE_NEPTUNE));
//            break;
//        case BUILDING_SMALL_TEMPLE_MERCURY:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_TEMPLE_MERCURY));
//            break;
//        case BUILDING_SMALL_TEMPLE_MARS:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_TEMPLE_MARS));
//            break;
//        case BUILDING_SMALL_TEMPLE_VENUS:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_TEMPLE_VENUS));
//            break;
//        case BUILDING_LARGE_TEMPLE_CERES:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_TEMPLE_CERES) + 1);
//            break;
//        case BUILDING_LARGE_TEMPLE_NEPTUNE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_TEMPLE_NEPTUNE) + 1);
//            break;
//        case BUILDING_LARGE_TEMPLE_MERCURY:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_TEMPLE_MERCURY) + 1);
//            break;
//        case BUILDING_LARGE_TEMPLE_MARS:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_TEMPLE_MARS) + 1);
//            break;
//        case BUILDING_LARGE_TEMPLE_VENUS:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_TEMPLE_VENUS) + 1);
//            break;
//        case BUILDING_ORACLE:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_ORACLE));
//            break;
//        case BUILDING_ROADBLOCK:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_ROADBLOCK));
////            add_building_tiles_image(b, mods_get_group_id("Keriew", "Roadblocks"));
//            map_terrain_add_roadblock_road(b->x, b->y, orientation);
//            map_tiles_update_area_roads(b->x, b->y, 5);
//            map_tiles_update_all_plazas();
//            break;
//            // ships
//        case BUILDING_SHIPYARD:
//            b->data.industry.orientation = waterside_orientation_abs;
//            map_water_add_building(b->id, b->x, b->y, 2,
//                                   image_id_from_group(GROUP_BUILDING_SHIPYARD) + waterside_orientation_rel);
//            break;
//        case BUILDING_WHARF:
//            b->data.industry.orientation = waterside_orientation_abs;
//            map_water_add_building(b->id, b->x, b->y, 2,
//                                   image_id_from_group(GROUP_BUILDING_WHARF) + waterside_orientation_rel);
//            break;
//        case BUILDING_DOCK:
//            city_buildings_add_dock();
//            b->data.dock.orientation = waterside_orientation_abs;
//            {
//                int image_id;
//                switch (waterside_orientation_rel) {
//                    case 0:
//                        image_id = image_id_from_group(GROUP_BUILDING_DOCK_1);
//                        break;
//                    case 1:
//                        image_id = image_id_from_group(GROUP_BUILDING_DOCK_2);
//                        break;
//                    case 2:
//                        image_id = image_id_from_group(GROUP_BUILDING_DOCK_3);
//                        break;
//                    default:
//                        image_id = image_id_from_group(GROUP_BUILDING_DOCK_4);
//                        break;
//                }
//                map_water_add_building(b->id, b->x, b->y, size, image_id);
//            }
//            break;
//            // defense
//        case BUILDING_TOWER:
//            map_terrain_remove_with_radius(b->x, b->y, 2, 0, TERRAIN_WALL);
//            map_building_tiles_add(b->id, b->x, b->y, size, image_id_from_group(GROUP_BUILDING_TOWER),
//                                   TERRAIN_BUILDING | TERRAIN_GATEHOUSE);
//            map_tiles_update_area_walls(b->x, b->y, 5);
//            break;
//        case BUILDING_GATEHOUSE_PH:
//        case BUILDING_GATEHOUSE:
//            map_building_tiles_add(b->id, b->x, b->y, size,
//                                   image_id_from_group(GROUP_BUILDING_TOWER) + orientation,
//                                   TERRAIN_BUILDING | TERRAIN_GATEHOUSE);
//            b->subtype.orientation = orientation;
//            map_orientation_update_buildings();
//            map_terrain_add_gatehouse_roads(b->x, b->y, orientation);
//            map_tiles_update_area_roads(b->x, b->y, 5);
//            map_tiles_update_all_plazas();
//            map_tiles_update_area_walls(b->x, b->y, 5);
//            break;
//        case BUILDING_TRIUMPHAL_ARCH:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_TRIUMPHAL_ARCH) + orientation - 1);
//            b->subtype.orientation = orientation;
//            map_orientation_update_buildings();
//            map_terrain_add_triumphal_arch_roads(b->x, b->y, orientation);
//            map_tiles_update_area_roads(b->x, b->y, 5);
//            map_tiles_update_all_plazas();
//            city_buildings_build_triumphal_arch();
//            building_menu_update(BUILDSET_NORMAL);
//            building_construction_clear_type();
//            break;
//        case BUILDING_VILLAGE_PALACE:
//        case BUILDING_TOWN_PALACE:
//        case BUILDING_CITY_PALACE:
//        case BUILDING_SENATE_UPGRADED:
//            switch (type) {
//                case BUILDING_VILLAGE_PALACE:
//                    add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_VILLAGE_PALACE));
//                    break;
//                case GROUP_BUILDING_TOWN_PALACE:
//                    add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_TOWN_PALACE));
//                    break;
//                case GROUP_BUILDING_CITY_PALACE:
//                    add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_CITY_PALACE));
//                    break;
//                case BUILDING_SENATE_UPGRADED:
//                    add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_SENATE));
//                    break;
//            }
//            city_buildings_add_senate(b);
//            break;
//        case BUILDING_BARRACKS:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_BARRACKS));
//            city_buildings_add_barracks(b);
//            break;
//        case BUILDING_WAREHOUSE:
//            add_warehouse(b);
//            break;
//        case BUILDING_HIPPODROME:
//            add_hippodrome(b);
//            break;
//        case BUILDING_FORT_LEGIONARIES:
//        case BUILDING_FORT_JAVELIN:
//        case BUILDING_FORT_MOUNTED:
//            add_fort(type, b);
//            break;
//            // native buildings (unused, I think)
//        case BUILDING_NATIVE_HUT:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_NATIVE) + (random_byte() & 1));
//            break;
//        case BUILDING_NATIVE_MEETING:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_NATIVE) + 2);
//            break;
//        case BUILDING_NATIVE_CROPS:
//            add_building_tiles_image(b, image_id_from_group(GROUP_BUILDING_FARMLAND));
//            break;
//            // distribution center (also unused)
//        case BUILDING_DISTRIBUTION_CENTER_UNUSED:
//            city_buildings_add_distribution_center(b);
//            break;
//        case BUILDING_DRAGGABLE_RESERVOIR:
//        case BUILDING_RESERVOIR:
//            if (GAME_ENV == ENGINE_ENV_PHARAOH) {
//                b->data.industry.orientation = waterside_orientation_abs;
//                map_water_add_building(b->id, b->x, b->y, 2,
//                                       image_id_from_group(GROUP_BUILDING_RESERVOIR) + waterside_orientation_rel);
//                break;
//            }
//        default:
//            auto p = building_properties_for_type(type);
//            add_building_tiles_image(b, image_id_from_group(p->image_group) + p->image_offset);
//            break;
//    }
//    map_routing_update_land();
//    map_routing_update_walls();
//}
//
//
//int building_construction_place_building(int type, int x, int y) {
//    int terrain_mask = TERRAIN_ALL;
//    if (type == BUILDING_GATEHOUSE || type == BUILDING_GATEHOUSE_PH || type == BUILDING_TRIUMPHAL_ARCH ||
//        type == BUILDING_ROADBLOCK)
//        terrain_mask = ~TERRAIN_ROAD;
//    else if (type == BUILDING_TOWER)
//        terrain_mask = ~TERRAIN_WALL;
//    else if (building_is_farm(type))
//        terrain_mask = ~TERRAIN_FLOODPLAIN;
//    int size = building_properties_for_type(type)->size;
//    if (type == BUILDING_WAREHOUSE)
//        size = 3;
//    int building_orientation = 0;
//    if (type == BUILDING_GATEHOUSE)
//        building_orientation = map_orientation_for_gatehouse(x, y);
//    else if (type == BUILDING_TRIUMPHAL_ARCH)
//        building_orientation = map_orientation_for_triumphal_arch(x, y);
//    switch (city_view_orientation()) {
//        case DIR_2_BOTTOM_RIGHT:
//            x = x - size + 1;
//            break;
//        case DIR_4_BOTTOM_LEFT:
//            x = x - size + 1;
//            y = y - size + 1;
//            break;
//        case DIR_6_TOP_LEFT:
//            y = y - size + 1;
//            break;
//    }
//    // extra checks
//    if (type == BUILDING_GATEHOUSE || type == BUILDING_GATEHOUSE_PH) {
//        if (!map_tiles_are_clear(x, y, size, terrain_mask)) {
//            city_warning_show(WARNING_CLEAR_LAND_NEEDED);
//            return 0;
//        }
//        if (!building_orientation) {
//            if (building_rotation_get_road_orientation() == 1)
//                building_orientation = 1;
//            else
//                building_orientation = 2;
//        }
//    }
//    if (type == BUILDING_ROADBLOCK) {
//        if (map_tiles_are_clear(x, y, size, TERRAIN_ROAD))
//            return 0;
//    }
//    if (type == BUILDING_TRIUMPHAL_ARCH) {
//        if (!map_tiles_are_clear(x, y, size, terrain_mask)) {
//            city_warning_show(WARNING_CLEAR_LAND_NEEDED);
//            return 0;
//        }
//        if (!building_orientation) {
//            if (building_rotation_get_road_orientation() == 1)
//                building_orientation = 1;
//            else
//                building_orientation = 3;
//        }
//    }
//    int waterside_orientation_abs = 0, waterside_orientation_rel = 0;
//    if (type == BUILDING_SHIPYARD || type == BUILDING_WHARF ||
//        type == BUILDING_WATER_LIFT && GAME_ENV == ENGINE_ENV_PHARAOH) {
//        if (map_water_determine_orientation_size2(
//                x, y, 0, &waterside_orientation_abs, &waterside_orientation_rel)) {
//            city_warning_show(WARNING_SHORE_NEEDED);
//            return 0;
//        }
//    } else if (type == BUILDING_DOCK) {
//        if (map_water_determine_orientation_size3(
//                x, y, 0, &waterside_orientation_abs, &waterside_orientation_rel)) {
//            city_warning_show(WARNING_SHORE_NEEDED);
//            return 0;
//        }
//        if (!building_dock_is_connected_to_open_water(x, y)) {
//            city_warning_show(WARNING_DOCK_OPEN_WATER_NEEDED);
//            return 0;
//        }
//    } else {
//        if (!map_tiles_are_clear(x, y, size, terrain_mask)) {
//            city_warning_show(WARNING_CLEAR_LAND_NEEDED);
//            return 0;
//        }
//        int warning_id;
//        if (!building_construction_can_place_on_terrain(x, y, &warning_id, size)) {
//            city_warning_show(warning_id);
//            return 0;
//        }
//    }
//    if (building_is_fort(type)) {
//        const int offsets_x[] = {3, -1, -4, 0};
//        const int offsets_y[] = {-1, -4, 0, 3};
//        int orient_index = building_rotation_get_rotation();
//        int x_offset = offsets_x[orient_index];
//        int y_offset = offsets_y[orient_index];
//        if (!map_tiles_are_clear(x + x_offset, y + y_offset, 4, terrain_mask)) {
//            city_warning_show(WARNING_CLEAR_LAND_NEEDED);
//            return 0;
//        }
//        if (formation_get_num_legions_cached() >= formation_get_max_legions()) {
//            city_warning_show(WARNING_MAX_LEGIONS_REACHED);
//            return 0;
//        }
//    }
//    if (type == BUILDING_HIPPODROME) {
//        if (city_buildings_has_hippodrome()) {
//            city_warning_show(WARNING_ONE_BUILDING_OF_TYPE);
//            return 0;
//        }
//        int x_offset_1, y_offset_1;
//        building_rotation_get_offset_with_rotation(5, building_rotation_get_rotation(), &x_offset_1, &y_offset_1);
//        int x_offset_2, y_offset_2;
//        building_rotation_get_offset_with_rotation(10, building_rotation_get_rotation(), &x_offset_2, &y_offset_2);
//        if (!map_tiles_are_clear(x + x_offset_1, y + y_offset_1, 5, terrain_mask) ||
//            !map_tiles_are_clear(x + x_offset_2, y + y_offset_2, 5, terrain_mask)) {
//            city_warning_show(WARNING_CLEAR_LAND_NEEDED);
//            return 0;
//        }
//    }
//    if ((type == BUILDING_SENATE_UPGRADED ||
//        type == BUILDING_VILLAGE_PALACE ||
//        type == BUILDING_TOWN_PALACE ||
//        type == BUILDING_CITY_PALACE) && city_buildings_has_senate()) {
//        city_warning_show(WARNING_ONE_BUILDING_OF_TYPE);
//        return 0;
//    }
//    if (type == BUILDING_BARRACKS && city_buildings_has_barracks() && !config_get(CONFIG_GP_CH_MULTIPLE_BARRACKS)) {
//        city_warning_show(WARNING_ONE_BUILDING_OF_TYPE);
//        return 0;
//    }
//    building_construction_warning_check_all(type, x, y, size);
//
//
//    // phew, checks done!
//    building *b;
//    if (building_is_fort(type))
//        b = building_create(BUILDING_FORT, x, y);
//    else
//        b = building_create(type, x, y);
//    game_undo_add_building(b);
//    if (b->id <= 0)
//        return 0;
//    add_to_map(type, b, size, building_orientation, waterside_orientation_abs, waterside_orientation_rel);
//    return 1;
//}

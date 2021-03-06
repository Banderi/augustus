#include "list.h"

#include <string.h>

#define MAX_SMALL 2500
#define MAX_LARGE 10000
#define MAX_BURNING 2500

static struct {
    struct {
        int size;
        int items[MAX_SMALL];
    } small;
    struct {
        int size;
        int items[MAX_LARGE];
    } large;
    struct {
        int size;
        int items[MAX_BURNING];
        int total;
    } burning;
} data;

void building_list_small_clear(void) {
    data.small.size = 0;
}

void building_list_small_add(int building_id) {
    data.small.items[data.small.size++] = building_id;
    if (data.small.size >= MAX_SMALL)
        data.small.size = MAX_SMALL - 1;

}

int building_list_small_size(void) {
    return data.small.size;
}

const int *building_list_small_items(void) {
    return data.small.items;
}

void building_list_large_clear(int clear_entries) {
    data.large.size = 0;
    if (clear_entries)
        memset(data.large.items, 0, MAX_LARGE * sizeof(int));

}

void building_list_large_add(int building_id) {
    if (data.large.size < MAX_LARGE)
        data.large.items[data.large.size++] = building_id;

}

int building_list_large_size(void) {
    return data.large.size;
}

const int *building_list_large_items(void) {
    return data.large.items;
}

void building_list_burning_clear(void) {
    data.burning.size = 0;
    data.burning.total = 0;
}

void building_list_burning_add(int building_id) {
    data.burning.total++;
    data.burning.items[data.burning.size++] = building_id;
    if (data.burning.size >= MAX_BURNING)
        data.burning.size = MAX_BURNING - 1;

}

int building_list_burning_size(void) {
    return data.burning.size;
}

const int *building_list_burning_items(void) {
    return data.burning.items;
}

void building_list_save_state(buffer *small, buffer *large, buffer *burning, buffer *burning_totals) {
    for (int i = 0; i < MAX_SMALL; i++) {
        small->write_i16(data.small.items[i]);
    }
    for (int i = 0; i < MAX_LARGE; i++) {
        large->write_i16(data.large.items[i]);
    }
    for (int i = 0; i < MAX_BURNING; i++) {
        burning->write_i16(data.burning.items[i]);
    }
    burning_totals->write_i32(data.burning.total);
    burning_totals->write_i32(data.burning.size);
}

void building_list_load_state(buffer *small, buffer *large, buffer *burning, buffer *burning_totals) {
    for (int i = 0; i < MAX_SMALL; i++) {
        if (!small->is_valid(1))
            break;
        data.small.items[i] = small->read_i16();
    }
    for (int i = 0; i < MAX_LARGE; i++) {
        if (!large->is_valid(1))
            break;
        data.large.items[i] = large->read_i16();
    }
    for (int i = 0; i < MAX_BURNING; i++) {
        if (!burning->is_valid(1))
            break;
        data.burning.items[i] = burning->read_i16();
    }
    data.burning.total = burning_totals->read_i32();
    data.burning.size = burning_totals->read_i32();
}

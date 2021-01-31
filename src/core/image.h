#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include "core/encoding.h"
#include "core/buffer.h"
#include "core/image_group.h"
#include "graphics/color.h"

#include <vector>
#include <string>
#include <algorithm>

#define IMAGE_FONT_MULTIBYTE_OFFSET 10000
#define IMAGE_FONT_MULTIBYTE_TRAD_CHINESE_MAX_CHARS 2188
#define IMAGE_FONT_MULTIBYTE_SIMP_CHINESE_MAX_CHARS 2130
#define IMAGE_FONT_MULTIBYTE_KOREAN_MAX_CHARS 2350

enum {
    IMAGE_TYPE_WITH_TRANSPARENCY = 0,
    IMAGE_TYPE_ISOMETRIC = 30,
    IMAGE_TYPE_MOD = 40
};


/**
 * Image class
 */
class image {
private:
    uint16_t width = 0;
    uint16_t height = 0;
    uint16_t num_animation_sprites = 0;
    int16_t sprite_offset_x = 0;
    int16_t sprite_offset_y = 0;
    int8_t animation_can_reverse = 0;
    uint8_t animation_speed_id = 0;
    int32_t offset_mirror = 0;
    uint8_t type = 0;
    int8_t fully_compressed = 0;
    int8_t external = 0;
    int8_t compressed_part = 0;
    uint8_t index = 0;
    uint8_t bitmap_index = 0;
    int32_t offset = 0;
    int32_t data_length = 0;
    int32_t uncompressed_length = 0;
    int32_t full_length = 0;
    uint32_t alpha_offset = 0;
    uint32_t alpha_length = 0;

    std::string bitmap_name;
    std::vector<color_t> data;

    static color_t to_32_bit(uint16_t c);

public:
    image() = default;
    ~image() = default;

    static int32_t convert_uncompressed(buffer *buf, int32_t amount, color_t *dst);
    static int32_t convert_compressed(buffer *buf, int32_t amount, color_t *dst);

    // dummy image
    static image& dummy() {
        static image dummy = {};
        return dummy;
    }
    bool is_dummy() const;

    // getters & setters
    void set_data(color_t *image_data, size_t size);
    const color_t *get_data() const;
    const char *get_bitmap_name() const;
    void set_bitmap_name(const char *filename);
    void set_bitmap_name(const char *filename, size_t size);
    uint16_t get_width() const;
    void set_width(uint16_t width);
    uint16_t get_height() const;
    void set_height(uint16_t height);
    uint8_t get_type() const;
    void set_type(uint8_t uint8_t);
    uint8_t is_fully_compressed() const;
    void set_fully_compressed(uint8_t new_is_fully_compressed);
    uint8_t has_compressed_part() const;
    void set_compressed_part(uint8_t new_has_compressed_part);
    int32_t get_offset() const;
    void set_offset(int32_t new_offset);
    int32_t get_data_length() const;
    void set_data_length(int32_t new_data_length);
    int32_t get_uncompressed_length() const;
    void set_uncompressed_length(int new_uncompressed_length);
    int32_t get_full_length() const;
    void set_full_length(int32_t new_full_length);
    uint8_t get_index() const;
    void set_index(uint8_t new_bmp_index);
    uint8_t get_bitmap_index() const;
    void set_bitmap_index(uint8_t new_bmp_index);
    int is_external() const;
    void set_external(int new_external);
    uint16_t get_num_animation_sprites() const;
    void set_num_animation_sprites(uint16_t new_num_animation_sprites);
    int16_t get_sprite_offset_x() const;
    void set_sprite_offset_x(int16_t new_sprite_offset_x);
    int16_t get_sprite_offset_y() const;
    void set_sprite_offset_y(int16_t new_sprite_offset_y);
    int8_t get_animation_can_reverse() const;
    void set_animation_can_reverse(int8_t new_animation_can_reverse);
    uint8_t get_animation_speed_id() const;
    void set_animation_speed_id(uint8_t new_animation_speed_id);
    int get_offset_mirror() const;
    void set_offset_mirror(int new_offset_mirror);
    uint32_t get_alpha_offset() const;
    void set_alpha_offset(uint32_t new_alpha_offset);
    uint32_t get_alpha_length() const;
    void set_alpha_length(uint32_t new_alpha_length);

    void print() const;
};

#endif // CORE_IMAGE_H

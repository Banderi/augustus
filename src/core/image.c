#include "core/image.h"
#include "core/file.h"
#include "core/io.h"

color_t image::to_32_bit(uint16_t c) {
    return ALPHA_OPAQUE |
           ((c & 0x7c00) << 9) | ((c & 0x7000) << 4) |
           ((c & 0x3e0) << 6) | ((c & 0x380) << 1) |
           ((c & 0x1f) << 3) | ((c & 0x1c) >> 2);
}

int32_t image::convert_uncompressed(buffer *buf, int32_t amount, color_t *dst) {
    for (int i = 0; i < amount; i += 2) {
        color_t c = to_32_bit(buf->read_u16());
        *dst = c;
        dst++;
    }
    return amount / 2;
}

int32_t image::convert_compressed(buffer *buf, int32_t amount, color_t *dst) {
    int dst_length = 0;
    while (amount > 0) {
        int control = buf->read_u8();
        if (control == 255) {
            // next byte = transparent pixels to skip
            *dst++ = 255;
            *dst++ = buf->read_u8();
            dst_length += 2;
            amount -= 2;
        } else {
            // control = number of concrete pixels
            *dst++ = control;
            for (int i = 0; i < control; i++) {
                *dst++ = to_32_bit(buf->read_u16());
            }
            dst_length += control + 1;
            amount -= control * 2 + 1;
        }
    }
    return dst_length;
}

void image::set_data(color_t *image_data, size_t size) {
    data.reserve(size);
    std::copy(image_data, image_data + size, std::back_inserter(data));
}

const color_t *image::image::get_data() const {
    return data.data();
}

const char *image::get_name() const {
    return bitmap_name.c_str();
}

void image::set_name(const char *filename) {
    bitmap_name = std::string(filename);
}

void image::set_name(const char *filename, size_t size) {
    bitmap_name = std::string(filename, size);
}

uint16_t image::get_width() const {
    return width;
}

void image::set_width(uint16_t new_width) {
    width = new_width;
}

uint16_t image::get_height() const {
    return height;
}

void image::set_height(uint16_t new_height) {
    height = new_height;
}

uint8_t image::get_type() const {
    return type;
}

void image::set_type(uint8_t new_type) {
    type = new_type;
}

uint8_t image::is_fully_compressed() const {
    return fully_compressed;
}

void image::set_fully_compressed(uint8_t new_fully_compressed) {
    fully_compressed = new_fully_compressed;
}

uint8_t image::has_compressed_part() const {
    return compressed_part;
}

void image::set_compressed_part(uint8_t new_compressed_part) {
    compressed_part = new_compressed_part;
}

int32_t image::get_offset() const {
    return offset;
}

void image::set_offset(int32_t new_offset) {
    offset = new_offset;
}

int32_t image::get_data_length() const {
    return data_length;
}

void image::set_data_length(int32_t new_data_length) {
    data_length = new_data_length;
}

int image::is_external() const {
    return external;
}

void image::set_external(int new_external) {
    external = new_external;
}

int32_t image::get_uncompressed_length() const {
    return uncompressed_length;
}

void image::set_uncompressed_length(int32_t new_uncompressed_length) {
    uncompressed_length = new_uncompressed_length;
}

int32_t image::get_full_length() const {
    return full_length;
}

void image::set_full_length(int32_t new_full_length) {
    full_length = new_full_length;
}

uint8_t image::get_bmp_index() const {
    return bmp_index;
}

void image::set_bmp_index(uint8_t new_bmp_index) {
    bmp_index = new_bmp_index;
}

uint16_t image::get_num_animation_sprites() const {
    return num_animation_sprites;
}

void image::set_num_animation_sprites(uint16_t new_num_animation_sprites) {
    num_animation_sprites = new_num_animation_sprites;
}

int16_t image::get_sprite_offset_x() const {
    return sprite_offset_x;
}

void image::set_sprite_offset_x(int16_t new_sprite_offset_x) {
    sprite_offset_x = new_sprite_offset_x;
}

int16_t image::get_sprite_offset_y() const {
    return sprite_offset_y;
}

void image::set_sprite_offset_y(int16_t new_sprite_offset_y) {
    sprite_offset_y = new_sprite_offset_y;
}

int8_t image::get_animation_can_reverse() const {
    return animation_can_reverse;
}

void image::set_animation_can_reverse(int8_t new_animation_can_reverse) {
    animation_can_reverse = new_animation_can_reverse;
}

uint8_t image::get_animation_speed_id() const {
    return animation_speed_id;
}

void image::set_animation_speed_id(uint8_t new_animation_speed_id) {
    animation_speed_id = new_animation_speed_id;
}

int32_t image::get_offset_mirror() const {
    return offset_mirror;
}

void image::set_offset_mirror(int32_t new_offset_mirror) {
    offset_mirror = new_offset_mirror;
}

bool image::is_dummy() const {
    return (this == &dummy());
}

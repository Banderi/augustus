#include "SDL.h"
#include "core/buffer.h"
#include "core/file.h"
#include "core/io.h"
#include "core/log.h"
#include "core/mods.h"
#include "core/config.h"
#include "core/game_environment.h"
#include "core/table_translation.h"
#include "core/image_collection.h"

image_collection::image_collection():
    initialized(false), id_shift_overall(0), name{0}, entries_num(0), groups_num(0),
    header_data{0}, group_image_ids{0}, /**/images(nullptr), data(nullptr) {
}

bool image_collection::check_initialized() const {
    return (initialized == 0);
}

int image_collection::load_555(const char *filename_555, const char *filename_sgx, int shift) {
    // read sgx /////////////
    // prepare sgx data
    buffer *buf = new buffer(SCRATCH_DATA_SIZE);
    if (!io_read_file_into_buffer(filename_sgx, MAY_BE_LOCALIZED, buf, SCRATCH_DATA_SIZE)) {//int MAIN_INDEX_SIZE = 660680; 
        return 0;
    }
    
    size_t header_size = HEADER_SG3_SIZE;
    if (file_has_extension(filename_sgx, "sg2")) {
        header_size = HEADER_SG2_SIZE; // sg2 has 100 bitmap entries
    } else if (file_has_extension(filename_sgx, "sg3")) {
        header_size = HEADER_SG3_SIZE;
    } else {
        SDL_Log("Loading image collection from file '%s': wrong extension", filename_sgx);
        return 0;
    }

    // read header
    buf->read_raw(header_data, sizeof(uint32_t) * 10);

    // allocate arrays
    entries_num = (size_t) header_data[4] + 1;
    //images.reserve(entries_num);

    set_shift(shift);
    set_name(filename_sgx);

    if (check_initialized()) {
        initialized = false;
        delete[] images;
        delete[] data;
    }
    images = new image[entries_num];
    data = new color_t[entries_num * 10000];
    initialized = true;

    buf->skip(40); // skip remaining 40 bytes

    // parse groups (always a fixed 300 pool)
    groups_num = 0;
    for (size_t i = 0; i < GROUP_IMAGE_IDS_SIZE; i++) {
        group_image_ids[i] = buf->read_u16();
        if (group_image_ids[i] != 0) {
            groups_num++;
//            SDL_Log("%s group %i -> id %i", filename_sgx, i, group_image_ids[i]);
        }
    }

    // parse bitmap names
    int num_bmp_names = (int) header_data[5];
    char bmp_names[num_bmp_names][200];
    // every line is 200 chars - 97 entries in the original c3.sg2 header (100 for good measure) and 18 in Pharaoh_General.sg3
    buf->read_raw(bmp_names, 200 * num_bmp_names);

    // move on to the rest of the content
    buf->set_offset(header_size);

    // fill in image data
    int bmp_lastbmp = 0;
    int bmp_lastindex = 1;
    for (size_t i = 0; i < entries_num; i++) {
        image img;
        img.draw.offset = buf->read_i32();
        img.draw.data_length = buf->read_i32();
        img.draw.uncompressed_length = buf->read_i32();
        buf->skip(4);
        img.offset_mirror = buf->read_i32(); // .sg3 only
        img.width = buf->read_u16();
        img.height = buf->read_u16();
        buf->skip(6);
        img.num_animation_sprites = buf->read_u16();
        buf->skip(2);
        img.sprite_offset_x = buf->read_i16();
        img.sprite_offset_y = buf->read_i16();
        buf->skip(10);
        img.animation_can_reverse = buf->read_i8();
        buf->skip(1);
        img.draw.type = buf->read_u8();
        img.draw.is_fully_compressed = buf->read_i8();
        img.draw.is_external = buf->read_i8();
        img.draw.has_compressed_part = buf->read_i8();
        buf->skip(2);
        int bitmap_id = buf->read_u8();
        const char *bmn = bmp_names[bitmap_id];
        strncpy(img.draw.bitmap_name, bmn, 200);
        if (bitmap_id != bmp_lastbmp) {// new bitmap name, reset bitmap grouping index
            bmp_lastindex = 1;
            bmp_lastbmp = bitmap_id;
        }
        img.draw.bmp_index = bmp_lastindex;
        bmp_lastindex++;
        buf->skip(1);
        img.animation_speed_id = buf->read_u8();
        if (header_data[1] < 214) {
            buf->skip(5);
        } else {
            buf->skip(5 + 8);
        }
//        images.push_back(img);
        images[i] = img;
    }

    // fill in bmp offset data
    int offset = 4;
    for (size_t i = 1; i < entries_num; i++) {
        image *img = &images[i];
        if (img->draw.is_external) {
            if (!img->draw.offset)
                img->draw.offset = 1;
        } else {
            img->draw.offset = offset;
            offset += img->draw.data_length;
        }
    }

    // read 555 /////////////////////////////
    // prepare bitmap data
    buf->clear();
    int data_size = io_read_file_into_buffer(filename_555, MAY_BE_LOCALIZED, buf, SCRATCH_DATA_SIZE);
    if (!data_size) {
        return 0;
    }

    // convert bitmap data for image pool
    color_t *start_dst = data;
    color_t *dst = data;
    dst++; // make sure img->offset > 0
    for (size_t i = 0; i < entries_num; i++) {
        image *img = &images[i];
        if (img->draw.is_external) {
            continue;
        }
        buf->set_offset(img->draw.offset);
        int img_offset = (int) (dst - start_dst);

        size_t image_size = 0;
        if (img->draw.is_fully_compressed) {
            image_size = convert_compressed(buf, img->draw.data_length, dst);
            dst+=image_size;
        } else if (img->draw.has_compressed_part) { // isometric tile
            size_t uncompressed_size = convert_uncompressed(buf, img->draw.uncompressed_length, dst);
            dst+=uncompressed_size;

            size_t compressed_size = convert_compressed(buf, img->draw.data_length - img->draw.uncompressed_length, dst);
            dst+=compressed_size;

            image_size = uncompressed_size + compressed_size;
        } else {
            image_size = convert_uncompressed(buf, img->draw.data_length, dst);
            dst+=image_size;
        }

        img->draw.offset = img_offset;
        img->draw.uncompressed_length /= 2;
//        img->draw.data = &data[img_offset];
        img->draw.size = image_size;
        img->set_data(&data[img_offset], image_size);
//        SDL_Log("Loading... %s : %i", filename_555, i);
    }
    delete[] data;
    return 1;
}

int image_collection::size() const {
    return entries_num;
}

int image_collection::get_id(int group) {
    if (group >= groups_num) {
        group = 0;
    }
    return group_image_ids[group] + get_shift();
}

const image *image_collection::get_image(int id, bool relative) {
    if (!relative) {
        id -= get_shift();
    }
    if (id < 0 || id >= entries_num) {
        return nullptr;
    }
    return &images[id];
}

int32_t image_collection::get_shift() const {
    return id_shift_overall;
}

void image_collection::set_shift(int32_t shift) {
    id_shift_overall = shift;
}

const char *image_collection::get_name() const {
    return name;
}

void image_collection::set_name(const char *filename) {
    memset(name, 0, NAME_SIZE);
    strncpy(name, filename, NAME_SIZE - 1);
}

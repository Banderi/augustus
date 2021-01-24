#include "SDL.h"
#include "core/buffer.h"
#include "core/file.h"
#include "core/io.h"
#include "core/log.h"
#include "core/mods.h"
#include "core/image_collection.h"

image_collection::image_collection():
    id_shift_overall(0), name{0}, entries_num(0), groups_num(0), header_data{0} {
}

int image_collection::load_sgx(const char *filename_sgx, int shift) {
    // prepare sgx data
    size_t file_size = io_get_file_size(filename_sgx);
    SDL_Log("Loading image collection from file '%s': %zu", filename_sgx, file_size);
    if (!file_size) {
        SDL_Log("Loading image collection from file '%s': empty file", filename_sgx);
        return 0;
    }

    buffer buffer_sgx(file_size);
    if (!io_read_file_into_buffer(filename_sgx, MAY_BE_LOCALIZED, &buffer_sgx, MAX_FILE_SIZE)) { //int MAIN_INDEX_SIZE = 660680;
        SDL_Log("Loading image collection from file '%s': can't read file", filename_sgx);
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
    buffer_sgx.read_raw(header_data, sizeof(uint32_t) * HEADER_DATA_SIZE);

    // allocate arrays
    entries_num = (size_t) header_data[4] + 1;
    images.reserve(entries_num);

    set_shift(shift);
    set_name(filename_sgx);

    buffer_sgx.skip(40); // skip remaining 40 bytes

    // parse groups (always a fixed 300 pool)
    groups_num = 0;
    for (size_t i = 0; i < GROUP_IMAGE_IDS_SIZE; i++) {
        auto image_id = buffer_sgx.read_u16();
        group_image_ids.push_back(image_id);
        if (image_id != 0) {
            groups_num++;
//            SDL_Log("%s group %i -> id %i", filename_sgx, i, group_image_ids[i]);
        }
    }

    // parse bitmap names
    int num_bmp_names = (int) header_data[5];
    char bmp_names[num_bmp_names][BMP_NAME_SIZE];
    // every line is 200 chars - 97 entries in the original c3.sg2 header (100 for good measure) and 18 in Pharaoh_General.sg3
    buffer_sgx.read_raw(bmp_names, BMP_NAME_SIZE * num_bmp_names);

    // move on to the rest of the content
    buffer_sgx.set_offset(header_size);

    // fill in image data
    int bmp_lastbmp = 0;
    int bmp_lastindex = 1;
    for (size_t i = 0; i < entries_num; i++) {
        image img;
        img.set_offset(buffer_sgx.read_i32());
        img.set_data_length(buffer_sgx.read_i32());
        img.set_uncompressed_length(buffer_sgx.read_i32());
        buffer_sgx.skip(4);
        img.set_offset_mirror(buffer_sgx.read_i32()); // .sg3 only
        img.set_width(buffer_sgx.read_u16());
        img.set_height(buffer_sgx.read_u16());
        buffer_sgx.skip(6);
        img.set_num_animation_sprites(buffer_sgx.read_u16());
        buffer_sgx.skip(2);
        img.set_sprite_offset_x(buffer_sgx.read_i16());
        img.set_sprite_offset_y(buffer_sgx.read_i16());
        buffer_sgx.skip(10);
        img.set_animation_can_reverse(buffer_sgx.read_i8());
        buffer_sgx.skip(1);
        img.set_type(buffer_sgx.read_u8());
        img.set_fully_compressed(buffer_sgx.read_i8());
        img.set_external(buffer_sgx.read_i8());
        img.set_compressed_part(buffer_sgx.read_i8());
        buffer_sgx.skip(2);
        int bitmap_id = buffer_sgx.read_u8();
        const char *bmn = bmp_names[bitmap_id];
        img.set_name(bmn, BMP_NAME_SIZE);
        if (bitmap_id != bmp_lastbmp) {// new bitmap name, reset bitmap grouping index
            bmp_lastindex = 1;
            bmp_lastbmp = bitmap_id;
        }
        img.set_bmp_index(bmp_lastindex);
        bmp_lastindex++;
        buffer_sgx.skip(1);
        img.set_animation_speed_id(buffer_sgx.read_u8());
        if (header_data[1] < 214) {
            buffer_sgx.skip(5);
        } else {
            buffer_sgx.skip(5 + 8);
        }
        images.push_back(img); // TODO: fix this shit
        images.at(i) = img;
    }

    // fill in bmp offset data
    int offset = 4;
    for (size_t i = 1; i < entries_num; i++) {
        image *img = &images.at(i);
        if (img->is_external()) {
            if (!img->get_offset()) {
                img->set_offset(1);
            }
        } else {
            img->set_offset(offset);
            offset += img->get_data_length();
        }
    }

    return 1;
}

int image_collection::load_555(const char *filename_555) {
    // prepare bitmap data
    size_t file_size = io_get_file_size(filename_555);
    SDL_Log("Loading image collection from file '%s': %zu", filename_555, file_size);
    if (!file_size) {
        SDL_Log("Loading image collection from file '%s': empty file", filename_555);
        return 0;
    }

    buffer buffer_555(file_size);
    int data_size = io_read_file_into_buffer(filename_555, MAY_BE_LOCALIZED, &buffer_555, MAX_FILE_SIZE);
    if (!data_size) {
        SDL_Log("Loading image collection from file '%s': can't read file", filename_555);
        return 0;
    }

    // temp variable for image data
    auto *data = new color_t[entries_num * MAX_IMAGE_SIZE];

    // convert bitmap data for image pool
    color_t *start_dst = data;
    color_t *dst = data;
    dst++; // make sure img->offset > 0
    for (size_t i = 0; i < entries_num; i++) {
        image *img = &images.at(i);
        // if external, image will automatically loaded in the runtime
        if (img->is_external()) {
            continue;
        }
        buffer_555.set_offset(img->get_offset());
        int img_offset = (int) (dst - start_dst);

        size_t image_size = 0;
        if (img->is_fully_compressed()) {
            image_size = image::convert_compressed(&buffer_555, img->get_data_length(), dst);
            dst+=image_size;
        } else if (img->has_compressed_part()) { // isometric tile
            size_t uncompressed_size = image::convert_uncompressed(&buffer_555, img->get_uncompressed_length(), dst);
            dst+=uncompressed_size;

            size_t compressed_size = image::convert_compressed(&buffer_555, img->get_data_length() - img->get_uncompressed_length(), dst);
            dst+=compressed_size;

            image_size = uncompressed_size + compressed_size;
        } else {
            image_size = image::convert_uncompressed(&buffer_555, img->get_data_length(), dst);
            dst+=image_size;
        }

        img->set_offset(img_offset);
        img->set_uncompressed_length(img->get_uncompressed_length()/2);
        img->set_full_length(image_size);
        img->set_data(&data[img_offset], image_size);
    }

    delete[] data;

    return 1;
}

int image_collection::load_files(const char *filename_555, const char *filename_sgx, int shift) {
    if (!load_sgx(filename_sgx, shift)) {
        return 0;
    }
    if (!load_555(filename_555)) {
        return 0;
    }

    return 1;
}

int image_collection::size() const {
    return entries_num;
}

int image_collection::get_id(int group) {
    if (group >= groups_num) {
        group = 0;
    }
    return group_image_ids.at(group) + get_shift();
}

image *image_collection::get_image(int id, bool relative) {
    if (!relative) {
        id -= get_shift();
    }
    if (id < 0 || id >= entries_num) {
        return &image::dummy();
    }
    return &images.at(id);
}

int32_t image_collection::get_shift() const {
    return id_shift_overall;
}

void image_collection::set_shift(int32_t shift) {
    id_shift_overall = shift;
}

const char *image_collection::get_name() const {
    return name.c_str();
}

void image_collection::set_name(const char *filename) {
    name = std::string(filename);
}
#ifndef IMAGE_COLLECTION_H
#define IMAGE_COLLECTION_H

#include "core/encoding.h"
#include "core/image.h"
#include "core/image_group.h"
#include "core/buffer.h"
#include "core/file.h"
#include "graphics/color.h"

#include <vector>
#include <string>

class image_collection {
private:
    static const size_t HEADER_DATA_SIZE = 10;
    static const size_t BMP_NAME_SIZE = 200;
    static const size_t GROUP_IMAGE_IDS_SIZE = 300;
    static const size_t MAX_FILE_SIZE = 20000000;
    static const size_t MAX_IMAGE_SIZE = 10000;
    static const size_t HEADER_SG2_SIZE = 20680;
    static const size_t HEADER_SG3_SIZE = 40680;

    int32_t id_shift_overall;
    std::string name;
    size_t entries_num;
    size_t groups_num;
    uint32_t header_data[HEADER_DATA_SIZE];
    std::vector<uint16_t> group_image_ids;
    std::vector<image> images;

public:
    image_collection();
    ~image_collection() = default;

    int load_sgx(const char *filename_sgx, int shift = 0);
    int load_555(const char *filename_555);
    int load_files(const char *filename_555, const char *filename_sgx, int shift = 0);

    int32_t get_shift() const;
    void set_shift(int32_t shift);
    const char *get_name() const;
    void set_name(const char *filename);

    int size() const;
    int get_id(int group);
    image *get_image(int id, bool relative = false);
};

#endif //IMAGE_COLLECTION_H

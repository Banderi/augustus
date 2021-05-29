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
    static const size_t NAME_SIZE = 256;
    static const size_t HEADER_DATA_SIZE = 10;
    static const size_t GROUP_IMAGE_IDS_SIZE = 300;
    static const size_t SCRATCH_DATA_SIZE = 20000000; ///???
    static const size_t HEADER_SG2_SIZE = 20680;
    static const size_t HEADER_SG3_SIZE = 40680;

    bool initialized;
    int32_t id_shift_overall;
    char name[NAME_SIZE];
    size_t entries_num;
    size_t groups_num;
    uint32_t header_data[HEADER_DATA_SIZE];
    uint16_t group_image_ids[GROUP_IMAGE_IDS_SIZE];
//    std::vector<image> images;
    image* images;
    color_t *data;

    bool check_initialized() const;

public:
    image_collection();

    int load_555(const char *filename_555, const char *filename_sgx, int shift = 0);

    int32_t get_shift() const;
    void set_shift(int32_t shift);
    const char *get_name() const;
    void set_name(const char *filename);

    int size() const;
    int get_id(int group);
    const image *get_image(int id, bool relative = false);
};

#endif //IMAGE_COLLECTION_H

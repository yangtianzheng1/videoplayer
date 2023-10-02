//
// Created by 杨天正 on 2023/9/17.
//

#ifndef VIDEOPLAYER_ONE_FRAME_H
#define VIDEOPLAYER_ONE_FRAME_H

#include <malloc.h>
#include "../utils/logger.h"
extern "C"{
#include <libavutil/rational.h>
};

class OneFrame{
public:
    uint8_t *data = NULL;
    int line_size;
    int64_t pts;
    AVRational time_base;
    uint8_t *ext_data = NULL;
    // 是否自动回收data和ext_data
    bool autoRecycle = true;

    OneFrame(uint8_t *data, int line_size, int64_t pts, AVRational time_base,
             uint8_t *ext_data = NULL, bool autoRecycle = true) {
        this->data = data;
        this->line_size = line_size;
        this->pts = pts;
        this->time_base = time_base;
        this->ext_data = ext_data;
        this->autoRecycle = autoRecycle;
    }

    ~OneFrame() {
        if (autoRecycle) {
            if (data != nullptr) {
                free(data);
                data = nullptr;
            }
            if (ext_data != nullptr) {
                free(ext_data);
                ext_data = nullptr;
            }
        }
    }

};

#endif //VIDEOPLAYER_ONE_FRAME_H

//
// Created by yangpc on 2023/10/6.
//

#ifndef VIDEOPLAYER_OPENGL_PIXEL_RECEIVER_H
#define VIDEOPLAYER_OPENGL_PIXEL_RECEIVER_H

#include <stdint.h>

class OpenGLPixelReceiver {
public:
    virtual void ReceivePixel(uint8_t *rgba) = 0;
};

#endif //VIDEOPLAYER_OPENGL_PIXEL_RECEIVER_H

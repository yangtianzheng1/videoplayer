//
// Created by yangpc on 2023/10/3.
//

#ifndef VIDEOPLAYER_DRAWER_PROXY_H
#define VIDEOPLAYER_DRAWER_PROXY_H


#include "../drawer.h"

class DrawerProxy {

public:
    virtual void AddDrawer(Drawer * drawer) = 0;
    virtual void Draw() = 0;
    virtual void Release() = 0;
    virtual ~DrawerProxy(){}
};


#endif //VIDEOPLAYER_DRAWER_PROXY_H

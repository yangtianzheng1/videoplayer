//
// Created by yangpc on 2023/10/4.
//

#ifndef VIDEOPLAYER_DEF_DRAWER_PROXY_IMPL_H
#define VIDEOPLAYER_DEF_DRAWER_PROXY_IMPL_H


#include "drawer_proxy.h"
#include <vector>

class DefDrawerProxyImpl: public DrawerProxy {
private:
    std::vector<Drawer *> m_drawers;

public:
    void AddDrawer(Drawer *drawer) override;
    void Draw() override;
    void Release() override;

};


#endif //VIDEOPLAYER_DEF_DRAWER_PROXY_IMPL_H

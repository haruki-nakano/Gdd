//
//  Bullet.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#ifndef __Yuzu__Bullet__
#define __Yuzu__Bullet__

#include "cocos2d.h"

class Bullet : public cocos2d::Sprite {
public:
    CREATE_FUNC(Bullet);
    void step();
    void setAngle(float angle);

private:
    bool init() override;
    void onEnter() override;

    float _angle;
};

#endif /* defined(__Yuzu__Bullet__) */

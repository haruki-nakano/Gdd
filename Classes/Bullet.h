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
    void setDirection(cocos2d::Vec2 v);
    cocos2d::Vec2 getDirectionVec();

    void step(float dt);
    float getLifePoint();
    void setLifePoint(float lifePoint);

private:
    bool init() override;
    void onEnter() override;

    cocos2d::Vec2 _direction;
    float _lifePoint;
};

#endif /* defined(__Yuzu__Bullet__) */

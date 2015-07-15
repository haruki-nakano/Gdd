//
//  Player.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#ifndef __Yuzu__Player__
#define __Yuzu__Player__

#include "cocos2d.h"

class Bullet;

class Player : public cocos2d::Sprite {
public:
    CREATE_FUNC(Player);

    void step();
    void setDirection(const cocos2d::Vec2 direction);

    // Players action
    void stop();
    Bullet *createBullet();

private:
    bool init() override;
    void onEnter() override;

    cocos2d::Vec2 _direction;
    int idd;
    std::string name;
};

#endif /* defined(__Yuzu__Player__) */

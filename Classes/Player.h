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

#include "Constants.h"

class Bullet;

class Player : public cocos2d::Sprite {
public:
    CREATE_FUNC(Player);

    constexpr static const float MOVE_FACTOR = 10.0f;

    void step();
    void setMovingState(const MovingState movingState);
    MovingState getMovingState();

    // Players action
    void stop();
    Bullet *createBullet();

private:
    bool init() override;
    void onEnter() override;

    cocos2d::Vec2 _direction;
    MovingState _moving;
    std::string _name;
};

#endif /* defined(__Yuzu__Player__) */

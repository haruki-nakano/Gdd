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

    void setMoveState(const MoveState MoveState);
    MoveState getMoveState();

    void setDirection(const Direction direction);
    Direction getDirection();

    // Players action
    void stop();
    Bullet *createBullet();

private:
    bool init() override;
    void onEnter() override;

    cocos2d::Vec2 _directionVec;
    MoveState _moving;
    Direction _direction;
    std::string _name;

    // PlayerImages
    cocos2d::Texture2D *_imgLeft;
    cocos2d::Texture2D *_imgRight;
    cocos2d::Texture2D *_imgUp;
    cocos2d::Texture2D *_imgDown;
};

#endif /* defined(__Yuzu__Player__) */

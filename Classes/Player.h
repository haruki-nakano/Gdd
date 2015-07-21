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

    void step(float dt);

    bool isCorrectUpdate(const cocos2d::Vec2 position);

    void setMoveState(const MoveState MoveState);
    MoveState getMoveState();

    void setDirection(const Direction direction);
    Direction getDirection();

    // Players action
    Bullet *createBullet();

    void hitShot();
    void setLifePoint(int lifePoints);
    int getLifePoint();

private:
    bool init() override;
    void onEnter() override;

    cocos2d::Vec2 _directionVec;
    MoveState _moving;
    Direction _direction;
    std::string _name;
    int _lifePoint;

    // PlayerImages
    cocos2d::Texture2D *_imgLeft;
    cocos2d::Texture2D *_imgRight;
    cocos2d::Texture2D *_imgUp;
    cocos2d::Texture2D *_imgDown;
};

#endif /* defined(__Yuzu__Player__) */

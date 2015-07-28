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
class LifeBar;

class Player : public cocos2d::Sprite {
public:
    CREATE_FUNC(Player);

    void step(float dt);

    bool isCorrectUpdate(const cocos2d::Vec2 position);
    bool isSwimming();

    // Fix argument
    void setIsSwimming(bool swimming, bool opponent);

    void setMoveState(const MoveState MoveState);
    MoveState getMoveState();

    void setDirection(const Direction direction);
    Direction getDirection();

    // Players action
    std::vector<Bullet *> createBullets(cocos2d::Vec2 touchPos, cocos2d::Vec2 stagePos);

    void bulletHits(Bullet *bullet);
    void setHitCount(int hitCount);
    int getHitCount();

    void gotHeal();
    void setHealCount(int healCount);
    int getHealCount();

    int getLifePoint();
    void setLifeBar(LifeBar *lifeBar);

    const char *getGunName();

private:
    bool init() override;
    void onEnter() override;

    void updateLifePoint();

    LifeBar *_lifeBar;
    cocos2d::Vec2 _directionVec;
    MoveState _moving;
    Direction _direction;
    Gun _gun;
    std::string _name;
    int _lifePoint;
    bool _isSwimming;
    int _hitCount;
    int _healCount;

    // PlayerImages
    cocos2d::Texture2D *_imgLeft;
    cocos2d::Texture2D *_imgRight;
    cocos2d::Texture2D *_imgUp;
    cocos2d::Texture2D *_imgDown;
    cocos2d::Texture2D *_imgUpperLeft;
    cocos2d::Texture2D *_imgUpperRight;
    cocos2d::Texture2D *_imgLowerLeft;
    cocos2d::Texture2D *_imgLowerRight;
};

#endif /* defined(__Yuzu__Player__) */

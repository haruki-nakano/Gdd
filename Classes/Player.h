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

    bool isCorrectUpdate(const cocos2d::Vec2 position) const;
    bool isSwimming() const;
    bool isFiring() const;

    // Fix argument
    void setIsSwimming(const bool swimming, const bool opponent);

    MoveState getMoveState() const;
    void setMoveState(const MoveState MoveState);

    void setDirection(const Direction direction);

    // Players action
    std::vector<Bullet *> createBullets(cocos2d::Vec2 touchPos, cocos2d::Vec2 stagePos);

    void bulletHits(Bullet *bullet);
    int getHitCount() const;
    void setHitCount(const int hitCount);

    void gotHeal();
    void setHealCount(const int healCount);
    int getHealCount() const;

    int getLifePoint() const;
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
    clock_t _lastTimeBulletCreated;
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

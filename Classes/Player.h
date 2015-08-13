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

class Bar;
class Bullet;
class LifeBar;

class Player : public cocos2d::Sprite {
public:
    static Player *create(bool opponent) {
        Player *pRet = new (std::nothrow) Player;
        if (pRet && pRet->init()) {
            pRet->autorelease();
            pRet->_isOpponent = opponent;
            return pRet;
        } else {
            delete pRet;
            pRet = NULL;
            return NULL;
        }
    }

    void step(float dt);

    bool isCorrectUpdate(const cocos2d::Vec2 position) const;
    bool isSwimming() const;
    bool isFiring() const;
    bool isCaptured() const;
    bool isInvincible() const;
    bool isOpponent() const;

    // Fix argument
    void setIsSwimming(const bool swimming, const bool forceUpdate = false);

    void setPlayerColor(const bool isHost);

    MoveState getMoveState() const;
    void setMoveState(const MoveState moveState);

    void setDirection(const Direction direction);
    Direction getDirection() const;

    // Players action
    std::vector<Bullet *> createBullets(cocos2d::Vec2 touchPos, cocos2d::Vec2 stagePos);

    void bulletHits(Bullet *bullet);
    int getHitCount() const;
    void setHitCount(const int hitCount);

    void gotHeal();
    void captured();
    void gotInvincible();

    void setHealCount(const int healCount);
    int getHealCount() const;

    int getLifePoint() const;
    void setLifeBar(Bar *lifeBar);

    int getWaterPoint() const;
    void setWaterPoint(const int waterPoint);
    void setWaterBar(Bar *waterBar);

    void setLastTimeBulletCreated(clock_t t);

    const char *getGunName();

    Gun replaceGun();
    void setGun(const Gun gun);

private:
    bool init() override;
    void onEnter() override;

    void updateLifePoint();
    void updateVelocity();

    void showToast();

    Bar *_lifeBar;
    Bar *_waterBar;
    cocos2d::Vec2 _directionVec;
    MoveState _moving;
    Direction _direction;
    Sprite *_splash;
    Sprite *_toast;
    Gun _gun;

    std::string _name;
    clock_t _lastTimeBulletCreated;
    float _addedWaterTimeCount;
    float _invincibleTimeCount;
    float _blinkTimeCount;
    float _capturedTimeCount;
    int _lifePoint;
    int _waterPoint;
    bool _isSwimming;
    bool _isOpponent;
    bool _isBlinking;
    bool _lastFiring;
    bool _lastInvincible;
    bool _lastCaptured;
    int _hitCount;
    int _healCount;

    // Splash

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

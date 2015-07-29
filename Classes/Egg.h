//
//  Egg.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/23.
//
//

#ifndef __Yuzu__Egg__
#define __Yuzu__Egg__

#include "Constants.h"

class Player;

class Egg : public cocos2d::Sprite {
public:
    CREATE_FUNC(Egg);

    int getLifePoint() const;
    void setLifePoint(const int lifePoint);

    clock_t getLastBrokenTime() const;

private:
    bool init() override;
    void onEnter() override;

    clock_t _lastBrokenTime;
    int _lifePoint;

    EggItemType _item;

    cocos2d::Texture2D *_egg;
    cocos2d::Texture2D *_egg2;
    cocos2d::Texture2D *_egg3;
};

#endif /* defined(__Yuzu__Egg__) */
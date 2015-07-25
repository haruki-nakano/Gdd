//
//  Egg.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/23.
//
//

#ifndef __Yuzu__Egg__
#define __Yuzu__Egg__

class Player;

class Egg : public cocos2d::Sprite {
public:
    CREATE_FUNC(Egg);

    int getLifePoint();
    void setLifePoint(int lifePoint);
    Player *getOwner();
    void setOwner(Player *player);
    clock_t getLastBrokenTime();

private:
    bool init() override;
    void onEnter() override;

    clock_t _lastBrokenTime;
    int _lifePoint;
    Player *_owner;
};

#endif /* defined(__Yuzu__Egg__) */
//
//  Weapon.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/08/13.
//
//

#ifndef __Yuzu__Weapon__
#define __Yuzu__Weapon__

#include "Constants.h"

class Weapon : public cocos2d::Sprite {
public:
    CREATE_FUNC(Weapon);

    enum State {
        INVISIBLE,
        STAGING,
    };

    void setState(const State state);
    State getState();
private:
    bool init() override;
    void onEnter() override;

    State _state;
};

#endif /* defined(__Yuzu__Weapon__) */

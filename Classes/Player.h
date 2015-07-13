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

class Player : public cocos2d::Sprite {
public:
    CREATE_FUNC(Player);

private:
    bool init() override;
    void onEnter() override;
};

#endif /* defined(__Yuzu__Player__) */

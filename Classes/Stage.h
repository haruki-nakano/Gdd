//
//  Stage.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#ifndef __Yuzu__Stage__
#define __Yuzu__Stage__

class Bullet;
class Player;

class Stage : public cocos2d::Sprite {
public:
    CREATE_FUNC(Stage);

    void step();
    void addBullet(Bullet *);

    Player *getPlayer();

private:
    bool init() override;
    void onEnter() override;

    Player *_player;
    std::vector<Bullet *> _bullets;
};

#endif /* defined(__Yuzu__Stage__) */

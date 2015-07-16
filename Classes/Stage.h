//
//  Stage.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#ifndef __Yuzu__Stage__
#define __Yuzu__Stage__

#include "cocos2d.h"
#include "Constants.h"
#include "JSONPacker.h"

class Bullet;
class Player;

class Stage : public cocos2d::Node {
public:
    CREATE_FUNC(Stage);

    void step();
    void addBullet(Bullet *);

    Player *getPlayer();
    Player *getEnemy();
    void setState(JSONPacker::GameState state);

private:
    bool init() override;
    void onEnter() override;

    bool collideWith(cocos2d::Vec2 coordinate);
    cocos2d::Vec2 convertPositionToCoordinate(cocos2d::Vec2 position);

    cocos2d::experimental::TMXTiledMap *_map;
    cocos2d::experimental::TMXLayer *_backgroundLayer;
    std::vector<Player *> _players;
    std::vector<Bullet *> _bullets;
};

#endif /* defined(__Yuzu__Stage__) */
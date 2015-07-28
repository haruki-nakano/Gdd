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
#include "Egg.h"
#include "JSONPacker.h"

class Bullet;
class Player;

class Stage : public cocos2d::Node {
public:
    CREATE_FUNC(Stage);

    void step(float dt);
    void addBullet(Bullet *);
    void generateEgg();

    Player *getPlayer();
    Player *getOpponent();
    // FIXME: Improve
    // replace around getOpponent
    // std::vector<Player *> getPlayers();
    Egg *getEgg();
    void setState(JSONPacker::GameState state);
    void initializePlayersPosition(bool isHost);

private:
    bool init() override;
    void onEnter() override;

    cocos2d::Vec2 convertPositionToTileCoordinate(cocos2d::Vec2 position);
    bool isCorrectTileCoordinate(cocos2d::Vec2 tileCoordinate, bool checkWallCollision = false);

    cocos2d::experimental::TMXTiledMap *_map;
    cocos2d::experimental::TMXLayer *_backgroundLayer;
    cocos2d::experimental::TMXLayer *_collisionLayer;
    cocos2d::Size _size;
    std::vector<Player *> _players;
    std::vector<Bullet *> _bullets;
    Egg *_egg;
};

#endif /* defined(__Yuzu__Stage__) */

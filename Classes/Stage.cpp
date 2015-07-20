//
//  Stage.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "Stage.h"

#include "Constants.h"
#include "Player.h"
#include "Bullet.h"
#include "Bullet.h"

using namespace cocos2d;

bool Stage::init() {
    if (!Node::init()) {
        return false;
    }

    _players.reserve(MAX_PLAYERS);

    return true;
}

void Stage::onEnter() {
    Node::onEnter();

    Size contentSize = getContentSize();

    _map = cocos2d::experimental::TMXTiledMap::create(DEFAULT_STAGE_FILE);
    _backgroundLayer = _map->getLayer(DEFAULT_BACKGROUND_LAYER_NAME);
    auto size = _backgroundLayer->getLayerSize();
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            auto tile = _backgroundLayer->getTileAt(Vec2(x, y));
            if (tile) {
                tile->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            }
        }
    }
    // TOOD
    auto collisionLayer = _map->getLayer(DEFAULT_COLLISION_LAYER_NAME);
    size = collisionLayer->getLayerSize();
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            auto tile = collisionLayer->getTileAt(Vec2(x, y));
            if (tile) {
                tile->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
                Vec2 v[4];
                Vec2 s = Vec2(32, 16);
                v[0] = Vec2(-s.x, 0);
                v[1] = Vec2(0, s.y);
                v[2] = Vec2(s.x, 0);
                v[3] = Vec2(0, -s.y);
                PhysicsBody *tilePhysics = PhysicsBody::createEdgePolygon(v, 4);
                tilePhysics->setDynamic(false);
                tilePhysics->setCategoryBitmask(CATEGORY_MASK_WALL);
                tilePhysics->setCollisionBitmask(CONTACT_MASK_WALL);
                tilePhysics->setContactTestBitmask(0);
                tile->setPhysicsBody(tilePhysics);
            }
        }
    }

    // setup map
    _map->setAnchorPoint(Vec2(0.0f, 0.0f));
    _map->setPosition(Vec2(0.0f, 0.0f));

    this->addChild(_map);

    // setup player
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player *player = Player::create();
        player->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

        PhysicsBody *playerPhysics = PhysicsBody::createBox(player->getBoundingBox().size);
        playerPhysics->setDynamic(true);
        playerPhysics->setGravityEnable(false);
        playerPhysics->setRotationEnable(false);
        playerPhysics->setCategoryBitmask(CATEGORY_MASK_PLAYER);
        playerPhysics->setCollisionBitmask(CONTACT_MASK_PLAYER);
        playerPhysics->setContactTestBitmask(0);
        player->setPhysicsBody(playerPhysics);
        this->addChild(player);
        _players.push_back(player);
    }
}

void Stage::initializePlayersPosition(bool isHost) {
    auto hostInitialCoordinate = Vec2(_backgroundLayer->getProperty("hostInitialPositionX").asFloat(),
                                      _backgroundLayer->getProperty("hostInitialPositionY").asFloat());
    auto clientInitialCoordinate = Vec2(_backgroundLayer->getProperty("clientInitialPositionX").asFloat(),
                                        _backgroundLayer->getProperty("clientInitialPositionY").asFloat());
    Vec2 hostInitialPosition = _backgroundLayer->getPositionAt(hostInitialCoordinate);
    Vec2 clientInitialPosition = _backgroundLayer->getPositionAt(clientInitialCoordinate);

    if (isHost) {
        getPlayer()->setPosition(hostInitialPosition);
        getOpponent()->setPosition(clientInitialPosition);
    } else {
        getPlayer()->setPosition(clientInitialPosition);
        getOpponent()->setPosition(hostInitialPosition);
    }

    // Scroll to put a player center on the screen
    Size size = Director::getInstance()->getVisibleSize();
    Vec2 pos = getPlayer()->getPosition();
    this->setPosition(Vec2(size.width * 0.5 - pos.x, size.height * 0.5 - pos.y));
}

void Stage::step(float dt) {
    Size size = Director::getInstance()->getVisibleSize();
    Vec2 pos = getPlayer()->getPosition();
    this->setPosition(Vec2(size.width * 0.5 - pos.x, size.height * 0.5 - pos.y));
}

void Stage::addBullet(Bullet *bullet) {
    _bullets.push_back(bullet);
    addChild(bullet);
}

Player *Stage::getPlayer() {
    return _players[0];
}

Player *Stage::getOpponent() {
    return _players[1];
}

void Stage::setState(JSONPacker::GameState state) {
    if (getOpponent()->isCorrectUpdate(state.opponentPosition)) {
        getOpponent()->setPosition(state.opponentPosition);
    }
    getOpponent()->setMoveState(state.opponentMoveState);

    if (state.newBullet) {
        addBullet(state.newBullet);
    }
}

bool Stage::onContactBegin(cocos2d::PhysicsContact &contact) {
    CCLOG("onContactBegin");
    return true;
}
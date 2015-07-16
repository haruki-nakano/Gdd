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

    // setup map
    _map->setAnchorPoint(Vec2(0.0f, 0.0f));
    auto size = Director::getInstance()->getVisibleSize();
    _map->setPosition(size * 0.5f + _map->getContentSize() * -0.5f);

    this->addChild(_map);

    // setup player
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player *player = Player::create();
        player->setAnchorPoint(Vec2(0.5f, 0.5f));
        player->setPosition(Vec2(size.width * 0.5f, size.height * 0.5f));

        this->addChild(player);
        _players.push_back(player);
    }
}

void Stage::step() {
    auto player = this->getPlayer();
    Vec2 lastPlayerPos = player->getPosition();
    Vec2 currentPosition = this->getPosition();
    player->step();

    this->setPosition(this->getPosition() - (player->getPosition() - lastPlayerPos));

    for (size_t i = 0; i < _bullets.size(); i++) {
        _bullets[i]->step();
    }
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
    getOpponent()->setPosition(state.position);
}
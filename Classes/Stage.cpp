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

    _map = TMXTiledMap::create("stage1.tmx");
    auto layer = _map->getLayer("background");

    this->addChild(_map, 0, 99);

    // setup player
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player *player = Player::create();
        player->setAnchorPoint(Vec2(0.5f, 0.5f));
        // player->setPosition(Vec2(contentSize.width * 0.5f, contentSize.height * 0.5f));

        Size s = layer->getLayerSize();
        for (int x = 0; x < s.width; ++x) {
            for (int y = 0; y < s.height; ++y) {
                auto gid = static_cast<FieldType>(layer->getTileGIDAt(Vec2(x, y)));
                if (gid == FieldType::LAKE) {
                    auto tile = layer->getTileAt(Vec2(x, y));
                    Vec2 pos = tile->getPosition() + tile->getContentSize() * 0.5f;
                    player->setPosition(pos);
                }
            }
        }

        this->addChild(player);
        _players.push_back(player);
    }
}

void Stage::step() {
    _players[0]->step();

    for (size_t i = 0; i < _bullets.size(); i++) {
        _bullets[i]->step();

        /*
        if (!this->getTextureRect().containsPoint(_bullets[i]->getPosition())) {
            _bullets[i]->removeFromParent();
            _bullets.erase(_bullets.begin() + i);
            i--;
        }
        */
    }
}

void Stage::addBullet(Bullet *bullet) {
    _bullets.push_back(bullet);
    addChild(bullet);
}

Player *Stage::getPlayer() {
    return _players[0];
}

Player *Stage::getEnemy() {
    return _players[1];
}

void Stage::setState(JSONPacker::GameState state) {
    CCLOG("%f %f", _players[1]->getPosition().x, _players[1]->getPosition().y);
    _players[1]->setPosition(state.position);
    CCLOG("%f %f", _players[1]->getPosition().x, _players[1]->getPosition().y);
}
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

    _map = cocos2d::experimental::TMXTiledMap::create("stage2.tmx");
    _backgroundLayer = _map->getLayer("background");
    _map->setAnchorPoint(Vec2(0.0f, 0.0f));
    auto size = Director::getInstance()->getVisibleSize();
    _map->setPosition(size * 0.5f + _map->getContentSize() * -0.5f);

    this->addChild(_map, 0, 99);

    float x = _backgroundLayer->getProperty("x").asFloat();
    float y = _backgroundLayer->getProperty("y").asFloat();

    auto start = _backgroundLayer->getTileAt(Vec2(x, y));

    // setup player
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player *player = Player::create();
        player->setAnchorPoint(Vec2(0.5f, 0.5f));
        player->setPosition(Vec2(size.width * 0.5f, size.height * 0.5f));
        auto position = start->getPosition();
        // player->setPosition(position);

        this->addChild(player);
        _players.push_back(player);
    }
}

void Stage::step() {
    Vec2 pos = _players[0]->getPosition();
    MoveState moving = _players[0]->getMoveState();
    Vec2 currentPosition = this->getPosition();
    switch (moving) {
        case MoveState::STOP:
            break;

        case MoveState::LEFT:
            this->setPosition(currentPosition + Vec2(1.0f, -0.5f) * Player::MOVE_FACTOR);
            _players[0]->setPosition(pos + Vec2(-1.0f, 0.5f) * Player::MOVE_FACTOR);
            break;

        case MoveState::RIGHT:
            this->setPosition(currentPosition + Vec2(-1.0f, 0.5f) * Player::MOVE_FACTOR);
            _players[0]->setPosition(pos + Vec2(1.0f, -0.5f) * Player::MOVE_FACTOR);
            break;

        case MoveState::UP:
            this->setPosition(currentPosition + Vec2(-1.0f, -0.5f) * Player::MOVE_FACTOR);
            _players[0]->setPosition(pos + Vec2(1.0f, 0.5f) * Player::MOVE_FACTOR);
            break;

        case MoveState::DOWN:
            this->setPosition(currentPosition + Vec2(1.0f, 0.5f) * Player::MOVE_FACTOR);
            _players[0]->setPosition(pos + Vec2(-1.0f, -0.5f) * Player::MOVE_FACTOR);
            break;
    }
    // _players[0]->step();
    /*
    Vec2 coordinate = convertPositionToCoordinate(_players[0]->getPosition());
    if (collideWith(coordinate)) {
        _players[0]->setPosition(pos);
    }

     */
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
    // CCLOG("%f %f", _players[1]->getPosition().x, _players[1]->getPosition().y);
    _players[1]->setPosition(state.position);
    // CCLOG("%f %f", _players[1]->getPosition().x, _players[1]->getPosition().y);
}

Vec2 Stage::convertPositionToCoordinate(Vec2 position) {
    // TODO: this way searches is very slow way
    Size s = _backgroundLayer->getLayerSize();
    for (int x = 0; x < s.width; ++x) {
        for (int y = 0; y < s.height; ++y) {
            auto tile = _backgroundLayer->getTileAt(Vec2(x, y));
            if (tile && tile->getBoundingBox().containsPoint(position)) {
                return Vec2(x, y);
            }
        }
    }

    return Vec2(-1.0f, -1.0f);
}

bool Stage::collideWith(Vec2 coordinate) {
    // validate
    Size s = _backgroundLayer->getLayerSize();
    if (coordinate.x >= s.width || coordinate.y >= s.height || coordinate.x < 0 || coordinate.y < 0) {
        return false;
    }

    return (static_cast<FieldType>(_backgroundLayer->getTileGIDAt(coordinate)) == FieldType::WALL);
}
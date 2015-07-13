//
//  Stage.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "Stage.h"

#include "Player.h"
#include "Bullet.h"

using namespace cocos2d;

bool Stage::init() {
    if (!Sprite::initWithFile("stage.png")) {
        return false;
    }

    return true;
}

void Stage::onEnter() {
    Sprite::onEnter();

    Size contentSize = getContentSize();

    // setup player
    _player = Player::create();
    _player->setAnchorPoint(Vec2(0.5f, 0.5f));
    _player->setPosition(Vec2(contentSize.width * 0.5f, contentSize.height * 0.5f));

    this->addChild(_player);
}

void Stage::step() {
    _player->step();

    for (size_t i = 0; i < _bullets.size(); i++) {
        _bullets[i]->step();

        if (!this->getTextureRect().containsPoint(_bullets[i]->getPosition())) {
            _bullets[i]->removeFromParent();
            _bullets.erase(_bullets.begin() + i);
            i--;
        }
    }
}

void Stage::addBullet(Bullet *bullet) {
    _bullets.push_back(bullet);
    addChild(bullet);
}

Player *Stage::getPlayer() {
    return _player;
}
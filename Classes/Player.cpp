//
//  Player.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "Player.h"

#include "Bullet.h"
#include "Constants.h"
#include "MathUtils.h"

using namespace cocos2d;

bool Player::init() {
    if (!Sprite::initWithFile("playerUp.png")) {
        return false;
    }

    _imgLeft = Director::getInstance()->getTextureCache()->addImage("playerLeft.png");
    _imgUp = Director::getInstance()->getTextureCache()->addImage("playerUp.png");
    _imgRight = Director::getInstance()->getTextureCache()->addImage("playerRight.png");
    _imgDown = Director::getInstance()->getTextureCache()->addImage("playerDown.png");

    return true;
}

void Player::onEnter() {
    Sprite::onEnter();
}

void Player::step() {
    Vec2 currentPosition = this->getPosition();

    if (_moving != MoveState::STOP) {
        this->setPosition(currentPosition + _directionVec * MOVE_FACTOR);
    }
}

void Player::setMoveState(const MoveState MoveState) {
    _moving = MoveState;
    switch (MoveState) {
        case MoveState::STOP:
            break;

        case MoveState::LEFT:
            this->setTexture(_imgLeft);
            _directionVec = Vec2(-1.0f, 0.5f);
            break;

        case MoveState::RIGHT:
            this->setTexture(_imgRight);
            _directionVec = Vec2(1.0f, -0.5f);
            break;

        case MoveState::UP:
            this->setTexture(_imgUp);
            _directionVec = Vec2(1.0f, 0.5f);
            break;

        case MoveState::DOWN:
            this->setTexture(_imgDown);
            _directionVec = Vec2(-1.0f, -0.5f);
            break;
    }
}

MoveState Player::getMoveState() {
    return _moving;
}

void Player::setDirection(const Direction direction) {
    _direction = direction;
}

Direction Player::getDirection() {
    return _direction;
}

Bullet *Player::createBullet() {
    Bullet *bullet = Bullet::create();
    bullet->setPosition(this->getPosition());
    bullet->setDirection(_directionVec);
    return bullet;
}
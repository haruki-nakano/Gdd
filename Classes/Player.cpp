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
#include "GameScene.h"
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

    _moving = MoveState::STOP;
    _directionVec = Vec2::ZERO;

    return true;
}

void Player::onEnter() {
    Sprite::onEnter();
}

bool Player::isCorrectUpdate(const Vec2 position) {
    return GameScene::convertVec2ToMoveState(position - this->getPosition()) == _moving;
}

void Player::setMoveState(const MoveState MoveState) {
    _moving = MoveState;
    switch (MoveState) {
        case MoveState::STOP:
            this->getPhysicsBody()->setVelocity(Vec2::ZERO);
            return;

        case MoveState::LEFT:
            this->setTexture(_imgLeft);
            _directionVec = Vec2(-1.0f, 0.5f);
            this->getPhysicsBody()->setVelocity(_directionVec * DEFAULT_PLAYER_SPEED);
            break;

        case MoveState::RIGHT:
            this->setTexture(_imgRight);
            _directionVec = Vec2(1.0f, -0.5f);
            this->getPhysicsBody()->setVelocity(_directionVec * DEFAULT_PLAYER_SPEED);
            break;

        case MoveState::UP:
            this->setTexture(_imgUp);
            _directionVec = Vec2(1.0f, 0.5f);
            this->getPhysicsBody()->setVelocity(_directionVec * DEFAULT_PLAYER_SPEED);
            break;

        case MoveState::DOWN:
            this->setTexture(_imgDown);
            _directionVec = Vec2(-1.0f, -0.5f);
            this->getPhysicsBody()->setVelocity(_directionVec * DEFAULT_PLAYER_SPEED);
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
    bullet->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    bullet->setPosition(this->getPosition());
    bullet->setDirection(_directionVec);

    return bullet;
}
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

    _lifePoint = DEFAULT_PLAYER_LIFE;

    _isSwimming = false;

    return true;
}

void Player::onEnter() {
    Sprite::onEnter();

    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    PhysicsBody *playerPhysics = PhysicsBody::createBox(this->getBoundingBox().size);
    playerPhysics->setDynamic(true);
    playerPhysics->setGravityEnable(false);
    playerPhysics->setRotationEnable(false);
    playerPhysics->setCategoryBitmask(CATEGORY_MASK_PLAYER);
    playerPhysics->setCollisionBitmask(COLLISION_MASK_PLAYER);
    playerPhysics->setContactTestBitmask(CONTACT_MASK_PLAYER);

    this->setTag(CATEGORY_MASK_PLAYER);
    this->setPhysicsBody(playerPhysics);
}

bool Player::isCorrectUpdate(const Vec2 position) {
    return GameScene::convertVec2ToMoveState(position - this->getPosition()) == _moving;
}

bool Player::isSwimming() {
    return _isSwimming;
}

void Player::setIsSwimming(bool swimming, bool isOpponent) {
    int waterOpacity = isOpponent ? 0 : 128;
    if (_isSwimming != swimming) {
        this->setOpacity(swimming ? waterOpacity : 255);
    }
    _isSwimming = swimming;
}

void Player::setMoveState(const MoveState MoveState) {
    _moving = MoveState;
    switch (MoveState) {
        case MoveState::STOP:
            this->getPhysicsBody()->setVelocity(Vec2::ZERO);
            return;

        case MoveState::LEFT:
            this->setTexture(_imgLeft);
            // TODO: Fix here for the game balance
            _directionVec = Vec2(-1.18f, 0.0f);
            this->getPhysicsBody()->setVelocity(_directionVec * DEFAULT_PLAYER_SPEED);
            break;

        case MoveState::RIGHT:
            this->setTexture(_imgRight);
            _directionVec = Vec2(1.18f, 0.0f);
            this->getPhysicsBody()->setVelocity(_directionVec * DEFAULT_PLAYER_SPEED);
            break;

        case MoveState::UP:
            this->setTexture(_imgUp);
            _directionVec = Vec2(0.0f, 0.75f);
            this->getPhysicsBody()->setVelocity(_directionVec * DEFAULT_PLAYER_SPEED);
            break;

        case MoveState::DOWN:
            this->setTexture(_imgDown);
            _directionVec = Vec2(0.0f, -0.75f);
            this->getPhysicsBody()->setVelocity(_directionVec * DEFAULT_PLAYER_SPEED);
            break;

        case MoveState::UPPER_LEFT:
            this->setTexture(_imgLeft);
            // TODO: Fix here for the game balance
            _directionVec = Vec2(-1.0f, 0.5f);
            this->getPhysicsBody()->setVelocity(_directionVec * DEFAULT_PLAYER_SPEED);
            break;

        case MoveState::UPPER_RIGHT:
            this->setTexture(_imgRight);
            _directionVec = Vec2(1.0f, 0.5f);
            this->getPhysicsBody()->setVelocity(_directionVec * DEFAULT_PLAYER_SPEED);
            break;

        case MoveState::LOWER_LEFT:
            this->setTexture(_imgLeft);
            _directionVec = Vec2(-1.0f, -0.5f);
            this->getPhysicsBody()->setVelocity(_directionVec * DEFAULT_PLAYER_SPEED);
            break;

        case MoveState::LOWER_RIGHT:
            this->setTexture(_imgRight);
            _directionVec = Vec2(1.0f, -0.5f);
            this->getPhysicsBody()->setVelocity(_directionVec * DEFAULT_PLAYER_SPEED);
            break;
    }
}

MoveState Player::getMoveState() {
    return _moving;
}

void Player::setDirection(const Direction direction) {
    if (_direction == direction) {
        return;
    }
    _direction = direction;
    switch (direction) {
        case Direction::LEFT:
            this->setTexture(_imgLeft);
            break;

        case Direction::RIGHT:
            this->setTexture(_imgRight);
            break;

        case Direction::UP:
            this->setTexture(_imgUp);
            break;

        case Direction::DOWN:
            this->setTexture(_imgDown);
            break;
    }
}

Direction Player::getDirection() {
    return _direction;
}

Bullet *Player::createBullet() {
    Bullet *bullet = Bullet::create();
    bullet->setPosition(this->getPosition());
    bullet->setDirection(_directionVec);
    bullet->setTag(this->getTag() == TAG_PLAYER ? TAG_PLAYER_BULLET : TAG_OPPOPENT_BULLET);

    return bullet;
}

void Player::hitShot() {
    setLifePoint(getLifePoint() - 1);
}

void Player::setLifePoint(int lifePoint) {
    _lifePoint = lifePoint;
    if (_lifeBar) {
        _lifeBar->setLifePoint(lifePoint);
    }
}

int Player::getLifePoint() {
    return _lifePoint;
}

void Player::setLifeBar(LifeBar *lifeBar) {
    _lifeBar = lifeBar;
}
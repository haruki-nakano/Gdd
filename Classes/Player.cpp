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
    if (!Sprite::initWithFile("playerLeft.png")) {
        return false;
    }

    // FIXME: Improve
    // Use initialier
    _imgLeft = Director::getInstance()->getTextureCache()->addImage("playerLeft.png");
    _imgUp = Director::getInstance()->getTextureCache()->addImage("playerUp.png");
    _imgRight = Director::getInstance()->getTextureCache()->addImage("playerRight.png");
    _imgDown = Director::getInstance()->getTextureCache()->addImage("playerDown.png");
    _imgUpperLeft = Director::getInstance()->getTextureCache()->addImage("playerUpperLeft.png");
    _imgUpperRight = Director::getInstance()->getTextureCache()->addImage("playerUpperRight.png");
    _imgLowerLeft = Director::getInstance()->getTextureCache()->addImage("playerLowerLeft.png");
    _imgLowerRight = Director::getInstance()->getTextureCache()->addImage("playerLowerRight.png");

    _moving = MoveState::STOP;
    _directionVec = Vec2::ZERO;

    // Exclude STRAIGHT_GUN: 0
    _gun = static_cast<Gun>(random(1, static_cast<int>(Gun::SIZE) - 1));
    _gun = Gun::CHARGER;

    _lifePoint = INITIAL_PLAYER_LIFE;
    _hitCount = 0;
    _healCount = 0;

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
    return MathUtils::convertVec2ToMoveState(position - this->getPosition()) == _moving;
}

bool Player::isSwimming() {
    return _isSwimming;
}

void Player::setIsSwimming(bool swimming, bool isOpponent) {
    int waterOpacity = isOpponent ? 0 : 128;
    if (_isSwimming != swimming) {
        this->setOpacity(swimming ? waterOpacity : 255);
        setMoveState(getMoveState());
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
            // FIXME: Improvement
            _directionVec = Vec2(-1.18f, 0.0f);
            break;

        case MoveState::RIGHT:
            this->setTexture(_imgRight);
            _directionVec = Vec2(1.18f, 0.0f);
            break;

        case MoveState::UP:
            this->setTexture(_imgUp);
            _directionVec = Vec2(0.0f, 0.75f);
            break;

        case MoveState::DOWN:
            this->setTexture(_imgDown);
            _directionVec = Vec2(0.0f, -0.75f);
            break;

        case MoveState::UPPER_LEFT:
            this->setTexture(_imgUpperLeft);
            _directionVec = Vec2(-1.0f, 0.5f);
            break;

        case MoveState::UPPER_RIGHT:
            this->setTexture(_imgUpperRight);
            _directionVec = Vec2(1.0f, 0.5f);
            break;

        case MoveState::LOWER_LEFT:
            this->setTexture(_imgLowerLeft);
            _directionVec = Vec2(-1.0f, -0.5f);
            break;

        case MoveState::LOWER_RIGHT:
            this->setTexture(_imgLowerRight);
            _directionVec = Vec2(1.0f, -0.5f);
            break;
    }
    this->getPhysicsBody()->setVelocity(_directionVec * DEFAULT_PLAYER_SPEED *
                                        (isSwimming() && HIGH_SPEED_IN_WATER ? 1.8f : 1.0f));
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

std::vector<Bullet *> Player::createBullets(Vec2 touchPos, Vec2 stagePos) {
    static clock_t lastCreatedTime;

    std::vector<Bullet *> bullets;
    Vec2 playerVisiblePos = this->getPosition() + stagePos;
    float distance = touchPos.distance(playerVisiblePos);
    Vec2 v = (touchPos - playerVisiblePos) / distance;
    float angle = MathUtils::degreesAngle(v);
    // TODO
    switch (USE_SIMPLE_AIMING ? Gun::STRAIGHT_GUN : _gun) {
        case Gun::STRAIGHT_GUN: {
            Bullet *bullet = Bullet::create();
            bullet->setPosition(this->getPosition());
            bullet->setDirection(_directionVec);
            bullet->setTag(this->getTag() == TAG_PLAYER ? TAG_PLAYER_BULLET : TAG_OPPOPENT_BULLET);
            bullets.push_back(bullet);
            break;
        }
        case Gun::BASIC_GUN: {
            Bullet *bullet = Bullet::create();
            bullet->setPosition(this->getPosition());
            bullet->setDirection(v);
            bullet->setTag(this->getTag() == TAG_PLAYER ? TAG_PLAYER_BULLET : TAG_OPPOPENT_BULLET);
            bullets.push_back(bullet);

            bullet = Bullet::create();
            bullet->setPosition(this->getPosition());
            bullet->setDirection(v * 0.8f);
            bullet->setTag(this->getTag() == TAG_PLAYER ? TAG_PLAYER_BULLET : TAG_OPPOPENT_BULLET);
            bullets.push_back(bullet);
            break;
        }
        case Gun::THREE_WAY_GUN: {
            for (int i = 0; i < 3; i++) {
                Bullet *bullet = Bullet::create();
                bullet->setPosition(this->getPosition());
                bullet->setDirection(MathUtils::forDegreesAngle(angle - 10.0f * (i - 1)));
                bullet->setLifePoint(INITIAL_BULLET_LIFE * 0.5f);
                bullet->setTag(this->getTag() == TAG_PLAYER ? TAG_PLAYER_BULLET : TAG_OPPOPENT_BULLET);
                bullets.push_back(bullet);
            }
            break;
        }
        case Gun::SPRINKLER: {
            for (int i = 0; i < 8; i++) {
                Bullet *bullet = Bullet::create();
                bullet->setPosition(this->getPosition());
                bullet->setDirection(MathUtils::forDegreesAngle(angle - 45.0f * i));
                bullet->setLifePoint(INITIAL_BULLET_LIFE * 0.2f);
                bullet->setTag(this->getTag() == TAG_PLAYER ? TAG_PLAYER_BULLET : TAG_OPPOPENT_BULLET);
                bullets.push_back(bullet);
            }
            break;
        }
        case Gun::V_LASER_GUN: {
            for (int i = 0; i < 2; i++) {
                Bullet *bullet = Bullet::create();
                bullet->setPosition(this->getPosition());
                bullet->setDirection(MathUtils::forDegreesAngle(angle + 25.0f * (i == 0 ? -1 : 1)));
                bullet->setLifePoint(INITIAL_BULLET_LIFE * 1.2f);
                bullet->setTag(this->getTag() == TAG_PLAYER ? TAG_PLAYER_BULLET : TAG_OPPOPENT_BULLET);
                bullets.push_back(bullet);
            }
            break;
        }
        case Gun::MARATHON_GUN: {
            Bullet *bullet = Bullet::create();
            bullet->setPosition(this->getPosition());
            bullet->setDirection(v);
            bullet->setLifePoint(INITIAL_BULLET_LIFE * 10.0f);
            bullet->setTag(this->getTag() == TAG_PLAYER ? TAG_PLAYER_BULLET : TAG_OPPOPENT_BULLET);
            bullets.push_back(bullet);
            break;
        }
        case Gun::CHARGER: {
            if (clock() - lastCreatedTime < CLOCKS_PER_SEC * 2) {
                break;
            }

            lastCreatedTime = clock();
            for (int i = 0; i < 10; i++) {
                Bullet *bullet = Bullet::create();
                bullet->setPosition(this->getPosition());
                bullet->setDirection(v * (2.0f - 0.2f * i));
                bullet->setLifePoint(INITIAL_BULLET_LIFE * 2.0);
                bullet->setTag(this->getTag() == TAG_PLAYER ? TAG_PLAYER_BULLET : TAG_OPPOPENT_BULLET);
                bullets.push_back(bullet);
            }
            break;
        }
        default:
            break;
    }

    return bullets;
}

void Player::bulletHits(Bullet *bullet) {
    _hitCount++;
    updateLifePoint();
}

void Player::setHitCount(int hitCount) {
    _hitCount = hitCount;
    updateLifePoint();
}

int Player::getHitCount() {
    return _hitCount;
}

void Player::gotHeal() {
    // FIXME: Do not use magic number
    int heal = MIN(5, INITIAL_PLAYER_LIFE - getLifePoint());
    _healCount += heal;
    updateLifePoint();
}

void Player::setHealCount(int healCount) {
    _healCount = healCount;
    updateLifePoint();
}

int Player::getHealCount() {
    return _healCount;
}

void Player::updateLifePoint() {
    int lastLifePoint = _lifePoint;
    _lifePoint = INITIAL_PLAYER_LIFE - _hitCount + _healCount;
    // Clamp
    _lifePoint = MAX(MIN(_lifePoint, INITIAL_PLAYER_LIFE), 0);

    if (_lifeBar) {
        _lifeBar->setLifePoint(_lifePoint);
    }

    if (lastLifePoint > _lifePoint) {
        Sequence *blink = Sequence::create(Blink::create(0.4f, 4), Show::create(), NULL);
        this->runAction(blink);
    }
}
int Player::getLifePoint() {
    return _lifePoint;
}

void Player::setLifeBar(LifeBar *lifeBar) {
    _lifeBar = lifeBar;
}

const char *Player::getGunName() {
    return GUN_NAMES[static_cast<int>(_gun)];
}
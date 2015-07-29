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
    _gun = Gun::THREE_WAY_GUN;

    _lastTimeBulletCreated = 0;
    _lifePoint = INITIAL_PLAYER_LIFE;
    _hitCount = 0;
    _healCount = 0;

    _isSwimming = false;

    return true;
}

void Player::onEnter() {
    Sprite::onEnter();

    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    PhysicsMaterial material = PhysicsMaterial(99.0f, 0.0f, 1.0f);
    PhysicsBody *playerPhysics = PhysicsBody::createBox(this->getBoundingBox().size, material);
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

bool Player::isFiring() {
    return clock() - _lastTimeBulletCreated < KEEP_FIRING_THRESHOLD;
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
    switch (MoveState) {
        case MoveState::STOP:
            _moving = MoveState;
            this->getPhysicsBody()->setVelocity(Vec2::ZERO);
            return;

        case MoveState::LEFT:
            if (_moving == MoveState::STOP || !isFiring()) {
                this->setTexture(_imgLeft);
            }
            // FIXME: Improvement
            _directionVec = Vec2(-1.18f, 0.0f);
            break;

        case MoveState::RIGHT:
            if (_moving == MoveState::STOP || !isFiring()) {
                this->setTexture(_imgRight);
            }
            _directionVec = Vec2(1.18f, 0.0f);
            break;

        case MoveState::UP:
            if (_moving == MoveState::STOP || !isFiring()) {
                this->setTexture(_imgUp);
            }
            _directionVec = Vec2(0.0f, 0.95f);
            break;

        case MoveState::DOWN:
            if (_moving == MoveState::STOP || !isFiring()) {
                this->setTexture(_imgDown);
            }
            _directionVec = Vec2(0.0f, -0.95f);
            break;

        case MoveState::UPPER_LEFT:
            if (_moving == MoveState::STOP || !isFiring()) {
                this->setTexture(_imgUpperLeft);
            }
            _directionVec = Vec2(-1.0f, 0.5f);
            break;

        case MoveState::UPPER_RIGHT:
            if (_moving == MoveState::STOP || !isFiring()) {
                this->setTexture(_imgUpperRight);
            }
            _directionVec = Vec2(1.0f, 0.5f);
            break;

        case MoveState::LOWER_LEFT:
            if (_moving == MoveState::STOP || !isFiring()) {
                this->setTexture(_imgLowerLeft);
            }
            _directionVec = Vec2(-1.0f, -0.5f);
            break;

        case MoveState::LOWER_RIGHT:
            if (_moving == MoveState::STOP || !isFiring()) {
                this->setTexture(_imgLowerRight);
            }
            _directionVec = Vec2(1.0f, -0.5f);
            break;
    }
    _moving = MoveState;
    Vec2 newVelocity = _directionVec * DEFAULT_PLAYER_SPEED;
    if (isSwimming() && HIGH_SPEED_IN_WATER) {
        newVelocity *= 1.8f;
    }
    if (isFiring()) {
        newVelocity *= 0.5f;
    }
    this->getPhysicsBody()->setVelocity(newVelocity);
}

MoveState Player::getMoveState() {
    return _moving;
}

void Player::setDirection(const Direction direction) {
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

        case Direction::UPPER_LEFT:
            this->setTexture(_imgUpperLeft);
            break;

        case Direction::UPPER_RIGHT:
            this->setTexture(_imgUpperRight);
            break;
        case Direction::LOWER_LEFT:
            this->setTexture(_imgLowerLeft);
            break;

        case Direction::LOWER_RIGHT:
            this->setTexture(_imgLowerRight);
            break;
    }
}

std::vector<Bullet *> Player::createBullets(Vec2 touchPos, Vec2 stagePos) {
    std::vector<Bullet *> bullets;
    Vec2 playerVisiblePos = this->getPosition() + stagePos;
    float distance = touchPos.distance(playerVisiblePos);
    Vec2 v = (touchPos - playerVisiblePos) / distance;
    float angle = MathUtils::degreesAngle(v);

    setDirection(MathUtils::convertVec2ToDirection(v));

    // TODO: this function is too big
    switch (USE_SIMPLE_AIMING ? Gun::STRAIGHT_GUN : _gun) {
        case Gun::STRAIGHT_GUN: {
            if (clock() - _lastTimeBulletCreated < CLOCKS_PER_SEC * 0.1f) {
                break;
            }
            _lastTimeBulletCreated = clock();

            Bullet *bullet = Bullet::create();
            bullet->setPosition(this->getPosition());
            bullet->setDirection(_directionVec);
            bullet->setTag(this->getTag() == TAG_PLAYER ? TAG_PLAYER_BULLET : TAG_OPPOPENT_BULLET);
            bullets.push_back(bullet);
            break;
        }
        case Gun::BASIC_GUN: {
            if (clock() - _lastTimeBulletCreated < CLOCKS_PER_SEC * 0.1f) {
                break;
            }
            _lastTimeBulletCreated = clock();

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
            if (clock() - _lastTimeBulletCreated < ABNORMAL_FIRING_THRESHOLD) {
                break;
            }
            _lastTimeBulletCreated = clock();

            for (int i = 0; i < 3; i++) {
                Bullet *bullet = Bullet::create();
                bullet->setPosition(this->getPosition());
                bullet->setDirection(MathUtils::forDegreesAngle(angle - 10.0f * (i - 1)));
                bullet->setLifePoint(INITIAL_BULLET_LIFE * 0.7f);
                bullet->setTag(this->getTag() == TAG_PLAYER ? TAG_PLAYER_BULLET : TAG_OPPOPENT_BULLET);
                bullets.push_back(bullet);
            }
            break;
        }
        case Gun::SPRINKLER: {
            if (clock() - _lastTimeBulletCreated < ABNORMAL_FIRING_THRESHOLD) {
                break;
            }
            _lastTimeBulletCreated = clock();

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
            if (clock() - _lastTimeBulletCreated < ABNORMAL_FIRING_THRESHOLD) {
                break;
            }
            _lastTimeBulletCreated = clock();

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
            if (clock() - _lastTimeBulletCreated < ABNORMAL_FIRING_THRESHOLD * 2) {
                break;
            }
            _lastTimeBulletCreated = clock();

            Bullet *bullet = Bullet::create();
            bullet->setPosition(this->getPosition());
            bullet->setDirection(v);
            bullet->setLifePoint(INITIAL_BULLET_LIFE * 10.0f);
            bullet->setTag(this->getTag() == TAG_PLAYER ? TAG_PLAYER_BULLET : TAG_OPPOPENT_BULLET);
            bullets.push_back(bullet);
            break;
        }
        case Gun::CHARGER: {
            if (clock() - _lastTimeBulletCreated < CLOCKS_PER_SEC * 3) {
                break;
            }
            _lastTimeBulletCreated = clock();

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
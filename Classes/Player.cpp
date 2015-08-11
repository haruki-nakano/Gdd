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
    // _gun = Gun::V_LASER_GUN;

    _lastTimeBulletCreated = 0;
    _invincibleTimeCount = INVINCIBLE_TIME + 1.0f;
    _capturedTimeCount = GOGGLES_TIME + 1.0f;
    _addedWaterTimeCount = 0;
    _lifePoint = INITIAL_PLAYER_LIFE;
    _waterPoint = INITIAL_WATER_LIFE;
    _hitCount = 0;
    _healCount = 0;

    _waterBar = nullptr;

    _lastFiring = false;
    _lastInvincible = false;
    _lastCaptured = false;
    _isSwimming = false;

    return true;
}

void Player::onEnter() {
    Sprite::onEnter();

    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    PhysicsMaterial material = PhysicsMaterial(10.0f, 0.0f, 1.0f);

    Vec2 v[4];
    Vec2 s = Vec2(32, 16);
    v[0] = Vec2(-s.x, 0.0f);
    v[1] = Vec2(0.0f, s.y);
    v[2] = Vec2(s.x, 0.0f);
    v[3] = Vec2(0.0f, -s.y);
    PhysicsBody *playerPhysics = PhysicsBody::createPolygon(v, 4, material);
    playerPhysics->setDynamic(true);
    playerPhysics->setGravityEnable(false);
    playerPhysics->setRotationEnable(false);
    playerPhysics->setCategoryBitmask(CATEGORY_MASK_PLAYER);
    playerPhysics->setCollisionBitmask(COLLISION_MASK_PLAYER);
    playerPhysics->setContactTestBitmask(CONTACT_MASK_PLAYER);

    this->setTag(CATEGORY_MASK_PLAYER);
    this->setPhysicsBody(playerPhysics);

    _splash = Sprite::create();
    _splash->setPosition(Vec2(0.0f, getBoundingBox().size.height - 40.0f));
    this->addChild(_splash);
}

void Player::step(float dt) {
    // FIXME: this is ugly way to change the speed
    bool firing = isFiring();
    if (_lastFiring != firing) {
        setMoveState(getMoveState());
    }
    _lastFiring = firing;

    //
    _invincibleTimeCount = MIN(_invincibleTimeCount + dt, INVINCIBLE_TIME + 1.0f);
    bool invincible = isInvincible();
    if (!invincible && _lastInvincible) {
        // FIXME: All action? Use setTag
        this->stopAllActions();
        this->setColor(Color3B::WHITE);
    }
    _lastInvincible = invincible;

    //
    _capturedTimeCount = MIN(_capturedTimeCount + dt, GOGGLES_TIME + 1.0f);
    bool captured = isCaptured();
    if (captured != _lastCaptured) {
        setIsSwimming(isSwimming(), true);
    }
    _lastCaptured = captured;

    _addedWaterTimeCount += dt;
    if (_addedWaterTimeCount > ADD_WATTER_THRESHOLD) {
        _addedWaterTimeCount = 0;
        setWaterPoint(getWaterPoint() + 1);
    }
}

bool Player::isCorrectUpdate(const Vec2 position) const {
    return MathUtils::convertVec2ToMoveState(position - this->getPosition()) == _moving;
}

bool Player::isSwimming() const {
    return _isSwimming;
}

bool Player::isFiring() const {
    // TODO: Do not use clock. it depends on device.
    return clock() - _lastTimeBulletCreated < KEEP_FIRING_THRESHOLD;
}

bool Player::isCaptured() const {
    return _capturedTimeCount < GOGGLES_TIME;
}

bool Player::isInvincible() const {
    return _invincibleTimeCount < INVINCIBLE_TIME;
}

bool Player::isOpponent() const {
    return _isOpponent;
}

void Player::setIsSwimming(const bool swimming, const bool forceUpdate) {
    int waterOpacity = _isOpponent ? (isCaptured() ? 64 : 0) : 32;
    if (_isSwimming != swimming || forceUpdate) {
        // We update _isSwimming here because updateVelocity() accesses _isSwimming.
        _isSwimming = swimming;
        this->setOpacity(swimming ? waterOpacity : 255);
        setMoveState(getMoveState());
    }
}

void Player::setPlayerColor(const bool isHost) {
    if (isHost) {
        return;
    }
    _imgLeft = Director::getInstance()->getTextureCache()->addImage("player2Left.png");
    _imgUp = Director::getInstance()->getTextureCache()->addImage("player2Up.png");
    _imgRight = Director::getInstance()->getTextureCache()->addImage("player2Right.png");
    _imgDown = Director::getInstance()->getTextureCache()->addImage("player2Down.png");
    _imgUpperLeft = Director::getInstance()->getTextureCache()->addImage("player2UpperLeft.png");
    _imgUpperRight = Director::getInstance()->getTextureCache()->addImage("player2UpperRight.png");
    _imgLowerLeft = Director::getInstance()->getTextureCache()->addImage("player2LowerLeft.png");
    _imgLowerRight = Director::getInstance()->getTextureCache()->addImage("player2LowerRight.png");

    setDirection(getDirection());
}

void Player::setMoveState(const MoveState moveState) {
    _splash->stopAllActions();
    _splash->setVisible(false);

    switch (moveState) {
        case MoveState::STOP:
            _directionVec = Vec2::ZERO;
            break;

        case MoveState::LEFT: {
            if (_moving == MoveState::STOP || !isFiring()) {
                setDirection(Direction::LEFT);
            }
            _directionVec = Vec2(-1.18f, 0.0f);

            // FIXME: should use sprite sheet
            if (isSwimming() && !isFiring() && !_isOpponent) {
                _splash->setVisible(true);
                _splash->setPosition(Vec2(getBoundingBox().size.width, getBoundingBox().size.height - 40.0f));
                Animation *animation = Animation::create();

                // TODO: Use batchnode
                animation->addSpriteFrameWithFileName("splashLeft1.png");
                animation->addSpriteFrameWithFileName("splashLeft2.png");
                animation->addSpriteFrameWithFileName("splashLeft3.png");

                animation->setDelayPerUnit(0.1);
                //(4コマかける5回ループで2秒になり、移動同時に終了するようにしている)

                // 4. アニメーションの実行
                Animate *animate = Animate::create(animation);
                _splash->runAction(RepeatForever::create(animate));
            }
            break;
        }

        case MoveState::RIGHT:
            if (_moving == MoveState::STOP || !isFiring()) {
                setDirection(Direction::RIGHT);
                this->setTexture(_imgRight);
            }
            _directionVec = Vec2(1.18f, 0.0f);

            if (isSwimming() && !isFiring() && !_isOpponent) {
                _splash->setVisible(true);
                _splash->setPosition(Vec2(0.0f, getBoundingBox().size.height - 40.0f));
                Animation *animation = Animation::create();

                // TODO: Use batchnode
                animation->addSpriteFrameWithFileName("splashRight1.png");
                animation->addSpriteFrameWithFileName("splashRight2.png");
                animation->addSpriteFrameWithFileName("splashRight3.png");

                animation->setDelayPerUnit(0.1);
                //(4コマかける5回ループで2秒になり、移動同時に終了するようにしている)

                // 4. アニメーションの実行
                Animate *animate = Animate::create(animation);
                _splash->runAction(RepeatForever::create(animate));
            }
            break;

        case MoveState::UP:
            if (_moving == MoveState::STOP || !isFiring()) {
                setDirection(Direction::UP);
                this->setTexture(_imgUp);
            }
            _directionVec = Vec2(0.0f, 0.95f);

            if (isSwimming() && !isFiring() && !_isOpponent) {
                _splash->setVisible(true);
                _splash->setPosition(Vec2(getBoundingBox().size.width * 0.5f, getBoundingBox().size.height - 64.0f));
                Animation *animation = Animation::create();

                // TODO: Use batchnode
                animation->addSpriteFrameWithFileName("splashUp1.png");
                animation->addSpriteFrameWithFileName("splashUp2.png");
                animation->addSpriteFrameWithFileName("splashUp3.png");

                animation->setDelayPerUnit(0.1);
                //(4コマかける5回ループで2秒になり、移動同時に終了するようにしている)

                // 4. アニメーションの実行
                Animate *animate = Animate::create(animation);
                _splash->runAction(RepeatForever::create(animate));
            }
            break;

        case MoveState::DOWN:
            if (_moving == MoveState::STOP || !isFiring()) {
                setDirection(Direction::DOWN);
                this->setTexture(_imgDown);
            }
            _directionVec = Vec2(0.0f, -0.95f);

            if (isSwimming() && !isFiring() && !_isOpponent) {
                _splash->setVisible(true);
                _splash->setPosition(Vec2(getBoundingBox().size.width * 0.5f, getBoundingBox().size.height - 8.0f));
                Animation *animation = Animation::create();

                // TODO: Use batchnode
                animation->addSpriteFrameWithFileName("splashDown1.png");
                animation->addSpriteFrameWithFileName("splashDown2.png");
                animation->addSpriteFrameWithFileName("splashDown3.png");

                animation->setDelayPerUnit(0.1);
                //(4コマかける5回ループで2秒になり、移動同時に終了するようにしている)

                // 4. アニメーションの実行
                Animate *animate = Animate::create(animation);
                _splash->runAction(RepeatForever::create(animate));
            }
            break;

        case MoveState::UPPER_LEFT:
            if (_moving == MoveState::STOP || !isFiring()) {
                setDirection(Direction::UPPER_LEFT);
            }
            _directionVec = Vec2(-1.0f, 0.5f);

            if (isSwimming() && !isFiring() && !_isOpponent) {
                _splash->setVisible(true);
                _splash->setPosition(Vec2(getBoundingBox().size.width - 16.0f, getBoundingBox().size.height - 56.0f));
                Animation *animation = Animation::create();

                // TODO: Use batchnode
                animation->addSpriteFrameWithFileName("splashUpperLeft1.png");
                animation->addSpriteFrameWithFileName("splashUpperLeft2.png");
                animation->addSpriteFrameWithFileName("splashUpperLeft3.png");

                animation->setDelayPerUnit(0.1);
                //(4コマかける5回ループで2秒になり、移動同時に終了するようにしている)

                // 4. アニメーションの実行
                Animate *animate = Animate::create(animation);
                _splash->runAction(RepeatForever::create(animate));
            }
            break;

        case MoveState::UPPER_RIGHT:
            if (_moving == MoveState::STOP || !isFiring()) {
                setDirection(Direction::UPPER_RIGHT);
            }
            _directionVec = Vec2(1.0f, 0.5f);

            if (isSwimming() && !isFiring() && !_isOpponent) {
                _splash->setVisible(true);
                _splash->setPosition(Vec2(16.0f, getBoundingBox().size.height - 56.0f));
                Animation *animation = Animation::create();

                // TODO: Use batchnode
                animation->addSpriteFrameWithFileName("splashUpperRight1.png");
                animation->addSpriteFrameWithFileName("splashUpperRight2.png");
                animation->addSpriteFrameWithFileName("splashUpperRight3.png");

                animation->setDelayPerUnit(0.1);
                //(4コマかける5回ループで2秒になり、移動同時に終了するようにしている)

                // 4. アニメーションの実行
                Animate *animate = Animate::create(animation);
                _splash->runAction(RepeatForever::create(animate));
            }
            break;

        case MoveState::LOWER_LEFT:
            if (_moving == MoveState::STOP || !isFiring()) {
                setDirection(Direction::LOWER_LEFT);
            }
            _directionVec = Vec2(-1.0f, -0.5f);

            if (isSwimming() && !isFiring() && !_isOpponent) {
                _splash->setVisible(true);
                _splash->setPosition(Vec2(getBoundingBox().size.width, getBoundingBox().size.height - 32.0f));
                Animation *animation = Animation::create();

                // TODO: Use batchnode
                animation->addSpriteFrameWithFileName("splashLowerLeft1.png");
                animation->addSpriteFrameWithFileName("splashLowerLeft2.png");
                animation->addSpriteFrameWithFileName("splashLowerLeft3.png");

                animation->setDelayPerUnit(0.1);
                //(4コマかける5回ループで2秒になり、移動同時に終了するようにしている)

                // 4. アニメーションの実行
                Animate *animate = Animate::create(animation);
                _splash->runAction(RepeatForever::create(animate));
            }
            break;

        case MoveState::LOWER_RIGHT:
            if (_moving == MoveState::STOP || !isFiring()) {
                setDirection(Direction::LOWER_RIGHT);
            }
            _directionVec = Vec2(1.0f, -0.5f);

            if (isSwimming() && !isFiring() && !_isOpponent) {
                _splash->setVisible(true);
                _splash->setPosition(Vec2(0.0f, getBoundingBox().size.height - 32.0f));
                Animation *animation = Animation::create();

                // TODO: Use batchnode
                animation->addSpriteFrameWithFileName("splashLowerRight1.png");
                animation->addSpriteFrameWithFileName("splashLowerRight2.png");
                animation->addSpriteFrameWithFileName("splashLowerRight3.png");

                animation->setDelayPerUnit(0.1);
                //(4コマかける5回ループで2秒になり、移動同時に終了するようにしている)

                // 4. アニメーションの実行
                Animate *animate = Animate::create(animation);
                _splash->runAction(RepeatForever::create(animate));
            }
            break;
    }
    _moving = moveState;
    updateVelocity();
}

MoveState Player::getMoveState() const {
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

Direction Player::getDirection() const {
    return _direction;
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
            int requiredWater = 10;
            if (clock() - _lastTimeBulletCreated < ABNORMAL_FIRING_THRESHOLD || getWaterPoint() < requiredWater) {
                break;
            }
            _lastTimeBulletCreated = clock();
            setWaterPoint(getWaterPoint() - requiredWater);

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
            int requiredWater = 15;
            if (clock() - _lastTimeBulletCreated < ABNORMAL_FIRING_THRESHOLD || getWaterPoint() < requiredWater) {
                break;
            }
            _lastTimeBulletCreated = clock();
            setWaterPoint(getWaterPoint() - requiredWater);

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
            int requiredWater = 0;
            if (clock() - _lastTimeBulletCreated < ABNORMAL_FIRING_THRESHOLD || getWaterPoint() < requiredWater) {
                break;
            }
            _lastTimeBulletCreated = clock();
            setWaterPoint(getWaterPoint() - requiredWater);

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
            int requiredWater = 8;
            if (clock() - _lastTimeBulletCreated < ABNORMAL_FIRING_THRESHOLD || getWaterPoint() < requiredWater) {
                break;
            }
            _lastTimeBulletCreated = clock();
            setWaterPoint(getWaterPoint() - requiredWater);

            for (int i = 0; i < 4; i++) {
                Bullet *bullet = Bullet::create();
                bullet->setPosition(this->getPosition());
                v = MathUtils::forDegreesAngle(angle + 20.0f * (i % 2 == 0 ? -1 : 1));
                bullet->setDirection(i > 1 ? v : v * 0.8);
                bullet->setLifePoint(INITIAL_BULLET_LIFE * 1.2f);
                bullet->setTag(this->getTag() == TAG_PLAYER ? TAG_PLAYER_BULLET : TAG_OPPOPENT_BULLET);
                bullets.push_back(bullet);
            }
            break;
        }
        case Gun::MARATHON_GUN: {
            int requiredWater = 0;
            if (clock() - _lastTimeBulletCreated < ABNORMAL_FIRING_THRESHOLD || getWaterPoint() < requiredWater) {
                break;
            }
            _lastTimeBulletCreated = clock();
            setWaterPoint(getWaterPoint() - requiredWater);

            Bullet *bullet = Bullet::create();
            bullet->setPosition(this->getPosition());
            bullet->setDirection(v);
            bullet->setLifePoint(INITIAL_BULLET_LIFE * 10.0f);
            bullet->setTag(this->getTag() == TAG_PLAYER ? TAG_PLAYER_BULLET : TAG_OPPOPENT_BULLET);
            bullets.push_back(bullet);

            break;
        }
        case Gun::CHARGER: {
            int requiredWater = 100;
            if (clock() - _lastTimeBulletCreated < ABNORMAL_FIRING_THRESHOLD || getWaterPoint() < requiredWater) {
                break;
            }
            _lastTimeBulletCreated = clock();
            setWaterPoint(getWaterPoint() - requiredWater);

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
    if (isInvincible()) {
        return;
    }
    _hitCount++;
    updateLifePoint();
}

void Player::gotHeal() {
    int heal = MIN(HEALING_POINTS, INITIAL_PLAYER_LIFE - getLifePoint());
    _healCount += heal;
    setWaterPoint(INITIAL_WATER_LIFE);
    updateLifePoint();
    ParticleSystemQuad *particle = ParticleSystemQuad::create("particle_texture_heal.plist");
    particle->setPosition(Vec2(this->getBoundingBox().size.width * 0.5, 0.0f));
    // TODO here
    particle->setAutoRemoveOnFinish(true);
    this->addChild(particle);
}

void Player::captured() {
    _capturedTimeCount = 0.0;
}

void Player::gotInvincible() {
    auto action = TintTo::create(0.1, 255, 255, 64);
    auto action2 = TintTo::create(0.1, 255, 64, 255);
    auto action3 = TintTo::create(0.1, 64, 255, 255);
    auto seq = Sequence::create(action, action2, action3, NULL);
    this->runAction(RepeatForever::create(seq));
    _invincibleTimeCount = 0.0;
}

void Player::updateLifePoint() {
    int lastLifePoint = _lifePoint;
    _lifePoint = INITIAL_PLAYER_LIFE - _hitCount + _healCount;
    // Clamp
    _lifePoint = MAX(MIN(_lifePoint, INITIAL_PLAYER_LIFE), 0);

    if (_lifeBar) {
        _lifeBar->setPoint(_lifePoint);
    }

    if (lastLifePoint > _lifePoint) {
        Sequence *blink = Sequence::create(Blink::create(0.4f, 4), Show::create(), NULL);
        this->runAction(blink);
    }
}

void Player::updateVelocity() {
    Vec2 newVelocity = _directionVec * DEFAULT_PLAYER_SPEED;
    if (isInvincible() || (isSwimming() && HIGH_SPEED_IN_WATER)) {
        newVelocity *= 1.8f;
    }
    if (isFiring()) {
        newVelocity *= 0.5f;
    }

    this->getPhysicsBody()->setVelocity(newVelocity);
}

#pragma mark -
#pragma Getter / Setter

void Player::setHitCount(const int hitCount) {
    _hitCount = hitCount;
    updateLifePoint();
}

int Player::getHitCount() const {
    return _hitCount;
}

void Player::setHealCount(const int healCount) {
    _healCount = healCount;
    updateLifePoint();
}

int Player::getHealCount() const {
    return _healCount;
}

int Player::getLifePoint() const {
    return _lifePoint;
}

void Player::setLifeBar(Bar *lifeBar) {
    _lifeBar = lifeBar;
}

int Player::getWaterPoint() const {
    return _waterPoint;
}

void Player::setWaterPoint(const int waterPoint) {
    // Clamp
    _waterPoint = MAX(MIN(waterPoint, INITIAL_WATER_LIFE), 0);

    if (_waterBar) {
        _waterBar->setPoint(_waterPoint);
    }
}

void Player::setWaterBar(Bar *waterBar) {
    _waterBar = waterBar;
}

void Player::setLastTimeBulletCreated(const clock_t t) {
    _lastTimeBulletCreated = t;
}

void Player::setInvincibleStartTime(const clock_t t) {
    // _invincibleStartTime = t;
}

const char *Player::getGunName() {
    return GUN_NAMES[static_cast<int>(_gun)];
}

void Player::setGun(const Gun gun) {
    _gun = gun;
}
//
//  Egg.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/23.
//
//

#include "Egg.h"
#include "Constants.h"

using namespace cocos2d;

bool Egg::init() {
    if (!Sprite::initWithFile("egg.png")) {
        return false;
    }

    this->setTag(TAG_EGG);

    // TODO: fix here
    _lifePoint = 10;
    _owner = nullptr;
    _lastBrokenTime = 0;

    return true;
}

void Egg::onEnter() {
    Sprite::onEnter();

    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    PhysicsBody *physics = PhysicsBody::createBox(this->getBoundingBox().size);
    physics->setDynamic(true);
    physics->setGravityEnable(false);
    physics->setRotationEnable(false);
    physics->setCategoryBitmask(CATEGORY_MASK_EGG);
    physics->setCollisionBitmask(COLLISION_MASK_EGG);
    physics->setContactTestBitmask(CONTACT_MASK_EGG);

    this->setPhysicsBody(physics);
}

#pragma mark -
#pragma mark Getter/Setter

int Egg::getLifePoint() {
    return _lifePoint;
}

void Egg::setLifePoint(int lifePoint) {
    _lifePoint = lifePoint;
    if (lifePoint <= 0) {
        // TODO
        setVisible(false);
        setPosition(-1.0, -1.0);
        _lastBrokenTime = clock();
    } else {
        setVisible(true);
    }
}

clock_t Egg::getLastBrokenTime() {
    return _lastBrokenTime;
}

Player *Egg::getOwner() {
    return _owner;
}

void Egg::setOwner(Player *player) {
    _owner = player;
}

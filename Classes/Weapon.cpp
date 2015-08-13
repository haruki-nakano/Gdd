//
//  Weapon.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/08/13.
//
//

#include "Weapon.h"

#include "PhysicsConstants.h"

using namespace cocos2d;

bool Weapon::init() {
    if (!Sprite::initWithFile("itemHot.png")) {
        return false;
    }

    return true;
}

void Weapon::onEnter() {
    Sprite::onEnter();

    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    PhysicsBody *physics = PhysicsBody::createBox(this->getBoundingBox().size * 0.5);
    physics->setDynamic(false);
    physics->setGravityEnable(false);
    physics->setRotationEnable(false);
    physics->setCategoryBitmask(CATEGORY_MASK_ITEM);
    physics->setCollisionBitmask(COLLISION_MASK_ITEM);
    physics->setContactTestBitmask(CONTACT_MASK_ITEM);
    this->setPhysicsBody(physics);
    this->setTag(TAG_WEAPON);
}

#pragma mark -
#pragma mark Getter/Setter

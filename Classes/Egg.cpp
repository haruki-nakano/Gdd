//
//  Egg.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/23.
//
//

#include "Egg.h"

using namespace cocos2d;

bool Egg::init() {
    if (!Sprite::initWithFile("egg.png")) {
        return false;
    }

    this->setTag(TAG_EGG);

    _lifePoint = INITIAL_EGG_LIFE;
    _lastBrokenTime = 0;

    _item = EggItemType::HEALING;

    _egg = Director::getInstance()->getTextureCache()->addImage("egg.png");
    _egg2 = Director::getInstance()->getTextureCache()->addImage("egg2.png");
    _egg3 = Director::getInstance()->getTextureCache()->addImage("egg3.png");
    return true;
}

void Egg::onEnter() {
    Sprite::onEnter();

    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    // FIXME: Critical
    PhysicsBody *physics = PhysicsBody::createBox(this->getBoundingBox().size);
    physics->setDynamic(false);
    physics->setGravityEnable(false);
    physics->setRotationEnable(false);
    physics->setCategoryBitmask(CATEGORY_MASK_EGG);
    physics->setCollisionBitmask(COLLISION_MASK_EGG);
    physics->setContactTestBitmask(CONTACT_MASK_EGG);

    this->setPhysicsBody(physics);
}

#pragma mark -
#pragma mark Getter/Setter

int Egg::getLifePoint() const {
    return _lifePoint;
}

void Egg::setLifePoint(const int lifePoint) {
    int lastLifePoint = _lifePoint;
    _lifePoint = lifePoint;

    if (lifePoint <= 0) {
        // Set disable if egg is broken
        this->setVisible(false);
        // FIXME: Critical
        this->setPosition(-1, -1);
        _lastBrokenTime = clock();
    } else {
        setVisible(true);
        if (lastLifePoint > lifePoint) {
            Sequence *blink = Sequence::create(Blink::create(0.4f, 4), Show::create(), NULL);
            this->runAction(blink);
        }
    }

    if (_lifePoint > INITIAL_EGG_LIFE * 0.7) {
        this->setTexture(_egg);
    } else if (_lifePoint > INITIAL_EGG_LIFE * 0.3) {
        this->setTexture(_egg2);
    } else {
        this->setTexture(_egg3);
    }
}

clock_t Egg::getLastBrokenTime() const {
    return _lastBrokenTime;
}

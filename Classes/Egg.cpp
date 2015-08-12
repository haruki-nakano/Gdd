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
    if (!Sprite::initWithFile("itemEgg.png")) {
        return false;
    }

    this->setTag(TAG_EGG);

    _lifePoint = INITIAL_EGG_LIFE;
    _state = EggState::EGG;
    _lastBrokenTime = 0;

    setItemType(static_cast<EggItemType>(random(0, static_cast<int>(EggItemType::SIZE))));

    _egg = Director::getInstance()->getTextureCache()->addImage("itemEgg.png");
    _egg2 = Director::getInstance()->getTextureCache()->addImage("itemEgg2.png");
    _egg3 = Director::getInstance()->getTextureCache()->addImage("itemEgg3.png");
    return true;
}

void Egg::onEnter() {
    Sprite::onEnter();

    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    PhysicsBody *physics = PhysicsBody::createBox(this->getBoundingBox().size * 0.5);
    physics->setDynamic(false);
    physics->setGravityEnable(false);
    physics->setRotationEnable(false);
    physics->setCategoryBitmask(CATEGORY_MASK_EGG);
    physics->setCollisionBitmask(COLLISION_MASK_EGG);
    physics->setContactTestBitmask(CONTACT_MASK_EGG);
    this->setPhysicsBody(physics);

    setState(EggState::IDLE);
}

#pragma mark -
#pragma mark Getter/Setter

EggState Egg::getState() const {
    return _state;
}
void Egg::setState(const EggState state) {
    if (_state == state) {
        return;
    }
    _state = state;

    PhysicsBody *physics;
    switch (state) {
        case EggState::IDLE:
            this->setVisible(false);
            this->setPosition(-1, -1);
            this->setTexture(_egg);
            _lifePoint = INITIAL_EGG_LIFE;
            _lastBrokenTime = clock();
            physics = this->getPhysicsBody();
            this->setTag(TAG_EGG);
            physics->setCategoryBitmask(CATEGORY_MASK_EGG);
            physics->setCollisionBitmask(COLLISION_MASK_EGG);
            physics->setContactTestBitmask(CONTACT_MASK_EGG);
            break;
        case EggState::EGG:
            setVisible(true);
            break;
        case EggState::ITEM:
            this->setTexture(_item);
            this->stopAllActions();
            this->setVisible(true);
            this->setTag(TAG_ITEM);
            physics = this->getPhysicsBody();
            physics->setCategoryBitmask(CATEGORY_MASK_ITEM);
            physics->setCollisionBitmask(COLLISION_MASK_ITEM);
            physics->setContactTestBitmask(CONTACT_MASK_ITEM);
            break;

        default:
            break;
    }
}

int Egg::getLifePoint() const {
    return _lifePoint;
}

void Egg::setLifePoint(const int lifePoint) {
    if (getState() != EggState::EGG) {
        return;
    }

    int lastLifePoint = _lifePoint;
    _lifePoint = MAX(lifePoint, 0);

    if (lastLifePoint > lifePoint) {
        Sequence *blink = Sequence::create(Blink::create(0.4f, 4), Show::create(), NULL);
        this->runAction(blink);
    }

    if (_lifePoint > INITIAL_EGG_LIFE * 0.7) {
        this->setTexture(_egg);
    } else if (_lifePoint > INITIAL_EGG_LIFE * 0.3) {
        this->setTexture(_egg2);
    } else if (_lifePoint > 0) {
        this->setTexture(_egg3);
    } else {
        setState(EggState::ITEM);
    }
}

EggItemType Egg::getItemType() const {
    return _itemType;
}

void Egg::setItemType(const EggItemType itemType) {
    _itemType = itemType;
    if (_itemType == EggItemType::SUPER_STAR) {
        _item = Director::getInstance()->getTextureCache()->addImage("itemSuperStar.png");
    } else if (_itemType == EggItemType::GOGGLES) {
        _item = Director::getInstance()->getTextureCache()->addImage("itemGoggles.png");
    } else if (_itemType == EggItemType::HEALING) {
        _item = Director::getInstance()->getTextureCache()->addImage("itemHealing.png");
    } else if (_itemType == EggItemType::RED_PEPPER) {
        _item = Director::getInstance()->getTextureCache()->addImage("itemHot.png");
    }
}

clock_t Egg::getLastBrokenTime() const {
    return _lastBrokenTime;
}

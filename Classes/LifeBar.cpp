//
//  LifeBar.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/21.
//
//

#include "LifeBar.h"

#include "cocos2d.h"
#include "Constants.h"

using namespace cocos2d;

bool LifeBar::init() {
    if (!Node::init()) {
        return false;
    }

    return true;
}
void LifeBar::onEnter() {
    Node::onEnter();

    _bar = Sprite::create("life.png");
    auto bg = Sprite::create("life_bg.png");

    _bar->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    _bar->setPosition(Vec2(9.0f - bg->getBoundingBox().size.width * 0.5, 0.0f));
    bg->setPosition(Vec2::ZERO);

    this->addChild(bg);
    this->addChild(_bar);
}

void LifeBar::setLifePoint(int lifePoints) {
    float remains = clampf((float)lifePoints / (float)MAX_PLAYER_LIFE, 0.0f, 100.0f);
    _bar->setScaleX(remains);
}
//
//  LifeBar.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/21.
//
//

#include "LifeBar.h"

#include "cocos2d.h"

using namespace cocos2d;

bool LifeBar::init() {
    if (!Node::init()) {
        return false;
    }

    return true;
}
void LifeBar::onEnter() {
    Node::onEnter();

    auto bar = Sprite::create("life.png");
    auto bg = Sprite::create("life_bg.png");

    bar->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    bar->setPosition(Vec2::ZERO);
    bg->setPosition(Vec2::ZERO);

    this->addChild(bg);
    this->addChild(bar);
}

void setLifePoint(int lifePoints) {
}
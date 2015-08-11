//
//  Bar.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/08/11.
//
//

#include "Bar.h"

#include "Constants.h"
#include "cocos-ext.h"

using namespace cocos2d;

USING_NS_CC_EXT;

bool Bar::init(Type type) {
    if (!Node::init()) {
        return false;
    }

    setType(type);

    return true;
}

void Bar::onEnter() {
    Node::onEnter();

    auto bg = Scale9Sprite::create("uiLifeBg.png", Rect(0, 0, 154, 41), Rect(11, 11, 132, 19));
    switch (_type) {
        case LIFE: {
            _bar = Scale9Sprite::create("uiLife.png", Rect(0, 0, 136, 25), Rect(3, 3, 130, 19));
            bg->setContentSize(Size(300.0f, 44.0f));
            _bar->setContentSize(Size(300.0f - 16.0f, 44.0f - 16.0f));

            _initialPoint = INITIAL_PLAYER_LIFE;
            break;
        }
        case CHARGING: {
            _bar = Scale9Sprite::create("uiCharging.png", Rect(0, 0, 136, 25), Rect(3, 3, 130, 19));
            bg->setContentSize(Size(300.0f, 28.0f));
            _bar->setContentSize(Size(300.0f - 16.0f, 28.0f - 16.0f));

            _initialPoint = INITIAL_WATER_LIFE;
            break;
        }
    }

    _initialContentsize = _bar->getContentSize();
    _bar->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    _bar->setPosition(Vec2(8.0f - bg->getBoundingBox().size.width * 0.5, 0.0f));
    bg->setPosition(Vec2::ZERO);

    this->addChild(bg);
    this->addChild(_bar);
}

void Bar::setType(Type type) {
    _type = type;
}

void Bar::setPoint(const int point) {
    if (point == 0) {
        _bar->setVisible(false);
    } else {
        _bar->setVisible(true);
        float remains = clampf((float)point / (float)_initialPoint, 0.0f, 100.0f);
        _bar->setContentSize(Size(_initialContentsize.width * remains, _initialContentsize.height));
    }
}

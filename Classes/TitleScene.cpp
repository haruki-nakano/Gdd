//
//  TitleScene.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "TitleScene.h"

USING_NS_CC;

Scene *TitleScene::createScene() {
    auto scene = Scene::create();
    auto layer = TitleScene::create();

    scene->addChild(layer);

    return scene;
}

bool TitleScene::init() {
    if (!Layer::init()) {
        return false;
    }

    LayerColor *background = LayerColor::create(Color4B(255, 255, 255, 255));

    this->addChild(background);

    return true;
}

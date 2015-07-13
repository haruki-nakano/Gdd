//
//  GameScene.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "GameScene.h"

#include "Player.h"
#include "SceneManager.h"

using namespace cocos2d;

#pragma mark -
#pragma mark Lifycycle

bool GameScene::init() {
    if (!Node::init()) {
        return false;
    }

    LayerColor *background = LayerColor::create(Color4B(255, 255, 255, 255));
    this->addChild(background);

    return true;
}
void GameScene::onEnter() {
    Node::onEnter();

    Size visibleSize = Director::getInstance()->getVisibleSize();

    // setup player
    player = Player::create();
    player->setAnchorPoint(Vec2(0.5f, 0.5f));
    player->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.5f));

    this->addChild(player);

    // setup menus
    ui::Button *backButton = ui::Button::create();
    backButton->setAnchorPoint(Vec2(0.0f, 1.0f));
    backButton->setPosition(Vec2(0.0f, visibleSize.height));
    backButton->loadTextures("backButton.png", "backButtonPressed.png");
    backButton->addTouchEventListener(CC_CALLBACK_2(GameScene::backButtonPressed, this));

    this->addChild(backButton);

    setupTouchHandling();
}

void GameScene::setupTouchHandling() {
    auto touchListener = EventListenerTouchOneByOne::create();

    touchListener->onTouchBegan = [&](Touch *touch, Event *event) { return true; };

    touchListener->onTouchMoved = [&](Touch *touch, Event *event) {};

    touchListener->onTouchEnded = [&](Touch *touch, Event *event) {};

    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

#pragma mark -
#pragma mark UI Methods

void GameScene::backButtonPressed(cocos2d::Ref *pSender, ui::Widget::TouchEventType eEventType) {
    if (eEventType == ui::Widget::TouchEventType::ENDED) {
        SceneManager::getInstance()->returnToLobby();
    }
}

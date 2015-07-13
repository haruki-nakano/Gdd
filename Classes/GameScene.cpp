//
//  GameScene.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "GameScene.h"

#include "Constants.h"
#include "Player.h"
#include "SceneManager.h"
#include "Stage.h"

using namespace cocos2d;

#pragma mark -
#pragma mark Lifycycle

bool GameScene::init() {
    if (!Node::init()) {
        return false;
    }

    LayerColor *background = LayerColor::create(Color4B(255, 255, 255, 255));
    this->addChild(background);

    _active = false;
    _stepInterval = INITIAL_STEP_INTERVAL;

    return true;
}
void GameScene::onEnter() {
    Node::onEnter();

    Size visibleSize = Director::getInstance()->getVisibleSize();

    // setup stage
    _stage = Stage::create();
    _stage->setAnchorPoint(Vec2(0.5f, 0.5f));
    _stage->setPosition(visibleSize * 0.5f);

    this->addChild(_stage);

    // setup menus
    ui::Button *backButton = ui::Button::create();
    backButton->setAnchorPoint(Vec2(0.0f, 1.0f));
    backButton->setPosition(Vec2(0.0f, visibleSize.height));
    backButton->loadTextures("backButton.png", "backButtonPressed.png");
    backButton->addTouchEventListener(CC_CALLBACK_2(GameScene::backButtonPressed, this));

    this->addChild(backButton);

    setupTouchHandling();

    setGameActive(true);
}

void GameScene::setupTouchHandling() {
    static Vec2 firstTouchPos;
    static bool isTap;

    auto touchListener = EventListenerTouchOneByOne::create();

    touchListener->onTouchBegan = [&](Touch *touch, Event *event) {
        firstTouchPos = this->convertTouchToNodeSpace(touch);
        isTap = true;
        return true;
    };

    touchListener->onTouchMoved = [&](Touch *touch, Event *event) {
        Vec2 touchPos = this->convertTouchToNodeSpace(touch);
        isTap = false;
        float distance = touchPos.distance(firstTouchPos);
        distance = sqrtf(distance);
        _stage->getPlayer()->setDirection((touchPos - firstTouchPos) / distance);
    };

    touchListener->onTouchEnded = [&](Touch *touch, Event *event) {
        if (isTap) {
            Bullet *bullet = _stage->getPlayer()->createBullet();
            _stage->addBullet(bullet);
        } else {
            _stage->getPlayer()->stop();
        }
    };

    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

#pragma mark -
#pragma mark Game Logics

void GameScene::setGameActive(bool active) {
    _active = active;
    if (active) {
        this->schedule(CC_SCHEDULE_SELECTOR(GameScene::step), _stepInterval);
        this->scheduleUpdate();
    } else {
        this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::step));
        this->unscheduleUpdate();
    }
}

void GameScene::step(float dt) {
    _stage->step();
}

#pragma mark -
#pragma mark UI Methods

void GameScene::backButtonPressed(cocos2d::Ref *pSender, ui::Widget::TouchEventType eEventType) {
    if (eEventType == ui::Widget::TouchEventType::ENDED) {
        SceneManager::getInstance()->returnToLobby();
    }
}

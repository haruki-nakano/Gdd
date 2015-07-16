//
//  GameScene.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "GameScene.h"

#include "Constants.h"
#include "MathUtils.h"
#include "JSONPacker.h"
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

    // TODO: Add background image
    LayerColor *background = LayerColor::create(Color4B(255, 255, 255, 255));
    this->addChild(background);

    _active = false;
    _networkedSession = false;
    _stepInterval = INITIAL_STEP_INTERVAL;

    return true;
}
void GameScene::onEnter() {
    Node::onEnter();

    Size visibleSize = Director::getInstance()->getVisibleSize();

    // setup stage
    _stage = Stage::create();
    _stage->setAnchorPoint(Vec2(0.0f, 0.0f));
    _stage->setPosition(Vec2(0.0f, 0.0f));

    this->addChild(_stage);

    // setup menus
    // TODO: Use other button image
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
    static Vec2 lastSyncPos;
    static bool isTap;

    auto touchListener = EventListenerTouchOneByOne::create();

    touchListener->onTouchBegan = [&](Touch *touch, Event *event) {
        firstTouchPos = this->convertTouchToNodeSpace(touch);
        lastSyncPos = firstTouchPos;
        isTap = true;
        return true;
    };

    touchListener->onTouchMoved = [&](Touch *touch, Event *event) {
        Vec2 touchPos = this->convertTouchToNodeSpace(touch);
        isTap = false;
        float distance = touchPos.distance(firstTouchPos);
        distance = sqrtf(distance);
        MoveState move = convertVec2ToMoveState((touchPos - firstTouchPos) / distance);
        _stage->getPlayer()->setMoveState(move);
        if (_networkedSession && lastSyncPos.distance(touchPos) > 2.0f) {
            sendGameStateOverNetwork();
            lastSyncPos = touchPos;
        }
    };

    touchListener->onTouchEnded = [&](Touch *touch, Event *event) {
        if (isTap) {
            Bullet *bullet = _stage->getPlayer()->createBullet();
            _stage->addBullet(bullet);
        } else {
            _stage->getPlayer()->setMoveState(MoveState::STOP);
        }
    };

    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

#pragma mark -
#pragma mark Networks

void GameScene::setNetworkedSession(bool networkedSession) {
    _networkedSession = networkedSession;
}

void GameScene::receivedData(const void *data, unsigned long length) {
    const char *cstr = reinterpret_cast<const char *>(data);
    std::string json = std::string(cstr, length);

    JSONPacker::GameState state = JSONPacker::unpackGameStateJSON(json);

    if (state.gameOver) {
        // TODO: Implement here
    }

    // CCLOG("received date: %f", (float)clock() / CLOCKS_PER_SEC);
    _stage->setState(state);
}

void GameScene::sendGameStateOverNetwork() {
    // CCLOG("send date: %f", (float)clock() / CLOCKS_PER_SEC);
    JSONPacker::GameState state;

    state.name = NetworkingWrapper::getDeviceName();
    state.gameOver = false;
    state.position = _stage->getPlayer()->getPosition();

    std::string json = JSONPacker::packGameStateJSON(state);
    SceneManager::getInstance()->sendData(json.c_str(), json.length());
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

#pragma mark -
#pragma mark UtilityMethods

MoveState GameScene::convertVec2ToMoveState(cocos2d::Vec2 v) {
    if (v.distance(Vec2(0.0f, 0.0f)) < 1.0f) {
        return MoveState::STOP;
    }

    float angle = MathUtils::degreesAngle(v);

    // TODO: Fix angles
    if (-180 <= angle && angle < -90.0f) {
        return MoveState::DOWN;
    } else if (-90.0f <= angle && angle < 0.0f) {
        return MoveState::LEFT;
    } else if (0.0f <= angle && angle < 90.0f) {
        return MoveState::UP;
    } else {
        return MoveState::RIGHT;
    }
}
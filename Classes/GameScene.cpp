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

    if (_networkedSession) {
        _stage->initializePlayersPosition(_isHost);
    } else {
        _stage->initializePlayersPosition(true);
    }

    // setup menus
    // TODO: Use other button image
    ui::Button *backButton = ui::Button::create();
    backButton->setAnchorPoint(Vec2(0.0f, 1.0f));
    backButton->setPosition(Vec2(0.0f, visibleSize.height));
    backButton->loadTextures("backButton.png", "backButtonPressed.png");
    backButton->addTouchEventListener(CC_CALLBACK_2(GameScene::backButtonPressed, this));

    this->addChild(backButton);

    _playerLifeBar = LifeBar::create();
    _playerLifeBar->setPosition(Vec2(visibleSize.width * 0.5 - 256.0f, visibleSize.height - 32.0f));

    _opponentsLifeBar = LifeBar::create();
    _opponentsLifeBar->setPosition(Vec2(visibleSize.width * 0.5 + 256.0f, visibleSize.height - 32.0f));

    this->addChild(_playerLifeBar);
    this->addChild(_opponentsLifeBar);

    _stage->getPlayer()->setLifeBar(_playerLifeBar);
    _stage->getOpponent()->setLifeBar(_opponentsLifeBar);

    setupTouchHandling();

    // setup contact handling
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    setGameActive(true);
}

void GameScene::setupTouchHandling() {
    static Vec2 firstTouchPos;
    static Vec2 lastSyncPos;
    static bool isTap;
    static int firstFingerId = -1;
    static int numFingers = 0;

    auto touchListener = EventListenerTouchOneByOne::create();

    touchListener->onTouchBegan = [&](Touch *touch, Event *event) {
        numFingers++;
        if (numFingers != 1) {
            return true;
        }
        firstFingerId = touch->getID();
        auto player = _stage->getPlayer()->getPosition();
        firstTouchPos = this->convertTouchToNodeSpace(touch);
        lastSyncPos = firstTouchPos;
        isTap = true;
        return true;
    };

    touchListener->onTouchMoved = [&](Touch *touch, Event *event) {
        if (touch->getID() != firstFingerId) {
            return;
        }
        Vec2 touchPos = this->convertTouchToNodeSpace(touch);
        isTap = false;
        float distance = touchPos.distance(firstTouchPos);
        distance = sqrtf(distance);
        MoveState move = convertVec2ToMoveState((touchPos - firstTouchPos) / distance);
        MoveState lastMoveState = _stage->getPlayer()->getMoveState();
        _stage->getPlayer()->setMoveState(move);
        if (_networkedSession && (move != lastMoveState || lastSyncPos.distance(touchPos) > 10.0f)) {
            // if (_networkedSession && move != lastMoveState) {
            sendGameStateOverNetwork(nullptr);
            lastSyncPos = touchPos;
        }
    };

    touchListener->onTouchEnded = [&](Touch *touch, Event *event) {
        if (isTap || touch->getID() != firstFingerId) {
            // TODO: Fix here for the game balance
            Vec2 touchPos = this->convertTouchToNodeSpace(touch);
            auto playerPos = _stage->getPlayer()->getPosition();
            auto stagePos = _stage->getPosition();
            Vec2 playerVisiblePos = playerPos + stagePos;
            Bullet *bullet = _stage->getPlayer()->createBullet();
            float distance = touchPos.distance(playerVisiblePos);
            bullet->setDirection((touchPos - playerVisiblePos) / distance);
            _stage->addBullet(bullet);
            if (_networkedSession) {
                // TODO: Try here many times
                sendGameStateOverNetwork(bullet);
            }
        } else {
            _stage->getPlayer()->setMoveState(MoveState::STOP);
            if (_networkedSession) {
                // TODO: Is it effective?
                sendGameStateOverNetwork(nullptr);
                sendGameStateOverNetwork(nullptr);
                sendGameStateOverNetwork(nullptr);
                sendGameStateOverNetwork(nullptr);
            }
        }

        if (numFingers == 1) {
            firstFingerId = -1;
        }
        numFingers--;
    };

    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

bool GameScene::onContactBegin(cocos2d::PhysicsContact &contact) {
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();

    int tagA = nodeA->getTag();
    int tagB = nodeB->getTag();

    // CCLOG("onContactBegin: %d %d", nodeA->getTag(), nodeB->getTag());

    // My shot hits other player
    Player *player = nullptr;
    Bullet *bullet = nullptr;
    if (tagA == TAG_OPPOPENT && tagB == TAG_PLAYER_BULLET) {
        player = dynamic_cast<Player *>(nodeA);
        bullet = dynamic_cast<Bullet *>(nodeB);
    } else if (tagB == TAG_OPPOPENT && tagA == TAG_PLAYER_BULLET) {
        player = dynamic_cast<Player *>(nodeB);
        bullet = dynamic_cast<Bullet *>(nodeA);
    }
    if (player && bullet) {
        player->hitShot();
        bullet->setLifePoint(-1.0f);
        if (player->getLifePoint() <= 0) {
            gameOver();
        }
        return false;
    }

    // Other players shot hits me
    if (tagA == TAG_PLAYER && tagB == TAG_OPPOPENT_BULLET) {
        player = dynamic_cast<Player *>(nodeA);
        bullet = dynamic_cast<Bullet *>(nodeB);
    } else if (tagB == TAG_PLAYER && tagA == TAG_OPPOPENT_BULLET) {
        player = dynamic_cast<Player *>(nodeB);
        bullet = dynamic_cast<Bullet *>(nodeA);
    }
    if (player && bullet) {
        player->hitShot();
        bullet->setLifePoint(-1.0f);
        if (player->getLifePoint() <= 0) {
            gameOver();
        }
        return false;
    }

    // If a shot hits wall, remove the shot from the stage
    if (tagA == TAG_WALL && (tagB == TAG_PLAYER_BULLET || tagB == TAG_OPPOPENT_BULLET)) {
        bullet = dynamic_cast<Bullet *>(nodeB);
    } else if (tagB == TAG_WALL && (tagA == TAG_PLAYER_BULLET || tagA == TAG_OPPOPENT_BULLET)) {
        bullet = dynamic_cast<Bullet *>(nodeA);
    }

    if (bullet) {
        bullet->setLifePoint(-1.0f);
        return false;
    }

    // bodies can collide
    return false;
}

#pragma mark -
#pragma mark Networks

void GameScene::setNetworkedSession(bool networkedSession, bool isHost) {
    _networkedSession = networkedSession;
    _isHost = isHost;
}

void GameScene::receivedData(const void *data, unsigned long length) {
    const char *cstr = reinterpret_cast<const char *>(data);
    std::string json = std::string(cstr, length);

    JSONPacker::GameState state = JSONPacker::unpackGameStateJSON(json);

    if (state.playersLifePoint <= 0 || state.opponentsLifePoint <= 0) {
        gameOver();
    }

    _stage->setState(state);
}

void GameScene::sendGameStateOverNetwork(Bullet *newBullet) {
    JSONPacker::GameState state;

    state.name = NetworkingWrapper::getDeviceName();
    state.opponentPosition = _stage->getPlayer()->getPosition();
    state.opponentMoveState = _stage->getPlayer()->getMoveState();
    state.playersLifePoint = _stage->getOpponent()->getLifePoint();
    state.opponentsLifePoint = _stage->getPlayer()->getLifePoint();
    state.newBullet = newBullet;

    std::string json = JSONPacker::packGameStateJSON(state);
    SceneManager::getInstance()->sendData(json.c_str(), json.length());
}

#pragma mark -
#pragma mark Game Logics

void GameScene::setGameActive(bool active) {
    _active = active;
    if (active) {
        this->scheduleUpdate();
    } else {
        this->unscheduleUpdate();
    }
}

void GameScene::update(float dt) {
    _stage->step(dt);
}

void GameScene::gameOver() {
    this->setGameActive(false);

    if (_networkedSession) {
        sendGameStateOverNetwork(nullptr);
        sendGameStateOverNetwork(nullptr);
        sendGameStateOverNetwork(nullptr);
        sendGameStateOverNetwork(nullptr);
    }

    int playerLife = _stage->getPlayer()->getLifePoint();
    int opponentLife = _stage->getOpponent()->getLifePoint();
    std::string messageContent;
    std::string playerScoreString = StringUtils::toString(playerLife);
    std::string opponentScoreString = StringUtils::toString(opponentLife);
    if (playerLife > opponentLife) {
        messageContent = "You win! (" + playerScoreString + ", " + opponentScoreString + ")";
    } else if (opponentLife > playerLife) {
        messageContent = "You lose... (" + playerScoreString + ", " + opponentScoreString + ")";
    } else {
        messageContent = "Draw";
    }

    MessageBox(messageContent.c_str(), "GAMEOVER");

    SceneManager::getInstance()->returnToLobby();
}

#pragma mark -
#pragma mark UI Methods

void GameScene::backButtonPressed(cocos2d::Ref *pSender, ui::Widget::TouchEventType eEventType) {
    if (eEventType == ui::Widget::TouchEventType::ENDED) {
        SceneManager::getInstance()->returnToLobby();
    }
}

void updateLifePoints(int playerLifePoint, int opponentLifePoint) {
}

#pragma mark -
#pragma mark UtilityMethods

MoveState GameScene::convertVec2ToMoveState(const cocos2d::Vec2 v) {
    if (v.distance(Vec2(0.0f, 0.0f)) < 1.0f) {
        return MoveState::STOP;
    }

    float angle = MathUtils::degreesAngle(v);

    // TODO: Fix angles
    if (angle < (-180.0f + 22.5f) || (180.0f - 22.5f) < angle) {
        return MoveState::DOWN;
    } else if ((-135.0f - 22.5f) <= angle && angle < (-135.0f + 22.5f)) {
        return MoveState::LOWER_LEFT;
    } else if ((-90.0f - 22.5f) <= angle && angle < (-90.0f + 22.5f)) {
        return MoveState::LEFT;
    } else if ((-45.0f - 22.5f) <= angle && angle < (-45.0f + 22.5f)) {
        return MoveState::UPPER_LEFT;
    } else if ((0.0f - 22.5f) <= angle && angle < (0.0f + 22.5f)) {
        return MoveState::UP;
    } else if ((45.0f - 22.5f) <= angle && angle < (45.0f + 22.5f)) {
        return MoveState::UPPER_RIGHT;
    } else if ((90.0f - 22.5f) <= angle && angle < (90.0f + 22.5f)) {
        return MoveState::RIGHT;
    } else if ((135.0f - 22.5f) <= angle && angle < (135.0f + 22.5f)) {
        return MoveState::LOWER_RIGHT;
    } else {
        return MoveState::STOP;
    }
}
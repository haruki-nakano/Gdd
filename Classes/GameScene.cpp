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
    _isHost = true;

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

    // FIXME: Use foreach
    auto labelString = _stage->getPlayer()->getGunName();
    auto gunLabel = ui::Text::create(labelString, FONT_NAME, FONT_SIZE);
    gunLabel->setAnchorPoint(Vec2(0.5f, 1.0f));
    gunLabel->setPosition(_playerLifeBar->getPosition() - Vec2(0.0f, 24.0f));
    gunLabel->setColor(LABEL_COLOR);
    this->addChild(gunLabel);

    gunLabel = ui::Text::create("?", FONT_NAME, FONT_SIZE);
    gunLabel->setAnchorPoint(Vec2(0.5f, 1.0f));
    gunLabel->setPosition(_opponentsLifeBar->getPosition() - Vec2(0.0f, 24.0f));
    gunLabel->setColor(LABEL_COLOR);
    this->addChild(gunLabel);

    setupTouchHandling();

    // setup contact handling
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    setGameActive(true);
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
    static clock_t delta;
    _stage->step(dt);

    //  Host is in charge of generating egg.
    if (_isHost && _stage->getEgg()->getLifePoint() <= 0 && _stage->getEgg()->getLastBrokenTime() + delta < clock()) {
        delta = random(MIN_EGG_INTERVAL_SEC, MAX_EGG_INTERVAL_SEC) * CLOCKS_PER_SEC;
        _stage->generateEgg();
        sendGameStateOverNetwork(EventType::APPEAR_EGG, std::vector<Bullet *>(), true);
    }
}

void GameScene::gameOver() {
    if (!_active) {
        // The game is already over
        return;
    }
    this->setGameActive(false);

    if (_networkedSession) {
        sendGameStateOverNetwork(EventType::GAME_OVER);
        sendGameStateOverNetwork(EventType::GAME_OVER);
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
#pragma mark Event

void GameScene::setupTouchHandling() {
    // Index in these vector must follow the touch order
    static std::vector<int> sTouchIds;
    static std::vector<Vec2> sTouchBeganPositions;
    static Vec2 sLastSyncPos;

    auto touchListener = EventListenerTouchOneByOne::create();

    touchListener->onTouchBegan = [&](Touch *touch, Event *event) {
        Vec2 touchPos = this->convertTouchToNodeSpace(touch);
        sTouchIds.push_back(touch->getID());
        sTouchBeganPositions.push_back(touchPos);

        // If it is first touch finger
        if (sTouchIds.size() == 1) {
            sLastSyncPos = this->convertTouchToNodeSpace(touch);
        }

        return true;
    };

    touchListener->onTouchMoved = [&](Touch *touch, Event *event) {
        if (sTouchIds.size() == 0 || sTouchBeganPositions.size() == 0 || touch->getID() != sTouchIds[0]) {
            return;
        }
        Vec2 touchPos = this->convertTouchToNodeSpace(touch);
        MoveState move = MathUtils::convertVec2ToMoveState(touchPos - sTouchBeganPositions[0]);
        MoveState lastMoveState = _stage->getPlayer()->getMoveState();
        if (move != lastMoveState) {
            _stage->getPlayer()->setMoveState(move);
        }
        // FIXME: this conditions
        if (_networkedSession && move != lastMoveState) {
            sendGameStateOverNetwork(EventType::CHANGE_PLAYERS_DIRECTION);
            sLastSyncPos = touchPos;
        }
    };

    touchListener->onTouchEnded = [&](Touch *touch, Event *event) {
        Vec2 touchPos = this->convertTouchToNodeSpace(touch);
        int touchId = touch->getID();
        // Fire the bullet or stop moving
        if (touchId == sTouchIds[0] && _stage->getPlayer()->getMoveState() != MoveState::STOP) {
            _stage->getPlayer()->setMoveState(MoveState::STOP);
            if (_networkedSession) {
                // FIXME: Improvement
                sendGameStateOverNetwork(EventType::STOP_PLAYERS_MOVING);
                sendGameStateOverNetwork(EventType::STOP_PLAYERS_MOVING);
            }
        } else if ((!_stage->getPlayer()->isSwimming() || ALLOW_WATER_SHOT) &&
                   (ALLOW_MORE_THAN_TWO_TAP || sTouchIds.size() < 3)) {
            // FIXME: Improvement
            std::vector<Bullet *> bullets = _stage->getPlayer()->createBullets(touchPos, _stage->getPosition());
            for (Bullet *bullet : bullets) {
                // Use addBullets
                _stage->addBullet(bullet);
            }
            if (_networkedSession && bullets.size() > 0) {
                // FIXME: Improvement
                sendGameStateOverNetwork(EventType::FIRE_BULLT, bullets);
            }
        }

        for (size_t i = 0; sTouchIds.size(); i++) {
            if (sTouchIds[i] == touchId) {
                sTouchIds.erase(sTouchIds.begin() + i);
                sTouchBeganPositions.erase(sTouchBeganPositions.begin() + i);
                break;
            }
        }
    };

    touchListener->onTouchCancelled = [&](Touch *touch, Event *event) {
        for (size_t i = 0; sTouchIds.size(); i++) {
            if (sTouchIds[i] == touch->getID()) {
                sTouchIds.erase(sTouchIds.begin() + i);
                sTouchBeganPositions.erase(sTouchBeganPositions.begin() + i);
                break;
            }
        }
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
        player->bulletHits(bullet);
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
        player->bulletHits(bullet);
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

    // If a player contacts egg
    if (tagA == TAG_EGG && (tagB == TAG_PLAYER || tagB == TAG_OPPOPENT)) {
        CCLOG("bar");
        return false;
    } else if (tagB == TAG_EGG && (tagA == TAG_PLAYER || tagA == TAG_OPPOPENT)) {
        CCLOG("foo");
        return false;
    }

    // If a bullet contacts egg
    Egg *egg = nullptr;
    if (tagA == TAG_EGG && (tagB == TAG_PLAYER_BULLET || tagB == TAG_OPPOPENT_BULLET)) {
        egg = dynamic_cast<Egg *>(nodeA);
        bullet = dynamic_cast<Bullet *>(nodeB);
        player = tagB == TAG_PLAYER_BULLET ? _stage->getPlayer() : _stage->getOpponent();
    } else if (tagB == TAG_EGG && (tagA == TAG_PLAYER_BULLET || tagA == TAG_OPPOPENT_BULLET)) {
        egg = dynamic_cast<Egg *>(nodeB);
        bullet = dynamic_cast<Bullet *>(nodeA);
        player = tagA == TAG_PLAYER_BULLET ? _stage->getPlayer() : _stage->getOpponent();
    }
    if (egg && bullet) {
        bullet->setLifePoint(-1.0f);
        egg->setLifePoint(egg->getLifePoint() - 1);
        // Egg is broken
        if (egg->getLifePoint() == 0) {
            player->gotHeal();
        }
        sendGameStateOverNetwork(EventType::HIT_EGG);
        return false;
    }

    // bodies can collide
    return false;
}

#pragma mark -
#pragma mark UI Methods

void GameScene::backButtonPressed(cocos2d::Ref *pSender, ui::Widget::TouchEventType eEventType) {
    if (eEventType == ui::Widget::TouchEventType::ENDED) {
        SceneManager::getInstance()->returnToLobby();
    }
}

#pragma mark -
#pragma mark Networking

void GameScene::setNetworkedSession(bool networkedSession, bool isHost) {
    _networkedSession = networkedSession;
    _isHost = isHost;
}

void GameScene::receivedData(const void *data, unsigned long length) {
    const char *cstr = reinterpret_cast<const char *>(data);
    std::string json = std::string(cstr, length);

    JSONPacker::GameState state = JSONPacker::unpackGameStateJSON(json);

    _stage->setState(state);

    auto players = _stage->getPlayers();
    for (Player *player : players) {
        if (player->getLifePoint() <= 0) {
            gameOver();
            break;
        }
    }
}

void GameScene::sendGameStateOverNetwork(EventType event, std::vector<Bullet *> newBullets, bool newEgg) {
    JSONPacker::GameState state;

    // FIXME: Improve
    // What contents should be send?
    state.event = event;
    state.name = NetworkingWrapper::getDeviceName();
    state.opponentPosition = _stage->getPlayer()->getPosition();
    state.opponentMoveState = _stage->getPlayer()->getMoveState();
    state.opponentDirection = _stage->getPlayer()->getDirection();
    state.playersHitCount = _stage->getOpponent()->getHitCount();
    state.opponentsHitCount = _stage->getPlayer()->getHitCount();
    state.playersHealCount = _stage->getOpponent()->getHealCount();
    state.opponentsHealCount = _stage->getPlayer()->getHealCount();
    state.newBullets = newBullets;

    Egg *egg = _stage->getEgg();
    state.eggLifePoint = egg->getLifePoint();
    if (newEgg) {
        state.eggPosition = egg->getPosition();
    } else {
        state.eggPosition = Vec2::ZERO;
    }

    std::string json = JSONPacker::packGameStateJSON(state);
    SceneManager::getInstance()->sendData(json.c_str(), json.length());
}
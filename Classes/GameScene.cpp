//
//  GameScene.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "GameScene.h"

#include "Bar.h"
#include "Constants.h"
#include "PhysicsConstants.h"
#include "UIConstants.h"
#include "GameOverDialog.h"
#include "MathUtils.h"
#include "JSONPacker.h"
#include "Player.h"
#include "SceneManager.h"
#include "Stage.h"
#include "Weapon.h"

using namespace cocos2d;

#pragma mark -
#pragma mark Lifycycle

bool GameScene::init() {
    if (!Node::init()) {
        return false;
    }

    LayerColor *background = LayerColor::create(Color4B(140, 200, 255, 255));
    this->addChild(background);

    auto size = Director::getInstance()->getVisibleSize();
    auto height = MAX(size.height - RESOLUTION_HEIGHT, 0) * 0.5f;

    if (USE_SAME_RESOLUTION) {
        LayerColor *statusArea1 = LayerColor::create(Color4B(0, 0, 0, 255), size.width, height);
        this->addChild(statusArea1, 101);

        LayerColor *statusArea2 = LayerColor::create(Color4B(0, 0, 0, 255), size.width, height);
        statusArea2->setPosition(0.0f, size.height - height);
        this->addChild(statusArea2, 100);
    }

    _active = false;
    _networkedSession = false;
    _isHost = true;
    _stageId = DEFAULT_STAGE_ID;

    return true;
}

void GameScene::onEnter() {
    Node::onEnter();

    Size visibleSize = Director::getInstance()->getVisibleSize();

    // setup stage
    std::stringstream ss;
    ss << "stage" << _stageId << ".tmx";
    _stage = Stage::createWithFileName(ss.str());
    _stage->setAnchorPoint(Vec2(0.0f, 0.0f));
    _stage->setPosition(Vec2(0.0f, 0.0f));

    this->addChild(_stage);

    if (_networkedSession) {
        _stage->initializePlayersPosition(_isHost);
    } else {
        _stage->initializePlayersPosition(true);
    }

    // setup menus
    ui::Button *backButton = ui::Button::create();
    backButton->setAnchorPoint(Vec2(0.0f, 1.0f));
    backButton->setPosition(Vec2(0.0f, visibleSize.height));
    backButton->loadTextures("backButton.png", "backButtonPressed.png");
    backButton->addTouchEventListener(CC_CALLBACK_2(GameScene::backButtonPressed, this));

    this->addChild(backButton, 500);

    _playerLifeBar = Bar::create(Bar::LIFE);
    _playerLifeBar->setPosition(Vec2(visibleSize.width * 0.5 - 256.0f, visibleSize.height - 32.0f));

    _opponentsLifeBar = Bar::create(Bar::LIFE);
    _opponentsLifeBar->setPosition(Vec2(visibleSize.width * 0.5 + 256.0f, visibleSize.height - 32.0f));

    _playerWaterBar = Bar::create(Bar::CHARGING);
    _playerWaterBar->setPosition(Vec2(visibleSize.width * 0.5 - 256.0f, visibleSize.height - 70.0f));

    this->addChild(_playerWaterBar, 501);
    this->addChild(_playerLifeBar, 503);
    this->addChild(_opponentsLifeBar, 504);

    _stage->getPlayer()->setWaterBar(_playerWaterBar);

    _stage->getPlayer()->setLifeBar(_playerLifeBar);
    _stage->getOpponent()->setLifeBar(_opponentsLifeBar);

    auto labelString = _stage->getPlayer()->getGunName();
    for (int i = 0; i < 2; i++) {
        auto gunLabel = ui::Text::create(i == 0 ? labelString : "?", FONT_NAME, FONT_SIZE);
        gunLabel->setAnchorPoint(Vec2(0.5f, 1.0f));
        Vec2 basePos;
        if (i == 0) {
            // Player
            basePos = _playerWaterBar->getPosition();
            _playerGunLabel = gunLabel;
        } else {
            // Opponent
            basePos = _opponentsLifeBar->getPosition() - Vec2(0.0f, 8.0f);
        }
        gunLabel->setPosition(basePos - Vec2(0.0f, 16.0f));
        gunLabel->setColor(LABEL_COLOR);
        this->addChild(gunLabel, 505 + i);
    }

    setupTouchHandling();

    // setup contact handling
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    setGameActive(true);
}

void GameScene::setStageId(int stageId) {
    _stageId = stageId;
}

#pragma mark -
#pragma mark Game Logics

bool GameScene::isGameActive() const {
    return _active;
}

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
    static float createWeaponCount = 0.0f;
    static float createWeaponInterval = random(MIN_WEAPON_INTERVAL_SEC, MAX_WEAPON_INTERVAL_SEC);

    _stage->step(dt);

    // Host is in charge of generating egg.
    Egg *egg = _stage->getEgg();
    if (_isHost && egg->getState() == EggState::IDLE && egg->getLastBrokenTime() + delta < clock()) {
        delta = random(MIN_EGG_INTERVAL_SEC, MAX_EGG_INTERVAL_SEC) * CLOCKS_PER_SEC;
        _stage->generateEgg();
        if (_networkedSession) {
            sendGameStateOverNetwork(EventType::APPEAR_EGG, std::vector<Bullet *>(), true);
        }
    }

    // Host is in charge of generating weapon.
    if (_isHost && !_stage->getWeapon()) {
        createWeaponCount += dt;
        // CCLOG("%f %f", createWeaponCount, createWeaponInterval);
        if (createWeaponCount > createWeaponInterval) {
            _stage->replaceWeapon();
            if (_networkedSession) {
                sendGameStateOverNetwork(EventType::APPEAR_WEAPON);
            }
            createWeaponCount = 0.0f;
            createWeaponInterval = random(MIN_WEAPON_INTERVAL_SEC, MAX_WEAPON_INTERVAL_SEC);
        }
    }
}

void GameScene::gameOver() {
    if (!isGameActive()) {
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
    Player *loser;
    if (playerLife > opponentLife) {
        loser = _stage->getOpponent();
    } else if (opponentLife > playerLife) {
        loser = _stage->getPlayer();
    } else {
        // Draw
        loser = _stage->getOpponent();
        Player *winner = _stage->getPlayer();

        winner->setMoveState(MoveState::STOP);
        // Explosion effect
        ParticleSystemQuad *particle = ParticleSystemQuad::create("particle_texture_explosion.plist");
        particle->setPosition(Vec2(winner->getBoundingBox().size * 0.5));
        particle->setAutoRemoveOnFinish(true);
        winner->setOpacity(0);
        winner->addChild(particle);

        // Wait for explosion
        DelayTime *delay = DelayTime::create(0.5f);
        winner->runAction(delay);
    }

    loser->setMoveState(MoveState::STOP);
    // Explosion effect
    ParticleSystemQuad *particle = ParticleSystemQuad::create("particle_texture_explosion.plist");
    particle->setPosition(Vec2(loser->getBoundingBox().size * 0.5));
    particle->setAutoRemoveOnFinish(true);
    loser->setOpacity(0);
    loser->addChild(particle);

    // Wait for explosion
    DelayTime *delay = DelayTime::create(2.0f);
    CallFunc *callfunc = CallFunc::create(CC_CALLBACK_0(GameScene::showDialog, this));
    auto seq = Sequence::createWithTwoActions(delay, callfunc);
    loser->runAction(seq);
}

void GameScene::showDialog() {
    int playerLife = _stage->getPlayer()->getLifePoint();
    int opponentLife = _stage->getOpponent()->getLifePoint();
    auto dialog = GameOverDialog::createWithLifePoints(playerLife, opponentLife);
    this->addChild(dialog, 999);
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
                sendGameStateOverNetwork(EventType::STOP_PLAYERS_MOVING);
                sendGameStateOverNetwork(EventType::STOP_PLAYERS_MOVING);
            }
        } else if ((!_stage->getPlayer()->isSwimming() || ALLOW_WATER_SHOT) &&
                   (ALLOW_MORE_THAN_TWO_TAP || sTouchIds.size() < 3)) {
            std::vector<Bullet *> bullets = _stage->getPlayer()->createBullets(touchPos, _stage->getPosition());
            for (Bullet *bullet : bullets) {
                // Use addBullets
                _stage->addBullet(bullet);
            }
            if (_networkedSession && bullets.size() > 0) {
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

    Player *player = nullptr;
    Bullet *bullet = nullptr;
    Egg *egg = nullptr;
    Weapon *weapon = nullptr;

    // CCLOG("onContactBegin: %d %d", nodeA->getTag(), nodeB->getTag());

    // My shot hits other player
    if (tagA == TAG_OPPOPENT && tagB == TAG_PLAYER_BULLET) {
        player = dynamic_cast<Player *>(nodeA);
        bullet = dynamic_cast<Bullet *>(nodeB);
    } else if (tagB == TAG_OPPOPENT && tagA == TAG_PLAYER_BULLET) {
        player = dynamic_cast<Player *>(nodeB);
        bullet = dynamic_cast<Bullet *>(nodeA);
    }
    if (player && bullet) {
        if (isGameActive()) {
            player->bulletHits(bullet);
            if (player->getLifePoint() <= 0) {
                gameOver();
            }
        }
        bullet->setLifePoint(-1.0f);
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
        if (isGameActive()) {
            player->bulletHits(bullet);
            if (player->getLifePoint() <= 0) {
                gameOver();
            }
        }
        bullet->setLifePoint(-1.0f);
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

    // If a player contacts item
    if (tagA == TAG_ITEM && (tagB == TAG_PLAYER || tagB == TAG_OPPOPENT)) {
        egg = dynamic_cast<Egg *>(nodeA);
        player = dynamic_cast<Player *>(nodeB);
    } else if (tagB == TAG_ITEM && (tagA == TAG_PLAYER || tagA == TAG_OPPOPENT)) {
        egg = dynamic_cast<Egg *>(nodeB);
        player = dynamic_cast<Player *>(nodeA);
    }

    if (egg && player) {
        if (egg->getState() == EggState::ITEM) {
            egg->setState(EggState::IDLE);
            // Do something for player.
            if (egg->getItemType() == EggItemType::HEALING) {
                player->gotHeal();
                if (_networkedSession) {
                    sendGameStateOverNetwork(EventType::GET_HEAL);
                }
            } else if (egg->getItemType() == EggItemType::GOGGLES) {
                if (!player->isOpponent()) {
                    _stage->getOpponent()->captured();
                }
                if (_networkedSession) {
                    sendGameStateOverNetwork(EventType::GET_GOGGLES);
                }
            } else if (egg->getItemType() == EggItemType::SUPER_STAR) {
                player->gotInvincible();
                if (_networkedSession) {
                    sendGameStateOverNetwork(player->isOpponent() ? EventType::PLAYER_GET_INVINCIBLE
                                                                  : EventType::OPPONENT_GET_INVINCIBLE);
                }
            }
            return false;
        } else {
            return true;
        }
    }

    // If a player contacts weapon
    if (tagA == TAG_WEAPON && (tagB == TAG_PLAYER || tagB == TAG_OPPOPENT)) {
        weapon = dynamic_cast<Weapon *>(nodeA);
        player = dynamic_cast<Player *>(nodeB);
    } else if (tagB == TAG_WEAPON && (tagA == TAG_PLAYER || tagA == TAG_OPPOPENT)) {
        weapon = dynamic_cast<Weapon *>(nodeB);
        player = dynamic_cast<Player *>(nodeA);
    }
    if (player && weapon) {
        player->replaceGun();
        if (!player->isOpponent()) {
            _playerGunLabel->setString(player->getGunName());
        }
        _stage->removeWeapon();
        if (_networkedSession) {
            sendGameStateOverNetwork(EventType::GET_WEAPON);
        }
        return true;
    }

    // If a bullet contacts egg
    if (tagA == TAG_EGG && (tagB == TAG_PLAYER_BULLET || tagB == TAG_OPPOPENT_BULLET)) {
        egg = dynamic_cast<Egg *>(nodeA);
        bullet = dynamic_cast<Bullet *>(nodeB);
    } else if (tagB == TAG_EGG && (tagA == TAG_PLAYER_BULLET || tagA == TAG_OPPOPENT_BULLET)) {
        egg = dynamic_cast<Egg *>(nodeB);
        bullet = dynamic_cast<Bullet *>(nodeA);
    }
    if (egg && bullet) {
        bullet->setLifePoint(-1.0f);
        egg->setLifePoint(egg->getLifePoint() - 1);
        if (_networkedSession) {
            sendGameStateOverNetwork(EventType::HIT_EGG);
        }
        return false;
    }

    // bodies can collide
    return false;
}

#pragma mark -
#pragma mark UI Methods

void GameScene::backButtonPressed(cocos2d::Ref *pSender, ui::Widget::TouchEventType eEventType) {
    if (eEventType == ui::Widget::TouchEventType::ENDED && isGameActive()) {
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
    // length 1 means stage select
    if (!_stage || !_networkedSession || !isGameActive()) {
        // CCLOG("ignored %p %lu %d", _stage, length, _networkedSession);
        return;
    }
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
        state.eggItemType = egg->getItemType();
    } else {
        state.eggPosition = Vec2::ZERO;
        state.eggItemType = EggItemType::SIZE;
    }

    if (event == EventType::APPEAR_WEAPON) {
        state.weaponPosition = _stage->getWeapon()->getPosition();
    } else {
        state.weaponPosition = Vec2::ZERO;
    }

    std::string json = JSONPacker::packGameStateJSON(state);
    SceneManager::getInstance()->sendData(json.c_str(), json.length());
}
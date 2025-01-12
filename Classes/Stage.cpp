//
//  Stage.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "Stage.h"

#include "Constants.h"
#include "UIConstants.h"
#include "PhysicsConstants.h"

#include "Bullet.h"
#include "Egg.h"
#include "Weapon.h"
#include "Player.h"

using namespace cocos2d;

#pragma mark -
#pragma mark Initialize

Stage *Stage::createWithFileName(std::string filename) {
    Stage *stage = new (std::nothrow) Stage();

    if (stage && stage->initWithFileName(filename)) {
        stage->autorelease();
        return stage;
    }

    CC_SAFE_DELETE(stage);
    return nullptr;
}

bool Stage::initWithFileName(std::string filename) {
    if (!Node::init()) {
        return false;
    }

    _stageFileName = filename;
    _players.reserve(MAX_PLAYERS);

    return true;
}

void Stage::onEnter() {
    Node::onEnter();

    Size contentSize = getContentSize();

    _map = cocos2d::experimental::TMXTiledMap::create(_stageFileName);
    _backgroundLayer = _map->getLayer(DEFAULT_BACKGROUND_LAYER_NAME);
    _size = _backgroundLayer->getLayerSize();
    for (int y = 0; y < _size.height; y++) {
        for (int x = 0; x < _size.width; x++) {
            auto tile = _backgroundLayer->getTileAt(Vec2(x, y));
            if (tile) {
                tile->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            }
        }
    }

    _collisionLayer = _map->getLayer(DEFAULT_COLLISION_LAYER_NAME);
    _collisionLayer->setPosition(Vec2(0.0f, TILE_HEIGHT * 0.5f));
    Node *tmp;
    auto size = _collisionLayer->getLayerSize();
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            auto tile = _collisionLayer->getTileAt(Vec2(x, y));
            if (tile) {
                tmp = tile->getParent();
                tile->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
                Vec2 v[4];
                Vec2 s = Vec2(TILE_WIDTH * 0.5f, TILE_HEIGHT * 0.5f);
                v[0] = Vec2(-s.x, -s.y);
                v[1] = Vec2(0.0f, 0);
                v[2] = Vec2(s.x, -s.y);
                v[3] = Vec2(0.0f, -s.y * 2.0f);

                PhysicsBody *tilePhysics = PhysicsBody::createPolygon(v, 4);
                tilePhysics->setDynamic(false);
                tilePhysics->setCategoryBitmask(CATEGORY_MASK_WALL);
                tilePhysics->setCollisionBitmask(COLLISION_MASK_WALL);
                tilePhysics->setContactTestBitmask(CONTACT_MASK_WALL);

                tile->setTag(TAG_WALL);
                tile->setPhysicsBody(tilePhysics);

                tile->setLocalZOrder(x + y);
            }
        }
    }

    // setup map
    _map->setAnchorPoint(Vec2::ZERO);
    _map->setPosition(Vec2::ZERO);

    this->addChild(_map);

    // setup player
    for (int i = 0; i < MAX_PLAYERS; i++) {
        bool isOpponent = i > 0;
        Player *player = Player::create(isOpponent);
        _collisionLayer->addChild(player);
        _players.push_back(player);
    }
    getPlayer()->setTag(TAG_PLAYER);
    getOpponent()->setTag(TAG_OPPOPENT);

    _egg = Egg::create();
    _collisionLayer->addChild(_egg);

    _weapon = nullptr;
}

void Stage::initializePlayersPosition(bool isHost) {
    auto hostInitialCoordinate = Vec2(_backgroundLayer->getProperty("hostInitialPositionX").asFloat(),
                                      _backgroundLayer->getProperty("hostInitialPositionY").asFloat());
    auto clientInitialCoordinate = Vec2(_backgroundLayer->getProperty("clientInitialPositionX").asFloat(),
                                        _backgroundLayer->getProperty("clientInitialPositionY").asFloat());
    Vec2 hostInitialPosition = _backgroundLayer->getPositionAt(hostInitialCoordinate);
    Vec2 clientInitialPosition = _backgroundLayer->getPositionAt(clientInitialCoordinate);

    if (isHost) {
        getPlayer()->setPosition(hostInitialPosition);
        getOpponent()->setPosition(clientInitialPosition);
        getPlayer()->setDirection(Direction::UP);
        getOpponent()->setDirection(Direction::DOWN);
    } else {
        getPlayer()->setPosition(clientInitialPosition);
        getOpponent()->setPosition(hostInitialPosition);
        getPlayer()->setDirection(Direction::DOWN);
        getOpponent()->setDirection(Direction::UP);
    }

    getPlayer()->setPlayerColor(isHost);
    getOpponent()->setPlayerColor(!isHost);

    // Scroll to put a player center on the screen
    step(0);
}

#pragma mark -
#pragma mark Public Method

void Stage::step(float dt) {
    Size size = Director::getInstance()->getVisibleSize();
    Vec2 pos = getPlayer()->getPosition();
    // Set players position to center of the screen
    this->setPosition(Vec2(size.width * 0.5 - pos.x, size.height * 0.5 - pos.y));

    for (int i = 0; i < _bullets.size(); i++) {
        Bullet *bullet = _bullets[i];
        bullet->step(dt);
        if (bullet->getLifePoint() < 0.0f) {
            _bullets.erase(_bullets.begin() + i);
            bullet->removeFromParent();
            i--;
        }
    }

    // Set water state
    auto players = getPlayers();
    for (Player *player : players) {
        player->step(dt);
        Vec2 coordinate = convertPositionToTileCoordinate(player->getPosition());
        if (isCorrectTileCoordinate(coordinate)) {
            int gid = _backgroundLayer->getTileGIDAt(coordinate);
            if (gid < 8) {
                player->setIsSwimming(false);
            } else if (gid >= 8) {
                player->setIsSwimming(true);
            }
        }
        // FIX ME: magic number
        player->setLocalZOrder(coordinate.x + coordinate.y - 3.0f);
    }
}

void Stage::addBullet(Bullet *bullet) {
    _bullets.push_back(bullet);
    this->addChild(bullet);
}

void Stage::generateEgg() {
    // FIXME: Critical
    float x = (float)random(1, (int)_size.width - 1);
    float y = (float)random(1, (int)_size.height - 1);

    Vec2 coordinate = Vec2(x, y);
    if (!isCorrectTileCoordinate(coordinate, true) || _backgroundLayer->getTileGIDAt(coordinate) >= 8) {
        return;
    }

    auto sp = _backgroundLayer->getTileAt(coordinate);
    Vec2 pos = sp->getPosition() + Vec2(0.0f, TILE_HEIGHT * 0.5f);
    float newZ = coordinate.y + coordinate.x;
    _egg->setLocalZOrder(newZ);
    _egg->setPosition(pos);
    _egg->setLifePoint(INITIAL_EGG_LIFE);
    _egg->setState(EggState::EGG);
    _egg->setItemType(static_cast<EggItemType>(random(0, static_cast<int>(EggItemType::SIZE) - 1)));
}

void Stage::replaceWeapon() {
    float x = (float)random(1, (int)_size.width - 1);
    float y = (float)random(1, (int)_size.height - 1);

    Vec2 coordinate = Vec2(x, y);
    if (!isCorrectTileCoordinate(coordinate, true) || _backgroundLayer->getTileGIDAt(coordinate) >= 8) {
        // FIXME: It can be endlesss.
        replaceWeapon();
        return;
    }

    _weapon = Weapon::create();
    auto sp = _backgroundLayer->getTileAt(coordinate);
    Vec2 pos = sp->getPosition() + Vec2(0.0f, TILE_HEIGHT * 0.5f);
    float newZ = coordinate.y + coordinate.x;
    _weapon->setLocalZOrder(newZ);
    _weapon->setPosition(pos);
    this->addChild(_weapon);
}

void Stage::removeWeapon() {
    if (_weapon) {
        _weapon->removeFromParent();
        _weapon = nullptr;
    }
}

#pragma mark -
#pragma mark Getter/Setter

std::vector<Player *> Stage::getPlayers() const {
    return _players;
}

Player *Stage::getPlayer() const {
    return _players[0];
}

Player *Stage::getOpponent() const {
    return _players[1];
}

Weapon *Stage::getWeapon() const {
    return _weapon;
}

Egg *Stage::getEgg() const {
    return _egg;
}

void Stage::setState(JSONPacker::GameState state) {
    Player *player = getPlayer();
    Player *opponent = getOpponent();
    // if (opponent->isCorrectUpdate(state.opponentPosition)) {
    opponent->setPosition(state.opponentPosition);
    //}
    opponent->setMoveState(state.opponentMoveState);
    opponent->setDirection(state.opponentDirection);

    // We accept false positive due to the network deplay
    player->setHitCount(MAX(state.playersHitCount, player->getHitCount()));
    opponent->setHitCount(MAX(state.opponentsHitCount, opponent->getHitCount()));

    player->setHealCount(MAX(state.playersHealCount, player->getHealCount()));
    opponent->setHealCount(MAX(state.opponentsHealCount, opponent->getHealCount()));

    if (state.newBullets.size() > 0) {
        opponent->setLastTimeBulletCreated(clock());
    }
    for (Bullet *bullet : state.newBullets) {
        addBullet(bullet);
    }

    if (state.eggPosition != Vec2::ZERO) {
        // New egg is created
        _egg->setPosition(state.eggPosition);
        _egg->setLifePoint(state.eggLifePoint);
        _egg->setState(EggState::EGG);
        _egg->setItemType(state.eggItemType);
    } else {
        _egg->setLifePoint(MIN(state.eggLifePoint, _egg->getLifePoint()));
    }

    if (state.event == EventType::PLAYER_GET_INVINCIBLE) {
        player->gotInvincible();
    } else if (state.event == EventType::OPPONENT_GET_INVINCIBLE) {
        opponent->gotInvincible();
    } else if (state.event == EventType::GET_WEAPON) {
        this->removeWeapon();
    } else if (state.event == EventType::APPEAR_WEAPON) {
        this->replaceWeapon();
        _weapon->setPosition(state.weaponPosition);
    }
}

#pragma mark -
#pragma mark Utility

bool Stage::isCorrectTileCoordinate(Vec2 tileCoordinate, bool checkWallCollision) {
    return tileCoordinate.x < _size.width && tileCoordinate.y < _size.height && tileCoordinate.x >= 0 &&
           tileCoordinate.y >= 0 && (!checkWallCollision || (_collisionLayer->getTileGIDAt(tileCoordinate) == 0)); //&&
    //_wallLayer->getTileGIDAt(tileCoordinate) == 0));
}

Vec2 Stage::convertPositionToTileCoordinate(Vec2 pos) {
    auto ms = _map->getTileSize();
    auto ls = _backgroundLayer->getLayerSize();

    return Vec2(roundf(pos.x / ms.width - pos.y / ms.height - ls.width / 2.0f + ls.height - 0.5f),
                roundf(ls.width / 2.0f + ls.height - 1.5f - pos.x / ms.width - pos.y / ms.height));
}

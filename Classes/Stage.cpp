//
//  Stage.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "Stage.h"

#include "Constants.h"
#include "Player.h"
#include "Bullet.h"
#include "Bullet.h"

using namespace cocos2d;

bool Stage::init() {
    if (!Node::init()) {
        return false;
    }

    _players.reserve(MAX_PLAYERS);

    return true;
}

void Stage::onEnter() {
    Node::onEnter();

    Size contentSize = getContentSize();

    _map = cocos2d::experimental::TMXTiledMap::create(DEFAULT_STAGE_FILE);
    _backgroundLayer = _map->getLayer(DEFAULT_BACKGROUND_LAYER_NAME);
    auto size = _backgroundLayer->getLayerSize();
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            auto tile = _backgroundLayer->getTileAt(Vec2(x, y));
            if (tile) {
                tile->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            }
        }
    }
    auto collisionLayer = _map->getLayer(DEFAULT_COLLISION_LAYER_NAME);
    size = collisionLayer->getLayerSize();
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            auto tile = collisionLayer->getTileAt(Vec2(x, y));
            if (tile) {
                tile->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
                // TODO: We should use Polygon instead of EdgePolygon
                Vec2 v[4];
                Vec2 s = Vec2(32, 16);
                v[0] = Vec2(-s.x, 0);
                v[1] = Vec2(0, s.y);
                v[2] = Vec2(s.x, 0);
                v[3] = Vec2(0, -s.y);
                PhysicsBody *tilePhysics = PhysicsBody::createEdgePolygon(v, 4);
                tilePhysics->setDynamic(false);
                tilePhysics->setCategoryBitmask(CATEGORY_MASK_WALL);
                tilePhysics->setCollisionBitmask(COLLISION_MASK_WALL);
                tilePhysics->setContactTestBitmask(CONTACT_MASK_WALL);

                tile->setTag(TAG_WALL);
                tile->setPhysicsBody(tilePhysics);
            }
        }
    }

    // setup map
    _map->setAnchorPoint(Vec2::ZERO);
    _map->setPosition(Vec2::ZERO);

    this->addChild(_map);

    // setup player
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player *player = Player::create();
        this->addChild(player);
        _players.push_back(player);
    }
    getPlayer()->setTag(TAG_PLAYER);
    getOpponent()->setTag(TAG_OPPOPENT);

    // setup contact handling
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(Stage::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
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

    // Scroll to put a player center on the screen
    step(0);
}

void Stage::step(float dt) {
    Size size = Director::getInstance()->getVisibleSize();
    Vec2 pos = getPlayer()->getPosition();
    this->setPosition(Vec2(size.width * 0.5 - pos.x, size.height * 0.5 - pos.y));

    for (int i = 0; i < _bullets.size(); i++) {
        Bullet *bullet = _bullets[i];
        bullet->step(dt);
        if (bullet->getLifePoint() < 0) {
            _bullets.erase(_bullets.begin() + i);
            bullet->removeFromParent();
            i--;
        }
    }
}

void Stage::addBullet(Bullet *bullet) {
    _bullets.push_back(bullet);
    addChild(bullet);
}

Player *Stage::getPlayer() {
    return _players[0];
}

Player *Stage::getOpponent() {
    return _players[1];
}

void Stage::setState(JSONPacker::GameState state) {
    if (getOpponent()->isCorrectUpdate(state.opponentPosition)) {
        getOpponent()->setPosition(state.opponentPosition);
    }
    getOpponent()->setMoveState(state.opponentMoveState);

    if (state.newBullet) {
        addBullet(state.newBullet);
    }
}

bool Stage::onContactBegin(cocos2d::PhysicsContact &contact) {
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
        bullet->setLifePoint(-1);
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
        bullet->setLifePoint(-1);
        return false;
    }

    // If a shot hits wall, remove the shot from the stage
    if (tagA == TAG_WALL && (tagB == TAG_PLAYER_BULLET || tagB == TAG_OPPOPENT_BULLET)) {
        bullet = dynamic_cast<Bullet *>(nodeB);
    } else if (tagB == TAG_WALL && (tagA == TAG_PLAYER_BULLET || tagA == TAG_OPPOPENT_BULLET)) {
        bullet = dynamic_cast<Bullet *>(nodeA);
    }

    if (bullet) {
        bullet->setLifePoint(-1);
        return false;
    }

    // bodies can collide
    return false;
}
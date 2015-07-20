//
//  Constants.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#ifndef Yuzu_Constants_h
#define Yuzu_Constants_h

static const std::string CODE_NAME = "Yuzu";
static const std::string GAME_NAME = "Rubber duck wars";

static const std::string DEFAULT_STAGE_FILE = "stage1.tmx";
static const std::string DEFAULT_BACKGROUND_LAYER_NAME = "background";
static const std::string DEFAULT_COLLISION_LAYER_NAME = "collision";

static const int MAX_PLAYERS = 2;

static const int DEFAULT_BULLET_LIFE = 30;

static const float DEFAULT_BULLET_SPEED = 1000.0f;
static const float DEFAULT_PLAYER_SPEED = 250.0f;

static const float TILE_WIDTH = 64.0f;
static const float TILE_HEIGHT = 32.0f;

// Physics
static const int CATEGORY_MASK_BULLET = 0x1;
static const int CATEGORY_MASK_WALL = 0x2;
static const int CATEGORY_MASK_PLAYER = 0x4;

static const int CONTACT_MASK_PLAYER = CATEGORY_MASK_BULLET;
static const int CONTACT_MASK_WALL = CATEGORY_MASK_BULLET;
static const int CONTACT_MASK_BULLET = CATEGORY_MASK_PLAYER | CATEGORY_MASK_WALL;

static const int COLLISION_MASK_PLAYER = CATEGORY_MASK_WALL;
static const int COLLISION_MASK_WALL = CATEGORY_MASK_PLAYER;
static const int COLLISION_MASK_BULLET = 0;

enum class FieldType {
    EMPTY = 0,
    GRASS = 1,
    LAKE = 2,
    WALL = 3,
};

enum class MoveState {
    STOP = 0,
    RIGHT = 1,
    LEFT = 2,
    UP = 3,
    DOWN = 4,
};

enum class Direction {
    RIGHT = 0,
    LEFT = 1,
    UP = 2,
    DOWN = 3,
};

#endif

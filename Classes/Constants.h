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
static const std::string GAME_NAME = "Rubber Duck Wars";

static const std::string DEFAULT_STAGE_FILE = "stage2.tmx";
static const std::string DEFAULT_BACKGROUND_LAYER_NAME = "background";
static const std::string DEFAULT_COLLISION_LAYER_NAME = "collision";

// Game Options
static const bool USE_SIMPLE_AIMING = false;
static const bool ALLOW_MORE_THAN_TWO_TAP = false;
static const bool ALLOW_WATER_SHOT = true;

static const int MAX_PLAYERS = 2;

static const int DEFAULT_PLAYER_LIFE = 32;
static const float DEFAULT_BULLET_LIFE = 0.64f;

static const float DEFAULT_BULLET_SPEED = 500.0f;
static const float DEFAULT_PLAYER_SPEED = 250.0f;

static const float TILE_WIDTH = 64.0f;
static const float TILE_HEIGHT = 32.0f;

// Physics
static const int TAG_PLAYER = 1;
static const int TAG_PLAYER_BULLET = 2;
static const int TAG_OPPOPENT = 3;
static const int TAG_OPPOPENT_BULLET = 4;
static const int TAG_WALL = 5;
static const int TAG_EGG = 6;

static const int CATEGORY_MASK_PLAYER = 1;
static const int CATEGORY_MASK_BULLET = 2;
static const int CATEGORY_MASK_WALL = 4;
static const int CATEGORY_MASK_EGG = 8;

static const int CONTACT_MASK_PLAYER = CATEGORY_MASK_BULLET | CATEGORY_MASK_EGG;
static const int CONTACT_MASK_WALL = CATEGORY_MASK_BULLET;
static const int CONTACT_MASK_BULLET = CATEGORY_MASK_PLAYER | CATEGORY_MASK_WALL | CATEGORY_MASK_EGG;
static const int CONTACT_MASK_EGG = CATEGORY_MASK_PLAYER | CATEGORY_MASK_BULLET;

static const int COLLISION_MASK_PLAYER = CATEGORY_MASK_WALL;
static const int COLLISION_MASK_WALL = CATEGORY_MASK_PLAYER;
static const int COLLISION_MASK_BULLET = 0;
static const int COLLISION_MASK_EGG = 0;

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
    UPPER_RIGHT = 5,
    UPPER_LEFT = 6,
    LOWER_RIGHT = 7,
    LOWER_LEFT = 8,
};

enum class Direction {
    RIGHT = 0,
    LEFT = 1,
    UP = 2,
    DOWN = 3,
};

#endif

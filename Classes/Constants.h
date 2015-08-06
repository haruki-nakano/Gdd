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

static const std::string DEFAULT_BACKGROUND_LAYER_NAME = "background";
static const std::string DEFAULT_COLLISION_LAYER_NAME = "collision";
static const std::string DEFAULT_WALL_LAYER_NAME = "wall";

// UI
static const char *FONT_NAME = "arial.ttf";
static const float FONT_SIZE = 32.0f;
static const float RESOLUTION_WIDTH = 1136.0f;
static const float RESOLUTION_HEIGHT = 640.0f;

static const cocos2d::Color3B LABEL_COLOR = cocos2d::Color3B(196, 196, 196);

// Game Options
static const int DEFAULT_STAGE_ID = 1;
static const int NUM_STAGES = 4;

static const bool USE_SIMPLE_AIMING = false;
static const bool ALLOW_MORE_THAN_TWO_TAP = false;
static const bool ALLOW_WATER_SHOT = true;
static const bool HIGH_SPEED_IN_WATER = true;

static const int MAX_PLAYERS = 2;

static const int INITIAL_PLAYER_LIFE = 30;
static const float INITIAL_BULLET_LIFE = 0.32;
static const float INITIAL_EGG_LIFE = 20;

static const int MAX_EGG_INTERVAL_SEC = 10;
static const int MIN_EGG_INTERVAL_SEC = 5;

static const clock_t KEEP_FIRING_THRESHOLD = CLOCKS_PER_SEC * 0.3f;
static const float INVINCIBLE_TIME = 10.0f; // 10 sec
static const clock_t ABNORMAL_FIRING_THRESHOLD = CLOCKS_PER_SEC * 0.1f;

static const int HEALING_POINTS = 5;

static const float DEFAULT_BULLET_SPEED = 1000.0f;
static const float DEFAULT_PLAYER_SPEED = 200.0f;

static const float TILE_WIDTH = 64.0f;
static const float TILE_HEIGHT = 32.0f;

// Physics
static const int TAG_PLAYER = 1;
static const int TAG_PLAYER_BULLET = 2;
static const int TAG_OPPOPENT = 3;
static const int TAG_OPPOPENT_BULLET = 4;
static const int TAG_WALL = 5;
static const int TAG_EGG = 6;
static const int TAG_ITEM = 7;

static const int CATEGORY_MASK_PLAYER = 1;
static const int CATEGORY_MASK_BULLET = 2;
static const int CATEGORY_MASK_WALL = 4;
static const int CATEGORY_MASK_EGG = 8;
static const int CATEGORY_MASK_ITEM = 16;

static const int CONTACT_MASK_PLAYER = CATEGORY_MASK_BULLET | CATEGORY_MASK_ITEM;
static const int CONTACT_MASK_WALL = CATEGORY_MASK_BULLET;
static const int CONTACT_MASK_BULLET = CATEGORY_MASK_PLAYER | CATEGORY_MASK_WALL | CATEGORY_MASK_EGG;
static const int CONTACT_MASK_EGG = CATEGORY_MASK_BULLET;
static const int CONTACT_MASK_ITEM = CATEGORY_MASK_PLAYER;

static const int COLLISION_MASK_PLAYER = CATEGORY_MASK_WALL | CATEGORY_MASK_EGG;
static const int COLLISION_MASK_WALL = CATEGORY_MASK_PLAYER | CATEGORY_MASK_EGG;
static const int COLLISION_MASK_BULLET = 0;
static const int COLLISION_MASK_EGG = CATEGORY_MASK_PLAYER | CATEGORY_MASK_WALL;
static const int COLLISION_MASK_ITEM = 0;

enum class EventType {
    CHANGE_PLAYERS_DIRECTION = 0,
    HIT_PLAYER = 1,
    FIRE_BULLT = 2,
    STOP_PLAYERS_MOVING = 3,
    GAME_OVER = 4,
    HIT_EGG = 5,
    APPEAR_EGG = 6,
    GET_HEAL = 7,
    PLAYER_GET_INVINCIBLE = 8,
    OPPONENT_GET_INVINCIBLE = 9,
};

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
    LEFT,
    UP,
    DOWN,
    UPPER_RIGHT,
    UPPER_LEFT,
    LOWER_RIGHT,
    LOWER_LEFT,
};

enum class EggState {
    IDLE = 0,
    EGG,
    ITEM,
};

enum class EggItemType {
    HEALING,
    // SPEED_UP,
    // SUPER_SHOT,
    SUPER_STAR,
    SIZE
};

static const char *GUN_NAMES[] = {"Straight Water Gun", "Basic Water Gun",    "3 Way Water Gun", "Sprinkler",
                                  "V-Laser Water Gun",  "Marathon Water Gun", "Charger"};
enum class Gun {
    STRAIGHT_GUN = 0,
    BASIC_GUN,
    THREE_WAY_GUN,
    SPRINKLER,
    V_LASER_GUN,
    MARATHON_GUN,
    CHARGER,
    SIZE,
};

enum class PlayerColor {
    DEFAULT = 0,
    YELLOW,
    GREEN,
    NUM_COLORS,
};

enum class SceneState {
    LOBBY = 0,
    TRAINING,
    VS_MODE,
    NUM_STATES,
};

#endif

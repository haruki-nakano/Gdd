//
//  Constants.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#ifndef Yuzu_Constants_h
#define Yuzu_Constants_h

/* General */
static const std::string CODE_NAME = "Yuzu";
static const std::string GAME_NAME = "Rubber Duck Wars";
static const std::string SERVICE_NAME = "RDW";

static const std::string DEFAULT_BACKGROUND_LAYER_NAME = "background";
static const std::string DEFAULT_COLLISION_LAYER_NAME = "collision";
static const std::string DEFAULT_WALL_LAYER_NAME = "wall";

/* Debugging */
static const bool PHYSICS_TEST = true;
static const bool USE_SAME_RESOLUTION = false;
static const bool USE_SIMPLE_AIMING = false;
static const bool ALLOW_MORE_THAN_TWO_TAP = false;
static const bool ALLOW_WATER_SHOT = true;
static const bool HIGH_SPEED_IN_WATER = true;

/* Game Settings */
static const int DEFAULT_STAGE_ID = 3;
static const int NUM_STAGES = 6;
static const int MAX_PLAYERS = 2;

static const int INITIAL_PLAYER_LIFE = 30;
static const int INITIAL_WATER_LIFE = 100;
static const float INITIAL_BULLET_LIFE = 0.32;
static const float INITIAL_EGG_LIFE = 20;

static const int MAX_WEAPON_INTERVAL_SEC = 5;
static const int MIN_WEAPON_INTERVAL_SEC = 3;

static const int MAX_EGG_INTERVAL_SEC = 10;
static const int MIN_EGG_INTERVAL_SEC = 5;

static const int HEALING_POINTS = 5;
static const float BLINK_TIME = 0.4f;                    // 10 sec
static const float INVINCIBLE_TIME = 10.0f;              // 10 sec
static const float GOGGLES_TIME = 10.0f;                 // 10 sec
static const float ADD_WATTER_THRESHOLD = 5.0f / 100.0f; // Need 5 sec for full charging

static const clock_t KEEP_FIRING_THRESHOLD = CLOCKS_PER_SEC * 0.3f;
static const clock_t ABNORMAL_FIRING_THRESHOLD = CLOCKS_PER_SEC * 0.1f;

static const float DEFAULT_BULLET_SPEED = 1000.0f;
static const float DEFAULT_PLAYER_SPEED = 200.0f;

// TODO: Refactoring
enum class EventType {
    CHANGE_PLAYERS_DIRECTION = 0,
    HIT_PLAYER = 1,
    FIRE_BULLT = 2,
    STOP_PLAYERS_MOVING = 3,
    GAME_OVER = 4,
    HIT_EGG = 5,
    APPEAR_EGG = 6,
    GET_HEAL = 7,
    GET_GOGGLES = 8,
    PLAYER_GET_INVINCIBLE = 9,
    OPPONENT_GET_INVINCIBLE = 10,
    GET_WEAPON = 11,
    APPEAR_WEAPON = 12,
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

enum class EggItemType { HEALING, GOGGLES, /* RED_PEPPER, */ SUPER_STAR, SIZE };

static const char *GUN_NAMES[] = {"Straight Water Gun", "Basic Water Gun", "3 Way Water Gun", "Sprinkler",
                                  "V-Laser Water Gun", "Marathon Water Gun", "Charger", "Shotgun", "Sniper"};
enum class Gun {
    STRAIGHT_GUN = 0,
    BASIC_GUN,
    THREE_WAY_GUN,
    SPRINKLER,
    V_LASER_GUN,
    MARATHON_GUN,
    CHARGER,
    SHOTGUN,
    SNIPER,
    SIZE,
};

enum class Animations {
    INVINCIBLE,
    BULLET_HIT,
    MOVING,
    NUM_ANIMATIONS,
};

enum class SceneState {
    LOBBY = 0,
    TRAINING,
    VS_MODE,
    NUM_STATES,
};

#endif

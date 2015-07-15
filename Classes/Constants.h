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

static const int MAX_PLAYERS = 2;
static const float INITIAL_STEP_INTERVAL = 1.0f / 60.0f;

enum class FieldType {
    EMPTY = 0,
    GRASS = 1,
    LAKE = 2,
    WALL = 3,
};

enum class MovingState {
    STOP = 0,
    RIGHT = 1,
    LEFT = 2,
    UP = 3,
    DOWN = 4,
};

#endif

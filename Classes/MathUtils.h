//
//  MathUtils.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#ifndef Yuzu_MathUtils_h
#define Yuzu_MathUtils_h

#include "Constants.h"

namespace MathUtils {

static inline cocos2d::Vec2 forDegreesAngle(const float a) {
    const float r = CC_DEGREES_TO_RADIANS(a);
    return cocos2d::Vec2(sinf(r), cosf(r));
}

static inline float degreesAngle(const cocos2d::Vec2 v) {
    return CC_RADIANS_TO_DEGREES(atan2f(v.x, v.y));
}

static MoveState convertVec2ToMoveState(const cocos2d::Vec2 v) {
    if (v.distance(cocos2d::Vec2(0.0f, 0.0f)) < 1.0f) {
        return MoveState::STOP;
    }

    float angle = degreesAngle(v);

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
}

#endif
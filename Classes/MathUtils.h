//
//  MathUtils.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#ifndef Yuzu_MathUtils_h
#define Yuzu_MathUtils_h

namespace MathUtils {
static inline cocos2d::Vec2 forDegreesAngle(const float a) {
    const float r = CC_DEGREES_TO_RADIANS(a);
    return cocos2d::Vec2(sinf(r), cosf(r));
}

static inline float degreesAngle(const cocos2d::Vec2 v) {
    return CC_RADIANS_TO_DEGREES(atan2f(v.x, v.y));
}
}

#endif
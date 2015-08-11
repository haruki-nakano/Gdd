//
//  Bar.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/08/11.
//
//

#ifndef __Yuzu__Bar__
#define __Yuzu__Bar__

#include "cocos-ext.h"

USING_NS_CC_EXT;

class Bar : public cocos2d::Node {
public:
    enum Type {
        LIFE,
        CHARGING,
    };

    static Bar *create(Type type) {
        Bar *pRet = new (std::nothrow) Bar;
        if (pRet && pRet->init(type)) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            pRet = NULL;
            return NULL;
        }
    }

    void setPoint(const int point);

    void setType(const Type type);

private:
    bool init(Type type);
    void onEnter() override;

    Type _type;
    cocos2d::ui::Scale9Sprite *_bar;
    cocos2d::Size _initialContentsize;
    int _initialPoint;
};

#endif /* defined(__Yuzu__Bar__) */

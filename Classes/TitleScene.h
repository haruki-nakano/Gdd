//
//  TitleScene.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#ifndef __Yuzu__TitleScene__
#define __Yuzu__TitleScene__

#include "cocos2d.h"
class TitleScene : public cocos2d::Layer {
public:
    static cocos2d::Scene *createScene();

    virtual bool init();

    CREATE_FUNC(TitleScene);
};

#endif /* defined(__Yuzu__TitleScene__) */

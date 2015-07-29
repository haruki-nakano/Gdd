//
//  LifeBar.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/21.
//
//

#ifndef __Yuzu__LifeBar__
#define __Yuzu__LifeBar__

class LifeBar : public cocos2d::Node {
public:
    CREATE_FUNC(LifeBar);

    void setLifePoint(const int lifePoints);

private:
    bool init() override;
    void onEnter() override;

    cocos2d::Sprite *_bar;
};

#endif /* defined(__Yuzu__LifeBar__) */

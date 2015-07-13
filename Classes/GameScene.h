//
//  GameScene.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#ifndef __Yuzu__GameScene__
#define __Yuzu__GameScene__

#include "CocosGUI.h"

class Player;

class GameScene : public cocos2d::Node {
public:
    CREATE_FUNC(GameScene);

protected:
    // Lifecycle
    bool init() override;
    void onEnter() override;
    void setupTouchHandling();

    // Game Logic
    void setGameActive(bool active);
    void step(float dt);

    // UI
    void backButtonPressed(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);

    bool _active;
    float _stepInterval;
    Player *_player;
};

#endif /* defined(__Yuzu__GameScene__) */

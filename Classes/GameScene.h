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
    Player *player;
    // Lifecycle
    bool init() override;
    void onEnter() override;
    void setupTouchHandling();

    // Game Logic

    // UI
    void backButtonPressed(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
};

#endif /* defined(__Yuzu__GameScene__) */

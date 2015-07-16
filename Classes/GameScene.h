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

#include "Constants.h"

class Player;
class Stage;

class GameScene : public cocos2d::Node {
public:
    CREATE_FUNC(GameScene);

    void setNetworkedSession(bool networkedSession);
    void receivedData(const void *data, unsigned long length);

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
    bool _networkedSession;
    float _stepInterval;
    Stage *_stage;

    // Networking
    void sendGameStateOverNetwork();

    // Utility
    MoveState convertVec2ToMoveState(cocos2d::Vec2 v);
};

#endif /* defined(__Yuzu__GameScene__) */

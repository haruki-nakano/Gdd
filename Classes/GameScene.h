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
#include "Bullet.h"
#include "LifeBar.h"

class Player;
class Stage;

class GameScene : public cocos2d::Node {
public:
    CREATE_FUNC(GameScene);

    void setNetworkedSession(bool networkedSession, bool isHost);
    void receivedData(const void *data, unsigned long length);

    // Utility
    static MoveState convertVec2ToMoveState(const cocos2d::Vec2 v);

protected:
    // Lifecycle
    bool init() override;
    void onEnter() override;
    void setupTouchHandling();
    bool onContactBegin(cocos2d::PhysicsContact &contact);

    // Game Logic
    void setGameActive(bool active);
    void step(float dt);
    void update(float dt) override;

    // UI
    void updateLifePoints(int playerLifePoint, int opponentLifePoint);
    void backButtonPressed(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);

    bool _active;
    bool _networkedSession;
    bool _isHost;
    Stage *_stage;
    LifeBar *_playerLifeBar;
    LifeBar *_opponentsLifeBar;

    // Networking
    void sendGameStateOverNetwork(Bullet *newBullet);
};

#endif /* defined(__Yuzu__GameScene__) */

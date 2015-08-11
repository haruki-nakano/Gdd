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
#include "Bar.h"

class Egg;
class Player;
class Stage;

class GameScene : public cocos2d::Node {
public:
    CREATE_FUNC(GameScene);

    // Networking
    void setNetworkedSession(bool networkedSession, bool isHost);
    void receivedData(const void *data, unsigned long length);

    void setStageId(int stageId);

    bool isGameActive() const;

protected:
    // Lifecycle
    bool init() override;
    void onEnter() override;

    // Game Logic

    void setGameActive(bool active);
    void step(float dt);
    void update(float dt) override;
    void gameOver();

    // Event
    void setupTouchHandling();
    bool onContactBegin(cocos2d::PhysicsContact &contact);
    void backButtonPressed(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);

    //
    void showDialog();

    int _stageId;
    bool _active;
    bool _networkedSession;
    bool _isHost;
    Stage *_stage;

    Bar *_playerWaterBar;
    Bar *_playerLifeBar;
    Bar *_opponentsLifeBar;

    // Networking
    void sendGameStateOverNetwork(EventType event, std::vector<Bullet *> newBullets = std::vector<Bullet *>(),
                                  bool newEgg = false);
};

#endif /* defined(__Yuzu__GameScene__) */

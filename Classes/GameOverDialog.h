//
//  GameOverDialog.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/08/05.
//
//

#ifndef __Yuzu__GameOverDialog__
#define __Yuzu__GameOverDialog__

#include "cocos2d.h"
#include "CocosGUI.h"

class GameOverDialog : public cocos2d::Node {
public:
    static GameOverDialog *createWithLifePoints(int playerLifePoint, int opponentLifePoint);

    void setLifePoints(int playerLifePoint, int opponentLifePoint);
    void buttonPressed(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);

private:
    bool initWithLifePoints(int playerLifePoint, int opponentLifePoint);
    void onEnter() override;

    int _playerLifePoint;
    int _opponentLifePoint;
    cocos2d::ui::Button *_button;
};
#endif /* defined(__Yuzu__GameOverDialog__) */

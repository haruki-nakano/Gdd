//
//  Lobby.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#ifndef __Yuzu__Lobby__
#define __Yuzu__Lobby__

#include "cocos2d.h"
#include "CocosGUI.h"
// #include "SceneManager.h"

class Lobby : public cocos2d::Node {
public:
    CREATE_FUNC(Lobby);

private:
    bool init() override;
    void onEnter() override;

    void setupUI();

    void singlePlayerPressed(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
    void multiPlayerPressed(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
    void howToPlayPressed(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
    void dialogTouched(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);

    cocos2d::ui::Button *_dialog;
};

#endif /* defined(__Yuzu__Lobby__) */

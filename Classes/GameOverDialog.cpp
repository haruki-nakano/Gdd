//
//
//  GameOverDialog.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/08/05.
//
//

#include "GameOverDialog.h"

#include "CocosGUI.h"
#include "Constants.h"
#include "SceneManager.h"

using namespace cocos2d;

GameOverDialog *GameOverDialog::createWithLifePoints(int playerLifePoint, int opponentLifePoint) {
    GameOverDialog *dialog = new (std::nothrow) GameOverDialog();
    if (dialog && dialog->initWithLifePoints(playerLifePoint, opponentLifePoint)) {
        dialog->autorelease();
        return dialog;
    }
    CC_SAFE_DELETE(dialog);
    return nullptr;
}

bool GameOverDialog::initWithLifePoints(int playerLifePoint, int opponentLifePoint) {
    if (!Node::init()) {
        return false;
    }
    _playerLifePoint = playerLifePoint;
    _opponentLifePoint = opponentLifePoint;
    return true;
}

void GameOverDialog::onEnter() {
    Node::onEnter();

    Size visibleSize = Director::getInstance()->getVisibleSize();
    // this->setContentSize(visibleSize);

    LayerColor *background = LayerColor::create(Color4B(0, 0, 0, 178));
    this->addChild(background);
    // background->setContentSize(visibleSize);

    // TODO: We should decide winner in the game scene
    std::string filename = _playerLifePoint == _opponentLifePoint
                               ? "draw.png"
                               : (_playerLifePoint > _opponentLifePoint ? "youWin.png" : "youLose.png");
    Sprite *title = Sprite::create(filename);
    title->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    title->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.9f));

    std::stringstream ss;
    ss << _playerLifePoint << " - " << _opponentLifePoint;
    // TODO: Do not use magic number
    auto scoreLabel = ui::Text::create(ss.str(), FONT_NAME, 96);
    scoreLabel->setAnchorPoint(Vec2(0.5f, 0.5f));
    scoreLabel->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.5f));
    scoreLabel->setColor(Color3B::WHITE);

    _button = ui::Button::create();
    _button->loadTextures("buttonOk.png", "buttonOkPressed.png");
    _button->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    _button->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.1f));
    _button->addTouchEventListener(CC_CALLBACK_2(GameOverDialog::buttonPressed, this));

    this->addChild(title);
    this->addChild(scoreLabel);
    this->addChild(_button);
}

void GameOverDialog::buttonPressed(cocos2d::Ref *pSender, ui::Widget::TouchEventType eEventType) {
    if (eEventType == ui::Widget::TouchEventType::ENDED) {
        SceneManager::getInstance()->returnToLobby();
    }
}

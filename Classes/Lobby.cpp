//
//  Lobby.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "Lobby.h"

#include "SceneManager.h"

using namespace cocos2d;

bool Lobby::init() {
    if (!Node::init()) {
        return false;
    }

    LayerColor *background = LayerColor::create(Color4B(255, 255, 255, 255));

    this->addChild(background);

    return true;
}

void Lobby::onEnter() {
    Node::onEnter();

    SceneManager::getInstance()->receiveMultiplayerInvitations();

    setupUI();
}

void Lobby::setupUI() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    Sprite *logo = Sprite::create("logo.png");
    logo->setAnchorPoint(Vec2(0.5f, 0.5f));
    logo->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.7f));
    this->addChild(logo);

    ui::Button *howToPlayButton = ui::Button::create();
    howToPlayButton->setAnchorPoint(Vec2(0.5f, 0.0f));
    howToPlayButton->setPosition(Vec2(visibleSize.width / 2.0f, visibleSize.height * 0.3f));
    howToPlayButton->loadTextures("buttonHowToPlay.png", "buttonPressedHowToPlay.png");
    howToPlayButton->addTouchEventListener(CC_CALLBACK_2(Lobby::howToPlayPressed, this));
    this->addChild(howToPlayButton);

    ui::Button *singlePlayerButton = ui::Button::create();
    singlePlayerButton->setAnchorPoint(Vec2(1.0f, 0.0f));
    singlePlayerButton->setPosition(Vec2(visibleSize.width / 2.0f - 48.0f, visibleSize.height * 0.1f));
    singlePlayerButton->loadTextures("buttonTraining.png", "buttonPressedTraining.png");
    singlePlayerButton->addTouchEventListener(CC_CALLBACK_2(Lobby::singlePlayerPressed, this));
    this->addChild(singlePlayerButton);

    ui::Button *multiPlayerButton = ui::Button::create();
    multiPlayerButton->setAnchorPoint(Vec2(0.0f, 0.0f));
    multiPlayerButton->setPosition(Vec2(visibleSize.width / 2.0f + 48.0f, visibleSize.height * 0.1f));
    multiPlayerButton->loadTextures("buttonVsMode.png", "buttonPressedVsMode.png");
    multiPlayerButton->addTouchEventListener(CC_CALLBACK_2(Lobby::multiPlayerPressed, this));
    this->addChild(multiPlayerButton);

    _dialog = ui::Button::create();
    _dialog->loadTextures("backgroundTitle.png", "backgroundTitle.png");
    _dialog->setAnchorPoint(Vec2(0.5f, 0.5f));
    _dialog->setPosition(Vec2(visibleSize.width / 2.0f, visibleSize.height / 2.0f));
    _dialog->setOpacity(0);
    _dialog->setEnabled(false);
    _dialog->addTouchEventListener(CC_CALLBACK_2(Lobby::dialogTouched, this));

    this->addChild(_dialog);

    return;
}

void Lobby::singlePlayerPressed(cocos2d::Ref *pSender, ui::Widget::TouchEventType eEventType) {
    if (eEventType == ui::Widget::TouchEventType::ENDED) {
        SceneManager::getInstance()->enterGameScene(false);
    }
}

void Lobby::multiPlayerPressed(cocos2d::Ref *pSender, ui::Widget::TouchEventType eEventType) {
    if (eEventType == ui::Widget::TouchEventType::ENDED) {
        SceneManager::getInstance()->showPeerList();
    }
}

void Lobby::howToPlayPressed(cocos2d::Ref *pSender, ui::Widget::TouchEventType eEventType) {
    if (eEventType == ui::Widget::TouchEventType::ENDED) {
        _dialog->setEnabled(true);
        auto action = FadeIn::create(0.1);
        _dialog->runAction(action);
    }
}

void Lobby::dialogTouched(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType) {
    if (eEventType == ui::Widget::TouchEventType::ENDED) {
        _dialog->setEnabled(false);
        auto action = FadeOut::create(0.1);
        _dialog->runAction(action);
    }
}

void Lobby::dismissAllDialogs() {
    if (_dialog) {
        _dialog->setEnabled(false);
        auto action = FadeOut::create(0.0);
        _dialog->runAction(action);
    }
}
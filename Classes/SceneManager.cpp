//
//  SceneManager.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "SceneManager.h"
#include "GameScene.h"

using namespace cocos2d;

#pragma mark -
#pragma mark LifeCycle

static SceneManager *sharedSceneManager = nullptr;

SceneManager *SceneManager::getInstance() {
    if (!sharedSceneManager) {
        sharedSceneManager = new (std::nothrow) SceneManager();
    }

    return sharedSceneManager;
}

SceneManager::SceneManager() {
    _gameScene = nullptr;
    _networkingWrapper = std::unique_ptr<NetworkingWrapper>(new NetworkingWrapper());
    _networkingWrapper->setDelegate(this);
}

SceneManager::~SceneManager() {
}

#pragma mark -
#pragma mark Public Methods

void SceneManager::enterGameScene(bool networked) {
    Scene *scene = Scene::create();
    _gameScene = GameScene::create();
    _gameScene->setNetworkedSession(networked);

    scene->addChild(_gameScene);

    Director::getInstance()->pushScene(scene);
}

void SceneManager::returnToLobby() {
    if (_gameScene) {
        Director::getInstance()->popScene();
        _gameScene = nullptr;
    }
}

void SceneManager::showPeerList() {
    _networkingWrapper->showPeerList();
}

void SceneManager::receiveMultiplayerInvitations() {
    _networkingWrapper->startAdvertisingAvailability();
}

void SceneManager::sendData(const void *data, unsigned long length) {
    _networkingWrapper->sendData(data, length);
}

#pragma mark -
#pragma mark NetworkingWrapperDelegate Methods

void SceneManager::receivedData(const void *data, unsigned long length) {
    if (_gameScene) {
        _gameScene->receivedData(data, length);
    }
}

void SceneManager::stateChanged(ConnectionState state) {
    switch (state) {
        case ConnectionState::CONNECTING:
            CCLOG("Connecting...");
            break;
        case ConnectionState::NOT_CONNECTED:
            CCLOG("Not connected");
            break;
        case ConnectionState::CONNECTED:
            _networkingWrapper->stopAdvertisingAvailability();
            enterGameScene(true);
            break;
    }
}
//
//  SceneManager.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "SceneManager.h"
#include "GameScene.h"
#include "Lobby.h"

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
    _lobby = nullptr;
    _waiting = false;
    _networkingWrapper = std::unique_ptr<NetworkingWrapper>(new NetworkingWrapper());
    _networkingWrapper->setDelegate(this);
}

SceneManager::~SceneManager() {
}

#pragma mark -
#pragma mark Public Methods

void SceneManager::enterGameScene(bool networked, int stageId) {
    if (_lobby) {
        _lobby->dismissAllDialogs();
    }
    Scene *scene = Scene::createWithPhysics();
#if defined(COCOS2D_DEBUG)
 scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
#endif
    _gameScene = GameScene::create();

    bool isHost = true;
    if (networked) {
        isHost = _networkingWrapper->isHost();
    }
    _gameScene->setNetworkedSession(networked, isHost);
    _gameScene->setStageId(stageId);

    scene->addChild(_gameScene);

    Director::getInstance()->pushScene(TransitionFade::create(0.5f, scene, Color3B::BLACK));
}

void SceneManager::returnToLobby() {
    if (_gameScene) {
        Director::getInstance()->popScene();
        _gameScene = nullptr;
        _waiting = false;
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

void SceneManager::setLobby(Lobby *lobby) {
    _lobby = lobby;
}

#pragma mark -
#pragma mark NetworkingWrapperDelegate Methods

void SceneManager::receivedData(const void *data, unsigned long length) {
    if (_gameScene) {
        _gameScene->receivedData(data, length);
    } else if (_waiting) {
        _waiting = false;
        const char *cstr = reinterpret_cast<const char *>(data);
        int stageId = atoi(cstr);
        enterGameScene(true, stageId);
    }
}

void SceneManager::stateChanged(ConnectionState state) {
    switch (state) {
        case ConnectionState::CONNECTING:
            CCLOG("Connecting...");
            break;
        case ConnectionState::NOT_CONNECTED:
            if (_gameScene) {
                MessageBox("Unable to connect, please check your internet connection", "CONNECTION ERROR");
                SceneManager::getInstance()->returnToLobby();
            }
            CCLOG("Not connected");
            break;
        case ConnectionState::CONNECTED:
            _networkingWrapper->stopAdvertisingAvailability();
            _waiting = true;

            if (_networkingWrapper->isHost()) {
                // Select stage
                int stageId = random(1, NUM_STAGES);
                std::stringstream ss;
                ss << stageId;
                std::string str = ss.str();
                // We must ensure that data is sent
                sendData(str.c_str(), str.size());
                enterGameScene(true, stageId);
            }
            break;
    }
}

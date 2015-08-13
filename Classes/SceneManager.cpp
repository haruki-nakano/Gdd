//
//  SceneManager.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "SceneManager.h"

#include "GameScene.h"
#include "JSONPacker.h"
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
    _connectionState = ConnectionState::NOT_CONNECTED;
    _networkingWrapper = std::unique_ptr<NetworkingWrapper>(new NetworkingWrapper());
    _networkingWrapper->setDelegate(this);
    _networkingWrapper->setServiceName(SERVICE_NAME);
    _networkingWrapper->setMinimumPeers(MAX_PLAYERS - 1);
    _networkingWrapper->setMaximumPeers(MAX_PLAYERS - 1);
}

SceneManager::~SceneManager() {
}

#pragma mark -
#pragma mark Public Methods

void SceneManager::enterGameScene(bool networked, int stageId) {
    _state = networked ? SceneState::VS_MODE : SceneState::TRAINING;

    if (_lobby) {
        _lobby->dismissAllDialogs();
    }
    Scene *scene = Scene::createWithPhysics();
#if defined(COCOS2D_DEBUG)
// scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
#endif
    _gameScene = GameScene::create();

    bool isHost = true;
    if (networked) {
        isHost = this->isHost();
    }
    _gameScene->setNetworkedSession(networked, isHost);
    _gameScene->setStageId(stageId);

    scene->addChild(_gameScene);

    Director::getInstance()->pushScene(TransitionFade::create(0.5f, scene, Color3B::BLACK));
}

void SceneManager::returnToLobby() {
    // if (_connectionState == ConnectionState::CONNECTED) {
    _networkingWrapper->disconnect();
    // }
    if (_state == SceneState::TRAINING || _state == SceneState::VS_MODE) {
        _state = SceneState::LOBBY;
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

void SceneManager::stopMultiplayerInvitations() {
    _networkingWrapper->stopAdvertisingAvailability();
}

void SceneManager::sendData(const void *data, unsigned long length, SendDataMode mode) {
    _networkingWrapper->sendData(data, length, mode);
}

void SceneManager::setLobby(Lobby *lobby) {
    _state = SceneState::LOBBY;
    _lobby = lobby;
}

#pragma mark -
#pragma mark NetworkingWrapperDelegate Methods

void SceneManager::receivedData(const void *data, unsigned long length) {
    const char *cstr = reinterpret_cast<const char *>(data);
    std::string json = std::string(cstr, length);
    if (_state == SceneState::VS_MODE && JSONPacker::dataTypeForData(json) == JSONPacker::DataType::GAME_STATE) {
        // TODO: we can send std::string
        _gameScene->receivedData(data, length);
    } else if (_state == SceneState::LOBBY && JSONPacker::dataTypeForData(json) == JSONPacker::DataType::STAGE_SELECT) {
        if (!this->isHost()) {
            int stageId = JSONPacker::unpackStageSelectJSON(json);
            enterGameScene(true, stageId);
        }
    }
}

void SceneManager::stateChanged(ConnectionState state) {
    switch (state) {
        case ConnectionState::CONNECTING:
            _connectionState = ConnectionState::CONNECTED;
            CCLOG("Connecting...");
            break;
        case ConnectionState::NOT_CONNECTED:
            _connectionState = ConnectionState::NOT_CONNECTED;
            if (_state == SceneState::VS_MODE && _gameScene && _gameScene->isGameActive()) {
                MessageBox("Unable to connect, please check your internet connection", "CONNECTION ERROR");
                SceneManager::getInstance()->returnToLobby();
            }
            CCLOG("Not connected");
            break;
        case ConnectionState::CONNECTED:
            _connectionState = ConnectionState::CONNECTED;
            if (_state == SceneState::LOBBY) {
                if (this->isHost()) {
                    int stageId = random(1, NUM_STAGES);
                    std::string json = JSONPacker::packStageSelectJSON(stageId);
                    // We must ensure that data is sent
                    sendData(json.c_str(), json.size(), SendDataMode::Reliable);
                    enterGameScene(true, stageId);
                }
            } else {
                _networkingWrapper->disconnect();
            }
    }
}

bool SceneManager::isHost() {
    auto peers = _networkingWrapper->getPeerList();
    std::string me = _networkingWrapper->getDeviceName();
    return peers[0].compare(me) > 0;
}

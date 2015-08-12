//
//  SceneManager.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#ifndef __Yuzu__SceneManager__
#define __Yuzu__SceneManager__

#include "NetworkingWrapper.h"
#include "Constants.h"

class GameScene;
class Lobby;

class SceneManager : public NetworkingDelegate {
public:
    static SceneManager *getInstance();

    void enterGameScene(bool networked, int stageId);
    void returnToLobby();

    void showPeerList();
    void receiveMultiplayerInvitations();
    void sendData(const void *data, unsigned long length, SendDataMode mode = SendDataMode::Unreliable);

    void setLobby(Lobby *lobby);

private:
    std::unique_ptr<NetworkingWrapper> _networkingWrapper;
    GameScene *_gameScene;
    Lobby *_lobby;
    ConnectionState _connectionState;
    bool _isAdvertising;

    SceneManager();
    ~SceneManager();

    SceneState _state;

    bool isHost();

    void receivedData(const void *data, unsigned long length);
    void stateChanged(ConnectionState state);
};

#endif /* defined(__Yuzu__SceneManager__) */

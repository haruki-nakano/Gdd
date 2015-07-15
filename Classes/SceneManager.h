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

class GameScene;

class SceneManager : public NetworkingDelegate {
public:
    static SceneManager *getInstance();

    void enterGameScene(bool networked);
    void returnToLobby();

    void showPeerList();
    void receiveMultiplayerInvitations();
    void sendData(const void *data, unsigned long length);

private:
    std::unique_ptr<NetworkingWrapper> _networkingWrapper;
    GameScene *_gameScene;

    SceneManager();
    ~SceneManager();

    void receivedData(const void *data, unsigned long length);
    void stateChanged(ConnectionState state);
};

#endif /* defined(__Yuzu__SceneManager__) */

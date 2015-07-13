//
//  SceneManager.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#ifndef __Yuzu__SceneManager__
#define __Yuzu__SceneManager__

class GameScene;

class SceneManager {
public:
    static SceneManager *getInstance();

    void enterGameScene(bool networked);
    void returnToLobby();

private:
    GameScene *_gameScene;

    SceneManager();
    ~SceneManager();
};

#endif /* defined(__Yuzu__SceneManager__) */

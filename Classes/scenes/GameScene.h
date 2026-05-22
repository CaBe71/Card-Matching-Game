#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "cocos2d.h"
#include "../controllers/GameController.h"

class GameScene : public cocos2d::Scene
{
public:
    static GameScene* create();
    virtual bool init() override;

private:
    void createUI();
    void onUndoButtonClicked();
    void onRestartButtonClicked();
    void updateScoreDisplay(int score);
    void updateComboDisplay(int combo);
    void updateStackCount(int count);
    void showScorePopup(int points);
    void showGameEnd(bool won);

    GameController* _gameController;
    cocos2d::Label* _scoreLabel;
    cocos2d::Label* _comboLabel;
    cocos2d::Label* _stackCountLabel;
};

#endif

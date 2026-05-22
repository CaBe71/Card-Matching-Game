#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "cocos2d.h"
#include "../models/GameModel.h"
#include "../views/GameView.h"
#include "../managers/UndoManager.h"

class LevelConfigLoader;
class GameModelGenerator;

class GameController
{
public:
    GameController();
    ~GameController();

    void startGame(int levelId);
    void restartGame();
    bool handleCardClick(int cardId);
    void handleDrawCard();
    void handleUndo();

    cocos2d::Node* getGameView() const { return _gameView; }
    GameModel* getGameModel() const { return _gameModel; }

    void setScoreCallback(const std::function<void(int)>& cb) { _scoreCallback = cb; }
    void setComboCallback(const std::function<void(int)>& cb) { _comboCallback = cb; }
    void setGameEndCallback(const std::function<void(bool)>& cb) { _gameEndCallback = cb; }
    void setStackCountCallback(const std::function<void(int)>& cb) { _stackCountCallback = cb; }

    // 检查是否有可匹配牌
    bool hasAnyMatch() const;

private:
    bool checkCardsMatch(const CardModel* c1, const CardModel* c2) const;
    void checkGameEnd();

    GameModel* _gameModel = nullptr;
    GameView* _gameView = nullptr;
    UndoManager* _undoManager = nullptr;
    LevelConfigLoader* _configLoader = nullptr;
    GameModelGenerator* _modelGenerator = nullptr;

    std::function<void(int)> _scoreCallback;
    std::function<void(int)> _comboCallback;
    std::function<void(bool)> _gameEndCallback;
    std::function<void(int)> _stackCountCallback;
};

#endif

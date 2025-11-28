#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "cocos2d.h"
#include "../models/GameModel.h"
#include "../views/GameView.h"
#include "../managers/UndoManager.h"
#include "../services/CardService.h"
#include "../services/GameModelGenerator.h"
#include "../configs/loaders/LevelConfigLoader.h"

USING_NS_CC;

// 游戏主控制器，协调模型和视图，处理游戏逻辑

class GameController : public Node
{
public:
    
    static GameController* create();

    
    virtual bool init() override;

    
    void startGame(int levelId = 1);

    // 处理卡牌点击事件
    bool handleCardClick(int cardId);

    // 处理回退按钮点击
    void handleUndo();

    // Getters
    GameModel* getGameModel() const { return _gameModel; }
    GameView* getGameView() const { return _gameView; }

private:

    // 处理桌面卡牌点击
    bool handlePlayfieldCardClick(CardModel* playfieldCard);
    
    // 处理底牌点击（从备用牌堆抽牌
    bool handleBottomCardClick(CardModel* clickedCard);


    /**
     * @brief 记录卡牌移动的回退动作
     * @param movedCard 移动的卡牌
     * @param originalPosition 原始位置
     * @param replacedCard 被替换的卡牌（可选）
     */
    void GameController::recordUndoAction(CardModel* movedCard, const Vec2& originalPosition, CardModel* replacedCard);
    void GameController::undoCardMatch(const UndoAction& action);
    void GameController::undoDrawCard(const UndoAction& action);

    // 刷新游戏视图
    void refreshGameView();

    // 从备用牌堆抽取新牌
    bool drawFromReserveToHand();

    CardModel* findCardInAllContainers(int cardId);
    void removeCardFromAllContainers(int cardId);


private:
    GameModel* _gameModel;          // 游戏数据模型
    GameView* _gameView;            // 游戏视图
    UndoManager* _undoManager;      // 回退管理器
};

#endif // GAME_CONTROLLER_H
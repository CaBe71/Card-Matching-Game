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

/**
 * @class GameController
 * @brief 游戏主控制器，协调模型和视图，处理游戏逻辑
 */
class GameController : public Node
{
public:
    /**
     * @brief 创建 GameController 实例
     */
    static GameController* create();

    /**
     * @brief 初始化控制器
     */
    virtual bool init() override;

    /**
     * @brief 开始游戏
     * @param levelId 关卡ID
     */
    void startGame(int levelId = 1);

    /**
     * @brief 处理卡牌点击事件
     * @param cardId 被点击的卡牌ID
     * @return 是否处理成功
     */
    bool handleCardClick(int cardId);

    /**
     * @brief 处理回退按钮点击
     */
    void handleUndo();

    // Getters
    GameModel* getGameModel() const { return _gameModel; }
    GameView* getGameView() const { return _gameView; }

private:
    /**
     * @brief 处理手牌区卡牌点击
     * @param cardId 卡牌ID
     * @return 是否处理成功
     */
    bool handleStackCardClick(int cardId);

    /**
     * @brief 处理桌面卡牌点击
     * @param cardId 卡牌ID
     * @return 是否处理成功
     */
    bool handlePlayfieldCardClick(int cardId);

    bool handlePlayfieldCardClick(CardModel* playfieldCard);
    /**
     * @brief 处理底牌点击（从备用牌堆抽牌）
     * @param clickedCard 被点击的底牌
     * @return 是否处理成功
     */
    bool handleBottomCardClick(CardModel* clickedCard);

    /**
     * @brief 替换手牌区顶部牌
     * @param newTopCard 新的顶部牌
     */
    void replaceStackTopCard(CardModel* newTopCard);

    /**
     * @brief 记录卡牌移动的回退动作
     * @param movedCard 移动的卡牌
     * @param originalPosition 原始位置
     * @param replacedCard 被替换的卡牌（可选）
     */
    void recordUndoAction(CardModel* movedCard, const Vec2& originalPosition, CardModel* replacedCard = nullptr);

    /**
     * @brief 刷新游戏视图
     */
    void refreshGameView();

    /**
     * @brief 从备用牌堆抽取新牌
     * @return 是否成功抽取
     */
    bool drawCardFromReserve();

    bool drawFromReserveToHand();

    bool handleHandCardClick(CardModel* handCard);

private:
    GameModel* _gameModel;          // 游戏数据模型
    GameView* _gameView;            // 游戏视图
    UndoManager* _undoManager;      // 回退管理器
};

#endif // GAME_CONTROLLER_H
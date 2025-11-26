// GameModel.h
#ifndef GAME_MODEL_H
#define GAME_MODEL_H

#include "cocos2d.h"
#include "CardModel.h"
#include <vector>

USING_NS_CC;

class GameModel
{
public:
    GameModel();
    ~GameModel();

    // 卡牌管理
    void addPlayfieldCard(CardModel* card);
    void addStackCard(CardModel* card);
    void addReserveCard(CardModel* card);  // 新增：添加备用牌

    bool removePlayfieldCard(int cardId);
    bool removeStackCard(int cardId);
    bool removeReserveCard(int cardId);    // 新增：移除备用牌

    // 设置卡牌集合
    void setPlayFieldCards(const std::vector<CardModel*>& cards) { _playfieldCards = cards; }
    void setStackCards(const std::vector<CardModel*>& cards) { _stackCards = cards; }
    void setReserveCards(const std::vector<CardModel*>& cards) { _reserveCards = cards; }  // 新增

    // 底牌管理
    void setBottomCard(CardModel* card) { _bottomCard = card; }
    CardModel* getBottomCard() const { return _bottomCard; }

    // 获取卡牌
    CardModel* getCardById(int cardId);
    CardModel* getTopStackCard();

    // 获取所有卡牌集合
    std::vector<CardModel*> getAllPlayfieldCards() const { return _playfieldCards; }
    std::vector<CardModel*> getAllStackCards() const { return _stackCards; }
    std::vector<CardModel*> getAllReserveCards() const { return _reserveCards; }  // 新增

    // 备用牌堆操作
    CardModel* drawFromReserve();  // 新增：从备用牌堆抽牌
    bool hasReserveCards() const { return !_reserveCards.empty(); }  // 新增

    // 撤销功能
    bool canUndo() const { return !_undoStack.empty(); }
    void clearUndoStack() { _undoStack.clear(); }

private:
    bool removeCardFromVector(std::vector<CardModel*>& cards, int cardId);

private:
    std::vector<CardModel*> _playfieldCards;   // 主牌区
    std::vector<CardModel*> _stackCards;       // 手牌区（当前显示的牌）
    std::vector<CardModel*> _reserveCards;     // 新增：备用牌堆
    CardModel* _bottomCard;                    // 底牌
    std::vector<std::string> _undoStack;
};

#endif // GAME_MODEL_H
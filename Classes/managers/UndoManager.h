#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include "cocos2d.h"
#include "../models/CardModel.h"
#include <vector>

USING_NS_CC;

// 回退动作数据结构，用于存储单次操作的回退信息
struct UndoAction {
    enum Type {
        CARD_MATCH,  // 卡牌匹配操作
        DRAW_CARD    // 抽牌操作
    };

    Type type;
    int movedCardId;
    Vec2 originalPosition;
    bool movedCardWasInPlayfield;
    bool movedCardWasTopCard;

    int replacedCardId;
    bool replacedCardWasInPlayfield;
    bool replacedCardWasTopCard;
    Vec2 replacedCardOriginalPosition;
    int replacedCardFace;    
    int replacedCardSuit;    
};


// 回退管理器类，负责管理游戏操作的撤销功能
class UndoManager
{
public:
    
    UndoManager();
    ~UndoManager() = default;

    /**
     * @brief 初始化回退管理器
     * @param maxSteps 最大回退步数，默认50步
     */
    void init(int maxSteps = 50);

    // 压入新的回退动作到栈中
    void pushAction(const UndoAction& action);

    // 执行回退操作，撤销上一步动作
    UndoAction undoLastAction();

    // 检查是否可以执行回退操作
    bool canUndo() const { return !_undoStack.empty(); }

    // 清空回退栈
    void clear();

    //获取当前回退栈的大小
    size_t getStackSize() const { return _undoStack.size(); }

private:
    std::vector<UndoAction> _undoStack;  // 回退动作存储栈，使用vector模拟栈行为
    int _maxUndoSteps;                   // 最大回退步数限制，防止内存无限增长
};

#endif // UNDO_MANAGER_H
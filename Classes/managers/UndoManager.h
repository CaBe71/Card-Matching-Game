#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include "cocos2d.h"
#include "../models/CardModel.h"
#include <vector>

USING_NS_CC;

/**
 * @struct UndoAction
 * @brief 回退动作数据
 */
struct UndoAction
{
    enum ActionType {
        CARD_CLICK,     // 卡牌点击
        CARD_MATCH      // 卡牌匹配
    };

    ActionType type;                 // 动作类型
    int movedCardId;                 // 移动的卡牌ID
    Vec2 originalPosition;           // 原始位置
    int replacedCardId;              // 被替换的卡牌ID（如果有）
    bool wasTopCard;                 // 原来是否是顶部牌
};

/**
 * @class UndoManager
 * @brief 回退管理器，负责管理回退操作栈
 */
class UndoManager
{
public:
    UndoManager();
    ~UndoManager() = default;

    /**
     * @brief 初始化回退管理器
     * @param maxSteps 最大回退步数
     */
    void init(int maxSteps = 50);

    /**
     * @brief 压入回退动作
     * @param action 回退动作
     */
    void pushAction(const UndoAction& action);

    /**
     * @brief 回退上一步操作
     * @return 回退动作，如果没有可回退的动作返回空
     */
    UndoAction undoLastAction();

    /**
     * @brief 是否可以回退
     * @return 是否可以回退
     */
    bool canUndo() const { return !_undoStack.empty(); }

    /**
     * @brief 清空回退栈
     */
    void clear();

    /**
     * @brief 获取回退栈大小
     * @return 回退栈大小
     */
    size_t getStackSize() const { return _undoStack.size(); }

private:
    std::vector<UndoAction> _undoStack;  // 回退栈
    int _maxUndoSteps;                   // 最大回退步数
};

#endif // UNDO_MANAGER_H
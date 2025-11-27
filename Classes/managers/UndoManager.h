#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include "cocos2d.h"
#include "../models/CardModel.h"
#include <vector>

USING_NS_CC;

struct UndoAction
{
    enum ActionType {
        CARD_CLICK,
        CARD_MATCH
    };

    ActionType type;
    int movedCardId;
    Vec2 originalPosition;
    int replacedCardId;
    bool wasTopCard;
};

class UndoManager
{
public:
    UndoManager();
    ~UndoManager() = default;

    void init(int maxSteps = 50);
    void pushAction(const UndoAction& action);
    UndoAction undoLastAction();
    bool canUndo() const { return !_undoStack.empty(); }
    void clear();
    size_t getStackSize() const { return _undoStack.size(); }

private:
    std::vector<UndoAction> _undoStack;
    int _maxUndoSteps;
};

#endif
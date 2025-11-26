#include "UndoManager.h"

UndoManager::UndoManager()
    : _maxUndoSteps(50)
{
}

void UndoManager::init(int maxSteps)
{
    _maxUndoSteps = maxSteps;
    _undoStack.clear();
}

void UndoManager::pushAction(const UndoAction& action)
{
    // 如果栈已满，移除最老的动作
    if (_undoStack.size() >= _maxUndoSteps) {
        _undoStack.erase(_undoStack.begin());
    }

    _undoStack.push_back(action);
}

UndoAction UndoManager::undoLastAction()
{
    if (_undoStack.empty()) {
        return UndoAction(); // 返回空动作
    }

    UndoAction lastAction = _undoStack.back();
    _undoStack.pop_back();

    return lastAction;
}

void UndoManager::clear()
{
    _undoStack.clear();
}
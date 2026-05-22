#include "UndoManager.h"

UndoManager::UndoManager()
{
}

UndoManager::~UndoManager()
{
    clear();
}

void UndoManager::init()
{
    clear();
}

void UndoManager::pushUndoRecord(UndoModel* undoModel)
{
    if (undoModel) {
        _undoStack.push_back(undoModel);
        CCLOG("UndoManager: Pushed undo record, stack size: %d", _undoStack.size());
    }
}

UndoModel* UndoManager::popUndoRecord()
{
    if (_undoStack.empty()) {
        CCLOG("UndoManager: Stack is empty");
        return nullptr;
    }

    UndoModel* undoModel = _undoStack.back();
    _undoStack.pop_back();
    CCLOG("UndoManager: Popped undo record, stack size: %d", _undoStack.size());
    return undoModel;
}

bool UndoManager::canUndo() const
{
    bool canUndo = !_undoStack.empty();
    CCLOG("UndoManager: canUndo=%d, stack size=%d", canUndo, _undoStack.size());
    return canUndo;
}

void UndoManager::clear()
{
    for (std::vector<UndoModel*>::iterator it = _undoStack.begin(); it != _undoStack.end(); ++it) {
        delete* it;
    }
    _undoStack.clear();
    CCLOG("UndoManager: Cleared all undo records");
}
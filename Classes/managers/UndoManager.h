#pragma once
#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include "../models/UndoModel.h"
#include <vector>

/**
 * 撤销管理器
 * 处理撤销功能，管理操作历史记录
 */
class UndoManager
{
public:
    UndoManager();
    ~UndoManager();

    /**
     * 初始化管理器
     */
    void init();

    /**
     * 添加撤销记录
     * @param undoModel 撤销数据模型
     */
    void pushUndoRecord(UndoModel* undoModel);

    /**
     * 弹出最近的撤销记录
     * @return 撤销数据模型，如果没有记录返回nullptr
     */
    UndoModel* popUndoRecord();

    /**
     * 检查是否有可撤销的操作
     * @return 是否有可撤销的操作
     */
    bool canUndo() const;

    /**
     * 清空所有撤销记录
     */
    void clear();

private:
    std::vector<UndoModel*> _undoStack; // 撤销操作栈
};

#endif // UNDO_MANAGER_H
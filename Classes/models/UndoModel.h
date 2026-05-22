#pragma once
#ifndef UNDO_MODEL_H
#define UNDO_MODEL_H

#include "cocos2d.h"
#include "CardDefines.h"

/**
 * 撤销操作数据模型
 * 记录一次操作，用于回退功能
 */
class UndoModel
{
    int _specialCardId;
    cocos2d::Vec2 _specialPosition;
public:
    UndoModel();
    ~UndoModel() = default;

    // 获取和设置操作类型
    OperationType getOperationType() const { return _operationType; }
    void setOperationType(OperationType type) { _operationType = type; }

    // 对于卡牌匹配操作，记录被匹配的桌面牌和之前的底牌
    int getMatchedCardId() const { return _matchedCardId; }
    void setMatchedCardId(int cardId) { _matchedCardId = cardId; }

    int getPreviousBottomCardId() const { return _previousBottomCardId; }
    void setPreviousBottomCardId(int cardId) { _previousBottomCardId = cardId; }

    // 对于抽牌操作，记录之前的底牌
    int getNewBottomCardId() const { return _newBottomCardId; }
    void setNewBottomCardId(int cardId) { _newBottomCardId = cardId; }

    // 获取和设置操作前的底牌位置
    const cocos2d::Vec2& getPreviousBottomPosition() const { return _previousBottomPosition; }
    void setPreviousBottomPosition(const cocos2d::Vec2& position) { _previousBottomPosition = position; }

    // 获取和设置操作前的匹配牌位置
    const cocos2d::Vec2& getMatchedCardPosition() const { return _matchedCardPosition; }
    void setMatchedCardPosition(const cocos2d::Vec2& position) { _matchedCardPosition = position; }

    int getSpecialCardId() const { return _specialCardId; }
    void setSpecialCardId(int cardId) { _specialCardId = cardId; }

    const cocos2d::Vec2& getSpecialPosition() const { return _specialPosition; }
    void setSpecialPosition(const cocos2d::Vec2& position) { _specialPosition = position; }

private:
    OperationType _operationType;           // 操作类型
    int _matchedCardId;                     // 匹配的桌面牌ID
    int _previousBottomCardId;              // 之前的底牌ID
    int _newBottomCardId;                   // 新的底牌ID（针对抽牌操作）
    cocos2d::Vec2 _previousBottomPosition;  // 操作前底牌位置
    cocos2d::Vec2 _matchedCardPosition;     // 操作前匹配牌位置
};

#endif // UNDO_MODEL_H
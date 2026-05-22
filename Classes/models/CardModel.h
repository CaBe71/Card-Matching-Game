#pragma once
#ifndef CARD_MODEL_H
#define CARD_MODEL_H

#include "cocos2d.h"
#include "CardDefines.h"

/**
 * 卡牌数据模型
 * 负责存储单张卡牌的数据和状态
 */
class CardModel
{
public:
    CardModel();
    ~CardModel() = default;

    // 获取和设置卡牌花色
    CardSuitType getSuit() const { return _suit; }
    void setSuit(CardSuitType suit) { _suit = suit; }

    // 获取和设置卡牌面值
    CardFaceType getFace() const { return _face; }
    void setFace(CardFaceType face) { _face = face; }

    // 获取和设置卡牌位置
    cocos2d::Vec2 getPosition() const { return _position; }
    void setPosition(const cocos2d::Vec2& position) { _position = position; }

    // 获取和设置卡牌翻转状态
    bool isFlipped() const { return _flipped; }
    void setFlipped(bool flipped) { _flipped = flipped; }

    // 获取和设置卡牌唯一ID
    int getCardId() const { return _cardId; }
    void setCardId(int cardId) { _cardId = cardId; }

    // 获取卡牌数值（A=1, J=11, Q=12, K=13）
    int getFaceValue() const;

    // 更新卡牌位置到目标区域
    void updatePositionToArea(const cocos2d::Vec2& areaPosition);

private:
    CardSuitType _suit;        // 花色
    CardFaceType _face;        // 面值
    cocos2d::Vec2 _position;   // 位置
    bool _flipped;             // 是否翻转
    int _cardId;               // 卡牌唯一ID
};

#endif // CARD_MODEL_H
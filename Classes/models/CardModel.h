#ifndef CARD_MODEL_H
#define CARD_MODEL_H

#include "cocos2d.h"
#include "CardTypes.h"

USING_NS_CC;

/**
 * @class CardModel
 * @brief 卡牌数据模型，存储卡牌的基本信息和状态
 */
class CardModel
{
public:
    CardModel();
    ~CardModel() = default;

    // Getters
    int getCardId() const { return _cardId; }
    CardFaceType getFace() const { return _face; }
    CardSuitType getSuit() const { return _suit; }
    Vec2 getPosition() const { return _position; }
    bool isTopCard() const { return _isTopCard; }
    bool isInPlayfield() const { return _isInPlayfield; }

    // Setters
    void setCardId(int id) { _cardId = id; }
    void setFace(CardFaceType face) { _face = face; }
    void setSuit(CardSuitType suit) { _suit = suit; }
    void setPosition(const Vec2& position) { _position = position; }
    void setTopCard(bool isTop) { _isTopCard = isTop; }
    void setInPlayfield(bool inPlayfield) { _isInPlayfield = inPlayfield; }

private:
    int _cardId;                    // 卡牌唯一ID
    CardFaceType _face;             // 牌面值
    CardSuitType _suit;             // 花色
    Vec2 _position;                 // 位置信息
    bool _isTopCard;                // 是否为顶部牌
    bool _isInPlayfield;            // 是否在桌面区域
};

#endif 
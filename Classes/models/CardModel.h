#ifndef CARD_MODEL_H
#define CARD_MODEL_H

#include "cocos2d.h"
#include "CardTypes.h"

USING_NS_CC;

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
    int _cardId;
    CardFaceType _face;
    CardSuitType _suit;
    Vec2 _position;
    bool _isTopCard;
    bool _isInPlayfield;
};

#endif
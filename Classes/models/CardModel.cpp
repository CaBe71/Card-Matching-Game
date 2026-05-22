#include "CardModel.h"

CardModel::CardModel()
    : _suit(CardSuitType::NONE)
    , _face(CardFaceType::NONE)
    , _position(cocos2d::Vec2::ZERO)
    , _flipped(false)
    , _cardId(0)
{
}

int CardModel::getFaceValue() const
{
    return static_cast<int>(_face) + 1; // ACE=0+1=1, TWO=1+1=2, ..., KING=12+1=13
}

void CardModel::updatePositionToArea(const cocos2d::Vec2& areaPosition)
{
    _position = areaPosition;
}
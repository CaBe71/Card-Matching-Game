#include "CardModel.h"

CardModel::CardModel()
    : _cardId(-1)
    , _face(CFT_NONE)
    , _suit(CST_NONE)
    , _position(Vec2::ZERO)
    , _isTopCard(false)
    , _isInPlayfield(false)
{
}
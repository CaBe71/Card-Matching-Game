#include "UndoModel.h"

UndoModel::UndoModel()
    : _operationType(OperationType::CARD_MATCH)
    , _matchedCardId(-1)
    , _previousBottomCardId(-1)
    , _newBottomCardId(-1)
    , _previousBottomPosition(cocos2d::Vec2::ZERO)
    , _matchedCardPosition(cocos2d::Vec2::ZERO)
{
}
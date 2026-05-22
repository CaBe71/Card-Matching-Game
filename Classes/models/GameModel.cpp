#include "GameModel.h"

GameModel::GameModel()
    : _bottomCard(nullptr)
{
}

GameModel::~GameModel()
{
    // 清理所有卡牌对象
    for (auto card : _playFieldCards) {
        delete card;
    }
    for (auto card : _stackCards) {
        delete card;
    }
    if (_bottomCard) {
        delete _bottomCard;
    }
}

void GameModel::setPlayFieldCards(const std::vector<CardModel*>& cards)
{
    _playFieldCards = cards;
    updateCardMap();
}

void GameModel::setBottomCard(CardModel* card)
{
    _bottomCard = card;
    if (card) {
        _cardMap[card->getCardId()] = card;
    }
}

void GameModel::setStackCards(const std::vector<CardModel*>& cards)
{
    _stackCards = cards;
    updateCardMap();
}

CardModel* GameModel::getCardById(int cardId) const
{
    auto it = _cardMap.find(cardId);
    if (it != _cardMap.end()) {
        return it->second;
    }
    return nullptr;
}

bool GameModel::removeCardFromPlayField(int cardId)
{
    for (auto it = _playFieldCards.begin(); it != _playFieldCards.end(); ++it) {
        if ((*it)->getCardId() == cardId) {
            _playFieldCards.erase(it);
            _cardMap.erase(cardId);
            return true;
        }
    }
    return false;
}

CardModel* GameModel::drawCardFromStack()
{
    if (_stackCards.empty()) {
        CCLOG("GameModel: Stack is empty, cannot draw");
        return nullptr;
    }

    CardModel* topCard = _stackCards.back();
    _stackCards.pop_back();

    // 更新卡牌映射
    _cardMap.erase(topCard->getCardId());

    CCLOG("GameModel: Drew card %d from stack, remaining stack size: %d",
        topCard->getCardId(), _stackCards.size());

    return topCard;
}

void GameModel::addCardToBottom(CardModel* card)
{
    CCLOG("GameModel: Adding card %d to bottom, current bottom card: %p",
        card ? card->getCardId() : -1, _bottomCard);

    if (_bottomCard) {
        // 将当前底部卡牌移回堆栈（如果需要）
        CCLOG("GameModel: Moving current bottom card %d back to stack", _bottomCard->getCardId());
        _stackCards.insert(_stackCards.begin(), _bottomCard);
    }

    _bottomCard = card;
    if (card) {
        _cardMap[card->getCardId()] = card;
        CCLOG("GameModel: New bottom card set to %d", card->getCardId());
    }

    CCLOG("GameModel: Stack size after addToBottom: %d", _stackCards.size());
}

void GameModel::updateCardMap()
{
    _cardMap.clear();
    for (auto card : _playFieldCards) {
        _cardMap[card->getCardId()] = card;
    }
    for (auto card : _stackCards) {
        _cardMap[card->getCardId()] = card;
    }
    if (_bottomCard) {
        _cardMap[_bottomCard->getCardId()] = _bottomCard;
    }
}

CardModel* GameModel::drawCardFromStackToPlayField(const cocos2d::Vec2& position)
{
    if (_stackCards.empty()) {
        CCLOG("GameModel: Stack is empty, cannot draw to play field");
        return nullptr;
    }

    CardModel* topCard = _stackCards.back();
    _stackCards.pop_back();

    // 设置卡牌位置和状态
    topCard->setPosition(position);
    topCard->setFlipped(true); // 桌面卡牌正面朝上
    _playFieldCards.push_back(topCard);

    // 更新映射表
    _cardMap[topCard->getCardId()] = topCard;

    CCLOG("GameModel: Drew card %d from stack to play field at (%.1f, %.1f), stack remaining: %d",
        topCard->getCardId(), position.x, position.y, _stackCards.size());

    return topCard;
}

void GameModel::addCardToPlayField(CardModel* card)
{
    if (card) {
        _playFieldCards.push_back(card);
        _cardMap[card->getCardId()] = card;
        CCLOG("GameModel: Added card %d to play field", card->getCardId());
    }
}
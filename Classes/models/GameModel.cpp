#include "GameModel.h"

GameModel::GameModel()
{
}

GameModel::~GameModel()
{
    // 清理内存
    for (auto card : _playfieldCards) {
        delete card;
    }
    for (auto card : _stackCards) {
        delete card;
    }
}

void GameModel::addPlayfieldCard(CardModel* card)
{
    card->setInPlayfield(true);
    _playfieldCards.push_back(card);
}

void GameModel::addStackCard(CardModel* card)
{
    card->setInPlayfield(false);
    _stackCards.push_back(card);
}

CardModel* GameModel::getCardById(int cardId)
{
    // 在桌面牌中查找
    for (auto card : _playfieldCards) {
        if (card->getCardId() == cardId) {
            return card;
        }
    }

    // 在手牌区中查找
    for (auto card : _stackCards) {
        if (card->getCardId() == cardId) {
            return card;
        }
    }

    return nullptr;
}

CardModel* GameModel::getTopStackCard()
{
    // 现在顶部牌就是底牌
    return _bottomCard;
}
bool GameModel::removePlayfieldCard(int cardId)
{
    return removeCardFromVector(_playfieldCards, cardId);
}

bool GameModel::removeStackCard(int cardId)
{
    return removeCardFromVector(_stackCards, cardId);
}

bool GameModel::removeCardFromVector(std::vector<CardModel*>& cards, int cardId)
{
    for (auto it = cards.begin(); it != cards.end(); ++it) {
        if ((*it)->getCardId() == cardId) {
            // 注意：这里不删除CardModel对象，因为可能在其他地方还在使用
            cards.erase(it);
            CCLOG("Removed card %d from vector", cardId);
            return true;
        }
    }
    CCLOG("Card %d not found in vector", cardId);
    return false;
}



bool GameModel::removeReserveCard(int cardId)
{
    return removeCardFromVector(_reserveCards, cardId);
}

// GameModel.cpp - 确保 drawFromReserve 方法存在
CardModel* GameModel::drawFromReserve()
{
    if (_reserveCards.empty()) {
        CCLOG("No reserve cards to draw");
        return nullptr;
    }

    // 从备用牌堆抽取最后一张牌（栈顶）
    CardModel* card = _reserveCards.back();
    _reserveCards.pop_back();

    CCLOG("Drew card from reserve: ID=%d, Remaining: %zu",
        card->getCardId(), _reserveCards.size());

    return card;
}


// 确保 addReserveCard 方法存在
void GameModel::addReserveCard(CardModel* card)
{
    card->setInPlayfield(false);
    _reserveCards.push_back(card);
    CCLOG("Added card to reserve: ID=%d, Total reserve: %zu",
        card->getCardId(), _reserveCards.size());
}

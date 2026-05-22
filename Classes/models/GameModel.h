#pragma once
#ifndef GAME_MODEL_H
#define GAME_MODEL_H

#include "cocos2d.h"
#include "CardModel.h"
#include <vector>
#include <unordered_map>

class CardModel;

class GameModel
{
public:
    GameModel();
    ~GameModel();

    const std::vector<CardModel*>& getPlayFieldCards() const { return _playFieldCards; }
    void setPlayFieldCards(const std::vector<CardModel*>& cards);

    CardModel* getBottomCard() const { return _bottomCard; }
    void setBottomCard(CardModel* card);

    const std::vector<CardModel*>& getStackCards() const { return _stackCards; }
    void setStackCards(const std::vector<CardModel*>& cards);

    CardModel* getCardById(int cardId) const;
    bool removeCardFromPlayField(int cardId);
    CardModel* drawCardFromStack();
    void addCardToBottom(CardModel* card);
    CardModel* drawCardFromStackToPlayField(const cocos2d::Vec2& position);
    void addCardToPlayField(CardModel* card);

    int getScore() const { return _score; }
    void addScore(int points) { _score += points; }
    void setScore(int score) { _score = score; }

    // 连击系统
    int getCombo() const { return _combo; }
    void addCombo() { _combo++; }
    void resetCombo() { _combo = 0; }
    int getStackRemaining() const { return static_cast<int>(_stackCards.size()); }

private:
    std::vector<CardModel*> _playFieldCards;
    CardModel* _bottomCard = nullptr;
    std::vector<CardModel*> _stackCards;
    int _score = 0;
    int _combo = 0;

    std::unordered_map<int, CardModel*> _cardMap;
    void updateCardMap();
};

#endif

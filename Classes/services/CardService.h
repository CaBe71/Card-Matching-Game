#ifndef CARD_SERVICE_H
#define CARD_SERVICE_H

#include "../models/CardModel.h"

class CardService
{
public:
    static bool canMatch(const CardModel* card1, const CardModel* card2);
    static CardModel* findTopCard(const std::vector<CardModel*>& cards);
    static int generateCardId();

    // 随机卡牌生成方法
    static CardModel* generateRandomCard();
    static CardModel* generateRandomCardAtPosition(const cocos2d::Vec2& position);

private:
    static int _nextCardId;
};

#endif // CARD_SERVICE_H
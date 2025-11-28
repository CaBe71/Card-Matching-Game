#ifndef CARD_GENERATOR_SERVICE_H
#define CARD_GENERATOR_SERVICE_H

#include "../models/CardModel.h"
#include "../models/CardTypes.h"

class CardGeneratorService
{
public:
    // 生成随机卡牌
    static std::vector<CardModel*> generateRandomCards(int count);

    // 生成特定位置的随机卡牌
    static std::vector<CardModel*> generateRandomCardsAtPosition(int count, const cocos2d::Vec2& position);

private:
    static CardFaceType getRandomFace();
    static CardSuitType getRandomSuit();
};

#endif // CARD_GENERATOR_SERVICE_H
#ifndef CARD_GENERATOR_SERVICE_H
#define CARD_GENERATOR_SERVICE_H

#include "../models/CardModel.h"
#include "../models/CardTypes.h"

class CardGeneratorService
{
public:
    /**
     * @brief 生成随机卡牌
     * @param count 生成数量
     * @return 卡牌列表
     */
    static std::vector<CardModel*> generateRandomCards(int count);

    /**
     * @brief 生成特定位置的随机卡牌
     * @param count 生成数量
     * @param position 位置
     * @return 卡牌列表
     */
    static std::vector<CardModel*> generateRandomCardsAtPosition(int count, const cocos2d::Vec2& position);

private:
    static CardFaceType getRandomFace();
    static CardSuitType getRandomSuit();
};

#endif // CARD_GENERATOR_SERVICE_H
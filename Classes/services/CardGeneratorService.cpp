#include "CardGeneratorService.h"
#include "CardService.h"
#include "cocos2d.h"
#include <cstdlib>
#include <ctime>

USING_NS_CC;

std::vector<CardModel*> CardGeneratorService::generateRandomCards(int count)
{
    std::vector<CardModel*> cards;

    // 初始化随机种子
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned int>(time(nullptr)));
        seeded = true;
    }

    for (int i = 0; i < count; i++) {
        CardModel* card = new CardModel();
        card->setCardId(CardService::generateCardId());
        card->setFace(getRandomFace());
        card->setSuit(getRandomSuit());
        card->setPosition(Vec2::ZERO);
        card->setInPlayfield(true);

        cards.push_back(card);
        CCLOG("Generated random card: ID=%d, Face=%d, Suit=%d",
            card->getCardId(), card->getFace(), card->getSuit());
    }

    return cards;
}

std::vector<CardModel*> CardGeneratorService::generateRandomCardsAtPosition(int count, const Vec2& position)
{
    auto cards = generateRandomCards(count);
    for (auto card : cards) {
        card->setPosition(position);
    }
    return cards;
}

CardFaceType CardGeneratorService::getRandomFace()
{
    // 生成 0-12 的随机数，对应 CFT_ACE 到 CFT_KING
    int randomFace = rand() % 13;
    return static_cast<CardFaceType>(randomFace);
}

CardSuitType CardGeneratorService::getRandomSuit()
{
    // 生成 0-3 的随机数，对应 CST_CLUBS 到 CST_SPADES
    int randomSuit = rand() % 4;
    return static_cast<CardSuitType>(randomSuit);
}
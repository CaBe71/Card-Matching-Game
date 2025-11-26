#include "CardService.h"
#include "cocos2d.h"
#include <cstdlib>
#include <ctime>
#include <random>

USING_NS_CC;

// 静态成员变量定义
int CardService::_nextCardId = 1;

bool CardService::canMatch(const CardModel* card1, const CardModel* card2)
{
    if (!card1 || !card2) {
        CCLOG("ERROR: One or both cards are null");
        return false;
    }

    int face1 = card1->getFace();
    int face2 = card2->getFace();

    // 点数差1即可匹配（A=0, K=12）
    int diff = abs(face1 - face2);
    bool canMatch = (diff == 1);

    CCLOG("Match check: %d(%d) vs %d(%d) -> diff=%d -> %s",
        card1->getCardId(), face1,
        card2->getCardId(), face2,
        diff, canMatch ? "MATCH" : "NO MATCH");

    return canMatch;
}

CardModel* CardService::findTopCard(const std::vector<CardModel*>& cards)
{
    if (cards.empty()) {
        return nullptr;
    }

    return cards.back();
}

int CardService::generateCardId()
{
    return _nextCardId++;
}

CardModel* CardService::generateRandomCard()
{
    // 使用更好的随机数生成方式
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> faceDist(0, 12); // 0-12 对应 A-K
    static std::uniform_int_distribution<> suitDist(0, 3);  // 0-3 对应 4种花色

    CardModel* card = new CardModel();
    card->setCardId(generateCardId());

    // 生成随机面值
    int randomFace = faceDist(gen);
    card->setFace(static_cast<CardFaceType>(randomFace));

    // 生成随机花色
    int randomSuit = suitDist(gen);
    card->setSuit(static_cast<CardSuitType>(randomSuit));

    card->setInPlayfield(true);

    CCLOG("Generated RANDOM card: ID=%d, Face=%d, Suit=%d",
        card->getCardId(), card->getFace(), card->getSuit());

    return card;
}

CardModel* CardService::generateRandomCardAtPosition(const Vec2& position)
{
    CardModel* card = generateRandomCard();
    card->setPosition(position);
    return card;
}
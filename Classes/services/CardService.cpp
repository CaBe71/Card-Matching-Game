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

    // ?? 修复：正确的面值到字符串映射
    const char* faceNames[] = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };

    int diff = abs(face1 - face2);
    bool canMatch = (diff == 1);

    // ?? 修复：添加A和K的特殊匹配
    if (!canMatch) {
        // 检查是否是 A 和 K 的情况
        if ((face1 == 0 && face2 == 12) || (face1 == 12 && face2 == 0)) {
            canMatch = true;
            CCLOG("Special match detected: A and K can match");
        }
    }

    CCLOG("Match check: %s(%d) vs %s(%d) -> diff=%d -> %s",
        (face1 >= 0 && face1 <= 12) ? faceNames[face1] : "?", face1,
        (face2 >= 0 && face2 <= 12) ? faceNames[face2] : "?", face2,
        diff, canMatch ? "MATCH" : "NO MATCH");

    return canMatch;
}

// ?? 新增：面值转字符串的辅助方法
std::string CardService::getFaceString(CardFaceType face)
{
    switch (face) {
    case CFT_ACE:   return "A";
    case CFT_TWO:   return "2";
    case CFT_THREE: return "3";
    case CFT_FOUR:  return "4";
    case CFT_FIVE:  return "5";
    case CFT_SIX:   return "6";
    case CFT_SEVEN: return "7";
    case CFT_EIGHT: return "8";
    case CFT_NINE:  return "9";
    case CFT_TEN:   return "10";
    case CFT_JACK:  return "J";
    case CFT_QUEEN: return "Q";
    case CFT_KING:  return "K";
    default:        return "?";
    }
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
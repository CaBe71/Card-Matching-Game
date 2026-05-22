#include "GameUtils.h"

USING_NS_CC;

static int s_cardIdCounter = 1;

int GameUtils::generateCardId()
{
    return s_cardIdCounter++;
}

bool GameUtils::isCardsMatch(int cardValue1, int cardValue2)
{
    return abs(cardValue1 - cardValue2) == 1;
}

int GameUtils::getRandomInt(int min, int max)
{
    return random(min, max);
}
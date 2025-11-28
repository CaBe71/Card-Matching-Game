#include "LevelConfigLoader.h"
#include "cocos2d.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include <random>
#include <algorithm>

USING_NS_CC;



// 根据难度级别生成不同配置
LevelConfig* LevelConfigLoader::loadLevelConfigWithDifficulty(int levelId, int difficulty)
{
    CCLOG("Loading level %d with difficulty %d", levelId, difficulty);

    // 根据难度调整卡牌数量
    int playfieldCount, stackCount, reserveCount;

    switch (difficulty) {
    case 1: // 简单
        playfieldCount = 4;
        stackCount = 5;
        reserveCount = 10;
        break;
    case 2: // 中等
        playfieldCount = 6;
        stackCount = 7;
        reserveCount = 15;
        break;
    case 3: // 困难
        playfieldCount = 8;
        stackCount = 10;
        reserveCount = 20;
        break;
    default:
        playfieldCount = 6;
        stackCount = 7;
        reserveCount = 15;
        break;
    }

    // 创建随机数生成器
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> faceDist(0, 12);
    static std::uniform_int_distribution<> suitDist(0, 3);
    static std::uniform_int_distribution<> posDist(200, 800);

    LevelConfig* levelConfig = new LevelConfig();

    // 生成主牌区卡牌
    for (int i = 0; i < playfieldCount; i++) {
        CardConfig cardConfig;
        cardConfig.cardFace = static_cast<CardFaceType>(faceDist(gen));
        cardConfig.cardSuit = static_cast<CardSuitType>(suitDist(gen));

        // 根据数量调整位置分布
        float xPos = 200 + (i % 3) * 300;
        float yPos = 800 + (i / 3) * 220;
        cardConfig.position = Vec2(xPos, yPos);

        levelConfig->playfieldCards.push_back(cardConfig);
    }

    // 生成手牌区卡牌
    for (int i = 0; i < stackCount; i++) {
        CardConfig cardConfig;
        cardConfig.cardFace = static_cast<CardFaceType>(faceDist(gen));
        cardConfig.cardSuit = static_cast<CardSuitType>(suitDist(gen));
        cardConfig.position = Vec2(0, 0);

        levelConfig->stackCards.push_back(cardConfig);
    }

    // 生成备用牌堆
    for (int i = 0; i < reserveCount; i++) {
        CardConfig cardConfig;
        cardConfig.cardFace = static_cast<CardFaceType>(faceDist(gen));
        cardConfig.cardSuit = static_cast<CardSuitType>(suitDist(gen));
        cardConfig.position = Vec2(0, 0);

        levelConfig->reserveCards.push_back(cardConfig);
    }

    CCLOG("Generated level %d (difficulty %d): %zu playfield, %zu stack, %zu reserve",
        levelId, difficulty, levelConfig->playfieldCards.size(),
        levelConfig->stackCards.size(), levelConfig->reserveCards.size());

    return levelConfig;
}


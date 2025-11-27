#include "LevelConfigLoader.h"
#include "cocos2d.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include <random>
#include <algorithm>

USING_NS_CC;

// 添加 LevelConfig 的 createFromJson 实现
LevelConfig* LevelConfig::createFromJson(const std::string& jsonStr)
{
    return LevelConfigLoader::loadLevelConfigFromString(jsonStr);
}

LevelConfig* LevelConfigLoader::loadLevelConfig(int levelId)
{
    CCLOG("=== Loading Level Config ===");
    CCLOG("Level ID: %d", levelId);

    // ?? 创建随机数生成器
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> faceDist(0, 12); // A-K
    static std::uniform_int_distribution<> suitDist(0, 3);  // 4种花色
    static std::uniform_int_distribution<> posDist(200, 800); // 位置范围

    LevelConfig* levelConfig = new LevelConfig();

    // ?? 生成主牌区卡牌 (6张随机卡牌)
    int playfieldCardCount = 6;
    for (int i = 0; i < playfieldCardCount; i++) {
        CardConfig cardConfig;
        cardConfig.cardFace = static_cast<CardFaceType>(faceDist(gen));
        cardConfig.cardSuit = static_cast<CardSuitType>(suitDist(gen));
        cardConfig.position = Vec2(posDist(gen), 800 + i * 80); // 垂直排列

        levelConfig->playfieldCards.push_back(cardConfig);

        CCLOG("Generated playfield card %d: Face=%d, Suit=%d, Position(%.1f, %.1f)",
            i, cardConfig.cardFace, cardConfig.cardSuit,
            cardConfig.position.x, cardConfig.position.y);
    }

    // ?? 生成手牌区卡牌 (7张随机卡牌)
    int stackCardCount = 7;
    for (int i = 0; i < stackCardCount; i++) {
        CardConfig cardConfig;
        cardConfig.cardFace = static_cast<CardFaceType>(faceDist(gen));
        cardConfig.cardSuit = static_cast<CardSuitType>(suitDist(gen));
        cardConfig.position = Vec2(0, 0); // 手牌区位置会在游戏中设置

        levelConfig->stackCards.push_back(cardConfig);

        CCLOG("Generated stack card %d: Face=%d, Suit=%d",
            i, cardConfig.cardFace, cardConfig.cardSuit);
    }

    // ?? 生成备用牌堆 (15张随机卡牌)
    int reserveCardCount = 15;
    for (int i = 0; i < reserveCardCount; i++) {
        CardConfig cardConfig;
        cardConfig.cardFace = static_cast<CardFaceType>(faceDist(gen));
        cardConfig.cardSuit = static_cast<CardSuitType>(suitDist(gen));
        cardConfig.position = Vec2(0, 0); // 备用牌堆位置

        levelConfig->reserveCards.push_back(cardConfig);

        CCLOG("Generated reserve card %d: Face=%d, Suit=%d",
            i, cardConfig.cardFace, cardConfig.cardSuit);
    }

    CCLOG("Level config generated: %zu playfield, %zu stack, %zu reserve cards",
        levelConfig->playfieldCards.size(),
        levelConfig->stackCards.size(),
        levelConfig->reserveCards.size());

    return levelConfig;
}

// ?? 新增：根据难度级别生成不同配置
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

    // ?? 创建随机数生成器
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

        // ?? 根据数量调整位置分布
        float xPos = 200 + (i % 3) * 300;
        float yPos = 800 + (i / 3) * 120;
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

LevelConfig* LevelConfigLoader::loadLevelConfigFromFile(const std::string& filename)
{
    // 从文件加载JSON配置的实现
    auto fileUtils = FileUtils::getInstance();
    std::string fullPath = fileUtils->fullPathForFilename(filename);
    std::string jsonStr = fileUtils->getStringFromFile(fullPath);

    if (jsonStr.empty()) {
        CCLOG("Failed to load level config file: %s", filename.c_str());
        return nullptr;
    }

    return loadLevelConfigFromString(jsonStr);
}

LevelConfig* LevelConfigLoader::loadLevelConfigFromString(const std::string& jsonStr)
{
    rapidjson::Document document;
    document.Parse(jsonStr.c_str());

    if (document.HasParseError()) {
        CCLOG("JSON parse error");
        return nullptr;
    }

    LevelConfig* levelConfig = new LevelConfig();

    // 解析桌面牌
    if (document.HasMember("Playfield") && document["Playfield"].IsArray()) {
        const rapidjson::Value& playfieldArray = document["Playfield"];
        for (rapidjson::SizeType i = 0; i < playfieldArray.Size(); i++) {
            const rapidjson::Value& cardObj = playfieldArray[i];
            CardConfig cardConfig;

            cardConfig.cardFace = static_cast<CardFaceType>(cardObj["CardFace"].GetInt());
            cardConfig.cardSuit = static_cast<CardSuitType>(cardObj["CardSuit"].GetInt());

            const rapidjson::Value& posObj = cardObj["Position"];
            cardConfig.position.x = posObj["x"].GetFloat();
            cardConfig.position.y = posObj["y"].GetFloat();

            levelConfig->playfieldCards.push_back(cardConfig);
        }
    }

    // 解析手牌区
    if (document.HasMember("Stack") && document["Stack"].IsArray()) {
        const rapidjson::Value& stackArray = document["Stack"];
        for (rapidjson::SizeType i = 0; i < stackArray.Size(); i++) {
            const rapidjson::Value& cardObj = stackArray[i];
            CardConfig cardConfig;

            cardConfig.cardFace = static_cast<CardFaceType>(cardObj["CardFace"].GetInt());
            cardConfig.cardSuit = static_cast<CardSuitType>(cardObj["CardSuit"].GetInt());

            const rapidjson::Value& posObj = cardObj["Position"];
            cardConfig.position.x = posObj["x"].GetFloat();
            cardConfig.position.y = posObj["y"].GetFloat();

            levelConfig->stackCards.push_back(cardConfig);
        }
    }

    // 解析备用牌堆
    if (document.HasMember("Reserve") && document["Reserve"].IsArray()) {
        const rapidjson::Value& reserveArray = document["Reserve"];
        for (rapidjson::SizeType i = 0; i < reserveArray.Size(); i++) {
            const rapidjson::Value& cardObj = reserveArray[i];
            CardConfig cardConfig;

            cardConfig.cardFace = static_cast<CardFaceType>(cardObj["CardFace"].GetInt());
            cardConfig.cardSuit = static_cast<CardSuitType>(cardObj["CardSuit"].GetInt());

            const rapidjson::Value& posObj = cardObj["Position"];
            cardConfig.position.x = posObj["x"].GetFloat();
            cardConfig.position.y = posObj["y"].GetFloat();

            levelConfig->reserveCards.push_back(cardConfig);
        }

        CCLOG("Loaded %zu reserve cards from JSON", levelConfig->reserveCards.size());
    }
    else {
        CCLOG("No reserve cards found in JSON config");
    }

    CCLOG("Level config loaded from JSON: %zu playfield, %zu stack, %zu reserve cards",
        levelConfig->playfieldCards.size(),
        levelConfig->stackCards.size(),
        levelConfig->reserveCards.size());

    return levelConfig;
}
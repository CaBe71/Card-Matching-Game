#include "LevelConfigLoader.h"
#include "cocos2d.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"

USING_NS_CC;

// 添加 LevelConfig 的 createFromJson 实现（如果还没实现）
LevelConfig* LevelConfig::createFromJson(const std::string& jsonStr)
{
    return LevelConfigLoader::loadLevelConfigFromString(jsonStr);
}

LevelConfig* LevelConfigLoader::loadLevelConfig(int levelId)
{
    // 这里简化处理，实际应该根据levelId加载对应的配置文件
    std::string defaultJson = R"({
        "Playfield": [
            {"CardFace": 12, "CardSuit": 0, "Position": {"x": 250, "y": 1000}},
            {"CardFace": 2, "CardSuit": 0, "Position": {"x": 300, "y": 800}},
            {"CardFace": 2, "CardSuit": 1, "Position": {"x": 350, "y": 600}},
            {"CardFace": 2, "CardSuit": 0, "Position": {"x": 850, "y": 1000}},
            {"CardFace": 2, "CardSuit": 0, "Position": {"x": 800, "y": 800}},
            {"CardFace": 1, "CardSuit": 3, "Position": {"x": 750, "y": 600}}
        ],
        "Stack": [
            {"CardFace": 2, "CardSuit": 0, "Position": {"x": 0, "y": 0}}
        ],
        "Reserve": [
            {"CardFace": 5, "CardSuit": 1, "Position": {"x": 0, "y": 0}},
            {"CardFace": 8, "CardSuit": 2, "Position": {"x": 0, "y": 0}},
            {"CardFace": 3, "CardSuit": 3, "Position": {"x": 0, "y": 0}},
            {"CardFace": 10, "CardSuit": 0, "Position": {"x": 0, "y": 0}},
            {"CardFace": 7, "CardSuit": 1, "Position": {"x": 0, "y": 0}},
            {"CardFace": 4, "CardSuit": 2, "Position": {"x": 0, "y": 0}},
            {"CardFace": 9, "CardSuit": 3, "Position": {"x": 0, "y": 0}},
            {"CardFace": 6, "CardSuit": 0, "Position": {"x": 0, "y": 0}}
        ]
    })";

    return loadLevelConfigFromString(defaultJson);
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

    // ?? 解析备用牌堆
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

        CCLOG("Loaded %zu reserve cards", levelConfig->reserveCards.size());
    }
    else {
        CCLOG("No reserve cards found in level config");
    }

    CCLOG("Level config loaded successfully: %zu playfield, %zu stack, %zu reserve cards",
        levelConfig->playfieldCards.size(),
        levelConfig->stackCards.size(),
        levelConfig->reserveCards.size());

    return levelConfig;
}
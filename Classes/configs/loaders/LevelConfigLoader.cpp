#include "LevelConfigLoader.h"
#include "cocos2d.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"

USING_NS_CC;

LevelConfigLoader::LevelConfigLoader()
{
}

LevelConfig* LevelConfigLoader::loadLevelConfig(int levelId)
{
    // 构建文件名，level_1.json
    std::string filename = StringUtils::format("level_%d.json", levelId);
    return parseLevelConfig(filename);
}

LevelConfig* LevelConfigLoader::parseLevelConfig(const std::string& filename)
{
    // 获取文件完整路径
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filename);
    if (fullPath.empty()) {
        CCLOG("LevelConfigLoader: file not found: %s", filename.c_str());
        return nullptr;
    }

    // 读取文件内容
    std::string content = FileUtils::getInstance()->getStringFromFile(fullPath);
    if (content.empty()) {
        CCLOG("LevelConfigLoader: file is empty: %s", filename.c_str());
        return nullptr;
    }

    rapidjson::Document doc;
    doc.Parse(content.c_str());
    if (doc.HasParseError()) {
        CCLOG("LevelConfigLoader: JSON parse error: %s", doc.GetParseError());
        return nullptr;
    }

    LevelConfig* levelConfig = new LevelConfig();

    // 解析主牌区卡牌
    if (doc.HasMember("Playfield") && doc["Playfield"].IsArray()) {
        std::vector<LevelConfig::CardConfig> playFieldCards;
        const rapidjson::Value& playfieldArray = doc["Playfield"];
        for (rapidjson::SizeType i = 0; i < playfieldArray.Size(); i++) {
            const rapidjson::Value& cardObj = playfieldArray[i];
            LevelConfig::CardConfig cardConfig;
            if (cardObj.HasMember("CardFace") && cardObj["CardFace"].IsInt()) {
                cardConfig.face = static_cast<CardFaceType>(cardObj["CardFace"].GetInt());
            }
            if (cardObj.HasMember("CardSuit") && cardObj["CardSuit"].IsInt()) {
                cardConfig.suit = static_cast<CardSuitType>(cardObj["CardSuit"].GetInt());
            }
            if (cardObj.HasMember("Position") && cardObj["Position"].IsObject()) {
                const rapidjson::Value& posObj = cardObj["Position"];
                if (posObj.HasMember("x") && posObj["x"].IsNumber() &&
                    posObj.HasMember("y") && posObj["y"].IsNumber()) {
                    cardConfig.position.x = posObj["x"].GetFloat();
                    cardConfig.position.y = posObj["y"].GetFloat();
                }
            }
            playFieldCards.push_back(cardConfig);
        }
        levelConfig->setPlayFieldCards(playFieldCards);
    }

    // 解析备用牌区卡牌
    if (doc.HasMember("Stack") && doc["Stack"].IsArray()) {
        std::vector<LevelConfig::CardConfig> stackCards;
        const rapidjson::Value& stackArray = doc["Stack"];
        for (rapidjson::SizeType i = 0; i < stackArray.Size(); i++) {
            const rapidjson::Value& cardObj = stackArray[i];
            LevelConfig::CardConfig cardConfig;
            if (cardObj.HasMember("CardFace") && cardObj["CardFace"].IsInt()) {
                cardConfig.face = static_cast<CardFaceType>(cardObj["CardFace"].GetInt());
            }
            if (cardObj.HasMember("CardSuit") && cardObj["CardSuit"].IsInt()) {
                cardConfig.suit = static_cast<CardSuitType>(cardObj["CardSuit"].GetInt());
            }
            // 备用牌堆的位置通常由程序计算，但这里我们读取配置中的位置
            if (cardObj.HasMember("Position") && cardObj["Position"].IsObject()) {
                const rapidjson::Value& posObj = cardObj["Position"];
                if (posObj.HasMember("x") && posObj["x"].IsNumber() &&
                    posObj.HasMember("y") && posObj["y"].IsNumber()) {
                    cardConfig.position.x = posObj["x"].GetFloat();
                    cardConfig.position.y = posObj["y"].GetFloat();
                }
            }
            stackCards.push_back(cardConfig);
        }
        levelConfig->setStackCards(stackCards);
    }

    return levelConfig;
}
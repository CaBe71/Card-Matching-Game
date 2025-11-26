#ifndef LEVEL_CONFIG_H
#define LEVEL_CONFIG_H

#include "cocos2d.h"
#include "../../models/CardTypes.h"

USING_NS_CC;

/**
 * @struct CardConfig
 * @brief 单张卡牌的配置信息
 */
struct CardConfig
{
    CardFaceType cardFace;
    CardSuitType cardSuit;
    Vec2 position;
};

/**
 * @class LevelConfig
 * @brief 关卡配置数据
 */
class LevelConfig
{
public:
    LevelConfig() {
        // 构造函数实现
    }

    ~LevelConfig() = default;

    std::vector<CardConfig> playfieldCards;  // 桌面牌配置
    std::vector<CardConfig> stackCards;      // 手牌区配置
    std::vector<CardConfig> reserveCards;    // 备用牌配置

    static LevelConfig* createFromJson(const std::string& jsonStr);
};

#endif // LEVEL_CONFIG_H
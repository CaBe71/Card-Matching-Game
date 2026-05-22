#pragma once
#ifndef LEVEL_CONFIG_H
#define LEVEL_CONFIG_H

#include "cocos2d.h"
#include "../../models/CardDefines.h"
#include <vector>

/**
 * 关卡配置数据模型
 * 存储关卡的静态配置数据
 */
class LevelConfig
{
public:
    // 卡牌配置项
    struct CardConfig
    {
        CardFaceType face;      // 卡牌点数
        CardSuitType suit;      // 卡牌花色
        cocos2d::Vec2 position; // 位置
    };

    LevelConfig();
    ~LevelConfig() = default;

    // 获取和设置主牌区的卡牌配置
    const std::vector<CardConfig>& getPlayFieldCards() const { return _playFieldCards; }
    void setPlayFieldCards(const std::vector<CardConfig>& cards) { _playFieldCards = cards; }

    // 获取和设置备用牌区的卡牌配置
    const std::vector<CardConfig>& getStackCards() const { return _stackCards; }
    void setStackCards(const std::vector<CardConfig>& cards) { _stackCards = cards; }

private:
    std::vector<CardConfig> _playFieldCards; // 主牌区卡牌配置
    std::vector<CardConfig> _stackCards;     // 备用牌区卡牌配置
};

#endif // LEVEL_CONFIG_H
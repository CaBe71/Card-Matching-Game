#pragma once
#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include "cocos2d.h"

/**
 * 游戏工具类
 * 提供通用的工具函数
 */
class GameUtils
{
public:
    /**
     * 生成唯一的卡牌ID
     */
    static int generateCardId();

    /**
     * 检查两张卡牌是否匹配（数字相差1）
     */
    static bool isCardsMatch(int cardValue1, int cardValue2);

    /**
     * 获取随机数
     */
    static int getRandomInt(int min, int max);
};

#endif // GAME_UTILS_H
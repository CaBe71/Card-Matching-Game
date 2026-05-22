#pragma once
#ifndef CARD_DEFINES_H
#define CARD_DEFINES_H

#include "cocos2d.h"

/**
 * 卡牌花色类型
 */
enum class CardSuitType
{
    NONE = -1,
    CLUBS,      // 梅花
    DIAMONDS,   // 方块
    HEARTS,     // 红心
    SPADES,     // 黑桃
    NUM_CARD_SUIT_TYPES
};

/**
 * 卡牌面值类型（点数）
 */
enum class CardFaceType
{
    NONE = -1,
    ACE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    NUM_CARD_FACE_TYPES
};

/**
 * 游戏区域类型
 */
enum class GameAreaType
{
    PLAY_FIELD, // 主游戏区域
    BOTTOM,     // 底部卡牌区域
    STACK       // 堆叠区域
};

/**
 * 操作类型（用于撤销功能）
 */
enum class OperationType
{
    CARD_MATCH,     // 卡牌匹配
    DRAW_CARD,      // 抽卡
    NEW_CARD_ADDED, // 新增卡牌
    SPECIAL_MOVE    // 特殊移动
};


#endif // CARD_DEFINES_H
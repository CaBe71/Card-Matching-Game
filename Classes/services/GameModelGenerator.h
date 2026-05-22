#pragma once
#ifndef GAME_MODEL_GENERATOR_H
#define GAME_MODEL_GENERATOR_H

#include "../models/GameModel.h"
#include "../configs/models/LevelConfig.h"
#include <algorithm>
#include <cstdlib>

/**
 * 游戏模型生成服务
 */
class GameModelGenerator
{
public:
    GameModelGenerator();
    ~GameModelGenerator() = default;

    GameModel* generateGameModel(LevelConfig* levelConfig);
    GameModel* generateRandomGameModel();

private:
    CardModel* createCardModel(const LevelConfig::CardConfig& cardConfig, int cardId);
    CardModel* createRandomCardModel(int cardId, const cocos2d::Vec2& position);

    template<typename T>
    void shuffleVector(std::vector<T>& vec);
};

template<typename T>
void GameModelGenerator::shuffleVector(std::vector<T>& vec) {
    for (size_t i = vec.size() - 1; i > 0; i--) {
        size_t j = static_cast<size_t>(rand()) % (i + 1);
        std::swap(vec[i], vec[j]);
    }
}

#endif

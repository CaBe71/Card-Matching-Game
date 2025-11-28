#ifndef GAME_MODEL_GENERATOR_H
#define GAME_MODEL_GENERATOR_H

#include "../models/GameModel.h"
#include "../configs/models/LevelConfig.h"

class GameModelGenerator
{
public:
    // 从关卡配置生成游戏模型
    static GameModel* generateFromLevelConfig(const LevelConfig* levelConfig);


private:
    static CardModel* createCardModel(const CardConfig& cardConfig, int cardId);
};

#endif // GAME_MODEL_GENERATOR_H
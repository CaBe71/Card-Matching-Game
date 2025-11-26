#ifndef GAME_MODEL_GENERATOR_H
#define GAME_MODEL_GENERATOR_H

#include "../models/GameModel.h"
#include "../configs/models/LevelConfig.h"

class GameModelGenerator
{
public:
    /**
     * @brief 从关卡配置生成游戏模型
     * @param levelConfig 关卡配置
     * @return 游戏模型
     */
    static GameModel* generateFromLevelConfig(const LevelConfig* levelConfig);

    // 为了兼容性，也提供 generateGameModel 别名
    static GameModel* generateGameModel(LevelConfig* levelConfig) {
        return generateFromLevelConfig(levelConfig);
    }

private:
    static CardModel* createCardModel(const CardConfig& cardConfig, int cardId);
};

#endif // GAME_MODEL_GENERATOR_H
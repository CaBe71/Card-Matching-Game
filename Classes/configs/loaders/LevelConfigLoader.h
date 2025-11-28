#ifndef LEVEL_CONFIG_LOADER_H
#define LEVEL_CONFIG_LOADER_H

#include "../../configs/models/LevelConfig.h"

// 关卡配置加载器，负责从JSON文件加载关卡配置

class LevelConfigLoader
{
public:
    

    
    static LevelConfig* loadLevelConfigWithDifficulty(int levelId, int difficulty);

};

#endif // LEVEL_CONFIG_LOADER_H
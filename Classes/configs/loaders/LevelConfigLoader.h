#pragma once
#ifndef LEVEL_CONFIG_LOADER_H
#define LEVEL_CONFIG_LOADER_H

#include "../models/LevelConfig.h"

/**
 * 关卡配置加载器
 * 负责从文件（如JSON）加载关卡配置
 */
class LevelConfigLoader
{
public:
    LevelConfigLoader();
    ~LevelConfigLoader() = default;

    /**
     * 加载指定关卡ID的配置
     * @param levelId 关卡ID
     * @return 关卡配置对象，如果加载失败返回nullptr
     */
    LevelConfig* loadLevelConfig(int levelId);

private:
    // 从JSON文件解析配置
    LevelConfig* parseLevelConfig(const std::string& filename);
};

#endif // LEVEL_CONFIG_LOADER_H
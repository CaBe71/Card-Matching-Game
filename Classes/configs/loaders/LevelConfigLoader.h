#ifndef LEVEL_CONFIG_LOADER_H
#define LEVEL_CONFIG_LOADER_H

#include "../../configs/models/LevelConfig.h"

/**
 * @class LevelConfigLoader
 * @brief 关卡配置加载器，负责从JSON文件加载关卡配置
 */
class LevelConfigLoader
{
public:
    /**
     * @brief 根据关卡ID加载关卡配置
     * @param levelId 关卡ID
     * @return 关卡配置对象
     */
    static LevelConfig* loadLevelConfig(int levelId);

    /**
     * @brief 根据难度级别加载关卡配置
     * @param levelId 关卡ID
     * @param difficulty 难度级别 (1-简单, 2-中等, 3-困难)
     * @return 关卡配置对象
     */
    static LevelConfig* loadLevelConfigWithDifficulty(int levelId, int difficulty);

    /**
     * @brief 从文件加载关卡配置
     * @param filename 文件名
     * @return 关卡配置对象
     */
    static LevelConfig* loadLevelConfigFromFile(const std::string& filename);

    /**
     * @brief 从JSON字符串加载关卡配置
     * @param jsonStr JSON字符串
     * @return 关卡配置对象
     */
    static LevelConfig* loadLevelConfigFromString(const std::string& jsonStr);
};

#endif // LEVEL_CONFIG_LOADER_H
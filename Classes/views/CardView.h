#pragma once
#ifndef CARD_VIEW_H
#define CARD_VIEW_H

#include "cocos2d.h"
#include "../models/CardModel.h"

// 前向声明，避免循环依赖
class CardController;

/**
 * 卡牌视图
 * 负责单张卡牌的显示和触摸事件处理
 */
class CardView : public cocos2d::Node
{
public:
    // 使用标准的cocos2d-x创建模式
    static CardView* create();
    virtual bool init() override;

    /**
     * 更新视图显示
     * @param cardModel 卡牌数据模型
     */
    void updateView(const CardModel* cardModel);

    /**
     * 设置卡牌点击回调
     * @param callback 回调函数，参数为卡牌ID
     */
    void setClickCallback(const std::function<void(int)>& callback) { _clickCallback = callback; }

    /**
     * 播放移动动画
     * @param targetPosition 目标位置
     * @param duration 动画时长
     * @param callback 动画完成后的回调
     */
    void playMoveAnimation(const cocos2d::Vec2& targetPosition, float duration, const std::function<void()>& callback = nullptr);

    /**
     * 播放匹配消除动画
     * @param callback 动画完成后的回调
     */
    void playMatchAnimation(const std::function<void()>& callback = nullptr);

    /**
     * 设置卡牌是否可点击
     * @param enabled 是否可点击
     */
    void setTouchEnabled(bool enabled);

    /**
     * 设置卡牌是否翻开
     * @param flipped 是否翻开
     */
    void setFlipped(bool flipped) { _flipped = flipped; }

private:
    void setupTouchHandling();
    void onTouched();

    cocos2d::Sprite* _cardSprite; // 卡牌精灵
    cocos2d::Sprite* _faceSprite; // 数字和花色精灵
    cocos2d::Sprite* _suitSprite; // 花色精灵（如果需要分开显示）
    std::function<void(int)> _clickCallback; // 点击回调
    int _cardId; // 卡牌ID
    bool _flipped; // 是否翻开 
};

#endif // CARD_VIEW_H
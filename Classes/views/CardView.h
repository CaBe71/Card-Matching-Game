#ifndef CARD_VIEW_H
#define CARD_VIEW_H

#include "cocos2d.h"
#include "../models/CardModel.h"

USING_NS_CC;

class CardView : public Sprite
{
public:
    CREATE_FUNC(CardView);

    virtual bool init() override;

    // 更新视图显示基于卡牌模型数据
    
    void updateWithModel(const CardModel* cardModel);

    /**
     * @brief 播放移动动画
     * @param targetPosition 目标位置
     * @param duration 动画时长
     * @param callback 动画完成回调
     */
    void playMoveAnimation(const Vec2& targetPosition, float duration = 0.5f,
        std::function<void()> callback = nullptr);

    /**
     * @brief 播放回退动画
     * @param originalPosition 原始位置
     * @param duration 动画时长
     * @param callback 动画完成回调
     */
    void playReverseAnimation(const Vec2& originalPosition, float duration = 0.5f,
        std::function<void()> callback = nullptr);

    /**
     * @brief 设置点击回调
     * @param callback 点击回调函数
     */
    void setClickCallback(std::function<void(int)> callback) {
        _clickCallback = callback;
    }

    // GetID
    int getCardId() const { return _cardId; }

private:
    // 辅助方法
    void setupTouchListener();
    void onCardTouched();
    

private:
    int _cardId;                                // 卡牌ID
    std::function<void(int)> _clickCallback;    // 点击回调
    Sprite* _cardSprite;                        // 卡牌背景精灵
    Sprite* _faceSprite;                        // 数字精灵
    Sprite* _suitSprite;                        // 花色精灵
    bool _flipped;                              // 是否翻开
};

#endif // CARD_VIEW_H
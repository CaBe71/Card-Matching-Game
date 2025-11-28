#ifndef GAME_VIEW_H
#define GAME_VIEW_H

#include "cocos2d.h"
#include "CardView.h"
#include "ui/CocosGUI.h" 
#include <unordered_map>

USING_NS_CC;

// 游戏主视图类，负责管理游戏界面的显示、布局和用户交互

class GameView : public Layer
{
public:
    
    CREATE_FUNC(GameView);

    // 设置视图的基本属性，创建游戏区域的UI组件，包括主牌区、手牌区、底牌区和回退按钮。

    virtual bool init() override;

    /*
     * 根据提供的卡牌数据模型创建对应的卡牌视图，并布置在相应的游戏区域中。
     * 会清除现有的所有卡牌视图并重新创建。
     */
    void initializeWithCards(const std::vector<CardModel*>& playfieldCards,
        const std::vector<CardModel*>& stackCards,
        CardModel* bottomCard,
        const std::vector<CardModel*>& reserveCards = std::vector<CardModel*>());

    // 为指定卡牌播放从当前位置移动到目标位置的动画效果
    void playCardMatchAnimation(int movingCardId, const Vec2& targetPosition);

    // 播放回退动画
    void playUndoAnimation(int cardId, const Vec2& originalPosition);

    // 设置卡牌点击回调函数
    void setCardClickCallback(std::function<void(int)> callback) {
        _cardClickCallback = callback;
    }

    // 设置回退按钮点击回调函数
    void setUndoCallback(std::function<void()> callback) {
        _undoCallback = callback;
    }

    //创建回退按钮
    void createUndoButton();

private:
    
    // 创建牌区
    void createPlayfieldArea();
    void createStackArea();
    void createBottomCardArea();
    void createReserveArea();
    void createReserveDeckDisplay(const std::vector<CardModel*>& reserveCards);

    
    std::string getFaceText(CardFaceType face);
    std::string getSuitText(CardSuitType suit);

private:
    std::unordered_map<int, CardView*> _cardViews;    // 卡牌视图映射表，键为卡牌ID，值为对应的CardView指针
    std::function<void(int)> _cardClickCallback;      // 卡牌点击事件回调函数
    std::function<void()> _undoCallback;              // 回退按钮点击事件回调函数
    Layer* _playfieldLayer;                           // 主牌区容器层指针
    Layer* _stackLayer;                               // 手牌区容器层指针
    Layer* _bottomCardLayer;                          // 底牌区容器层指针
    CardView* _bottomCardView;                        // 当前底牌的视图指针
};

#endif // GAME_VIEW_H
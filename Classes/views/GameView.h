#pragma once
#ifndef GAME_VIEW_H
#define GAME_VIEW_H

#include "cocos2d.h"
#include "CardView.h"

class GameModel;

class GameView : public cocos2d::Node
{
public:
    static GameView* create();
    virtual bool init() override;

    void updateView(GameModel* gameModel);

    void setCardClickCallback(const std::function<void(int)>& callback) {
        _cardClickCallback = callback;
    }

    // 统一的抽牌区回调（取代原来的Stack+HandArea）
    void setDrawAreaClickCallback(const std::function<void()>& callback) {
        _drawAreaClickCallback = callback;
    }

    CardView* getCardView(int cardId) const;
    void playMatchAnimation(int cardId, const std::function<void()>& callback = nullptr);
    void playCardMoveAnimation(int cardId, const cocos2d::Vec2& targetPosition, float duration, const std::function<void()>& callback = nullptr);

    cocos2d::Vec2 getBottomNodePosition() const { return _bottomNode->getPosition(); }
    cocos2d::Vec2 getDrawAreaNodePosition() const { return _drawAreaNode->getPosition(); }

    cocos2d::Node* getPlayFieldNode() const { return _playFieldNode; }
    cocos2d::Node* getBottomNode() const { return _bottomNode; }
    cocos2d::Node* getDrawAreaNode() const { return _drawAreaNode; }

private:
    void setupUI();
    void setupDrawAreaTouch();
    void createCardView(CardModel* cardModel);

    std::unordered_map<int, CardView*> _cardViews;
    std::function<void(int)> _cardClickCallback;
    std::function<void()> _drawAreaClickCallback;

    cocos2d::Node* _playFieldNode;
    cocos2d::Node* _bottomNode;
    cocos2d::Node* _drawAreaNode; // 统一的抽牌/换牌区

    cocos2d::EventListenerTouchOneByOne* _drawAreaTouchListener;
};

#endif

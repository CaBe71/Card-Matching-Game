#include "GameView.h"
#include "models/GameModel.h"
#include "models/CardModel.h"

USING_NS_CC;

GameView* GameView::create() {
    GameView* pRet = new GameView();
    if (pRet && pRet->init()) { pRet->autorelease(); return pRet; }
    delete pRet; return nullptr;
}

bool GameView::init() {
    if (!Node::init()) return false;
    setupUI();
    return true;
}

void GameView::setupUI() {
    _playFieldNode = Node::create();
    _bottomNode = Node::create();
    _drawAreaNode = Node::create();

    auto playFieldBg = LayerColor::create(Color4B{30, 45, 30, 255}, 1080, 1500);
    _playFieldNode->addChild(playFieldBg, -1);

    auto goldTop = LayerColor::create(Color4B{180, 160, 60, 180}, 1080, 2);
    goldTop->setPosition(0, 1498);
    _playFieldNode->addChild(goldTop, 1);
    auto goldBot = LayerColor::create(Color4B{180, 160, 60, 180}, 1080, 2);
    _playFieldNode->addChild(goldBot, 1);

    auto bottomBg = LayerColor::create(Color4B{20, 30, 45, 255}, 1080, 580);
    addChild(bottomBg, -2);

    auto divider = LayerColor::create(Color4B{180, 160, 60, 120}, 1080, 2);
    divider->setPosition(0, 578);
    addChild(divider, 1);

    addChild(_playFieldNode);
    addChild(_bottomNode);
    addChild(_drawAreaNode);

    _playFieldNode->setPosition(0, 580);
    _bottomNode->setPosition(340, 290);
    _drawAreaNode->setPosition(740, 290);
    _drawAreaNode->setContentSize(Size(180, 260));

    // 底牌区装饰
    auto bottomDeco = DrawNode::create();
    Vec2 bp[4] = {Vec2(-90,-130), Vec2(90,-130), Vec2(90,130), Vec2(-90,130)};
    bottomDeco->drawPolygon(bp, 4, Color4F{0.15f,0.20f,0.35f,1.0f}, 1.5f, Color4F{0.55f,0.48f,0.18f,1.0f});
    _bottomNode->addChild(bottomDeco, -1);
    auto bottomLabel = Label::createWithSystemFont("BOTTOM", "Arial Bold", 16);
    bottomLabel->setPosition(0, -100);
    bottomLabel->setTextColor(Color4B{200, 180, 100, 255});
    _bottomNode->addChild(bottomLabel);

    // DRAW区：空牌背样式装饰（点击触发抽牌）
    auto drawDeco = DrawNode::create();
    Vec2 dp[4] = {Vec2(-90,-130), Vec2(90,-130), Vec2(90,130), Vec2(-90,130)};
    drawDeco->drawPolygon(dp, 4, Color4F{0.12f,0.18f,0.30f,1.0f}, 1.5f, Color4F{0.55f,0.48f,0.18f,1.0f});
    _drawAreaNode->addChild(drawDeco, -1);
    auto drawLabel = Label::createWithSystemFont("DRAW", "Arial Bold", 16);
    drawLabel->setPosition(0, -95);
    drawLabel->setTextColor(Color4B{150, 200, 255, 255});
    _drawAreaNode->addChild(drawLabel);

    setupDrawAreaTouch();
}

void GameView::setupDrawAreaTouch()
{
    if (_drawAreaTouchListener) {
        _eventDispatcher->removeEventListener(_drawAreaTouchListener);
        _drawAreaTouchListener = nullptr;
    }
    _drawAreaTouchListener = EventListenerTouchOneByOne::create();
    _drawAreaTouchListener->setSwallowTouches(true);

    _drawAreaTouchListener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        if (!_drawAreaNode || !_drawAreaNode->isVisible()) return false;
        Vec2 loc = _drawAreaNode->convertToNodeSpace(touch->getLocation());
        return Rect(-90, -130, 180, 260).containsPoint(loc);
    };

    _drawAreaTouchListener->onTouchEnded = [this](Touch* touch, Event* event) {
        CCLOG("GameView: DRAW area clicked!");
        if (_drawAreaClickCallback) _drawAreaClickCallback();
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(_drawAreaTouchListener, _drawAreaNode);
}

void GameView::updateView(GameModel* gameModel) {
    if (!gameModel) return;

    std::vector<Node*> nodesToRemove;
    for (auto child : _playFieldNode->getChildren())
        if (dynamic_cast<CardView*>(child)) nodesToRemove.push_back(child);
    for (auto child : _bottomNode->getChildren())
        if (dynamic_cast<CardView*>(child)) nodesToRemove.push_back(child);
    // DRAW 区不管理 CardView，不需要清理

    for (auto node : nodesToRemove) node->removeFromParent();
    _cardViews.clear();

    // 桌面卡牌
    for (auto cardModel : gameModel->getPlayFieldCards()) {
        if (cardModel) {
            CardView* cv = CardView::create();
            if (cv) {
                cv->updateView(cardModel);
                cv->setClickCallback(_cardClickCallback);
                _cardViews[cardModel->getCardId()] = cv;
                _playFieldNode->addChild(cv);
            }
        }
    }

    // 底牌
    CardModel* bc = gameModel->getBottomCard();
    if (bc) {
        CardView* cv = CardView::create();
        if (cv) {
            bc->setFlipped(true);
            cv->updateView(bc);
            cv->setScale(1.1f);
            _cardViews[bc->getCardId()] = cv;
            _bottomNode->addChild(cv);
        }
    }

    // DRAW 区：只保留装饰背景和标签，不添加任何 CardView
    // 点击事件由 setupDrawAreaTouch 的监听器直接处理
}

CardView* GameView::getCardView(int cardId) const {
    auto it = _cardViews.find(cardId);
    return (it != _cardViews.end()) ? it->second : nullptr;
}

void GameView::playMatchAnimation(int cardId, const std::function<void()>& callback) {
    CardView* cv = getCardView(cardId);
    if (cv) cv->playMatchAnimation(callback);
    else if (callback) callback();
}

void GameView::playCardMoveAnimation(int cardId, const cocos2d::Vec2& tp, float dur, const std::function<void()>& callback) {
    CardView* cv = getCardView(cardId);
    if (cv) cv->playMoveAnimation(tp, dur, callback);
    else if (callback) callback();
}

// GameView.cpp - 简化版本
#include "GameView.h"

USING_NS_CC;

bool GameView::init()
{
    if (!Layer::init()) {
        return false;
    }

    // 设置层大小
    this->setContentSize(Size(1080, 2080));

    // 创建游戏区域
    createPlayfieldArea();
    createStackArea();
    createBottomCardArea();
    createUndoButton();

    CCLOG("GameView initialized successfully");
    return true;
}

void GameView::createPlayfieldArea()
{
    _playfieldLayer = Layer::create();
    _playfieldLayer->setContentSize(Size(1080, 1200));
    _playfieldLayer->setPosition(Vec2(0, 800));
    this->addChild(_playfieldLayer);

    // 调试背景
    auto debugBg = LayerColor::create(Color4B(200, 200, 200, 100), 1080, 1200);
    _playfieldLayer->addChild(debugBg, -1);

    auto label = Label::createWithSystemFont("PLAYFIELD", "Arial", 24);
    label->setPosition(Vec2(540, 1150));
    label->setTextColor(Color4B::BLACK);
    _playfieldLayer->addChild(label);
}

void GameView::createStackArea()
{
    _stackLayer = Layer::create();
    _stackLayer->setContentSize(Size(1080, 600));
    _stackLayer->setPosition(Vec2(0, 200));
    this->addChild(_stackLayer);

    // 调试背景
    auto debugBg = LayerColor::create(Color4B(150, 150, 150, 100), 1080, 600);
    _stackLayer->addChild(debugBg, -1);

    auto label = Label::createWithSystemFont("STACK & RESERVE", "Arial", 24);
    label->setPosition(Vec2(540, 550));
    label->setTextColor(Color4B::BLACK);
    _stackLayer->addChild(label);
}

void GameView::createBottomCardArea()
{
    _bottomCardLayer = Layer::create();
    _bottomCardLayer->setContentSize(Size(200, 200));
    _bottomCardLayer->setPosition(Vec2(440, 50));
    this->addChild(_bottomCardLayer);

    // 底牌区域背景
    auto debugBg = LayerColor::create(Color4B(100, 100, 200, 150), 200, 200);
    _bottomCardLayer->addChild(debugBg, -1);

    auto label = Label::createWithSystemFont("BOTTOM CARD\n(Click to Draw)", "Arial", 20);
    label->setPosition(Vec2(100, 180));
    label->setTextColor(Color4B::WHITE);
    label->setAlignment(TextHAlignment::CENTER);
    _bottomCardLayer->addChild(label);
}

void GameView::createUndoButton()
{
    auto undoButton = ui::Button::create();
    undoButton->setTitleText("UNDO");
    undoButton->setTitleFontSize(24);
    undoButton->setPosition(Vec2(900, 1900));
    undoButton->addClickEventListener([this](Ref* sender) {
        CCLOG("Undo button clicked");
        if (_undoCallback) {
            _undoCallback();
        }
        });
    this->addChild(undoButton);
}

void GameView::initializeWithCards(const std::vector<CardModel*>& playfieldCards,
    const std::vector<CardModel*>& stackCards,
    CardModel* bottomCard,
    const std::vector<CardModel*>& reserveCards)
{
    CCLOG("=== GameView::initializeWithCards ===");
    CCLOG("Playfield: %zu, Stack: %zu, Reserve: %zu cards",
        playfieldCards.size(), stackCards.size(), reserveCards.size());

    // 清除所有现有视图
    _cardViews.clear();
    _playfieldLayer->removeAllChildren();
    _stackLayer->removeAllChildren();
    _bottomCardLayer->removeAllChildren();

    // 重新创建区域背景
    createPlayfieldArea();
    createStackArea();
    createBottomCardArea();

    // ?? 创建主牌区卡牌
    for (auto cardModel : playfieldCards) {
        auto cardView = CardView::create();
        if (cardView) {
            cardView->updateWithModel(cardModel);
            cardView->setClickCallback(_cardClickCallback);
            _playfieldLayer->addChild(cardView);
            _cardViews[cardModel->getCardId()] = cardView;
            CCLOG("Created playfield card: %d", cardModel->getCardId());
        }
    }

    // ?? 创建备用牌堆显示
    if (!reserveCards.empty()) {
        auto reserveBg = LayerColor::create(Color4B(80, 80, 80, 200), 100, 150);
        reserveBg->setPosition(Vec2(200, 225));
        _stackLayer->addChild(reserveBg);

        auto reserveLabel = Label::createWithSystemFont("RESERVE\n" + std::to_string(reserveCards.size()), "Arial", 16);
        reserveLabel->setPosition(Vec2(50, 75));
        reserveLabel->setTextColor(Color4B::WHITE);
        reserveLabel->setAlignment(TextHAlignment::CENTER);
        reserveBg->addChild(reserveLabel);
    }

    // ?? 创建底牌
    if (bottomCard) {
        _bottomCardView = CardView::create();
        if (_bottomCardView) {
            _bottomCardView->updateWithModel(bottomCard);
            _bottomCardView->setClickCallback(_cardClickCallback);
            _bottomCardLayer->addChild(_bottomCardView);
            _cardViews[bottomCard->getCardId()] = _bottomCardView;
            CCLOG("Created bottom card: %d", bottomCard->getCardId());
        }
    }

    CCLOG("Total card views created: %zu", _cardViews.size());
}

void GameView::playCardMatchAnimation(int movingCardId, const Vec2& targetPosition)
{
    auto it = _cardViews.find(movingCardId);
    if (it != _cardViews.end()) {
        it->second->playMoveAnimation(targetPosition, 0.5f, nullptr);
        CCLOG("Playing match animation for card %d", movingCardId);
    }
}

void GameView::playUndoAnimation(int cardId, const Vec2& originalPosition)
{
    auto it = _cardViews.find(cardId);
    if (it != _cardViews.end()) {
        it->second->playReverseAnimation(originalPosition, 0.5f, nullptr);
        CCLOG("Playing undo animation for card %d", cardId);
    }
}
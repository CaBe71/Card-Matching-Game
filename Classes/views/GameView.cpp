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
    CCLOG("Playfield: %zu, Stack: %zu, Reserve: %zu, Bottom: %s",
        playfieldCards.size(), stackCards.size(), reserveCards.size(),
        bottomCard ? "YES" : "NO");

    // 清除所有现有视图
    _cardViews.clear();
    _playfieldLayer->removeAllChildren();
    _stackLayer->removeAllChildren();
    _bottomCardLayer->removeAllChildren();

    // 重新创建区域背景
    createPlayfieldArea();
    createStackArea();
    createBottomCardArea();

    // 创建主牌区卡牌
    for (auto cardModel : playfieldCards) {
        auto cardView = CardView::create();
        if (cardView) {
            cardView->updateWithModel(cardModel);
            cardView->setClickCallback(_cardClickCallback);
            _playfieldLayer->addChild(cardView);
            _cardViews[cardModel->getCardId()] = cardView;
            CCLOG("Created playfield card: ID=%d", cardModel->getCardId());
        }
    }

    // 创建手牌区卡牌
    for (auto cardModel : stackCards) {
        auto cardView = CardView::create();
        if (cardView) {
            cardView->updateWithModel(cardModel);
            cardView->setClickCallback(_cardClickCallback);
            _stackLayer->addChild(cardView);
            _cardViews[cardModel->getCardId()] = cardView;
            CCLOG("Created stack card: ID=%d", cardModel->getCardId());
        }
    }

    // ?? 创建底牌（确保位置正确）
    if (bottomCard) {
        _bottomCardView = CardView::create();
        if (_bottomCardView) {
            // ?? 重要：设置底牌在底牌层内的相对位置
            bottomCard->setPosition(Vec2(60, 90)); // 在底牌层中心
            _bottomCardView->updateWithModel(bottomCard);
            _bottomCardView->setClickCallback(_cardClickCallback);
            _bottomCardLayer->addChild(_bottomCardView);
            _cardViews[bottomCard->getCardId()] = _bottomCardView;

            CCLOG("? Created bottom card: ID=%d, Face=%d at (%.1f, %.1f)",
                bottomCard->getCardId(), bottomCard->getFace(),
                bottomCard->getPosition().x, bottomCard->getPosition().y);
        }
    }
    else {
        CCLOG("? No bottom card to create");
    }

    // 创建备牌堆显示
    createReserveDeckDisplay(reserveCards);

    CCLOG("Total card views created: %zu", _cardViews.size());
}

void GameView::createReserveArea()
{
    auto reserveArea = Layer::create();
    reserveArea->setContentSize(Size(120, 180));
    reserveArea->setPosition(Vec2(800, 250));
    _stackLayer->addChild(reserveArea);

    // 备牌区背景（可点击）
    auto reserveBg = LayerColor::create(Color4B(60, 60, 60, 255), 120, 180);
    reserveArea->addChild(reserveBg);

    // 添加触摸监听器让备牌堆可点击
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        Vec2 touchLocation = touch->getLocation();
        auto reserveArea = _stackLayer->getChildByTag(100); // 假设设置tag

        if (reserveArea && reserveArea->getBoundingBox().containsPoint(touchLocation)) {
            CCLOG("Reserve deck clicked");
            // ?? 发送特殊卡牌ID表示备牌堆点击
            if (_cardClickCallback) {
                _cardClickCallback(-999); // 使用特殊ID表示备牌堆
            }
            return true;
        }
        return false;
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, reserveArea);
}

// ?? 新增：创建备牌堆显示
void GameView::createReserveDeckDisplay(const std::vector<CardModel*>& reserveCards)
{
    auto reserveArea = _stackLayer->getChildByTag(100);
    if (!reserveArea) return;

    // 清除旧内容
    reserveArea->removeAllChildren();

    // 备牌堆背景
    auto reserveBg = LayerColor::create(Color4B(80, 80, 80, 200), 100, 150);
    reserveBg->setPosition(Vec2(10, 15));
    reserveArea->addChild(reserveBg);

    // 显示备牌数量
    std::string reserveText = "RESERVE\n" + std::to_string(reserveCards.size());
    auto reserveLabel = Label::createWithSystemFont(reserveText, "Arial", 16);
    reserveLabel->setPosition(Vec2(50, 75));
    reserveLabel->setTextColor(Color4B::WHITE);
    reserveLabel->setAlignment(TextHAlignment::CENTER);
    reserveBg->addChild(reserveLabel);

    // ?? 如果备牌堆有卡牌，显示最上面一张的预览
    if (!reserveCards.empty()) {
        auto topCardPreview = LayerColor::create(Color4B(255, 255, 255, 100), 80, 120);
        topCardPreview->setPosition(Vec2(20, 90));
        reserveArea->addChild(topCardPreview);

        // 显示最上面一张卡牌的信息
        CardModel* topCard = reserveCards.back();
        std::string previewText = getFaceText(topCard->getFace()) + "\n" + getSuitText(topCard->getSuit());
        auto previewLabel = Label::createWithSystemFont(previewText, "Arial", 12);
        previewLabel->setPosition(Vec2(40, 60));
        previewLabel->setTextColor(Color4B::BLACK);
        previewLabel->setAlignment(TextHAlignment::CENTER);
        topCardPreview->addChild(previewLabel);
    }
}

std::string GameView::getFaceText(CardFaceType face)
{
    switch (face) {
    case CFT_ACE:   return "A";
    case CFT_TWO:   return "2";
    case CFT_THREE: return "3";
    case CFT_FOUR:  return "4";
    case CFT_FIVE:  return "5";
    case CFT_SIX:   return "6";
    case CFT_SEVEN: return "7";
    case CFT_EIGHT: return "8";
    case CFT_NINE:  return "9";
    case CFT_TEN:   return "10";
    case CFT_JACK:  return "J";
    case CFT_QUEEN: return "Q";
    case CFT_KING:  return "K";
    default:        return "?";
    }
}

std::string GameView::getSuitText(CardSuitType suit)
{
    switch (suit) {
    case CST_CLUBS:    return "?";
    case CST_DIAMONDS: return "?";
    case CST_HEARTS:   return "?";
    case CST_SPADES:   return "?";
    default:           return "?";
    }
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
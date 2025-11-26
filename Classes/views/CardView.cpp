#include "CardView.h"
#include <string>

USING_NS_CC;

bool CardView::init()
{
    if (!Sprite::init()) {
        return false;
    }

    // 初始化成员变量
    _cardSprite = nullptr;
    _faceSprite = nullptr;
    _suitSprite = nullptr;
    _flipped = true;
    _cardId = -1;

    // 设置默认内容大小，确保触摸检测正常工作
    this->setContentSize(Size(100, 150));
    this->setAnchorPoint(Vec2(0.5f, 0.5f));

    CCLOG("CardView initialized with default size 100x150");
    return true;
}

// CardView.cpp - 确保底牌的触摸检测正常工作
void CardView::setupTouchListener()
{
    // 移除旧的监听器
    _eventDispatcher->removeEventListenersForTarget(this);

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        Vec2 touchLocation = touch->getLocation();
        Vec2 localPos = this->convertToNodeSpace(touchLocation);

        Rect rect = Rect(0, 0, this->getContentSize().width, this->getContentSize().height);
        bool hit = rect.containsPoint(localPos);

        if (hit) {
            CCLOG("? TOUCH BEGAN on Card %d at (%.1f, %.1f)", _cardId, localPos.x, localPos.y);
            this->setScale(0.95f); // ?? 添加点击反馈
        }

        return hit;
        };

    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        this->setScale(1.0f); // ?? 恢复缩放

        CCLOG("?? TOUCH ENDED on Card %d - Calling callback", _cardId);

        if (_clickCallback) {
            _clickCallback(_cardId);
        }
        else {
            CCLOG("? ERROR: No click callback for card %d", _cardId);
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CardView::onCardTouched()
{
    CCLOG("Card %d touched, calling callback", _cardId);

    if (_clickCallback) {
        _clickCallback(_cardId);
    }
    else {
        CCLOG("ERROR: Click callback is null for card %d", _cardId);
    }
}

void CardView::updateWithModel(const CardModel* cardModel)
{
    if (!cardModel) {
        CCLOG("ERROR: cardModel is null in updateWithModel");
        return;
    }

    CCLOG("CardView: Updating card %d with face %d and suit %d",
        cardModel->getCardId(),
        (int)cardModel->getFace(),
        (int)cardModel->getSuit());

    _cardId = cardModel->getCardId();
    _flipped = true;

    // 设置位置
    setPosition(cardModel->getPosition());

    // 移除旧的显示内容
    removeAllChildren();

    // 显示卡牌正面 - 使用 card_general.png 作为背景
    _cardSprite = Sprite::create("card_general.png");
    if (!_cardSprite) {
        // 如果卡牌背景图片不存在，使用备用方案
        CCLOG("card_general.png not found, using fallback background");
        _cardSprite = Sprite::create();
        auto drawNode = DrawNode::create();
        Vec2 rectangle[4];
        rectangle[0] = Vec2(0, 0);
        rectangle[1] = Vec2(100, 0);
        rectangle[2] = Vec2(100, 150);
        rectangle[3] = Vec2(0, 150);
        drawNode->drawPolygon(rectangle, 4, Color4F(1, 1, 1, 1), 1, Color4F(0, 0, 0, 1));
        _cardSprite->addChild(drawNode);
        _cardSprite->setContentSize(Size(100, 150));
    }
    else {
        // 如果使用图片，确保图片大小正确
        _cardSprite->setContentSize(Size(100, 150));
    }

    // 重要：设置卡牌本身的内容大小，这是触摸检测的关键
    this->setContentSize(Size(100, 150));
    this->setAnchorPoint(Vec2(0.5f, 0.5f));

    addChild(_cardSprite);

    // 获取卡牌数据
    CardFaceType face = cardModel->getFace();
    CardSuitType suit = cardModel->getSuit();

    // 根据花色确定颜色
    std::string faceColor = (suit == CST_HEARTS || suit == CST_DIAMONDS) ? "red" : "black";

    // 1. 创建中间大数字精灵 - 调整位置避免重叠
    std::string bigFaceFilename;
    switch (face) {
    case CFT_ACE:   bigFaceFilename = "number/big_" + faceColor + "_A.png"; break;
    case CFT_TWO:   bigFaceFilename = "number/big_" + faceColor + "_2.png"; break;
    case CFT_THREE: bigFaceFilename = "number/big_" + faceColor + "_3.png"; break;
    case CFT_FOUR:  bigFaceFilename = "number/big_" + faceColor + "_4.png"; break;
    case CFT_FIVE:  bigFaceFilename = "number/big_" + faceColor + "_5.png"; break;
    case CFT_SIX:   bigFaceFilename = "number/big_" + faceColor + "_6.png"; break;
    case CFT_SEVEN: bigFaceFilename = "number/big_" + faceColor + "_7.png"; break;
    case CFT_EIGHT: bigFaceFilename = "number/big_" + faceColor + "_8.png"; break;
    case CFT_NINE:  bigFaceFilename = "number/big_" + faceColor + "_9.png"; break;
    case CFT_TEN:   bigFaceFilename = "number/big_" + faceColor + "_10.png"; break;
    case CFT_JACK:  bigFaceFilename = "number/big_" + faceColor + "_J.png"; break;
    case CFT_QUEEN: bigFaceFilename = "number/big_" + faceColor + "_Q.png"; break;
    case CFT_KING:  bigFaceFilename = "number/big_" + faceColor + "_K.png"; break;
    default:        bigFaceFilename = "number/big_" + faceColor + "_A.png"; break;
    }

    CCLOG("Loading big face: %s", bigFaceFilename.c_str());
    _faceSprite = Sprite::create(bigFaceFilename);
    if (_faceSprite) {
        // 设置大数字在卡牌正中间偏下的位置，避免与顶部重叠
        _faceSprite->setPosition(Vec2(50, 60));  // 从75调整到60，向下移动
        _cardSprite->addChild(_faceSprite);
        CCLOG("Big face sprite created successfully");
    }
    else {
        // 如果数字图片不存在，使用文本标签
        CCLOG("Failed to load big face: %s, using text fallback", bigFaceFilename.c_str());
        auto label = createFallbackLabel(getFaceText(face), 36, faceColor);
        label->setPosition(Vec2(50, 60));  // 同样调整位置
        _cardSprite->addChild(label);
    }

    // 2. 创建左上角小数字 - 调整位置到更角落
    std::string smallFaceFilename;
    switch (face) {
    case CFT_ACE:   smallFaceFilename = "number/small_" + faceColor + "_A.png"; break;
    case CFT_TWO:   smallFaceFilename = "number/small_" + faceColor + "_2.png"; break;
    case CFT_THREE: smallFaceFilename = "number/small_" + faceColor + "_3.png"; break;
    case CFT_FOUR:  smallFaceFilename = "number/small_" + faceColor + "_4.png"; break;
    case CFT_FIVE:  smallFaceFilename = "number/small_" + faceColor + "_5.png"; break;
    case CFT_SIX:   smallFaceFilename = "number/small_" + faceColor + "_6.png"; break;
    case CFT_SEVEN: smallFaceFilename = "number/small_" + faceColor + "_7.png"; break;
    case CFT_EIGHT: smallFaceFilename = "number/small_" + faceColor + "_8.png"; break;
    case CFT_NINE:  smallFaceFilename = "number/small_" + faceColor + "_9.png"; break;
    case CFT_TEN:   smallFaceFilename = "number/small_" + faceColor + "_10.png"; break;
    case CFT_JACK:  smallFaceFilename = "number/small_" + faceColor + "_J.png"; break;
    case CFT_QUEEN: smallFaceFilename = "number/small_" + faceColor + "_Q.png"; break;
    case CFT_KING:  smallFaceFilename = "number/small_" + faceColor + "_K.png"; break;
    default:        smallFaceFilename = "number/small_" + faceColor + "_A.png"; break;
    }

    CCLOG("Loading small face: %s", smallFaceFilename.c_str());
    auto smallFaceSprite = Sprite::create(smallFaceFilename);
    if (smallFaceSprite) {
        // 设置小数字在左上角更角落的位置
        smallFaceSprite->setPosition(Vec2(20, 130));  // 从(25,125)调整到(20,130)，更靠角落
        _cardSprite->addChild(smallFaceSprite);
        CCLOG("Small face sprite created successfully");
    }
    else {
        // 如果数字图片不存在，使用文本标签
        CCLOG("Failed to load small face: %s, using text fallback", smallFaceFilename.c_str());
        auto smallLabel = createFallbackLabel(getFaceText(face), 16, faceColor);  // 字体稍小
        smallLabel->setPosition(Vec2(20, 130));  // 同样调整位置
        _cardSprite->addChild(smallLabel);
    }

    // 3. 创建花色精灵 - 调整位置到右上角
    std::string suitFilename;
    switch (suit) {
    case CST_CLUBS:    suitFilename = "suits/club.png"; break;
    case CST_DIAMONDS: suitFilename = "suits/diamond.png"; break;
    case CST_HEARTS:   suitFilename = "suits/heart.png"; break;
    case CST_SPADES:   suitFilename = "suits/spade.png"; break;
    default:           suitFilename = ""; break;
    }

    CCLOG("Loading suit: %s", suitFilename.c_str());
    if (!suitFilename.empty()) {
        _suitSprite = Sprite::create(suitFilename);
        if (_suitSprite) {
            // 设置花色在右上角更角落的位置
            _suitSprite->setPosition(Vec2(80, 130));  // 从(75,125)调整到(80,130)，更靠角落
            _cardSprite->addChild(_suitSprite);
            CCLOG("Suit sprite created successfully");
        }
        else {
            CCLOG("Failed to load suit: %s, using text fallback", suitFilename.c_str());
            auto suitLabel = createFallbackLabel(getSuitText(suit), 16, faceColor);  // 字体稍小
            suitLabel->setPosition(Vec2(80, 130));  // 同样调整位置
            _cardSprite->addChild(suitLabel);
        }
    }

    // 设置触摸监听 - 必须在所有显示设置完成后调用
    setupTouchListener();

    CCLOG("Card %d setup complete, content size: %.1fx%.1f",
        _cardId, getContentSize().width, getContentSize().height);
}

std::string CardView::getFaceText(CardFaceType face)
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

std::string CardView::getSuitText(CardSuitType suit)
{
    switch (suit) {
    case CST_CLUBS:    return "?";
    case CST_DIAMONDS: return "?";
    case CST_HEARTS:   return "?";
    case CST_SPADES:   return "?";
    default:           return "?";
    }
}

Label* CardView::createFallbackLabel(const std::string& text, int fontSize, const std::string& color)
{
    auto label = Label::createWithSystemFont(text, "Arial", fontSize);
    if (color == "red") {
        label->setTextColor(Color4B::RED);
    }
    else {
        label->setTextColor(Color4B::BLACK);
    }
    return label;
}

void CardView::playMoveAnimation(const Vec2& targetPosition, float duration,
    std::function<void()> callback)
{
    auto moveAction = MoveTo::create(duration, targetPosition);
    auto sequence = Sequence::create(
        moveAction,
        CallFunc::create([callback]() {
            if (callback) callback();
            }),
        nullptr
    );

    this->runAction(sequence);
}

void CardView::playReverseAnimation(const Vec2& originalPosition, float duration,
    std::function<void()> callback)
{
    auto moveAction = MoveTo::create(duration, originalPosition);
    auto sequence = Sequence::create(
        moveAction,
        CallFunc::create([callback]() {
            if (callback) callback();
            }),
        nullptr
    );

    this->runAction(sequence);
}
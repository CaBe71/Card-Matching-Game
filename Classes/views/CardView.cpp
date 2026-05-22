#include "CardView.h"
#include "cocos2d.h"

USING_NS_CC;

CardView* CardView::create()
{
    CardView* pRet = new CardView();
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool CardView::init()
{
    if (!Node::init()) {
        return false;
    }
    _cardSprite = nullptr;
    _faceSprite = nullptr;
    _suitSprite = nullptr;
    _cardId = 0;
    _flipped = false;

    return true;
}

void CardView::updateView(const CardModel* cardModel)
{
    if (!cardModel) {
        return;
    }

    CCLOG("CardView: Updating card %d with face %d and suit %d",
        cardModel->getCardId(),
        (int)cardModel->getFace(),
        (int)cardModel->getSuit());

    _cardId = cardModel->getCardId();
    _flipped = cardModel->isFlipped();

    setPosition(cardModel->getPosition());

    removeAllChildren();

    if (_flipped) {
        _cardSprite = Sprite::create("res/card_general.png");
        if (!_cardSprite) {
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
        addChild(_cardSprite);

        int faceValue = cardModel->getFaceValue();
        CardSuitType suit = cardModel->getSuit();

        std::string faceColor = (suit == CardSuitType::HEARTS || suit == CardSuitType::DIAMONDS) ? "red" : "black";

        // 中间大数字
        std::string bigFaceFilename;
        switch (faceValue) {
        case 1: bigFaceFilename = "res/number/big_" + faceColor + "_A.png"; break;
        case 11: bigFaceFilename = "res/number/big_" + faceColor + "_J.png"; break;
        case 12: bigFaceFilename = "res/number/big_" + faceColor + "_Q.png"; break;
        case 13: bigFaceFilename = "res/number/big_" + faceColor + "_K.png"; break;
        default: bigFaceFilename = "res/number/big_" + faceColor + "_" + std::to_string(faceValue) + ".png"; break;
        }

        _faceSprite = Sprite::create(bigFaceFilename);
        if (_faceSprite) {
            _faceSprite->setPosition(Vec2(_cardSprite->getContentSize().width / 2, _cardSprite->getContentSize().height / 2));
            _cardSprite->addChild(_faceSprite);
        }
        else {
            auto label = Label::createWithSystemFont(
                StringUtils::format("%d", faceValue), "Arial", 36);
            label->setPosition(Vec2(_cardSprite->getContentSize().width / 2, _cardSprite->getContentSize().height / 2));
            label->setTextColor((faceColor == "red") ? Color4B::RED : Color4B::BLACK);
            _cardSprite->addChild(label);
        }

        // 左上角小数字
        std::string smallFaceFilename;
        switch (faceValue) {
        case 1: smallFaceFilename = "res/number/small_" + faceColor + "_A.png"; break;
        case 11: smallFaceFilename = "res/number/small_" + faceColor + "_J.png"; break;
        case 12: smallFaceFilename = "res/number/small_" + faceColor + "_Q.png"; break;
        case 13: smallFaceFilename = "res/number/small_" + faceColor + "_K.png"; break;
        default: smallFaceFilename = "res/number/small_" + faceColor + "_" + std::to_string(faceValue) + ".png"; break;
        }

        auto smallFaceSprite = Sprite::create(smallFaceFilename);
        if (smallFaceSprite) {
            smallFaceSprite->setPosition(Vec2(25, _cardSprite->getContentSize().height - 25));
            _cardSprite->addChild(smallFaceSprite);
        }

        // 右上角花色
        std::string suitFilename;
        switch (suit) {
        case CardSuitType::CLUBS:    suitFilename = "res/suits/club.png"; break;
        case CardSuitType::DIAMONDS: suitFilename = "res/suits/diamond.png"; break;
        case CardSuitType::HEARTS:   suitFilename = "res/suits/heart.png"; break;
        case CardSuitType::SPADES:   suitFilename = "res/suits/spade.png"; break;
        default: suitFilename = ""; break;
        }

        if (!suitFilename.empty()) {
            _suitSprite = Sprite::create(suitFilename);
            if (_suitSprite) {
                _suitSprite->setPosition(Vec2(_cardSprite->getContentSize().width - 25, _cardSprite->getContentSize().height - 25));
                _cardSprite->addChild(_suitSprite);
            }
        }
    }
    else {
        _cardSprite = Sprite::create("res/card_general.png");
        if (!_cardSprite) {
            _cardSprite = Sprite::create();
            auto drawNode = DrawNode::create();
            Vec2 rectangle[4];
            rectangle[0] = Vec2(0, 0);
            rectangle[1] = Vec2(100, 0);
            rectangle[2] = Vec2(100, 150);
            rectangle[3] = Vec2(0, 150);
            drawNode->drawPolygon(rectangle, 4, Color4F(0.3f, 0.3f, 0.5f, 1.0f), 1, Color4F(0.0f, 0.0f, 0.0f, 1.0f));
            _cardSprite->addChild(drawNode);
            _cardSprite->setContentSize(Size(100, 150));
        }
        addChild(_cardSprite);
    }

    setupTouchHandling();
}

void CardView::setupTouchHandling()
{
    _eventDispatcher->removeEventListenersForTarget(this);

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        if (!_cardSprite || !isVisible()) return false;

        Vec2 locationInNode = convertToNodeSpace(touch->getLocation());
        Size s = _cardSprite->getContentSize();
        // 使用以锚点(0.5,0.5)为中心的检测矩形，并扩大20像素容差
        float padding = 20.0f;
        Rect rect(-s.width / 2 - padding, -s.height / 2 - padding,
                  s.width + padding * 2, s.height + padding * 2);

        if (rect.containsPoint(locationInNode)) {
            _cardSprite->setColor(Color3B{200, 200, 255});
            return true;
        }
        return false;
        };

    listener->onTouchMoved = [this](Touch* touch, Event* event) {
        if (!_cardSprite) return;
        Vec2 locationInNode = convertToNodeSpace(touch->getLocation());
        Size s = _cardSprite->getContentSize();
        float padding = 20.0f;
        Rect rect(-s.width / 2 - padding, -s.height / 2 - padding,
                  s.width + padding * 2, s.height + padding * 2);

        if (!rect.containsPoint(locationInNode)) {
            _cardSprite->setColor(Color3B::WHITE);
        } else {
            _cardSprite->setColor(Color3B{200, 200, 255});
        }
        };

    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        if (_cardSprite) {
            _cardSprite->setColor(Color3B::WHITE);
        }

        // 只有在松开时手指仍在卡牌区域才触发点击
        Vec2 locationInNode = convertToNodeSpace(touch->getLocation());
        Size s = _cardSprite ? _cardSprite->getContentSize() : Size(140, 200);
        float padding = 20.0f;
        Rect rect(-s.width / 2 - padding, -s.height / 2 - padding,
                  s.width + padding * 2, s.height + padding * 2);

        if (rect.containsPoint(locationInNode) && _clickCallback) {
            CCLOG("CardView: Valid click on card %d", _cardId);
            _clickCallback(_cardId);
        } else {
            CCLOG("CardView: Touch ended outside card %d area", _cardId);
        }
        };

    listener->onTouchCancelled = [this](Touch* touch, Event* event) {
        if (_cardSprite) {
            _cardSprite->setColor(Color3B::WHITE);
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CardView::onTouched()
{
    if (_clickCallback) {
        _clickCallback(_cardId);
    }
}

void CardView::playMoveAnimation(const cocos2d::Vec2& targetPosition, float duration, const std::function<void()>& callback)
{
    auto move = MoveTo::create(duration, targetPosition);
    if (callback) {
        auto sequence = Sequence::create(move, CallFunc::create(callback), nullptr);
        runAction(sequence);
    }
    else {
        runAction(move);
    }
}

void CardView::playMatchAnimation(const std::function<void()>& callback)
{
    auto scaleUp = ScaleTo::create(0.1f, 1.2f);
    auto scaleDown = ScaleTo::create(0.1f, 1.0f);
    if (callback) {
        auto sequence = Sequence::create(scaleUp, scaleDown, CallFunc::create(callback), nullptr);
        runAction(sequence);
    }
    else {
        auto sequence = Sequence::create(scaleUp, scaleDown, nullptr);
        runAction(sequence);
    }
}

void CardView::setTouchEnabled(bool enabled)
{
    _eventDispatcher->pauseEventListenersForTarget(this, !enabled);
    CCLOG("CardView: Touch %s for card %d", enabled ? "enabled" : "disabled", _cardId);
}

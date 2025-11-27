#include "GameController.h"

GameController* GameController::create()
{
    GameController* controller = new GameController();
    if (controller && controller->init()) {
        controller->autorelease();
        return controller;
    }
    CC_SAFE_DELETE(controller);
    return nullptr;
}

bool GameController::init()
{
    if (!Node::init()) {
        return false;
    }

    // 初始化指针
    _gameModel = nullptr;
    _gameView = nullptr;
    _undoManager = new UndoManager();

    _undoManager->init(50);

    CCLOG("GameController initialized successfully");
    return true;
}

void GameController::startGame(int levelId)
{
    CCLOG("=== Starting Game ===");
    CCLOG("Level ID: %d", levelId);

    // ?? 使用新的随机生成方法，设置中等难度
    LevelConfig* levelConfig = LevelConfigLoader::loadLevelConfigWithDifficulty(levelId, 2);
    if (!levelConfig) {
        CCLOG("ERROR: Failed to load level config: %d", levelId);
        return;
    }

    CCLOG("Level config loaded: %zu playfield, %zu stack, %zu reserve cards",
        levelConfig->playfieldCards.size(),
        levelConfig->stackCards.size(),
        levelConfig->reserveCards.size());

    // 生成游戏模型
    _gameModel = GameModelGenerator::generateFromLevelConfig(levelConfig);
    delete levelConfig;

    if (!_gameModel) {
        CCLOG("ERROR: Failed to generate game model");
        return;
    }

    // 调试：检查初始游戏状态
    CCLOG("Game model generated:");
    CCLOG("  - Playfield cards: %zu", _gameModel->getAllPlayfieldCards().size());
    CCLOG("  - Stack cards: %zu", _gameModel->getAllStackCards().size());
    CCLOG("  - Reserve cards: %zu", _gameModel->getAllReserveCards().size());
    CCLOG("  - Bottom card: %s", _gameModel->getBottomCard() ? "Present" : "None");

    if (_gameModel->getBottomCard()) {
        CardModel* bottomCard = _gameModel->getBottomCard();
        CCLOG("  - Bottom card details: ID=%d, Face=%d, Suit=%d",
            bottomCard->getCardId(), bottomCard->getFace(), bottomCard->getSuit());
    }

    // 创建游戏视图
    _gameView = GameView::create();
    if (!_gameView) {
        CCLOG("ERROR: Failed to create game view");
        return;
    }

    // 设置回调函数
    _gameView->setCardClickCallback([this](int cardId) {
        CCLOG("?? Card click callback received for card: %d", cardId);
        this->handleCardClick(cardId);
        });

    _gameView->setUndoCallback([this]() {
        CCLOG("?? Undo button clicked");
        this->handleUndo();
        });

    // 将游戏视图添加到场景
    auto scene = Director::getInstance()->getRunningScene();
    if (scene) {
        scene->addChild(_gameView);
        CCLOG("GameView added to scene");
    }

    // 初始化视图
    refreshGameView();

    CCLOG("=== Game Started Successfully ===");
}

bool GameController::handleCardClick(int cardId)
{
    CCLOG("=== GameController::handleCardClick ===");
    CCLOG("Card clicked: %d", cardId);

    if (!_gameModel || !_gameView) {
        CCLOG("ERROR: GameModel or GameView is null!");
        return false;
    }

    // ?? 首先检查是否是底牌的特殊点击
    CardModel* bottomCard = _gameModel->getBottomCard();
    if (bottomCard && bottomCard->getCardId() == cardId) {
        CCLOG("Bottom card clicked directly: ID=%d", cardId);
        // ?? 底牌点击：从备牌堆抽牌
        return drawFromReserveToHand();
    }

    // 获取点击的卡牌
    CardModel* clickedCard = _gameModel->getCardById(cardId);
    if (!clickedCard) {
        CCLOG("ERROR: Card not found: %d", cardId);

        // ?? 调试：列出所有卡牌
        CCLOG("Available cards:");
        auto allCards = _gameModel->getAllPlayfieldCards();
        for (auto card : allCards) {
            CCLOG("  Playfield - ID: %d, Face: %d", card->getCardId(), card->getFace());
        }
        auto stackCards = _gameModel->getAllStackCards();
        for (auto card : stackCards) {
            CCLOG("  Stack - ID: %d, Face: %d", card->getCardId(), card->getFace());
        }
        auto reserveCards = _gameModel->getAllReserveCards();
        for (auto card : reserveCards) {
            CCLOG("  Reserve - ID: %d, Face: %d", card->getCardId(), card->getFace());
        }
        if (bottomCard) {
            CCLOG("  Bottom - ID: %d, Face: %d", bottomCard->getCardId(), bottomCard->getFace());
        }

        return false;
    }

    CCLOG("Clicked card: ID=%d, Face=%d, Suit=%d, InPlayfield=%d",
        clickedCard->getCardId(), clickedCard->getFace(),
        clickedCard->getSuit(), clickedCard->isInPlayfield());

    // ?? 简化逻辑：只处理主牌区卡牌
    if (clickedCard->isInPlayfield()) {
        // 主牌区卡牌：与底牌匹配
        return handlePlayfieldCardClick(clickedCard);
    }
    else {
        CCLOG("Non-playfield card clicked, no action defined");
        return false;
    }
}

bool GameController::handleHandCardClick(CardModel* handCard)
{
    CCLOG("=== Handling Hand Card Click ===");

    CardModel* bottomCard = _gameModel->getBottomCard();
    if (!bottomCard) {
        CCLOG("ERROR: No bottom card to match with");
        return false;
    }

    CCLOG("Hand card: ID=%d, Face=%d", handCard->getCardId(), handCard->getFace());
    CCLOG("Bottom card: ID=%d, Face=%d", bottomCard->getCardId(), bottomCard->getFace());

    // 检查匹配条件
    int handFace = handCard->getFace();
    int bottomFace = bottomCard->getFace();
    int diff = abs(handFace - bottomFace);

    CCLOG("Hand match calculation: %d - %d = %d (absolute: %d)",
        handFace, bottomFace, handFace - bottomFace, diff);

    // ?? 修复：添加A和K的特殊匹配
    bool canMatch = (diff == 1);
    if (!canMatch) {
        // 检查是否是 A 和 K 的情况
        if ((handFace == 0 && bottomFace == 12) || (handFace == 12 && bottomFace == 0)) {
            canMatch = true;
            CCLOG("Special match: A and K can match");
        }
    }

    if (!canMatch) {
        CCLOG("? Hand card cannot match with bottom card: difference is %d, need 1", diff);
        return false;
    }

    CCLOG("? Hand card matches bottom card! Processing...");

    // 记录原始位置
    Vec2 originalPosition = handCard->getPosition();

    // 记录回退动作
    recordUndoAction(handCard, originalPosition, bottomCard);

    // 手牌与底牌匹配的逻辑
    // 1. 从手牌区移除
    _gameModel->removeStackCard(handCard->getCardId());

    // 2. 设置手牌为新的底牌
    handCard->setPosition(Vec2(540, 290));
    handCard->setTopCard(true);
    handCard->setInPlayfield(false);
    _gameModel->setBottomCard(handCard);

    // 3. 旧底牌移到弃牌区
    bottomCard->setPosition(Vec2(-1000, -1000));
    bottomCard->setTopCard(false);
    bottomCard->setInPlayfield(false);

    CCLOG("? New bottom card: ID=%d, Face=%d", handCard->getCardId(), handCard->getFace());

    // 播放动画
    _gameView->playCardMatchAnimation(handCard->getCardId(), Vec2(540, 290));

    // 刷新视图
    refreshGameView();

    CCLOG("? Successfully matched hand card with bottom card");
    return true;
}

bool GameController::handlePlayfieldCardClick(CardModel* playfieldCard)
{
    CCLOG("=== Handling Playfield Card Click ===");

    CardModel* bottomCard = _gameModel->getBottomCard();
    if (!bottomCard) {
        CCLOG("ERROR: No bottom card to match with");
        return false;
    }

    CCLOG("Playfield card: ID=%d, Face=%d", playfieldCard->getCardId(), playfieldCard->getFace());
    CCLOG("Bottom card: ID=%d, Face=%d", bottomCard->getCardId(), bottomCard->getFace());

    // 检查匹配条件
    int playfieldFace = playfieldCard->getFace();
    int bottomFace = bottomCard->getFace();
    int diff = abs(playfieldFace - bottomFace);

    CCLOG("Match calculation: %d - %d = %d (absolute: %d)",
        playfieldFace, bottomFace, playfieldFace - bottomFace, diff);

    // ?? 修复：添加A和K的特殊匹配
    bool canMatch = (diff == 1);
    if (!canMatch) {
        // 检查是否是 A 和 K 的情况
        if ((playfieldFace == 0 && bottomFace == 12) || (playfieldFace == 12 && bottomFace == 0)) {
            canMatch = true;
            CCLOG("Special match: A and K can match");
        }
    }

    if (!canMatch) {
        CCLOG("? Cards cannot match: difference is %d, need 1", diff);
        return false;
    }

    CCLOG("? Cards can match! Processing match...");

    // 记录原始位置
    Vec2 originalPosition = playfieldCard->getPosition();

    // 记录回退动作
    recordUndoAction(playfieldCard, originalPosition, bottomCard);

    // 主牌与底牌匹配的逻辑
    // 1. 从主牌区移除
    _gameModel->removePlayfieldCard(playfieldCard->getCardId());

    // 2. 设置主牌为新的底牌
    playfieldCard->setPosition(Vec2(540, 290));
    playfieldCard->setTopCard(true);
    playfieldCard->setInPlayfield(false);
    _gameModel->setBottomCard(playfieldCard);

    // 3. 旧底牌移到弃牌区
    bottomCard->setPosition(Vec2(-1000, -1000));
    bottomCard->setTopCard(false);
    bottomCard->setInPlayfield(false);

    CCLOG("? New bottom card: ID=%d, Face=%d", playfieldCard->getCardId(), playfieldCard->getFace());

    // 4. 在原位置生成新的随机卡牌
    CardModel* newCard = CardService::generateRandomCardAtPosition(originalPosition);
    if (newCard) {
        _gameModel->addPlayfieldCard(newCard);
        CCLOG("? Generated new random card: ID=%d, Face=%d", newCard->getCardId(), newCard->getFace());
    }

    // 播放动画
    _gameView->playCardMatchAnimation(playfieldCard->getCardId(), Vec2(540, 290));

    // 刷新视图
    refreshGameView();

    CCLOG("? Successfully matched playfield card with bottom card");
    return true;
}

bool GameController::drawFromReserveToHand()
{
    CCLOG("=== Drawing from Reserve to Hand ===");

    if (!_gameModel->hasReserveCards()) {
        CCLOG("? No reserve cards available");
        return false;
    }

    // 从备牌堆抽牌
    CardModel* newCard = _gameModel->drawFromReserve();
    if (!newCard) {
        CCLOG("ERROR: Failed to draw card from reserve");
        return false;
    }

    CCLOG("? Drew card from reserve: ID=%d, Face=%d, Suit=%d",
        newCard->getCardId(), newCard->getFace(), newCard->getSuit());

    // ?? 添加到手牌区
    newCard->setInPlayfield(false);
    newCard->setPosition(Vec2(200 + (_gameModel->getAllStackCards().size() * 120), 150));
    _gameModel->addStackCard(newCard);

    // 记录回退动作
    recordUndoAction(newCard, Vec2(0, 0));

    // 播放动画
    _gameView->playCardMatchAnimation(newCard->getCardId(), newCard->getPosition());

    // 刷新视图
    refreshGameView();

    CCLOG("? Successfully drew card from reserve to hand");
    return true;
}

bool GameController::handleStackCardClick(int cardId)
{
    CCLOG("Stack card clicked: %d - No special action", cardId);
    return false;
}

bool GameController::handlePlayfieldCardClick(int cardId)
{
    CCLOG("Playfield card clicked: %d - Handled by main logic", cardId);
    return false;
}

bool GameController::drawCardFromReserve()
{
    CCLOG("Legacy drawCardFromReserve called, using new logic");
    return drawFromReserveToHand();
}

void GameController::refreshGameView()
{
    if (_gameView && _gameModel) {
        _gameView->initializeWithCards(
            _gameModel->getAllPlayfieldCards(),
            _gameModel->getAllStackCards(),
            _gameModel->getBottomCard(),
            _gameModel->getAllReserveCards()
        );
        CCLOG("Game view refreshed with %zu reserve cards",
            _gameModel->getAllReserveCards().size());
    }
}

void GameController::replaceStackTopCard(CardModel* newTopCard)
{
    CCLOG("=== Replacing Stack Top Card ===");

    // 获取当前顶部牌
    CardModel* currentTopCard = _gameModel->getTopStackCard();

    if (currentTopCard) {
        currentTopCard->setTopCard(false);
        CCLOG("Removed top status from card: %d", currentTopCard->getCardId());
    }

    // 设置新的顶部牌
    newTopCard->setTopCard(true);

    // 更新位置到手牌区顶部位置
    Vec2 stackPosition(540, 290);
    newTopCard->setPosition(stackPosition);

    CCLOG("Set card %d as new top card at position (%.1f, %.1f)",
        newTopCard->getCardId(), stackPosition.x, stackPosition.y);

    // 更新视图显示
    refreshGameView();
}

void GameController::recordUndoAction(CardModel* movedCard, const Vec2& originalPosition, CardModel* replacedCard)
{
    UndoAction action;
    action.type = UndoAction::CARD_MATCH;
    action.movedCardId = movedCard->getCardId();
    action.originalPosition = originalPosition;
    action.replacedCardId = replacedCard ? replacedCard->getCardId() : -1;
    action.wasTopCard = movedCard->isTopCard();

    _undoManager->pushAction(action);
    CCLOG("Undo action recorded for card: %d", movedCard->getCardId());
}

void GameController::handleUndo()
{
    if (!_undoManager->canUndo()) {
        CCLOG("No actions to undo");
        return;
    }

    UndoAction lastAction = _undoManager->undoLastAction();

    // 获取移动的卡牌
    CardModel* movedCard = _gameModel->getCardById(lastAction.movedCardId);
    if (!movedCard) {
        CCLOG("Card not found for undo: %d", lastAction.movedCardId);
        return;
    }

    // 恢复卡牌位置
    movedCard->setPosition(lastAction.originalPosition);

    // 恢复顶部牌状态
    movedCard->setTopCard(lastAction.wasTopCard);

    // 如果有被替换的卡牌，恢复其为顶部牌
    if (lastAction.replacedCardId != -1) {
        CardModel* replacedCard = _gameModel->getCardById(lastAction.replacedCardId);
        if (replacedCard) {
            replacedCard->setTopCard(true);
        }
    }

    // 播放回退动画
    _gameView->playUndoAnimation(lastAction.movedCardId, lastAction.originalPosition);

    // 刷新视图
    refreshGameView();

    CCLOG("Undo action executed for card: %d", lastAction.movedCardId);
}
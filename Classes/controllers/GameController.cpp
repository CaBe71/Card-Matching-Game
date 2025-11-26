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

    // 加载关卡配置
    LevelConfig* levelConfig = LevelConfigLoader::loadLevelConfig(levelId);
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

    // 获取点击的卡牌
    CardModel* clickedCard = _gameModel->getCardById(cardId);
    if (!clickedCard) {
        CCLOG("ERROR: Card not found: %d", cardId);
        return false;
    }

    CCLOG("Clicked card: ID=%d, Face=%d, Suit=%d, InPlayfield=%d",
        clickedCard->getCardId(), clickedCard->getFace(),
        clickedCard->getSuit(), clickedCard->isInPlayfield());

    // 根据开源项目逻辑，区分处理
    if (clickedCard->isInPlayfield()) {
        // 主牌区卡牌：检查是否能与底牌匹配
        return handlePlayfieldCardToBottom(clickedCard);
    }
    else {
        // 底牌或其他区域卡牌：从备用牌堆抽牌
        return handleBottomCardClick(clickedCard);
    }
}

bool GameController::handlePlayfieldCardToBottom(CardModel* clickedCard)
{
    CCLOG("=== Handling Playfield Card to Bottom ===");

    CardModel* currentBottomCard = _gameModel->getBottomCard();

    if (!currentBottomCard) {
        CCLOG("ERROR: No current bottom card");
        return false;
    }

    CCLOG("Current bottom card: ID=%d, Face=%d",
        currentBottomCard->getCardId(), currentBottomCard->getFace());
    CCLOG("Clicked playfield card: ID=%d, Face=%d",
        clickedCard->getCardId(), clickedCard->getFace());

    // 检查匹配条件（点数差1）
    if (!CardService::canMatch(clickedCard, currentBottomCard)) {
        CCLOG("Cards cannot match: %d and %d (difference: %d)",
            clickedCard->getFace(), currentBottomCard->getFace(),
            abs(clickedCard->getFace() - currentBottomCard->getFace()));
        return false;
    }

    CCLOG("? Cards can match! Moving card %d to bottom", clickedCard->getCardId());

    // 记录原始位置用于回退
    Vec2 originalPosition = clickedCard->getPosition();

    // 记录回退动作
    recordUndoAction(clickedCard, originalPosition, currentBottomCard);

    // 更新卡牌状态
    // 1. 移除点击的卡牌从主牌区
    _gameModel->removePlayfieldCard(clickedCard->getCardId());

    // 2. 将当前底牌移动到弃牌区（隐藏）
    currentBottomCard->setPosition(Vec2(-1000, -1000));
    currentBottomCard->setTopCard(false);

    // 3. 设置点击的卡牌为新的底牌
    clickedCard->setPosition(Vec2(540, 290)); // 底牌位置
    clickedCard->setTopCard(true);
    clickedCard->setInPlayfield(false);
    _gameModel->setBottomCard(clickedCard);

    CCLOG("? New bottom card set: %d", clickedCard->getCardId());

    // 在被移除的位置生成新的随机卡牌
    CardModel* newCard = CardService::generateRandomCardAtPosition(originalPosition);
    if (newCard) {
        _gameModel->addPlayfieldCard(newCard);
        CCLOG("? Generated new random card at (%.1f, %.1f): ID=%d",
            originalPosition.x, originalPosition.y, newCard->getCardId());
    }

    // 播放动画
    _gameView->playCardMatchAnimation(clickedCard->getCardId(), Vec2(540, 290));

    // 刷新视图
    refreshGameView();

    CCLOG("? Successfully moved card %d to bottom", clickedCard->getCardId());
    return true;
}

bool GameController::handleBottomCardClick(CardModel* clickedCard)
{
    CCLOG("=== Handling Bottom Card Click ===");

    // 检查点击的是否真的是底牌
    CardModel* currentBottomCard = _gameModel->getBottomCard();
    if (!currentBottomCard || currentBottomCard->getCardId() != clickedCard->getCardId()) {
        CCLOG("Not a bottom card click");
        return false;
    }

    CCLOG("Bottom card clicked, drawing from reserve...");

    // 从备用牌堆抽牌
    return drawCardFromReserve();
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
    CCLOG("=== Drawing from Reserve ===");

    if (!_gameModel) {
        CCLOG("ERROR: GameModel is null!");
        return false;
    }

    // 检查是否有备用牌
    if (!_gameModel->hasReserveCards()) {
        CCLOG("No reserve cards available");
        return false;
    }

    CardModel* currentBottomCard = _gameModel->getBottomCard();

    // 调用 GameModel 的 drawFromReserve 方法
    CardModel* newCard = _gameModel->drawFromReserve();

    if (!newCard) {
        CCLOG("ERROR: Failed to draw card from reserve");
        return false;
    }

    CCLOG("Drew new card from reserve: ID=%d, Face=%d, Suit=%d",
        newCard->getCardId(), newCard->getFace(), newCard->getSuit());

    // 记录回退动作
    if (currentBottomCard) {
        recordUndoAction(newCard, newCard->getPosition(), currentBottomCard);
        currentBottomCard->setPosition(Vec2(-1000, -1000));
        currentBottomCard->setTopCard(false);
    }
    else {
        recordUndoAction(newCard, newCard->getPosition());
    }

    // 设置新牌为底牌
    newCard->setPosition(Vec2(540, 290));
    newCard->setTopCard(true);
    newCard->setInPlayfield(false);
    _gameModel->setBottomCard(newCard);

    CCLOG("New bottom card set: ID=%d, Remaining reserve: %zu",
        newCard->getCardId(), _gameModel->getAllReserveCards().size());

    // 播放动画
    if (_gameView) {
        _gameView->playCardMatchAnimation(newCard->getCardId(), Vec2(540, 290));
    }

    // 刷新视图
    refreshGameView();

    CCLOG("Successfully drew card from reserve and updated game state");
    return true;
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
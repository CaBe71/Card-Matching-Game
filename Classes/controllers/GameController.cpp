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

    // 使用新的随机生成方法，设置中等难度
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

    // 重要修复：如果没有底牌，从备牌堆抽一张作为初始底牌
    if (!_gameModel->getBottomCard()) {
        CCLOG("No bottom card found, checking if we can draw one from reserve...");

        // 详细检查备牌堆状态
        auto reserveCards = _gameModel->getAllReserveCards();
        CCLOG("Reserve cards count: %zu", reserveCards.size());
        CCLOG("hasReserveCards(): %s", _gameModel->hasReserveCards() ? "true" : "false");

        if (_gameModel->hasReserveCards() && !reserveCards.empty()) {
            CCLOG("Drawing initial bottom card from reserve...");
            CardModel* initialBottomCard = _gameModel->drawFromReserve();
            if (initialBottomCard) {
                initialBottomCard->setPosition(Vec2(540, 290));
                initialBottomCard->setTopCard(true);
                initialBottomCard->setInPlayfield(false);
                _gameModel->setBottomCard(initialBottomCard);
                CCLOG("Successfully set initial bottom card: ID=%d, Face=%d, Suit=%d",
                    initialBottomCard->getCardId(), initialBottomCard->getFace(), initialBottomCard->getSuit());

                // 验证底牌设置
                CCLOG("Bottom card after setting: %s", _gameModel->getBottomCard() ? "Present" : "None");
            }
            else {
                CCLOG("ERROR: drawFromReserve() returned nullptr");
            }
        }
        else {
            CCLOG("WARNING: Cannot draw initial bottom card - reserve is empty or hasReserveCards() returned false");
        }
    }
    else {
        CCLOG("Bottom card already exists: ID=%d", _gameModel->getBottomCard()->getCardId());
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

    // 最终检查底牌状态
    CCLOG("Final bottom card status: %s", _gameModel->getBottomCard() ? "Present" : "None");
    if (_gameModel->getBottomCard()) {
        CCLOG("Final bottom card details: ID=%d, Face=%d",
            _gameModel->getBottomCard()->getCardId(),
            _gameModel->getBottomCard()->getFace());
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

    // 首先检查是否是底牌的特殊点击
    CardModel* bottomCard = _gameModel->getBottomCard();
    if (bottomCard && bottomCard->getCardId() == cardId) {
        CCLOG("Bottom card clicked directly: ID=%d", cardId);
        // 底牌点击：从备牌堆抽牌
        return drawFromReserveToHand();
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

    // 如果没有底牌，任何主牌区卡牌点击都从备牌堆抽牌
    if (!bottomCard && clickedCard->isInPlayfield()) {
        CCLOG("No bottom card available, drawing from reserve instead");
        return drawFromReserveToHand();
    }

    // 只处理主牌区卡牌
    if (clickedCard->isInPlayfield()) {
        // 主牌区卡牌：与底牌匹配
        return handlePlayfieldCardClick(clickedCard);
    }
    else {
        CCLOG("Non-playfield card clicked, no action defined");
        return false;
    }
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

    // 添加A和K的特殊匹配
    bool canMatch = (diff == 1);
    if (!canMatch) {
        // 检查是否是 A 和 K 的情况
        if ((playfieldFace == 0 && bottomFace == 12) || (playfieldFace == 12 && bottomFace == 0)) {
            canMatch = true;
            CCLOG("Special match: A and K can match");
        }
    }

    if (!canMatch) {
        CCLOG("Cards cannot match: difference is %d, need 1", diff);
        return false;
    }

    CCLOG("Cards can match! Processing match...");

    // 记录原始位置和状态
    Vec2 originalPosition = playfieldCard->getPosition();

    // 记录回退动作（在修改状态之前）
    recordUndoAction(playfieldCard, originalPosition, bottomCard);

    // 主牌与底牌匹配的逻辑
    // 1. 从主牌区移除
    _gameModel->removePlayfieldCard(playfieldCard->getCardId());

    // 2. 设置主牌为新的底牌
    playfieldCard->setPosition(Vec2(540, 290));
    playfieldCard->setTopCard(true);
    playfieldCard->setInPlayfield(false);
    _gameModel->setBottomCard(playfieldCard);

    // 3. 重要修改：将旧底牌放回备牌堆，而不是弃牌区
    bottomCard->setPosition(Vec2::ZERO);
    bottomCard->setTopCard(false);
    bottomCard->setInPlayfield(false);
    _gameModel->addReserveCard(bottomCard); // 放回备牌堆
    CCLOG("Returned old bottom card to reserve: ID=%d", bottomCard->getCardId());

    CCLOG("New bottom card: ID=%d, Face=%d", playfieldCard->getCardId(), playfieldCard->getFace());

    // 4. 在原位置生成新的随机卡牌
    CardModel* newCard = CardService::generateRandomCardAtPosition(originalPosition);
    if (newCard) {
        _gameModel->addPlayfieldCard(newCard);
        CCLOG("Generated new random card: ID=%d, Face=%d", newCard->getCardId(), newCard->getFace());
    }

    // 播放动画
    _gameView->playCardMatchAnimation(playfieldCard->getCardId(), Vec2(540, 290));

    // 刷新视图
    refreshGameView();

    CCLOG("Successfully matched playfield card with bottom card");
    return true;
}



bool GameController::drawFromReserveToHand()
{
    CCLOG("=== Drawing from Reserve to Bottom Card ===");

    if (!_gameModel->hasReserveCards()) {
        CCLOG("No reserve cards available");
        return false;
    }

    // 从备牌堆抽牌
    CardModel* newCard = _gameModel->drawFromReserve();
    if (!newCard) {
        CCLOG("ERROR: Failed to draw card from reserve");
        return false;
    }

    CCLOG("Drew card from reserve: ID=%d, Face=%d, Suit=%d",
        newCard->getCardId(), newCard->getFace(), newCard->getSuit());

    // 获取当前底牌
    CardModel* currentBottomCard = _gameModel->getBottomCard();

    // 记录回退动作（在修改状态之前）
    recordUndoAction(newCard, Vec2(0, 0), currentBottomCard);

    if (currentBottomCard) {
        // 重要修改：将被替换的底牌放回备牌堆，而不是弃牌区
        currentBottomCard->setPosition(Vec2::ZERO);
        currentBottomCard->setTopCard(false);
        currentBottomCard->setInPlayfield(false);
        _gameModel->addReserveCard(currentBottomCard); // 放回备牌堆
        CCLOG("Returned old bottom card to reserve: ID=%d", currentBottomCard->getCardId());
    }
    else {
        CCLOG("No current bottom card to replace");
    }

    // 设置新底牌
    newCard->setPosition(Vec2(540, 290));
    newCard->setTopCard(true);
    newCard->setInPlayfield(false);
    _gameModel->setBottomCard(newCard);

    CCLOG("Set new bottom card: ID=%d, Face=%d",
        newCard->getCardId(), newCard->getFace());

    // 播放动画
    _gameView->playCardMatchAnimation(newCard->getCardId(), Vec2(540, 290));

    // 刷新视图
    refreshGameView();

    CCLOG("Successfully drew card from reserve and set as new bottom card");
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



void GameController::handleUndo()
{
    if (!_gameModel || !_gameView || !_undoManager->canUndo()) {
        CCLOG("GameController: Cannot undo - gameModel=%p, gameView=%p, canUndo=%d",
            _gameModel, _gameView, _undoManager ? _undoManager->canUndo() : false);
        return;
    }

    UndoAction lastAction = _undoManager->undoLastAction();
    CCLOG("=== Executing Undo Action ===");
    CCLOG("Operation type: %d, Moved card: %d, Replaced card: %d",
        lastAction.type, lastAction.movedCardId, lastAction.replacedCardId);

    // 根据操作类型执行不同的回退逻辑
    switch (lastAction.type) {
    case UndoAction::CARD_MATCH:
        undoCardMatch(lastAction);
        break;
    case UndoAction::DRAW_CARD:
        undoDrawCard(lastAction);
        break;
    default:
        CCLOG("WARNING: Unknown undo action type: %d", lastAction.type);
        break;
    }

    CCLOG("Undo action executed successfully");
}



void GameController::undoCardMatch(const UndoAction& action)
{
    CCLOG("=== Undoing Card Match ===");

    // 1. 首先移除在匹配时生成的新随机卡牌
    std::vector<CardModel*> playfieldCards = _gameModel->getAllPlayfieldCards();
    std::vector<CardModel*> cardsToRemove;

    for (CardModel* card : playfieldCards) {
        if (card && card->getPosition() == action.originalPosition && card->getCardId() != action.movedCardId) {
            CCLOG("Found newly generated card to remove: ID=%d", card->getCardId());
            cardsToRemove.push_back(card);
        }
    }

    for (CardModel* card : cardsToRemove) {
        if (card && card->getCardId() > 0) {
            _gameModel->removePlayfieldCard(card->getCardId());
            delete card;
            CCLOG("Removed newly generated card: ID=%d", card->getCardId());
        }
    }

    // 2. 恢复移动的卡牌到主牌区
    CardModel* movedCard = findCardInAllContainers(action.movedCardId);
    if (!movedCard) {
        CCLOG("ERROR: Moved card not found: %d", action.movedCardId);
        return;
    }

    // 从当前位置移除
    removeCardFromAllContainers(movedCard->getCardId());

    // 恢复到原始状态
    movedCard->setPosition(action.originalPosition);
    movedCard->setTopCard(false);
    movedCard->setInPlayfield(true);
    _gameModel->addPlayfieldCard(movedCard);
    CCLOG("Restored card %d to playfield", movedCard->getCardId());

    // 3. 恢复被替换的底牌 - 改进的逻辑
    if (action.replacedCardId != -1) {
        CardModel* replacedCard = findCardInAllContainers(action.replacedCardId);
        if (replacedCard) {
            // 从当前位置移除
            removeCardFromAllContainers(replacedCard->getCardId());

            // 恢复到原始状态（底牌位置）
            replacedCard->setPosition(Vec2(540, 290));
            replacedCard->setTopCard(true);
            replacedCard->setInPlayfield(false);
            _gameModel->setBottomCard(replacedCard);
            CCLOG("Restored bottom card: ID=%d", replacedCard->getCardId());
        }
        else {
            CCLOG("WARNING: Replaced card not found: %d", action.replacedCardId);
            _gameModel->setBottomCard(nullptr);
        }
    }
    else {
        _gameModel->setBottomCard(nullptr);
        CCLOG("No replaced card to restore");
    }

    // 播放动画和刷新视图
    _gameView->playUndoAnimation(action.movedCardId, action.originalPosition);
    refreshGameView();
}



void GameController::undoDrawCard(const UndoAction& action)
{
    CCLOG("=== Undoing Draw Card ===");

    // 1. 将抽出的卡牌放回备牌堆
    CardModel* drawnCard = findCardInAllContainers(action.movedCardId);
    if (drawnCard) {
        // 从当前位置移除
        removeCardFromAllContainers(drawnCard->getCardId());

        // 放回备牌堆
        drawnCard->setPosition(Vec2::ZERO);
        drawnCard->setTopCard(false);
        drawnCard->setInPlayfield(false);
        _gameModel->addReserveCard(drawnCard);
        CCLOG("Returned card %d to reserve deck", drawnCard->getCardId());
    }
    else {
        CCLOG("ERROR: Drawn card not found: %d", action.movedCardId);
    }

    // 2. 恢复被替换的底牌
    if (action.replacedCardId != -1) {
        CardModel* replacedCard = findCardInAllContainers(action.replacedCardId);
        if (replacedCard) {
            // 从当前位置移除
            removeCardFromAllContainers(replacedCard->getCardId());

            // 恢复到原始状态
            replacedCard->setPosition(Vec2(540, 290));
            replacedCard->setTopCard(true);
            replacedCard->setInPlayfield(false);
            _gameModel->setBottomCard(replacedCard);
            CCLOG("Restored previous bottom card: ID=%d", replacedCard->getCardId());
        }
        else {
            CCLOG("WARNING: Replaced card not found: %d", action.replacedCardId);
            _gameModel->setBottomCard(nullptr);
        }
    }
    else {
        _gameModel->setBottomCard(nullptr);
        CCLOG("No replaced card to restore");
    }

    // 播放动画和刷新视图
    _gameView->playUndoAnimation(action.movedCardId, Vec2::ZERO);
    refreshGameView();
}



void GameController::recordUndoAction(CardModel* movedCard, const Vec2& originalPosition, CardModel* replacedCard)
{
    UndoAction action;

    // 判断操作类型：如果移动的卡牌来自备牌堆（originalPosition是0,0），则是抽牌操作
    if (originalPosition == Vec2::ZERO && !movedCard->isInPlayfield()) {
        action.type = UndoAction::DRAW_CARD;
        CCLOG("Recording DRAW_CARD undo action");
    }
    else {
        action.type = UndoAction::CARD_MATCH;
        CCLOG("Recording CARD_MATCH undo action");
    }

    action.movedCardId = movedCard->getCardId();
    action.originalPosition = originalPosition;
    action.movedCardWasInPlayfield = movedCard->isInPlayfield();
    action.movedCardWasTopCard = movedCard->isTopCard();

    // 记录被替换卡牌的信息
    if (replacedCard) {
        action.replacedCardId = replacedCard->getCardId();
        action.replacedCardWasInPlayfield = replacedCard->isInPlayfield();
        action.replacedCardWasTopCard = replacedCard->isTopCard();
        action.replacedCardOriginalPosition = replacedCard->getPosition();

        // 记录被替换卡牌的其他重要状态
        action.replacedCardFace = replacedCard->getFace();
        action.replacedCardSuit = replacedCard->getSuit();

        CCLOG("Replaced card info: ID=%d, wasTop=%d, wasInPlayfield=%d, pos(%.1f,%.1f)",
            replacedCard->getCardId(), replacedCard->isTopCard(),
            replacedCard->isInPlayfield(), replacedCard->getPosition().x,
            replacedCard->getPosition().y);
    }
    else {
        action.replacedCardId = -1;
        action.replacedCardWasInPlayfield = false;
        action.replacedCardWasTopCard = false;
        action.replacedCardOriginalPosition = Vec2::ZERO;
        action.replacedCardFace = -1;
        action.replacedCardSuit = -1;
        CCLOG("No replaced card");
    }

    _undoManager->pushAction(action);
    CCLOG("Undo action recorded: type=%d, movedCard=%d, replacedCard=%d",
        action.type, action.movedCardId, action.replacedCardId);
}



CardModel* GameController::findCardInAllContainers(int cardId)
{
    CCLOG("Searching for card %d in all containers", cardId);

    // 1. 检查底牌
    CardModel* bottomCard = _gameModel->getBottomCard();
    if (bottomCard && bottomCard->getCardId() == cardId) {
        CCLOG("Found card %d in bottom card", cardId);
        return bottomCard;
    }

    // 2. 检查主牌区
    auto playfieldCards = _gameModel->getAllPlayfieldCards();
    for (CardModel* card : playfieldCards) {
        if (card && card->getCardId() == cardId) {
            CCLOG("Found card %d in playfield", cardId);
            return card;
        }
    }

    // 3. 检查手牌区
    auto stackCards = _gameModel->getAllStackCards();
    for (CardModel* card : stackCards) {
        if (card && card->getCardId() == cardId) {
            CCLOG("Found card %d in stack", cardId);
            return card;
        }
    }

    // 4. 检查备牌堆
    auto reserveCards = _gameModel->getAllReserveCards();
    for (CardModel* card : reserveCards) {
        if (card && card->getCardId() == cardId) {
            CCLOG("Found card %d in reserve", cardId);
            return card;
        }
    }

    // 5. 最后尝试通过ID直接查找（如果GameModel支持）
    CardModel* cardById = _gameModel->getCardById(cardId);
    if (cardById) {
        CCLOG("Found card %d via direct ID lookup", cardId);
        return cardById;
    }

    CCLOG("WARNING: Card %d not found in any container", cardId);
    return nullptr;
}



// 改进的移除函数
void GameController::removeCardFromAllContainers(int cardId)
{
    CCLOG("Removing card %d from all containers", cardId);

    // 从底牌移除
    if (_gameModel->getBottomCard() && _gameModel->getBottomCard()->getCardId() == cardId) {
        _gameModel->setBottomCard(nullptr);
        CCLOG("Removed card %d from bottom", cardId);
    }

    // 从主牌区移除
    if (_gameModel->removePlayfieldCard(cardId)) {
        CCLOG("Removed card %d from playfield", cardId);
    }

    // 从手牌区移除
    if (_gameModel->removeStackCard(cardId)) {
        CCLOG("Removed card %d from stack", cardId);
    }

    // 从备牌堆移除 - 需要手动实现
    auto reserveCards = _gameModel->getAllReserveCards();
    for (auto it = reserveCards.begin(); it != reserveCards.end(); ++it) {
        if ((*it)->getCardId() == cardId) {
            // 这里需要根据你的 GameModel 实现来移除
            // 如果 GameModel 没有提供 removeReserveCard 方法，可能需要其他方式
            CCLOG("Found card %d in reserve, but need manual removal", cardId);
            break;
        }
    }
}
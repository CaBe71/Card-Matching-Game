#include "GameController.h"
#include "../configs/loaders/LevelConfigLoader.h"
#include "../services/GameModelGenerator.h"
#include "../utils/GameUtils.h"

USING_NS_CC;

GameController::GameController() {
    _undoManager = new UndoManager();
    _configLoader = new LevelConfigLoader();
    _modelGenerator = new GameModelGenerator();
}

GameController::~GameController() {
    delete _gameModel;
    delete _gameView;
    delete _undoManager;
    delete _configLoader;
    delete _modelGenerator;
}

void GameController::startGame(int levelId) {
    _gameModel = _modelGenerator->generateRandomGameModel();
    if (!_gameModel) return;

    _gameView = GameView::create();
    if (!_gameView) return;

    _gameView->setCardClickCallback([this](int cardId) { handleCardClick(cardId); });
    _gameView->setDrawAreaClickCallback([this]() { handleDrawCard(); });

    _gameView->updateView(_gameModel);
    _undoManager->init();
    _gameModel->setScore(0);

    if (_stackCountCallback) _stackCountCallback(_gameModel->getStackRemaining());
    if (_comboCallback) _comboCallback(0);
}

void GameController::restartGame() {
    delete _gameModel;
    _gameModel = nullptr;
    // 重建 view 不重建，由 scene 管理
    _gameModel = _modelGenerator->generateRandomGameModel();
    if (!_gameModel) return;
    _undoManager->init();
    _gameModel->setScore(0);
    _gameView->updateView(_gameModel);
    if (_stackCountCallback) _stackCountCallback(_gameModel->getStackRemaining());
    if (_comboCallback) _comboCallback(0);
    CCLOG("GameController: Game restarted");
}

bool GameController::hasAnyMatch() const {
    if (!_gameModel) return false;
    CardModel* bc = _gameModel->getBottomCard();
    if (!bc) return false;
    for (auto& pf : _gameModel->getPlayFieldCards()) {
        if (pf && checkCardsMatch(pf, bc)) return true;
    }
    return false;
}

void GameController::checkGameEnd() {
    if (!_gameModel || !_gameEndCallback) return;

    if (_gameModel->getStackCards().empty() && !hasAnyMatch()) {
        // 栈空且无匹配 → 胜利
        _gameEndCallback(true);
    }
}

bool GameController::handleCardClick(int cardId)
{
    if (!_gameModel || !_gameView) return false;

    CardModel* clickedCard = _gameModel->getCardById(cardId);
    CardModel* bottomCard = _gameModel->getBottomCard();
    if (!clickedCard || !bottomCard) return false;

    if (checkCardsMatch(clickedCard, bottomCard)) {
        cocos2d::Vec2 pos = clickedCard->getPosition();

        UndoModel* undoModel = new UndoModel();
        undoModel->setOperationType(OperationType::CARD_MATCH);
        undoModel->setMatchedCardId(clickedCard->getCardId());
        undoModel->setPreviousBottomCardId(bottomCard->getCardId());
        undoModel->setMatchedCardPosition(pos);
        undoModel->setPreviousBottomPosition(Vec2::ZERO);
        _undoManager->pushUndoRecord(undoModel);

        // 连击加分：combo 0→+1, combo 1→+2, combo 2→+3...
        _gameModel->addCombo();
        int combo = _gameModel->getCombo();
        int addPoints = 1 + combo;
        _gameModel->addScore(addPoints);
        CCLOG("GameController: Combo x%d! Score +%d, total: %d", combo + 1, addPoints, _gameModel->getScore());

        if (_scoreCallback) _scoreCallback(addPoints);
        if (_comboCallback) _comboCallback(combo + 1);

        _gameModel->removeCardFromPlayField(clickedCard->getCardId());

        auto bottomWorld = _gameView->getBottomNode()->convertToWorldSpace(Vec2::ZERO);
        auto pfNode = _gameView->getPlayFieldNode();
        auto target = pfNode->convertToNodeSpace(bottomWorld);

        _gameView->playCardMoveAnimation(clickedCard->getCardId(), target, 0.5f,
            [this, clickedCard, bottomCard, pos]() {
                if (bottomCard) {
                    auto sc = _gameModel->getStackCards();
                    sc.insert(sc.begin(), bottomCard);
                    _gameModel->setStackCards(sc);
                }
                clickedCard->setPosition(Vec2::ZERO);
                _gameModel->setBottomCard(clickedCard);

                if (!_gameModel->getStackCards().empty()) {
                    _gameModel->drawCardFromStackToPlayField(pos);
                }

                if (_stackCountCallback) _stackCountCallback(_gameModel->getStackRemaining());
                _gameView->updateView(_gameModel);
                checkGameEnd();
            });
        return true;
    }
    return false;
}

void GameController::handleDrawCard()
{
    if (!_gameModel || !_gameView) return;
    if (_gameModel->getStackCards().empty()) return;

    CardModel* prevBottom = _gameModel->getBottomCard();
    CardModel* drawnCard = _gameModel->drawCardFromStack();
    if (!drawnCard) return;

    // 断连：DRAW操作会重置连击
    _gameModel->resetCombo();
    if (_comboCallback) _comboCallback(0);

    // 罚分判定
    if (prevBottom && hasAnyMatch()) {
        _gameModel->addScore(-2);
        CCLOG("GameController: DRAW penalty -2, total: %d", _gameModel->getScore());
        if (_scoreCallback) _scoreCallback(-2);
    }

    UndoModel* um = new UndoModel();
    um->setOperationType(OperationType::DRAW_CARD);
    if (prevBottom) um->setPreviousBottomCardId(prevBottom->getCardId());
    um->setNewBottomCardId(drawnCard->getCardId());
    _undoManager->pushUndoRecord(um);

    auto drawWorld = _gameView->getDrawAreaNode()->convertToWorldSpace(Vec2::ZERO);
    auto daNode = _gameView->getDrawAreaNode();
    auto targetDA = daNode->convertToNodeSpace(drawWorld);

    _gameView->playCardMoveAnimation(drawnCard->getCardId(), targetDA, 0.4f,
        [this, drawnCard, prevBottom]() {
            if (prevBottom) {
                auto sc = _gameModel->getStackCards();
                sc.insert(sc.begin(), prevBottom);
                _gameModel->setStackCards(sc);
            }
            drawnCard->setPosition(Vec2::ZERO);
            _gameModel->setBottomCard(drawnCard);

            if (_stackCountCallback) _stackCountCallback(_gameModel->getStackRemaining());
            _gameView->updateView(_gameModel);
        });
}

void GameController::handleUndo() {
    if (!_gameModel || !_gameView || !_undoManager->canUndo()) return;

    UndoModel* undoModel = _undoManager->popUndoRecord();
    if (!undoModel) return;

    switch (undoModel->getOperationType()) {
    case OperationType::CARD_MATCH:
    {
        CardModel* mc = _gameModel->getCardById(undoModel->getMatchedCardId());
        CardModel* pb = _gameModel->getCardById(undoModel->getPreviousBottomCardId());
        if (mc && pb) {
            mc->setPosition(undoModel->getMatchedCardPosition());
            auto pfc = _gameModel->getPlayFieldCards();
            pfc.push_back(mc);
            _gameModel->setPlayFieldCards(pfc);

            pb->setPosition(Vec2::ZERO);
            _gameModel->setBottomCard(pb);

            auto sc = _gameModel->getStackCards();
            if (!sc.empty() && sc.front()->getCardId() == pb->getCardId()) {
                sc.erase(sc.begin());
                _gameModel->setStackCards(sc);
            }
            if (_stackCountCallback) _stackCountCallback(_gameModel->getStackRemaining());
            _gameView->updateView(_gameModel);
        }
        break;
    }
    case OperationType::DRAW_CARD:
    {
        CardModel* nb = _gameModel->getCardById(undoModel->getNewBottomCardId());
        CardModel* pb = _gameModel->getCardById(undoModel->getPreviousBottomCardId());
        if (nb && pb) {
            nb->setFlipped(false);
            auto sc = _gameModel->getStackCards();
            sc.push_back(nb);
            _gameModel->setStackCards(sc);

            pb->setPosition(undoModel->getPreviousBottomPosition());
            pb->setFlipped(true);
            _gameModel->setBottomCard(pb);
            if (_stackCountCallback) _stackCountCallback(_gameModel->getStackRemaining());
            _gameView->updateView(_gameModel);
        }
        break;
    }
    default: break;
    }
    delete undoModel;
}

bool GameController::checkCardsMatch(const CardModel* c1, const CardModel* c2) const {
    if (!c1 || !c2) return false;
    return GameUtils::isCardsMatch(c1->getFaceValue(), c2->getFaceValue());
}

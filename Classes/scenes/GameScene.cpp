#include "GameScene.h"
#include "../controllers/GameController.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

GameScene* GameScene::create() {
    GameScene* pRet = new GameScene();
    if (pRet && pRet->init()) { pRet->autorelease(); return pRet; }
    delete pRet; return nullptr;
}

bool GameScene::init() {
    if (!Scene::init()) return false;

    auto bg = LayerColor::create(Color4B{10, 18, 30, 255});
    addChild(bg, -2);

    _gameController = new GameController();
    _gameController->startGame(1);

    _gameController->setScoreCallback([this](int pts) {
        showScorePopup(pts);
        updateScoreDisplay(_gameController->getGameModel()->getScore());
    });
    _gameController->setComboCallback([this](int c) { updateComboDisplay(c); });
    _gameController->setGameEndCallback([this](bool w) { showGameEnd(w); });
    _gameController->setStackCountCallback([this](int c) { updateStackCount(c); });

    if (_gameController->getGameView()) addChild(_gameController->getGameView());

    createUI();
    updateScoreDisplay(0);
    updateStackCount(49);
    updateComboDisplay(0);

    Director::getInstance()->setDisplayStats(false);
    return true;
}

void GameScene::createUI() {
    // ===== Title bar =====
    auto titleBar = LayerColor::create(Color4B{15, 25, 45, 240}, 1080, 90);
    titleBar->setPosition(0, 1990);
    addChild(titleBar, 10);

    // Title
    auto titleLabel = Label::createWithSystemFont("CARD MATCH", "Arial Bold", 36);
    titleLabel->setPosition(Vec2(150, 2038));
    titleLabel->setTextColor(Color4B{220, 200, 120, 255});
    titleLabel->enableOutline(Color4B{0, 0, 0, 200}, 3);
    addChild(titleLabel, 11);

    // Stack count
    _stackCountLabel = Label::createWithSystemFont("49", "Arial Bold", 24);
    _stackCountLabel->setPosition(Vec2(370, 2038));
    _stackCountLabel->setTextColor(Color4B{150, 200, 255, 255});
    addChild(_stackCountLabel, 11);

    auto stackIcon = Label::createWithSystemFont("STACK:", "Arial Bold", 15);
    stackIcon->setPosition(Vec2(290, 2038));
    stackIcon->setTextColor(Color4B{130, 160, 210, 230});
    addChild(stackIcon, 11);

    // ===== UNDO + RESTART (top-left area, inside title bar) =====
    auto undoBtn = Button::create();
    undoBtn->setTitleText("UNDO");
    undoBtn->setTitleFontName("Arial Bold");
    undoBtn->setTitleFontSize(24);
    undoBtn->setTitleColor(Color3B::WHITE);
    undoBtn->loadTextureNormal("res/card_general.png");
    undoBtn->setColor(Color3B{90, 65, 30});
    undoBtn->setOpacity(230);
    undoBtn->setScale9Enabled(true);
    undoBtn->setContentSize(Size(140, 55));
    undoBtn->setCapInsets(Rect(10, 10, 80, 130));
    undoBtn->setPosition(Vec2(435, 2035));

    undoBtn->addTouchEventListener([this](Ref* s, ui::Widget::TouchEventType t) {
        auto btn = static_cast<ui::Button*>(s);
        if (t == ui::Widget::TouchEventType::BEGAN) {
            btn->setScale(0.9f); btn->setColor(Color3B{140, 90, 25});
        } else if (t == ui::Widget::TouchEventType::ENDED) {
            btn->setScale(1.0f); btn->setColor(Color3B{90, 65, 30});
            onUndoButtonClicked();
        } else if (t == ui::Widget::TouchEventType::CANCELED) {
            btn->setScale(1.0f); btn->setColor(Color3B{90, 65, 30});
        }
    });
    addChild(undoBtn, 10);

    auto restartBtn = Button::create();
    restartBtn->setTitleText("RESTART");
    restartBtn->setTitleFontName("Arial Bold");
    restartBtn->setTitleFontSize(22);
    restartBtn->setTitleColor(Color3B::WHITE);
    restartBtn->loadTextureNormal("res/card_general.png");
    restartBtn->setColor(Color3B{30, 90, 45});
    restartBtn->setOpacity(230);
    restartBtn->setScale9Enabled(true);
    restartBtn->setContentSize(Size(140, 55));
    restartBtn->setCapInsets(Rect(10, 10, 80, 130));
    restartBtn->setPosition(Vec2(590, 2035));

    restartBtn->addTouchEventListener([this](Ref* s, ui::Widget::TouchEventType t) {
        auto btn = static_cast<ui::Button*>(s);
        if (t == ui::Widget::TouchEventType::BEGAN) {
            btn->setScale(0.9f); btn->setColor(Color3B{20, 110, 40});
        } else if (t == ui::Widget::TouchEventType::ENDED) {
            btn->setScale(1.0f); btn->setColor(Color3B{30, 90, 45});
            onRestartButtonClicked();
        } else if (t == ui::Widget::TouchEventType::CANCELED) {
            btn->setScale(1.0f); btn->setColor(Color3B{30, 90, 45});
        }
    });
    addChild(restartBtn, 10);

    // ===== Score board =====
    auto scorePanel = DrawNode::create();
    Vec2 sp[4] = {Vec2(750,1990), Vec2(1080,1990), Vec2(1080,2080), Vec2(750,2080)};
    scorePanel->drawPolygon(sp, 4, Color4F{0.10f,0.16f,0.28f,1.0f}, 2.5f, Color4F{0.52f,0.46f,0.20f,1.0f});
    addChild(scorePanel, 10);

    auto scoreTitle = Label::createWithSystemFont("SCORE", "Arial Bold", 20);
    scoreTitle->setPosition(Vec2(915, 2058));
    scoreTitle->setTextColor(Color4B{190, 170, 90, 255});
    addChild(scoreTitle, 11);

    _scoreLabel = Label::createWithSystemFont("0", "Arial Bold", 60);
    _scoreLabel->setPosition(Vec2(915, 2018));
    _scoreLabel->setTextColor(Color4B{255, 225, 60, 255});
    _scoreLabel->enableOutline(Color4B{0, 0, 0, 220}, 5);
    addChild(_scoreLabel, 11);

    // Combo display
    _comboLabel = Label::createWithSystemFont("", "Arial Bold", 28);
    _comboLabel->setPosition(Vec2(525, 2064));
    _comboLabel->setTextColor(Color4B{255, 140, 0, 255});
    _comboLabel->setVisible(false);
    addChild(_comboLabel, 11);

    // ===== Bottom tip bar =====
    auto bottomBar = LayerColor::create(Color4B{15, 25, 45, 200}, 1080, 24);
    bottomBar->setOpacity(180);
    addChild(bottomBar, 10);

    auto tipLabel = Label::createWithSystemFont("Match cards  |  Tap DRAW  |  UNDO  |  RESTART", "Arial", 12);
    tipLabel->setPosition(Vec2(540, 12));
    tipLabel->setTextColor(Color4B{150, 150, 170, 220});
    addChild(tipLabel, 11);
}

void GameScene::onUndoButtonClicked() {
    if (_gameController) {
        _gameController->handleUndo();
        if (_gameController->getGameModel()) {
            updateScoreDisplay(_gameController->getGameModel()->getScore());
            _comboLabel->setVisible(false);
        }
    }
}

void GameScene::onRestartButtonClicked() {
    if (_gameController) {
        _gameController->restartGame();
        updateScoreDisplay(0);
        updateComboDisplay(0);
        updateStackCount(_gameController->getGameModel() ? _gameController->getGameModel()->getStackRemaining() : 0);
    }
}

void GameScene::updateScoreDisplay(int score) {
    if (_scoreLabel) {
        char buf[16]; sprintf(buf, "%d", score);
        _scoreLabel->setString(buf);
        _scoreLabel->setTextColor(score < 0 ? Color4B{255, 80, 60, 255} : Color4B{255, 225, 60, 255});
        _scoreLabel->runAction(Sequence::create(ScaleTo::create(0.08f, 1.3f), ScaleTo::create(0.12f, 1.0f), nullptr));
    }
}

void GameScene::updateComboDisplay(int combo) {
    if (!_comboLabel) return;
    if (combo >= 2) {
        char buf[32]; sprintf(buf, "COMBO x%d !", combo);
        _comboLabel->setString(buf);
        _comboLabel->setVisible(true);
        _comboLabel->setTextColor(combo >= 4 ? Color4B{255, 50, 50, 255} : Color4B{255, 140, 30, 255});
        _comboLabel->runAction(Sequence::create(ScaleTo::create(0.1f, 1.4f), ScaleTo::create(0.15f, 1.0f), nullptr));
    } else {
        _comboLabel->setVisible(false);
    }
}

void GameScene::updateStackCount(int count) {
    if (_stackCountLabel) {
        char buf[8]; sprintf(buf, "%d", count);
        _stackCountLabel->setString(buf);
        _stackCountLabel->setTextColor(count <= 5 ? Color4B{255, 180, 80, 255} : Color4B{150, 200, 255, 255});
    }
}

void GameScene::showScorePopup(int points) {
    char txt[8];
    if (points >= 0) sprintf(txt, "+%d", points);
    else sprintf(txt, "%d", points);

    auto popup = Label::createWithSystemFont(txt, "Arial Bold", 48);
    popup->setPosition(Vec2(915, 2100));
    popup->setTextColor(points >= 0 ? Color4B{255, 220, 50, 255} : Color4B{255, 80, 60, 255});
    popup->enableOutline(Color4B{0, 0, 0, 220}, 3);
    addChild(popup, 20);

    auto moveUp = MoveBy::create(0.7f, Vec2(0, 60));
    popup->runAction(Sequence::create(EaseOut::create(moveUp->clone(), 2.0f), RemoveSelf::create(), nullptr));
    popup->runAction(Sequence::create(DelayTime::create(0.15f), FadeOut::create(0.6f), nullptr));
}

void GameScene::showGameEnd(bool won) {
    auto mask = LayerColor::create(Color4B{0, 0, 0, 180}, 1080, 2080);
    mask->setTag(999);
    addChild(mask, 50);

    auto panel = DrawNode::create();
    Vec2 pp[4] = {Vec2(240,850), Vec2(840,850), Vec2(840,1230), Vec2(240,1230)};
    panel->drawPolygon(pp, 4, Color4F{0.08f,0.14f,0.26f,1.0f}, 3.0f, Color4F{0.60f,0.52f,0.20f,1.0f});
    mask->addChild(panel);

    auto title = Label::createWithSystemFont(won ? "YOU WIN!" : "NO MOVES", "Arial Bold", 56);
    title->setPosition(Vec2(540, 1140));
    title->setTextColor(won ? Color4B{255, 220, 60, 255} : Color4B{255, 100, 80, 255});
    title->enableOutline(Color4B{0, 0, 0, 200}, 4);
    mask->addChild(title);

    char buf[32];
    sprintf(buf, "Final Score: %d", _gameController->getGameModel()->getScore());
    auto finalScore = Label::createWithSystemFont(buf, "Arial Bold", 34);
    finalScore->setPosition(Vec2(540, 1060));
    finalScore->setTextColor(Color4B::WHITE);
    mask->addChild(finalScore);

    auto retryBtn = Button::create();
    retryBtn->setTitleText("PLAY AGAIN");
    retryBtn->setTitleFontName("Arial Bold");
    retryBtn->setTitleFontSize(28);
    retryBtn->setTitleColor(Color3B::WHITE);
    retryBtn->loadTextureNormal("res/card_general.png");
    retryBtn->setColor(Color3B{40, 100, 50});
    retryBtn->setScale9Enabled(true);
    retryBtn->setContentSize(Size(240, 70));
    retryBtn->setCapInsets(Rect(10, 10, 80, 130));
    retryBtn->setPosition(Vec2(540, 950));

    retryBtn->addTouchEventListener([this, mask](Ref*, ui::Widget::TouchEventType t) {
        if (t == ui::Widget::TouchEventType::ENDED) {
            mask->removeFromParent();
            onRestartButtonClicked();
        }
    });
    mask->addChild(retryBtn);
}

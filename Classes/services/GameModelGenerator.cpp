#include "GameModelGenerator.h"
#include "../utils/GameUtils.h"

USING_NS_CC;

GameModelGenerator::GameModelGenerator() {}

GameModel* GameModelGenerator::generateGameModel(LevelConfig* levelConfig)
{
    if (!levelConfig) return nullptr;
    GameModel* gameModel = new GameModel();

    std::vector<CardModel*> playFieldCards;
    for (auto& cfg : levelConfig->getPlayFieldCards()) {
        int cardId = GameUtils::generateCardId();
        CardModel* cm = createCardModel(cfg, cardId);
        if (cm) playFieldCards.push_back(cm);
    }
    gameModel->setPlayFieldCards(playFieldCards);

    std::vector<CardModel*> stackCards;
    for (auto& cfg : levelConfig->getStackCards()) {
        int cardId = GameUtils::generateCardId();
        CardModel* cm = createCardModel(cfg, cardId);
        if (cm) stackCards.push_back(cm);
    }
    gameModel->setStackCards(stackCards);

    if (!stackCards.empty()) {
        auto nsc = stackCards;
        CardModel* ib = nsc.back(); nsc.pop_back();
        gameModel->setBottomCard(ib);
        gameModel->setStackCards(nsc);
    }
    return gameModel;
}

GameModel* GameModelGenerator::generateRandomGameModel()
{
    GameModel* gameModel = new GameModel();
    const int TOTAL_CARDS = 56;
    const int PLAYFIELD_COUNT = 6;

    // 1. 生成56张随机牌
    std::vector<CardModel*> allCards;
    for (int i = 0; i < TOTAL_CARDS; i++) {
        CardSuitType suit = static_cast<CardSuitType>(GameUtils::getRandomInt(0, 3));
        CardFaceType face = static_cast<CardFaceType>(GameUtils::getRandomInt(0, 12));
        int cardId = GameUtils::generateCardId();
        CardModel* card = new CardModel();
        card->setCardId(cardId);
        card->setSuit(suit);
        card->setFace(face);
        card->setPosition(Vec2::ZERO);
        card->setFlipped(false);
        allCards.push_back(card);
    }

    // 2. 洗牌
    shuffleVector(allCards);

    // 3. 桌面牌：3x2 居中紧凑
    float colPositions[2] = { 380.0f, 700.0f };
    float rowPositions[3] = { 1050.0f, 720.0f, 390.0f };

    std::vector<CardModel*> playFieldCards;
    for (int i = 0; i < PLAYFIELD_COUNT && !allCards.empty(); i++) {
        CardModel* card = allCards.back(); allCards.pop_back();
        int col = i % 2;
        int row = i / 2;
        card->setPosition(Vec2(colPositions[col], rowPositions[row]));
        card->setFlipped(true);
        playFieldCards.push_back(card);
    }
    gameModel->setPlayFieldCards(playFieldCards);

    // 4. 底牌
    if (!allCards.empty()) {
        CardModel* btm = allCards.back(); allCards.pop_back();
        btm->setPosition(Vec2::ZERO);
        btm->setFlipped(true);
        gameModel->setBottomCard(btm);
    }

    // 5. 牌堆
    gameModel->setStackCards(allCards);
    return gameModel;
}

CardModel* GameModelGenerator::createCardModel(const LevelConfig::CardConfig& cfg, int cardId)
{
    CardModel* cm = new CardModel();
    cm->setCardId(cardId);
    cm->setFace(cfg.face);
    cm->setSuit(cfg.suit);
    cm->setPosition(cfg.position);
    cm->setFlipped(true);
    return cm;
}

CardModel* GameModelGenerator::createRandomCardModel(int cardId, const cocos2d::Vec2& pos)
{
    CardModel* cm = new CardModel();
    cm->setCardId(cardId);
    cm->setSuit(static_cast<CardSuitType>(GameUtils::getRandomInt(0, 3)));
    cm->setFace(static_cast<CardFaceType>(GameUtils::getRandomInt(0, 12)));
    cm->setPosition(pos);
    cm->setFlipped(true);
    return cm;
}

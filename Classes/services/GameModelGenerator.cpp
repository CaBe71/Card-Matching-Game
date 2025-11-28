#include "GameModelGenerator.h"
#include "CardService.h"

USING_NS_CC;

GameModel* GameModelGenerator::generateFromLevelConfig(const LevelConfig* levelConfig)
{
    if (!levelConfig) {
        return nullptr;
    }

    GameModel* gameModel = new GameModel();

    // 生成主牌区的卡牌
    std::vector<CardModel*> playFieldCards;
    const std::vector<CardConfig>& playFieldConfigs = levelConfig->playfieldCards;
    for (const auto& cardConfig : playFieldConfigs) {
        int cardId = CardService::generateCardId();
        CardModel* cardModel = createCardModel(cardConfig, cardId);
        if (cardModel) {
            playFieldCards.push_back(cardModel);
        }
    }
    gameModel->setPlayFieldCards(playFieldCards);

    // 生成备用牌堆
    std::vector<CardModel*> reserveCards;
    const std::vector<CardConfig>& reserveConfigs = levelConfig->reserveCards;
    for (const auto& cardConfig : reserveConfigs) {
        int cardId = CardService::generateCardId();
        CardModel* cardModel = createCardModel(cardConfig, cardId);
        if (cardModel) {
            reserveCards.push_back(cardModel);
        }
    }
    gameModel->setReserveCards(reserveCards);

    // 生成手牌区卡牌
    std::vector<CardModel*> stackCards;
    const std::vector<CardConfig>& stackConfigs = levelConfig->stackCards;
    for (const auto& cardConfig : stackConfigs) {
        int cardId = CardService::generateCardId();
        CardModel* cardModel = createCardModel(cardConfig, cardId);
        if (cardModel) {
            stackCards.push_back(cardModel);
        }
    }
    gameModel->setStackCards(stackCards);

    // 设置初始底牌（从备用牌堆抽取第一张）
    if (!reserveCards.empty()) {
        // 从备用牌堆抽取底牌
        std::vector<CardModel*> newReserveCards = reserveCards;
        CardModel* initialBottomCard = newReserveCards.back();
        newReserveCards.pop_back(); // 从备用牌堆移除

        // 设置底牌和更新备用牌堆
        initialBottomCard->setPosition(Vec2(540, 290));
        initialBottomCard->setTopCard(true);
        initialBottomCard->setInPlayfield(false);
        gameModel->setBottomCard(initialBottomCard);
        gameModel->setReserveCards(newReserveCards);

        CCLOG("? Initial bottom card set: ID=%d, Face=%d",
            initialBottomCard->getCardId(), initialBottomCard->getFace());
    }
    else {
        CCLOG("? WARNING: No reserve cards for initial bottom card");
        // 如果没有备用牌，从手牌区抽取
        if (!stackCards.empty()) {
            std::vector<CardModel*> newStackCards = stackCards;
            CardModel* initialBottomCard = newStackCards.back();
            newStackCards.pop_back();

            initialBottomCard->setPosition(Vec2(540, 290));
            initialBottomCard->setTopCard(true);
            initialBottomCard->setInPlayfield(false);
            gameModel->setBottomCard(initialBottomCard);
            gameModel->setStackCards(newStackCards);

            CCLOG("? Initial bottom card from stack: ID=%d, Face=%d",
                initialBottomCard->getCardId(), initialBottomCard->getFace());
        }
    }

    CCLOG("Game model generated: %zu playfield, %zu stack, %zu reserve",
        playFieldCards.size(), stackCards.size(), gameModel->getAllReserveCards().size());

    return gameModel;
}

CardModel* GameModelGenerator::createCardModel(const CardConfig& cardConfig, int cardId)
{
    CardModel* cardModel = new CardModel();
    cardModel->setCardId(cardId);
    cardModel->setFace(cardConfig.cardFace);
    cardModel->setSuit(cardConfig.cardSuit);
    cardModel->setPosition(cardConfig.position);
    cardModel->setInPlayfield(true);

    return cardModel;
}
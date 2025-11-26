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

    // 设置初始底牌（从备用牌堆抽取第一张）
    if (!reserveCards.empty()) {
        // 从备用牌堆抽取底牌
        CardModel* initialBottomCard = gameModel->drawFromReserve();
        if (initialBottomCard) {
            // 正确设置底牌位置和状态
            initialBottomCard->setPosition(Vec2(540, 290)); // 底牌位置
            initialBottomCard->setTopCard(true);
            initialBottomCard->setInPlayfield(false);
            gameModel->setBottomCard(initialBottomCard);

            CCLOG("? Initial bottom card set: ID=%d, Face=%d",
                initialBottomCard->getCardId(), initialBottomCard->getFace());
        }
    }
    else {
        CCLOG("WARNING: No reserve cards for initial bottom card");
    }

    CCLOG("Game model generated: %zu playfield, %zu reserve",
        playFieldCards.size(), gameModel->getAllReserveCards().size());

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
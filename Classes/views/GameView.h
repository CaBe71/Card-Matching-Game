#ifndef GAME_VIEW_H
#define GAME_VIEW_H

#include "cocos2d.h"
#include "CardView.h"
#include "ui/CocosGUI.h" 
#include <unordered_map>

USING_NS_CC;

class GameView : public Layer
{
public:
    CREATE_FUNC(GameView);

    virtual bool init() override;

    void initializeWithCards(const std::vector<CardModel*>& playfieldCards,
        const std::vector<CardModel*>& stackCards,
        CardModel* bottomCard,
        const std::vector<CardModel*>& reserveCards = std::vector<CardModel*>());

    void playCardMatchAnimation(int movingCardId, const Vec2& targetPosition);
    void playUndoAnimation(int cardId, const Vec2& originalPosition);

    void setCardClickCallback(std::function<void(int)> callback) {
        _cardClickCallback = callback;
    }

    void setUndoCallback(std::function<void()> callback) {
        _undoCallback = callback;
    }

    void createUndoButton();

private:
    void createPlayfieldArea();
    void createStackArea();
    void createBottomCardArea();
    void createReserveArea();

    void createReserveDeckDisplay(const std::vector<CardModel*>& reserveCards);

    std::string getFaceText(CardFaceType face);
    std::string getSuitText(CardSuitType suit);
private:
    std::unordered_map<int, CardView*> _cardViews;
    std::function<void(int)> _cardClickCallback;
    std::function<void()> _undoCallback;
    Layer* _playfieldLayer;
    Layer* _stackLayer;
    Layer* _bottomCardLayer;
    CardView* _bottomCardView;
};

#endif // GAME_VIEW_H
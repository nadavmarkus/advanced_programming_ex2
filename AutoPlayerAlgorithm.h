#ifndef __AUTO_PLAYER_ALGORITHM_H_
#define __AUTO_PLAYER_ALGORITHM_H_

#include "PlayerAlgorithm.h"
#include "Board.h"
#include "FightInfo.h"
#include "Move.h"
#include "ConcreteBoard.h"
#include "Globals.h"
#include "ConcretePiecePosition.h"
#include "PiecePosition.h"
#include "ConcretePoint.h"
#include "ConcreteMove.h"
#include "GameUtils.h"

#include <memory>
#include <vector>
#include <map>
#include <ctime>
#include <cstdlib>
#include <set>
#include <random>
#include <assert.h>
#include <stdlib.h>

using piece_set_iterator = std::set<ConcretePoint>::iterator;

class AutoPlayerAlgorithm : public PlayerAlgorithm
{
private:
    ConcreteBoard my_board_view;
    int my_player_number;
    int other_player;
    std::vector<unique_ptr<PiecePosition>> *vector_to_fill;
    //TODO: Remove these from being instance members if I don't intend to move the joker.
    bool flag_up;
    bool flag_left;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> bool_generator;
    std::uniform_int_distribution<> x_generator;
    std::uniform_int_distribution<> y_generator;
    std::set<ConcretePoint> possible_opponent_flag_locations;
    
    /*
     * We can invalidate opponent's move and not update the board
     * if we won/ there was a tie. This gets updated the notification on fights.
     */
    bool invalidate_move;
    
    void fillVectorAndUpdateBoard(int x, int y, char type, char joker_type='#')
    {
        assert(nullptr != vector_to_fill);
        ConcretePiecePosition position(my_player_number, x, y, type, joker_type);
        vector_to_fill->push_back(std::make_unique<ConcretePiecePosition>(position));
        my_board_view.addPosition(position);
    }
    
    void placeOnePieceAtRandomPosition(char type)
    {
        for (;;) {
            int x = x_generator(gen);
            int y = y_generator(gen);
            
            assert(1 <= x && x <= static_cast<int>(Globals::M));
            assert(1 <= y && y <= static_cast<int>(Globals::N));
            
            /* We count on the fact that eventually we will hit an empty spot. */
            if (my_player_number == my_board_view.getPlayerAt(x , y)) {
                continue;
            }
            
            /* All right, we are good to go. */
            fillVectorAndUpdateBoard(x, y, type);
            break;
        }
    }
    
    void placePiecesAtRandomPosition(char type)
    {
        for (size_t i = 0; i < Globals::ALLOWED_PIECES_COUNT[type]; ++i) {
            placeOnePieceAtRandomPosition(type);
        }
    }
    
    void placeMovablePieces()
    {
        placePiecesAtRandomPosition('P');
        placePiecesAtRandomPosition('R');
        placePiecesAtRandomPosition('S');
    }
    
    /*
     * Updates our view with known other player unit types.
     * Note that we don't need to handle ties, since in case of a tie
     * a 0 will be reported as the player number, and the initialization loop
     * would have invalidated the position.
     */
    void updateWithFightResult(const FightInfo &info)
    {
        if (other_player == info.getWinner()) {
            const Point &where = info.getPosition();
            const ConcretePiecePosition pos(other_player, where, info.getPiece(other_player));
            my_board_view.addPosition(pos);
            
            /* This also means that the position doesn't contain a flag. */
            possible_opponent_flag_locations.erase(where);
        } else if(my_player_number == info.getWinner()) {
            
            
        } else {
            /* Should not happen. */
            assert(false);
        }
    }
    
    bool matchingPiece(int x, int y, char type, int player) const
    {
        const ConcretePiecePosition &pos = my_board_view.getPiece(x, y);
        return (type == pos.effectivePieceType() && player == pos.getPlayer());
    }
    
    bool hasAdjacentPieceOfType(int x,
                                int y,
                                char type,
                                int player,
                                int &result_x,
                                int &result_y) const
    {
        if ( x < static_cast<int>(Globals::M)) {
            if (y < static_cast<int>(Globals::N)) {
                if (matchingPiece(x + 1, y + 1, type, player)) {
                    result_x = x + 1;
                    result_y = y + 1;
                    return true;
                }
            }
            
            if (y > 1) {
                if (matchingPiece(x + 1, y - 1, type, player)) {
                    result_x = x + 1;
                    result_y = y - 1;
                    return true;
                }
            }
        }
        
        if (x > 1) {
            if (y < static_cast<int>(Globals::N)) {
                if (matchingPiece(x - 1, y + 1, type, player)) {
                    result_x = x - 1;
                    result_y = y + 1;
                    return true;
                }
            }
            
            if (y > 1) {
                if (matchingPiece(x - 1, y - 1, type, player)) {
                    result_x = x - 1;
                    result_y = y - 1;
                    return true;
                }
            }
        }
        
        return false;
    }
    
    unique_ptr<Move> attemptToEatOpponentPiece() const
    {
        for (size_t y = 1; y <= static_cast<int>(Globals::N); ++y) {
            for (size_t x = 1; x <= static_cast<int>(Globals::M); ++x) {
                const ConcretePiecePosition &pos = my_board_view.getPiece(x, y);
                
                if (other_player == pos.getPlayer() &&
                    '#' != pos.effectivePieceType()) {
                    char opponent_type = pos.effectivePieceType();
                    assert('R' == opponent_type || 'S' == opponent_type || 'P' == opponent_type);
                    
                    char stronger_piece = GameUtils::getStrongerPiece(opponent_type);
                    
                    int x_from;
                    int y_from;
                    if (hasAdjacentPieceOfType(x, y, stronger_piece, my_player_number, x_from, y_from)) {
                        return std::make_unique<ConcreteMove>(x_from, y_from, x, y);
                    }
                }
            }
        }
        
        return nullptr;
    }
    
public:
    AutoPlayerAlgorithm() : my_board_view(),
                            my_player_number(0),
                            other_player(0),
                            vector_to_fill(nullptr),
                            flag_up(false),
                            flag_left(false),
                            rd(),
                            gen(rd),
                            bool_generator(0, 1),
                            x_generator(1, Globals::M),
                            y_generator(1, Globals::N),
                            possible_opponent_flag_locations(),
                            invalidate_move(false) {}

    /* Note: This algorithm assumes that there is a single flag. */
    virtual void getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>> &vectorToFill) override
    {
        vector_to_fill = &vectorToFill;
        my_player_number = player;
        other_player = (2 == my_player_number) ? 1: 2;
        
        /*
         * We will position our flag in one of the corners, than start surrounding it
         * with guarding bombs and pieces.
         */
        flag_up = static_cast<bool>(bool_generator(gen));
        flag_left = static_cast<bool>(bool_generator(gen));
        
        int x, y;
        
        x = flag_left ? 1 : Globals::M;
        y = flag_up ? 1 : Globals::N;
        
        fillVectorAndUpdateBoard(x, y, 'F');
        
        /* Surround it with our two available bombs. */
        int same_row_bomb_x = (flag_left) ? 2 : Globals::M - 1;
        fillVectorAndUpdateBoard(same_row_bomb_x, y, 'B');
        
        int same_column_bomb_y = (flag_up) ? 2 : Globals::N - 1;
        fillVectorAndUpdateBoard(x, same_column_bomb_y, 'B');
        
        /* We will keep our jokers nearby as a second line of bombs. */
        int same_row_joker_x = (flag_left) ? 3 : Globals::M - 2;
        fillVectorAndUpdateBoard(same_row_joker_x, y, 'J', 'B');
        
        int same_column_joker_y = (flag_up) ? 3 : Globals::N - 2;
        fillVectorAndUpdateBoard(x, same_column_joker_y, 'J', 'B');
        
        placeMovablePieces();
        
        /* We don't copy the vector, so get rid of the pointer. */
        vector_to_fill = nullptr;
    }

    virtual void notifyOnInitialBoard(const Board& b, const std::vector<unique_ptr<FightInfo>>& fights) override
    {
        for (int y = 1; y <= static_cast<int>(Globals::N); ++y) {
            for (int x = 1; x <= static_cast<int>(Globals::M); ++x) {
                const ConcretePoint point(x, y);
                int player = b.getPlayer(point);
                
                if (0 == player) {
                    my_board_view.invalidatePosition(point);
                    
                } else if (player == my_player_number) {
                    /* Nothing to do in this case actually.. */
                
                } else if (player == other_player) {
                    /* We don't know yet the type of the opponent's piece. */
                    const ConcretePiecePosition pos(player, point, '#', '#');
                    my_board_view.addPosition(pos);
                    possible_opponent_flag_locations.insert(pos.getPosition());
                } else {
                    /* Should not happen. */
                    assert(false);
                }
            }
        }
        
        for (auto const &info: fights) {
            updateWithFightResult(*info);
        }
    }
    
    virtual void notifyOnOpponentMove(const Move& move) override
    {
        const Point &from = move.getFrom();
        const Point &to = move.getTo();
    
        /* A flag can't move. */
        if (possible_opponent_flag_locations.end() != possible_opponent_flag_locations.find(from)) {
            possible_opponent_flag_locations.erase(from);
        }
    
        if (!invalidate_move) {
            my_board_view.movePiece(from, to);
        }
        
        /* Reset invalidation state for next move. */
        invalidate_move = false;
    }
    
    virtual void notifyFightResult(const FightInfo &fightInfo) override
    {
        updateWithFightResult(fightInfo);
        
        /* In this case the opponent's piece is destroyed, and when we will get a notification the move is invalid. */
        //TODO: Invalidate only on fights that correlate to moves of an opponent.
        if (0 == fightInfo.getWinner() || my_player_number == fightInfo.getWinner()) {
            invalidate_move = true;
        }
    }
    
    /*
     * We try the following steps, in order:
     * - If we can capture an opponent's piece (no suicide), we will try to.
     * - If we have a weaker piece near an opponent's piece, we try to run
     * - Otherwise, we attempt to search the opponent's flag.
     */
    virtual unique_ptr<Move> getMove() override
    {
        //TODO: Implement me.
        return nullptr;
    }
    
    virtual unique_ptr<JokerChange> getJokerChange() override
    {
        /* We just want to keep our jokers as bombs, no need to change them. */
        return nullptr;
    }
};

#endif
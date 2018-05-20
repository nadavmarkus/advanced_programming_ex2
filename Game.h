#ifndef __GAME_H_
#define __GAME_H_

#include "ConcreteBoard.h"
#include "PlayerAlgorithm.h"
#include "FilePlayerAlgorithm.h"
#include "PiecePosition.h"
#include "Globals.h"
#include "GameUtils.h"
#include "BaseError.h"
#include "PositionError.h"
#include "Board.h"
#include "ConcreteBoard.h"
#include "FightInfo.h"

#include <vector>
#include <memory>
#include <set>
#include <map>
#include <assert.h>

class Game
{
private:
    bool player1_lost, player2_lost;
    std::string player1_loss_reason;
    std::string player2_loss_reason;
    std::vector<std::unique_ptr<PiecePosition>> player1_positions;
    std::vector<std::unique_ptr<PiecePosition>> player2_positions;
    PlayerAlgorithm *player1;
    PlayerAlgorithm *player2;
    std::unique_ptr<ConcreteBoard> board;
    
    void verifyJokerPositioning(int player, const PiecePosition &position) const
    {
        std::stringstream error_message;
        char masquerade_type = position.getJokerRep();
        char piece_type = position.getPiece();
        
        if ('F' != piece_type && '#' != masquerade_type) {
            error_message << "Player " << player << " attempted to supply masquerade type for non joker";
            throw PositionError(error_message.str());
        }
        
        if (!GameUtils::isValidJokerMasqueradeType(masquerade_type)) {
            error_message << "Player " << player << " joker attempted to be invalid piece";
            throw PositionError(error_message.str());
        }
    }
    
    void verifyPlayerPosition(int player,
                              const std::vector<std::unique_ptr<PiecePosition>> &positions) const
    {
        std::set<std::pair<unsigned int, unsigned int>> used_coords;
        std::map<char, unsigned int> piece_counters;
        std::pair<unsigned int, unsigned int> cur_coord;
        unsigned int x, y;
        char type;
        std::stringstream error_message;
        
        for (auto const &position: positions) {
            const Point &piece_point = position->getPosition();
            x = piece_point.getX();
            y = piece_point.getY();
            
            if ((x > Globals::M) || (y > Globals::N) || (1 > x) || (1 > y)) {
                error_message << "Player " << player << " bad piece position";
                throw PositionError(error_message.str());
            }
            
            cur_coord = std::make_pair(x, y);
            
            if (used_coords.count(cur_coord)) {
                error_message << "Player " << player << " two overlapping pieces at " << x + 1 << "," << y + 1;
                throw PositionError(error_message.str());
            }
            
            used_coords.insert(cur_coord);
            
            type = position->getPiece();
            
            if (!GameUtils::isValidType(type)) {
                error_message << "Player " << player << " bad piece type";
                throw PositionError(error_message.str());
            }
            
            piece_counters[type]++;
            
            verifyJokerPositioning(player, *position);
            
            if (piece_counters[type] > Globals::ALLOWED_PIECES_COUNT[type]) {
                error_message << "Player " << player << " has too many pieces of type " << type;
                throw PositionError(error_message.str());
            }
        }
        
        /* Verify flag count. */
        if (piece_counters['F'] != Globals::ALLOWED_PIECES_COUNT['F']) {
            error_message << "Player " << player << " invalid flag count";
            throw PositionError(error_message.str());
        }
    }
    
    int calculateWinner(ConcretePiecePosition &player1_piece,
                        ConcretePiecePosition &player2_piece) const
    {
        char piece1_type = player1_piece.getPiece();
        char piece2_type = player2_piece.getPiece();
        
        if ('J' == piece1_type) piece1_type = player1_piece.getJokerRep();
        if ('J' == piece2_type) piece2_type = player2_piece.getJokerRep();
        
        /* Both pieces are destroyed in this case. */
        if ('B' == piece1_type || 'B' == piece2_type || piece1_type == piece2_type) {
            return 0;
        }
        
        if (('F' == piece1_type) ||
            ('S' == piece1_type && 'R' == piece2_type) ||
            ('P' == piece1_type && 'S' == piece2_type) ||
            ('R' == piece1_type && 'P' == piece2_type)) {
            return 2;
        }
        
        if (('F' == piece2_type) ||
            ('S' == piece2_type && 'R' == piece1_type) ||
            ('P' == piece2_type && 'S' == piece1_type) ||
            ('R' == piece2_type && 'P' == piece1_type)) {
            return 1;
        }
        
        /* Should not happen. */
        assert(false);
    }
    
    /*
     * This method is called to resolve the initial conflicts between the players
     * after the initial placement of pieces.
     */
    void doInitialMoves()
    {
         std::map<std::pair<unsigned int, unsigned int>, ConcretePiecePosition> point_to_piece_position;
         std::pair<unsigned int, unsigned int> cur_coord;
         std::vector<unique_ptr<
         
         /* After iterating only on one player's positions, no possible conflict is possible. */
         for (auto const &position: player1_positions) {
            cur_coord = std::make_pair(position->getPosition().getX(), position->getPosition().getY());
            point_to_piece_position[cur_coord] = ConcretePiecePosition(1, *position);
         }
         
         /* When we go over the second one, we try to resolve possible conflicts via fights. */
         for (auto const &position: player2_positions) {
            cur_coord = std::make_pair(position->getPosition().getX(), position->getPosition().getY());
            
            if (point_to_piece_position.count(cur_coord)) {
                ConcretePiecePosition pos(2, *position);
                int conflict_result = calculateWinner(point_to_piece_position[cur_coord], pos);
                
                switch(conflict_result) {
                    case 2:
                        point_to_piece_position[cur_coord] = pos;
                        break;
                        
                    case 1:
                        /* Nothing to do, winner is player 1 and he is already there. */
                        break;
                        
                    case 0:
                        /* Both players lost. */
                        point_to_piece_position.erase(cur_coord);
                        break;
                }
                
            } else {
                point_to_piece_position[cur_coord] = ConcretePiecePosition(2, *position);
            }
         }
    }
    
    /*
     * This method actually runs the players' supplied moves, one by one.
     * It starts by 
     */
    void doMoves()
    {
        doInitialMoves();
        
        for(;;) {
            
        }
    }

public:
    //TODO: Complete constructor
    void run()
    {
        //TODO: Instantiate according to command line.
        FilePlayerAlgorithm player1_algorithm, player2_algorithm;
        player1 = &player1_algorithm;
        player2 = &player2_algorithm;
        
        player1->getInitialPositions(1, player1_positions);
        player2->getInitialPositions(2, player2_positions);
        
        try {
            verifyPlayerPosition(1, player1_positions);
        } catch (const BaseError &error) {
            player1_loss_reason = error.getMessage();
            player1_lost = true;
        }
        
        try {
            verifyPlayerPosition(2, player2_positions);
        } catch (const BaseError &error) {
            player2_loss_reason = error.getMessage();
            player2_lost = true;
        }
        
        //TODO: Handle case one of the guys lost
    }
};

#endif
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

#include <vector>
#include <memory>
#include <set>

class Game
{
private:
    ConcreteBoard board;
    bool player1_lost, player2_lost;
    BaseError player1_error, player2_error;
    
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

public:
    //TODO: Complete constructor
    void run()
    {
        //TODO: Instantiate according to command line.
        FilePlayerAlgorithm player1_algorithm, player2_algorithm;
        PlayerAlgorithm &player1 = player1_algorithm;
        PlayerAlgorithm &player2 = player2_algorithm;
        
        std::vector<std::unique_ptr<PiecePosition>> player1_positions, player2_positions;
        
        player1.getInitialPositions(1, player1_positions);
        player2.getInitialPositions(2, player2_positions);
        
        try {
            verifyPlayerPosition(1, player1_positions);
        } catch (const BaseError &error) {
            player1_error = error;
            player1_lost = true;
        }
        
        try {
            verifyPlayerPosition(2, player2_positions);
        } catch (const BaseError &error) {
            player2_error = error;
            player2_lost = true;
        }
    }
};

#endif
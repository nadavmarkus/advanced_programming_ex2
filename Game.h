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
#include "FightInfo.h"
#include "ConcreteFightInfo.h"
#include "JokerChange.h"
#include "Move.h"
#include "BadMoveError.h"

#include <vector>
#include <memory>
#include <set>
#include <map>
#include <iostream>
#include <assert.h>
#include <stdlib.h>

class Game
{
private:
    std::string player1_loss_reason;
    std::string player2_loss_reason;
    std::vector<std::unique_ptr<PiecePosition>> player1_positions;
    std::vector<std::unique_ptr<PiecePosition>> player2_positions;
    PlayerAlgorithm *player1;
    PlayerAlgorithm *player2;
    std::unique_ptr<ConcreteBoard> board;
    size_t player1_flags;
    size_t player2_flags;
    
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
    
    /* Note: This function also updates the amount of flags for each player. */
    int calculateWinner(const ConcretePiecePosition &player1_piece,
                        const ConcretePiecePosition &player2_piece)
    {
        char piece1_type = player1_piece.getPiece();
        char piece2_type = player2_piece.getPiece();
        
        if ('J' == piece1_type) piece1_type = player1_piece.getJokerRep();
        if ('J' == piece2_type) piece2_type = player2_piece.getJokerRep();
        
        /* Both pieces are destroyed in this case. */
        if ('B' == piece1_type || 'B' == piece2_type || piece1_type == piece2_type) {
            if ('F' == piece1_type) player1_flags--;
            if ('F' == piece2_type) player2_flags--;
            return 0;
        }
        
        /* Player 2 won */
        if (('F' == piece1_type) ||
            ('S' == piece1_type && 'R' == piece2_type) ||
            ('P' == piece1_type && 'S' == piece2_type) ||
            ('R' == piece1_type && 'P' == piece2_type)) {
            if ('F' == piece1_type) player1_flags--;
            return 2;
        }
        
        /* Player 1 won */
        if (('F' == piece2_type) ||
            ('S' == piece2_type && 'R' == piece1_type) ||
            ('P' == piece2_type && 'S' == piece1_type) ||
            ('R' == piece2_type && 'P' == piece1_type)) {
            if ('F' == piece2_type) player2_flags--;
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
         std::vector<unique_ptr<FightInfo>> fights;
         
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
                
                char player1_type = point_to_piece_position[cur_coord].getPiece();
                char player2_type = pos.getPiece();
                
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
                
                fights.push_back(std::make_unique<ConcreteFightInfo>(conflict_result,
                                                                     player1_type,
                                                                     player2_type,
                                                                     position->getPosition().getX(),
                                                                     position->getPosition().getY()));
                
            } else {
                point_to_piece_position[cur_coord] = ConcretePiecePosition(2, *position);
            }
        }
        
        /* All right, we are finished. we can populate the board, and call the notify routines. */
        for (auto const &point_to_piece: point_to_piece_position) {
            board->addPosition(point_to_piece.second);
        }
        
        player1->notifyOnInitialBoard(*board, fights);
        player2->notifyOnInitialBoard(*board, fights);
    }
    
    void verifyCoordinatesInRange(const Point &point) const
    {
        //TODO: Construct useful messages here.
        if (static_cast<unsigned int>(point.getX()) > Globals::M || 0 == point.getX()) {
            throw BadMoveError(std::string("Invalid move"));
        }
        
        if (static_cast<unsigned int>(point.getY()) > Globals::N || 0 == point.getY()) {
            throw BadMoveError(std::string("Invalid move"));
        }
    }
    
    void verifyMove(int player_number, const Move &move)
    {
        //TODO: Construct useful messages here.
        const Point &from = move.getFrom();
        const Point &to = move.getTo();
        
        verifyCoordinatesInRange(from);
        verifyCoordinatesInRange(to);
        
        int from_owning_player = board->getPlayer(from);
        
        /* Make sure the player attempted to move its own piece.. */
        if (from_owning_player != player_number) {
            throw BadMoveError(std::string("Invalid move"));
        }
        
        int target_owning_player = board->getPlayer(to);
        
        /* You can't move pieces into spaces owned by yourself.. */
        if (target_owning_player == player_number) {
            throw BadMoveError(std::string("Invalid move"));
        }
        
        /* Make sure the player didn't attempt to move an unmovable piece. */
        const ConcretePiecePosition &position = board->getPiece(from);
        
        char type = position.getPiece();
        
        if ('J' == type) type = position.getJokerRep();
        
        if (!GameUtils::isMovablePiece(type)) {
            throw BadMoveError(std::string("Invalid move"));
        }
        
        /* All good! */
    }
    
    void verifyJokerChange(int player_number, const JokerChange &joker_change)
    {
        //TODO: Construct useful messages here.
        const Point& where = joker_change.getJokerChangePosition();
        verifyCoordinatesInRange(where);
        char new_joker_type = joker_change.getJokerNewRep();
        
        if (!GameUtils::isValidJokerMasqueradeType(new_joker_type)) {
            throw BadMoveError(std::string("Invalid move"));
        }
        
        int owning_player = board->getPlayer(where);
        
        if (owning_player != player_number) {
            throw BadMoveError(std::string("Invalid move"));
        }
    }
    
    void extractPieceTypes(const Point &to,
                           const Point &from,
                           char &player1_type,
                           char &player2_type) const
    {
        const ConcretePiecePosition &toPiece = board->getPiece(to);
        const ConcretePiecePosition &fromPiece = board->getPiece(from);
    
        if (1 == fromPiece.getPlayer()) {
            assert(2 == toPiece.getPlayer());
            player1_type = fromPiece.getPiece();
            player2_type = toPiece.getPiece();
            
        } else if (1 == toPiece.getPlayer()) {
            assert(2 == fromPiece.getPlayer());
            player1_type = toPiece.getPiece();
            player2_type = fromPiece.getPiece();

        } else {
            /* Should not happen. */
            assert(false);
        }
    }
    
    void invokeMove(PlayerAlgorithm *player, int player_number)
    {
        unique_ptr<Move> move = player->getMove();
        verifyMove(player_number, *move);
        
        unique_ptr<JokerChange> joker_change = player->getJokerChange();
        
        /* OK - time to apply the logic to the board. */
        int other_player = board->getPlayer(move->getTo());
        
        const Point &from = move->getFrom();
        const Point &to = move->getTo();
        
        /* This surely means that the other player is the opponent! */
        if (0 != other_player) {
            assert(other_player == 1 + (player_number % 2));
            /* This information will later be used in the fight info notification. */
            char player1_type, player2_type;
            extractPieceTypes(to, from, player1_type, player2_type);
            
            int winner = calculateWinner(board->getPiece(from), board->getPiece(to));
            
            /* Attacker won - update accordingly. */
            if (winner == player_number) {
                board->movePiece(from, to);
            
            /* Defender won - update accordingly. */
            } else if (winner == other_player) {
                board->invalidatePosition(from);
                
            /* Tie - both positions are invalidated. */
            } else if (0 == winner) {
                board->invalidatePosition(to);
                board->invalidatePosition(from);
                
            } else {
                /* Should not happen. */
                assert(false);
            }
            
            /* Notify players on result. */
            ConcreteFightInfo info(winner, player1_type, player2_type, to.getX(), to.getY());
            player1->notifyFightResult(info);
            player2->notifyFightResult(info);
            
        } else {
            board->movePiece(from, to);
        }
        
        /* And now to apply the potential joker change. */
        if (nullptr != joker_change) {
            verifyJokerChange(player_number, *joker_change);
            board->updateJokerPiece(joker_change->getJokerChangePosition(), joker_change->getJokerNewRep());
        }
        
        /* Notify the other player on the current player's move. */
        if (player1 == player) {
            player2->notifyOnOpponentMove(*move);
        } else {
            player1->notifyOnOpponentMove(*move);
        }
    }
    
    /*
     * This method actually runs the players' supplied moves, one by one.
     * It returns the winner (0 in case of a tie)
     */
    int doMoves()
    {
        doInitialMoves();
        
        for(size_t move_count = 0; move_count < 50; ++move_count) {
            /* Do we have a winner yet? */
            //TODO: add appropriate messages for winning
            if (0 == player1_flags || 0 == player2_flags) {
                if (0 == player1_flags && 0 == player2_flags) return 0;
                if (0 == player1_flags) return 2;
                return 1;
            }
            
            try {
                invokeMove(player1, 1);
            } catch (const BaseError& error) {
                //TODO: handle error
                return 2;
            }
            
            if (0 == player1_flags || 0 == player2_flags) {
                //TODO: add appropriate messages for winning
                if (0 == player1_flags && 0 == player2_flags) return 0;
                if (0 == player1_flags) return 2;
                return 1;
            }
            
            try {
                invokeMove(player1, 2);
            } catch (const BaseError& error) {
                //TODO: handle error
                return 1;
            }
        }
        
        /* We got to a tie. */
        return 0;
    }

public:
    //TODO: Complete constructor
    void run()
    {
        Globals::initGlobals();
        //TODO: Instantiate according to command line.
        FilePlayerAlgorithm player1_algorithm, player2_algorithm;
        player1 = &player1_algorithm;
        player2 = &player2_algorithm;
        
        player1->getInitialPositions(1, player1_positions);
        player2->getInitialPositions(2, player2_positions);
        
        bool player1_lost = false, player2_lost = false;
        
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
        
        if (player1_lost || player2_lost) {
            //TODO: Handle case one of the guys lost
        }
        
        player1_flags = Globals::ALLOWED_PIECES_COUNT['F'];
        player2_flags = Globals::ALLOWED_PIECES_COUNT['F'];
        
        int winner = doMoves();
        
        std::cout << "Winner is " << winner << std::endl;
    }
};

#endif
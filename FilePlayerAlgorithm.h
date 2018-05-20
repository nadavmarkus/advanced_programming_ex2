#ifndef __FILE_PLAYER_ALGORITHM_H_
#define __FILE_PLAYER_ALGORITHM_H_

#include "PlayerAlgorithm.h"
#include "PiecePosition.h"
#include "FileParsingError.h"
#include "Globals.h"
#include "ConcretePiecePosition.h"
#include "BadFilePathError.h"
#include "Move.h"
#include "ConcreteMove.h"
#include "JokerChange.h"

#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <algorithm>
#include <sstream>
#include <set>
#include <fstream>

static inline bool is_valid_type(char type)
{
    switch (type) {
        case 'R':
            /* Fallthrough */
        case 'P':
            /* Fallthrough */
        case 'S':
            /* Fallthrough */
        case 'B':
            /* Fallthrough */
        case 'F':
            /* Fallthrough */
        case 'J':
            return true;
            
        default:
            return false;
    }
}

class FilePlayerAlgorithm : public PlayerAlgorithm
{
private:
    /* The player this algorithms handles */
    int player;
    /* The coordinates for a joker move, if one is supplied. */
    int joker_x, joker_y;
    char new_joker_type;
    bool joker_parsing_failed;
    std::ifstream player_move_file;
    
    void parseJokerParameters()
    {
        
    }

    void parseBoardFile(std::ifstream &player_file,
                 std::vector<unique_ptr<PiecePosition>> &vectorToFill) const
    {
        //TODO: Factor to smaller methods.
        std::string line;
        size_t line_number = 0;
        std::stringstream error_message;
        char type, masquerade_type;
        unsigned int x, y;
        std::pair<unsigned int, unsigned int> cur_coord;
        std::set<std::pair<unsigned int, unsigned int>> used_coords;
        std::map<char, unsigned int> piece_counters;
    
        for (;;)
        {
            std::getline(player_file, line);
            
            if (player_file.fail() && player_file.eof()) {
                break;
            }
            
            line_number++;
            
            if (player_file.fail()) {
                error_message << "Failed to read player's input file." << std::endl;
                throw FileParsingError(error_message.str(), line_number);
            }
            
            /* Skip empty lines. */
            if (std::all_of(line.begin(), line.end(),isspace)) {
                continue;
            }
            
            std::stringstream line_formmater(line);
            
            line_formmater >> type >> x >> y;
            
            if (line_formmater.fail()) {
                error_message << "Player " << player << " bad format at line " << line_number;
                throw FileParsingError(error_message.str(), line_number);
            }
            
            if (!is_valid_type(type)) {
                error_message << "Player " << player << " bad piece type at line " << line_number;
                throw FileParsingError(error_message.str(), line_number);
            }
            
            masquerade_type = '#';
            
            /* Special handling for joker pieces. */
            if ('J' == type) {
                line_formmater >> masquerade_type;
                
                if (line_formmater.fail()) {
                    error_message << "Player " << player << " bad joker command at line " << line_number;
                    throw FileParsingError(error_message.str(), line_number);
                }
                
                if (!is_valid_type(masquerade_type)) {
                    error_message << "Player " << player << " bad joker type at line " << line_number;
                    throw FileParsingError(error_message.str(), line_number);
                }
                
                if (('F' == masquerade_type) || ('J' == masquerade_type)) {
                    error_message << "Player " << player << " joker attempted to be invalid piece at line " << line_number;
                    throw FileParsingError(error_message.str(), line_number);
                }
            }
            
            /* We are given coordinates that are based on 1. Normalize to zero based coordinates. */
            x--;
            y--;
            
            if ((x >= Globals::M) || (y >= Globals::N)) {
                error_message << "Player " << player << " bad piece position at line " << line_number;
                throw FileParsingError(error_message.str(), line_number);
            }
            
            cur_coord = std::make_pair(x, y);
            
            if (used_coords.count(cur_coord)) {
                //TODO: Check error message - do we need to put line number?
                error_message << "Player " << player << " two overlapping pieces at " << x + 1 << "," << y + 1;
                throw FileParsingError(error_message.str(), line_number);
            }
            
            piece_counters[type]++;
            
            if (piece_counters[type] > Globals::ALLOWED_PIECES_COUNT[type]) {
                //TODO: Check error message - do we need to put line number?
                error_message << "Player " << player << " has too many pieces of type " << type;
                throw FileParsingError(error_message.str(), line_number);
            }
            
            vectorToFill.push_back(std::make_unique<ConcretePiecePosition>(x, y, type, masquerade_type));
        }
        
        if (piece_counters['F'] != Globals::ALLOWED_PIECES_COUNT['F']) {
            error_message << "Player " << player << " invalid flag count";
            throw FileParsingError(error_message.str(), line_number);
        }
    }

public:
    PlayerAlgorithm() : player(0), player_move_file() {}

    virtual void getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill) override
    {
        this->player = player;
        std::stringstream file_path;
        std::ifstream player_board_file;
        
        file_path << "./player" << player << ".rps_board";
        player_board_file.open(file_path.str());
        
        if (player_board_file.fail()) {
            throw BadFilePathError(file_path.str());
        }
        
        file_path.clear();
        file_path << "./player" << player << ".rps_moves";
        player_move_file.open(file_path.str());
        
        if (player_move_file.fail()) {
            throw BadFilePathError(file_path.str());
        }
        
        /* Let RAII take care of the file descriptor for us in case of exceptions. */
        parseBoardFile(player_board_file, vectorToFill);
    }
    
    /* We cast to void in these methods to avoid the unreferenced parameter warning. */
    virtual void notifyOnInitialBoard(const Board& b, const std::vector<unique_ptr<FightInfo>>& fights) override
    {
        (void) b;
        (void) fights;
    }
    
    virtual void notifyOnOpponentMove(const Move& move) override
    {
        (void) move;
    }
    
    virtual void notifyFightResult(const FightInfo& fightInfo) override
    {
        (void) fightInfo;
    }
    
    virtual unique_ptr<Move> getMove() override
    {
        std::string line;
        std::getline(player_move_file, line);
        
        joker_x = 0;
        joker_y = 0;
        new_joker_type = '#';
        joker_parsing_failed = false;
        
        // TODO: Handle error.
        if (player_move_file.fail()) {
        }
        
        std::stringstream formatted_line(line);
        int source_x, source_y, dest_x, dest_y;
        
        formatted_line >> source_x >> source_y >> dest_x >> dest_y;
        
        if (formatted_line.fail()) {
            //TODO: handle error
        }
    }
    
    virtual unique_ptr<JokerChange> getJokerChange() override
    {
        if (joker_parsing_failed) {
            //TODO: Handle error.
        }
        
        //TODO: Implement me.
        if (0 == joker_x || 0 == joker_y || '#' == new_joker_type) {
            return nullptr;
        }
    }
};

#endif 
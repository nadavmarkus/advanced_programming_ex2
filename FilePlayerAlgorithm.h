#ifndef __FILE_PLAYER_ALGORITHM_H_
#define __FILE_PLAYER_ALGORITHM_H_

#include "PlayerAlgorithm.h"
#include "PiecePosition.h"
#include "FileParsingError.h"
#include "Globals.h"
#include "ConcretePiecePosition.h"
#include "BadFilePathError.h"

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
    void doParse(int player,
                 std::ifstream &player_file,
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
    virtual void getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill) override
    {
        std::stringstream file_path;
        std::ifstream player_board_file;
        
        file_path << "./player" << player << ".rps_board";
        player_board_file.open(file_path.str());
        
        if (player_board_file.fail()) {
            throw BadFilePathError(file_path.str());
        }
        
        /* Let RAII take care of the file descriptor for us in case of exceptions. */
        doParse(player, player_board_file, vectorToFill);
    }
    
    virtual void notifyOnInitialBoard(const Board& b, const std::vector<unique_ptr<FightInfo>>& fights) override
    {
        (void) b;
        (void) fights;
    }
};

#endif 
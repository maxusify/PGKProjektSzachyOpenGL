#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include "Pawn.hpp"
#include "Rook.hpp"
#include "Knight.hpp"
#include "Bishop.hpp"
#include "Queen.hpp"
#include "King.hpp"

class Player
{
    public:
        Player();
        virtual ~Player();
        void init(int side);
        void init(int side, const std::vector<Piece *> &_pieces);
        void loose();
        void win();
        std::vector<Piece*> getPieces() const { return pieces; }
        Piece * getPieceByVao(int VaoId);
        Piece* getKing() const { return king; }
        void computeAvailableMovements(std::vector<Piece*>, std::vector<Piece*>);
        void deletePieceAt(std::vector<int>);
        void deletePieces();

    private:
        int number;
        std::vector<Piece*> pieces;
        King *king;
};

#endif // PLAYER_H

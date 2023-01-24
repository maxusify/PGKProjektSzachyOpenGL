#ifndef PAWN_H
#define PAWN_H

#include "Piece.hpp"

class Pawn : public Piece
{
    public:
        Pawn(int);

        virtual void computeAvailableMovements(std::vector<Piece*>, std::vector<Piece*>);

        virtual std::string getModelPath() const { return PIECE_PAWN_MODEL_PATH; }
        virtual std::string getName() const{ return PIECE_PAWN_NAME; }
    private:
        int whichSide;
};

#endif // PAWN_H

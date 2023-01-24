#ifndef ROOK_H
#define ROOK_H

#include "Piece.hpp"

class Rook : public Piece
{
public:
    virtual void computeAvailableMovements(std::vector<Piece*>, std::vector<Piece*>);

    virtual std::string getModelPath() const { return PIECE_ROOK_MODEL_PATH; }
    virtual std::string getName() const{ return PIECE_ROOK_NAME; }
};

#endif // ROOK_H

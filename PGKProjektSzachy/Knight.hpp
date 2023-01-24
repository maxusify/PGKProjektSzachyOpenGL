#ifndef KNIGHT_H
#define KNIGHT_H

#include "Piece.hpp"

class Knight : public Piece
{
public:
    virtual void computeAvailableMovements(std::vector<Piece*>, std::vector<Piece*>);

    virtual std::string getModelPath() const { return PIECE_KING_MODEL_PATH; }
    virtual std::string getName() const { return PIECE_KING_NAME; }
};

#endif // KNIGHT_H

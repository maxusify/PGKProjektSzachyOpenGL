#ifndef KING_H
#define KING_H

#include "Piece.hpp"

class King : public Piece
{
public:
    virtual void computeAvailableMovements(std::vector<Piece*>, std::vector<Piece*>);

    virtual std::string getModelPath() const { return PIECE_KING_MODEL_PATH; }
    virtual std::string getName() const { return PIECE_KING_NAME; }
};

#endif // KING_H

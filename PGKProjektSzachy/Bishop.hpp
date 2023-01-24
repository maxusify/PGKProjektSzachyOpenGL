#ifndef BISHOP_H
#define BISHOP_H

#include "Piece.hpp"

class Bishop : public Piece
{
public:
    virtual void computeAvailableMovements(std::vector<Piece*>, std::vector<Piece*>);

    virtual std::string getModelPath() const { return PIECE_BISHOP_MODEL_PATH; }
    virtual std::string getName() const { return PIECE_BISHOP_NAME; }
};

#endif // BISHOP_H

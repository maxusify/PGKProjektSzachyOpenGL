#ifndef QUEEN_H
#define QUEEN_H

#include "Piece.hpp"

class Queen : public Piece
{
public:
    virtual void computeAvailableMovements(std::vector<Piece*>, std::vector<Piece*>);

    virtual std::string getModelPath() const { return PIECE_QUEEN_MODEL_PATH; }
    virtual std::string getName() const{ return PIECE_QUEEN_NAME; }
};

#endif // QUEEN_H

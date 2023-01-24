#ifndef PIECE_H
#define PIECE_H

#include <iostream>
#include <vector>
#include <string>

#define PIECE_KING_MODEL_PATH "models/king.obj"
#define PIECE_BISHOP_MODEL_PATH "models/bishop.obj"
#define PIECE_KNIGHT_MODEL_PATH "models/knight.obj"
#define PIECE_QUEEN_MODEL_PATH "models/queen.obj"
#define PIECE_PAWN_MODEL_PATH "models/pawn.obj"
#define PIECE_ROOK_MODEL_PATH "models/rook.obj"

#define PIECE_KING_NAME "King"
#define PIECE_BISHOP_NAME "Bishop"
#define PIECE_KNIGHT_NAME "Knight"
#define PIECE_QUEEN_NAME "Queen"
#define PIECE_PAWN_NAME "Pawn"
#define PIECE_ROOK_NAME "Rook"

class Piece
{
    public:
        Piece();
        virtual ~Piece();

        virtual void init(int, int);

        virtual int getVaoID() const {return vaoID;}
        virtual void setVaoID(int _vaoID) {vaoID = _vaoID;}
        
        bool canMoveTo(int, int);
        void moveTo(int, int);
        void moveTo(std::vector<int> pos){ moveTo(pos[0], pos[1]);}

        virtual std::string getModelPath() const{ return "";}

        const std::vector<std::vector<int> > & getAvailableMovements() const { return availableMovements; }
        virtual std::vector<int> const getPosition();

        virtual void computeAvailableMovements(std::vector<Piece*>, std::vector<Piece*>);
        virtual void deleteAvailableMovements(std::vector<int>);
        void clearAvailableMovements();

        virtual std::string toString() const;
        virtual std::string getName() const = 0;
    
    protected:
        int posX;
        int posY;
        int vaoID;

        std::vector<std::vector<int>> availableMovements;
};

#endif // PIECE_H


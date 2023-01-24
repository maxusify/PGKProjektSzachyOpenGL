#ifndef BOARD_H
#define BOARD_H

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "Scene.hpp"
#include "Piece.hpp"
#include "Pawn.hpp"
#include "Rook.hpp"
#include "Knight.hpp"
#include "Bishop.hpp"
#include "Queen.hpp"
#include "King.hpp"

class Board
{
public:
    Board();
    virtual ~Board();

    std::vector<std::vector <Piece *>> initialize(Scene * _scene);
    std::vector<std::vector <Piece *>> initWithFile(Scene * _scene, std::string);
    inline glm::vec3 getPosAt(int i, int j) { return squares[i][j]; }
    inline glm::vec3 getPosAt(std::vector<int> square) { return squares[square[0]][square[1]]; }
    void computeAllSquares();
    Piece * getPieceByVao(int vao){ return vaoIDsMap[vao]; }
    void movePieceTo(int vao, int i, int j);
    const glm::vec3 getOut() { return outOfBound; }

private:
    std::vector<Piece *> initPieceFromFile(int side);
    std::vector<Piece *> initPiece(int side);
    glm::vec3 computeRealPosition(int i, int j);
    
    Scene * scene;
    std::map<int, Piece *> vaoIDsMap;
    std::vector<std::vector<glm::vec3> > squares;
    const glm::vec3 centerToSquare0 = glm::vec3(-265.0f, 0.0f, -267.0f);
    const glm::vec3 outOfBound = glm::vec3(2000.0f, 0.0f, 2000.0f);
    const float squareOffset = 76.0f;
};

#endif // BOARD_H

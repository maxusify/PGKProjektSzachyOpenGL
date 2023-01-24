#ifndef GAME_H
#define GAME_H

#include <iostream>
#include "Board.hpp"
#include "Player.hpp"
#include "Scene.hpp"
#include "Piece.hpp"

class Game
{
public:
    void init(Scene * _scene);
    void loadFromFile(Scene *scene);
    void saveToFile();
    std::vector<Piece *> check(Player player, Player opponent, std::vector<int> KingPos);
    bool checkMate(Player player, Player opponent, std::vector<Piece *> threateningPieces);
    void changeTurn();
    void computeAvailableMovements();
    void computeKingMovements(Player player, Player opponent);
    void tryMovement(int vaoId, int caseX, int caseY);
    void tryMovement(int vaoId1, int vaoId2);
    int getPlayerId() const { return turn; }
    void endGame(int winner);
    void testDebug();
    void ejectPiece(int, int);
    
private:
    Scene *scene;
    Board board;
    Player player1, player2, none;
    int turn;
};

#endif // GAME_H

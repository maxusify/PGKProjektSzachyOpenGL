#include "Game.hpp"
#include <fstream>

/**
* Sprawdza ruch
*/
void Game::tryMovement(int vaoId1, int vaoId2)
{
    Player &current2 = 
        (turn == 2) ? player1 : player2;

    Piece  *playerPiece2 = current2.getPieceByVao(vaoId2);
    
    if (playerPiece2 != nullptr)
    {
        tryMovement(vaoId1, playerPiece2->getPosition()[0], playerPiece2->getPosition()[1]);
    }
}

void Game::tryMovement(int vaoId, int caseX, int caseY)
{
    Player &current = (turn == 1)?player1:player2;
    Player &opponent = (turn == 1)?player2:player1;

    Piece *current_piece = current.getPieceByVao(vaoId);

    if (current_piece != nullptr)
    {
        std::cout << "Gra: Gracz " << turn << " wybral " << vaoId << "(" << current_piece->getPosition()[0] << ";" << current_piece->getPosition()[1] << ") i kliknal na (" << caseX << "," << caseY << ")" <<  std::endl;

        if (current_piece->canMoveTo(caseX, caseY))
        {
            std::vector<int> tempPos = current_piece->getPosition();
            
            current_piece->moveTo(caseX,caseY);
            opponent.computeAvailableMovements(opponent.getPieces(),current.getPieces());
            
            if (check(current, opponent, current.getKing()->getPosition()).size() == 0)
            {
                current_piece->moveTo(tempPos);
                ejectPiece(caseX, caseY);
                board.movePieceTo(current_piece->getVaoID(), caseX, caseY);
                changeTurn();
            } else
            {
                std::cout  << "\tNieprawidlowy ruch!\n";
                current_piece->moveTo(tempPos);
                opponent.computeAvailableMovements(opponent.getPieces(),current.getPieces());
            }
        }
        else
            std::cout  << "\tNieprawidlowy ruch\n";
    }
    else {
        std::cout << "Gra: Gracz " << turn << " wybral " << vaoId << " i kliknal na (" << caseX << "," << caseY << ")" << std::endl;
    }
}
        

/**
* Inicjalizuje grę
*/
void Game::init(Scene * _scene) {
    scene = _scene;

    std::vector<std::vector<Piece *>> pieces;
    pieces = board.initialize(_scene);

    player1.deletePieces();
    player2.deletePieces();

    none.init(0);

    player1.init(1, pieces[0]);
    player2.init(2, pieces[1]);

    turn = 1;
    computeAvailableMovements();
}

/**
* Sprawdza ruchy
*/
std::vector<Piece *> Game::check(Player player, Player opponent, std::vector<int> KingPos) 
{
    std::vector<Piece *> threateningPieces = std::vector<Piece *>(0);

    for (unsigned int i = 0 ; i < opponent.getPieces().size() ; i++) 
    {
        for (unsigned int j = 0 ; j < opponent.getPieces()[i]->getAvailableMovements().size() ; j++) 
        {
            std::vector<int> checkSquare = opponent.getPieces()[i]->getAvailableMovements()[j];
            
            if (checkSquare[0] == KingPos[0] && checkSquare[1] == KingPos[1]) 
            {
                threateningPieces.push_back(opponent.getPieces()[i]);
                
                if (threateningPieces.size() >= 2)
                {
                    return threateningPieces;
                }
            }
        }
    }

    return threateningPieces;
}

/**
* Przetwarza możliwy szachmat
*/
bool Game::checkMate(Player player, Player opponent, std::vector<Piece *> threateningPieces) 
{
    bool isInCheckMate = true;

    std::vector<int> tempPos = player.getKing()->getPosition();
    std::vector<int> kingPos = tempPos;

    for (unsigned int j = 0 ; j < player.getKing()->getAvailableMovements().size() ; j++) 
    {
        kingPos = player.getKing()->getAvailableMovements()[j] ;
        player.getKing()->moveTo(kingPos[0], kingPos[1]);
        opponent.computeAvailableMovements(opponent.getPieces(), player.getPieces());
    
        if (check(player, opponent,kingPos).size() == 0)
        {
            isInCheckMate = false;
            break;
        }
    }

    player.getKing()->moveTo(tempPos[0], tempPos[1]);
    opponent.computeAvailableMovements(opponent.getPieces(), player.getPieces());

    if (!isInCheckMate) 
        return false;

    if (threateningPieces.size() >1) 
        return true;

    std::vector<int> positionToReach = threateningPieces[0]->getPosition();

    for (unsigned int i = 0 ; i < player.getPieces().size() ; i++) 
    {
        for (unsigned int j = 0 ; j < player.getPieces()[i]->getAvailableMovements().size(); j++) 
        {
            std::vector<int> possibleMovement = player.getPieces()[i]->getAvailableMovements()[j];

            if ((possibleMovement[0] == positionToReach[0]) && (possibleMovement[1] == positionToReach[1])) 
                return false;
        }
    }

    kingPos = player.getKing()->getPosition();

    for (unsigned int i = 0 ; i < player.getPieces().size() ; i++) 
    {
        if (player.getPieces()[i]->getName() == "King" ) 
            continue;
       
        tempPos = player.getPieces()[i]->getPosition();
        
        for (unsigned int j = 0 ; j < player.getPieces()[i]->getAvailableMovements().size(); j++) 
        {
            std::vector<int> possibleMovement = player.getPieces()[i]->getAvailableMovements()[j];
            
            player.getPieces()[i]->moveTo(possibleMovement);
            opponent.computeAvailableMovements(opponent.getPieces(), player.getPieces());
            
            if ( check(player, opponent,kingPos).size() == 0 ) 
            {
                player.getPieces()[i]->moveTo(tempPos);
                return false;
            }
        }

        player.getPieces()[i]->moveTo(tempPos);
    }

    std::cout << "\nSzach mat! !\n";
    return true;
}

/**
* Zmień gracza (kolejność)
*/
void Game::changeTurn() {
    turn = 
        (turn == 1) ? 2 : 1;

    computeAvailableMovements();
    scene->unselect();

    if (turn == 1)
    {
        std::vector<Piece *> checkState = check(player1, player2, player1.getKing()->getPosition());
     
        if (checkState.size()>0)
        {
            scene->setSelectTex(1);
            if(checkMate(player1, player2,checkState)) endGame(2);
        } 
        else 
        {
            scene->setSelectTex(0);
        }
    } 
    else if (turn == 2)
    {
        std::vector<Piece *> checkState = check(player2, player1, player2.getKing()->getPosition());
        
        if (checkState.size())
        {
            scene->setSelectTex(1);
            if(checkMate(player2, player1, checkState)) endGame(1);
        } 
        else 
        {
            scene->setSelectTex(0);
        }
    }
}

/**
* Oblicz możliwe ruchy
*/
void Game::computeAvailableMovements() 
{
    player1.computeAvailableMovements(player1.getPieces(), player2.getPieces());
    player2.computeAvailableMovements(player2.getPieces(), player1.getPieces());
}

/**
* Zbija pionek
*/
void Game::ejectPiece(int x, int y) 
{
    Piece * piece;
    std::vector<int> position;

    position.resize(2);
    position[0] = x;
    position[1] = y;

    for (unsigned int i = 0 ; i < player1.getPieces().size() ; i++) 
    {
        piece = player1.getPieces()[i];
        
        if (piece->getPosition()[0] == position[0] && piece->getPosition()[1] == position[1]) 
        {
            scene->ejectVAO(piece->getVaoID()-1);
            player1.deletePieceAt(position);
        }
    }

    for (unsigned int j = 0 ; j < player2.getPieces().size() ; j++)
    {
        piece = player2.getPieces()[j];

        if (piece->getPosition()[0] == position[0] && piece->getPosition()[1] == position[1]) 
        {
            scene->ejectVAO(piece->getVaoID()-1);
            player2.deletePieceAt(position);
        }
    }
}

/**
* Zakańcza grę i ustala zwycięzcę
*/
void Game::endGame(int winner)
{
    scene->setSelectTex(2);

    Player looser;

    if (winner == 1){
        looser = player2;
    }
    else
    {
        looser = player1;
    }

    std::cout << "\nGracz " << winner << " wygrywa!";

    for (unsigned int i = 0 ; i< looser.getPieces().size() ; i++)
    {
        Piece * piece = looser.getPieces()[i];
     
        if (piece->getName() == "King") 
            continue;
        
        scene->ejectVAO(piece->getVaoID()-1);
    }
}

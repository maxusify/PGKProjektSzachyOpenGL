#include "Player.hpp"
#include <typeinfo>

Player::Player() 
{
}

Player::~Player() 
{
}

void Player::init(int side) 
{
    number = side;
}

/**
* Inicjalizuje gracza
*/
void Player::init(int side,  std::vector<Piece *> const & _pieces) 
{
    number = side;
    pieces = _pieces;

    for (unsigned int i =0; i < pieces.size() ; i++)
    {
        if (pieces[i]->getName() == "King") 
        {
            king = (King *) pieces[i];
            break;
        }
    }
}

void Player::loose() 
{
}

void Player::win() 
{
}

/**
* Wylicza mo¿liwe ruchy gracza
*/
void Player::computeAvailableMovements(std::vector<Piece*> own, std::vector<Piece*> opp) 
{
    for (unsigned int i = 0 ; i < pieces.size() ; i++) 
    {
        pieces[i]->clearAvailableMovements();
        pieces[i]->computeAvailableMovements(own, opp);
    }
}

/**
* Zwraca pionek o podanym identyfikatorze VAO
*/
Piece* Player::getPieceByVao(int VaoId)
{
    for(unsigned int i =0; i < pieces.size() ;i++)
    {
        if (pieces[i]->getVaoID() == VaoId)
        {
            return pieces[i];
        }
    }

    return nullptr;
}

/**
* Usuwa pionek w danej pozycji
*/
void Player::deletePieceAt(std::vector<int> position) 
{
    for (int i = 0 ; i < pieces.size() ; i++) 
    {
        if (pieces[i]->getPosition() == position) 
        {
            pieces.erase(pieces.begin() + i);
            return;
        }
    }
}

/**
* Usuwa pionki gracza z pamiêci
*/
void Player::deletePieces() 
{
    for (int i = 0 ; i < pieces.size() ; i++) 
    {
        delete pieces[i];
    }
}

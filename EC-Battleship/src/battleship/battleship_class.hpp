#ifndef __BATTLESHIP_HPP__
#define __BATTLESHIP_HPP__

#include <Vector.h>

//typedef Vector<int> ejes;

using ejes = Vector<int>;

class Battleship
{
    public:
    //Constructores.
    Battleship();
    Battleship(ejes &varEjeX, ejes &varEjeY);
    //Destructor.
    ~Battleship();
    //Métodos
    void mover(int x_increment,int y_increment);
    bool find(unsigned jugador,int &puntero[2]);
    //set
    void set_hit(const unsigned &var_hit);
    void set_derribado(const bool &var_derribado);
    //get
    ejes get_ejeX();
    ejes get_ejeY();
    unsigned get_hit();
    bool get_derribado();
    size_t get_size();
    //Operadores
    Battleship operator=(const Battleship &barco);
    Battleship operator^(const bool &rot);
    Battleship operator+(int vec[]);

    private:
    ejes ejeX;
    ejes ejeY;
    unsigned hit; //No hace falta un vector, con que cuente el numero de golpes basta.
    bool derribado;

};
#endif
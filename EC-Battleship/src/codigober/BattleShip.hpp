
#ifndef BARCO_HPP
#define BARCO_HPP

#include <Adafruit_NeoPixel.h>

// Dimensiones de la matriz LED
const int ANCHO_MATRIZ = 16;  // Número de columnas
const int ALTO_MATRIZ = 16;   // Número de filas

// Matriz para indicar si un LED está ocupado por un barco fijado
extern bool leds_ocupados[ALTO_MATRIZ][ANCHO_MATRIZ];

// Clase Barco: Representa un barco en el juego
class Barco {
public:
    int posX, posY; // Posición del barco en la matriz LED
    int forma[5][5]; // Matriz que representa la forma del barco
    bool estaVivo; // Indica si el barco sigue vivo o no
    bool estaFijo; // Indica si el barco está fijado en la matriz
    int formaActual; // Índice que identifica la forma actual del barco

    // Constructor: Inicializa un barco en una posición inicial
    Barco(int inicioX, int inicioY);

    // Cambia entre las diferentes formas del barco (cubo, línea, L)
    void cambiarForma();

    // Define la forma del barco como un cubo
    void establecerFormaCubo();

    // Define la forma del barco como una línea
    void establecerFormaLinea();

    // Define la forma del barco como una "L"
    void establecerFormaL();

    // Limpia la forma actual del barco
    void limpiarForma();

    // Rota la forma del barco 90 grados
    void rotar();

    // Calcula los límites de una matriz dada
    void calcularLimites(int matriz[5][5], int &minX, int &minY, int &maxX, int &maxY);

    // Calcula los límites de la forma actual del barco
    void calcularLimites(int &minX, int &minY, int &maxX, int &maxY);

    // Dibuja el barco en la matriz LED
    void dibujar(Adafruit_NeoPixel &matriz, uint32_t color = 0);

    // Verifica si el barco puede fijarse en la posición actual
    bool puedeFijarse();

    // Fija el barco en su posición actual
    void fijar();

    // Mueve el barco en la dirección indicada
    void mover(int desplazamientoX, int desplazamientoY);
};

#endif

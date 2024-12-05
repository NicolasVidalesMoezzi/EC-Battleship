
#include "BattleShip.hpp"

// Matriz para marcar qué LEDs están ocupados por barcos fijados
bool leds_ocupados[ALTO_MATRIZ][ANCHO_MATRIZ] = {false};

// Constructor: Inicializa un barco con una posición inicial y configura su estado inicial
Barco::Barco(int inicioX, int inicioY) {
    posX = inicioX; // Posición inicial X del barco
    posY = inicioY; // Posición inicial Y del barco
    estaVivo = true; // El barco comienza en estado "vivo"
    estaFijo = false; // El barco no está fijado inicialmente
    formaActual = 0; // Por defecto, el barco tiene la primera forma (cubo)
    cambiarForma(); // Configura la forma inicial
}

// Cambia la forma del barco según el índice formaActual
void Barco::cambiarForma() {
    if (formaActual == 0) {
        establecerFormaCubo();
    } else if (formaActual == 1) {
        establecerFormaLinea();
    } else if (formaActual == 2) {
        establecerFormaL();
    }
}

// Define la forma del barco como un cubo
void Barco::establecerFormaCubo() {
    limpiarForma(); // Limpia la matriz de forma
    forma[1][1] = 1;
    forma[1][2] = 1;
    forma[2][1] = 1;
    forma[2][2] = 1; // Define un cuadrado de 2x2 en el centro de la matriz de forma
}

// Define la forma del barco como una línea horizontal
void Barco::establecerFormaLinea() {
    limpiarForma(); // Limpia la matriz de forma
    forma[2][0] = 1;
    forma[2][1] = 1;
    forma[2][2] = 1;
    forma[2][3] = 1;
    forma[2][4] = 1; // Crea una línea recta horizontal en el centro
}

// Define la forma del barco como una "L"
void Barco::establecerFormaL() {
    limpiarForma(); // Limpia la matriz de forma
    forma[1][1] = 1;
    forma[2][1] = 1;
    forma[3][1] = 1;
    forma[3][2] = 1; // Forma en "L" en la parte inferior derecha
}

// Limpia la matriz de forma del barco
void Barco::limpiarForma() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            forma[i][j] = 0; // Rellena con ceros toda la matriz
        }
    }
}

// Rota la forma del barco 90 grados en sentido horario
void Barco::rotar() {
    int nuevaForma[5][5] = {0}; // Nueva matriz para almacenar la forma rotada
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            nuevaForma[j][4 - i] = forma[i][j]; // Transforma las coordenadas para rotar
        }
    }

    // Calcula los límites de la nueva forma
    int minX, minY, maxX, maxY;
    calcularLimites(nuevaForma, minX, minY, maxX, maxY);

    // Verifica si la rotación cabe dentro de los límites de la matriz LED
    if (posX + maxX < ANCHO_MATRIZ && posY + maxY < ALTO_MATRIZ && posX + minX >= 0 && posY + minY >= 0) {
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                forma[i][j] = nuevaForma[i][j]; // Aplica la rotación si es válida
            }
        }
    }
}

// Calcula los límites (mínimo y máximo) de una matriz de forma
void Barco::calcularLimites(int matriz[5][5], int &minX, int &minY, int &maxX, int &maxY) {
    minX = 5;
    minY = 5;
    maxX = -1;
    maxY = -1;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (matriz[i][j] == 1) { // Busca celdas ocupadas
                if (j < minX) minX = j;
                if (i < minY) minY = i;
                if (j > maxX) maxX = j;
                if (i > maxY) maxY = i;
            }
        }
    }
}

// Calcula los límites de la forma actual del barco
void Barco::calcularLimites(int &minX, int &minY, int &maxX, int &maxY) {
    calcularLimites(forma, minX, minY, maxX, maxY);
}

// Dibuja el barco en la matriz LED con un color específico
void Barco::dibujar(Adafruit_NeoPixel &matriz, uint32_t color) {
    if (color == 0) color = matriz.Color(0, 255, 0); // Color verde por defecto
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (forma[i][j]) { // Dibuja solo las celdas ocupadas
                int nx = posX + j;
                int ny = posY + i;
                if (nx >= 0 && nx < ANCHO_MATRIZ && ny >= 0 && ny < ALTO_MATRIZ) {
                    int numPixel = ((ny % 2 == 0) ? ny * ANCHO_MATRIZ + nx
                                                  : ny * ANCHO_MATRIZ + (ANCHO_MATRIZ - 1 - nx));
                    matriz.setPixelColor(numPixel, color); // Establece el color del píxel correspondiente
                }
            }
        }
    }
}

// Verifica si el barco puede fijarse en su posición actual
bool Barco::puedeFijarse() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (forma[i][j]) { // Revisa cada celda ocupada
                int nx = posX + j;
                int ny = posY + i;
                if (nx < 0 || nx >= ANCHO_MATRIZ || ny < 0 || ny >= ALTO_MATRIZ || leds_ocupados[ny][nx]) {
                    return false; // No puede fijarse si está fuera de los límites o la celda está ocupada
                }
            }
        }
    }
    return true;
}

// Fija el barco en la posición actual y actualiza la matriz de LEDs ocupados
void Barco::fijar() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (forma[i][j]) {
                int nx = posX + j;
                int ny = posY + i;
                leds_ocupados[ny][nx] = true; // Marca la celda como ocupada
            }
        }
    }
    estaFijo = true; // Marca el barco como fijado
}

// Mueve el barco a una nueva posición si es válida
void Barco::mover(int desplazamientoX, int desplazamientoY) {
    int minX, minY, maxX, maxY;
    calcularLimites(minX, minY, maxX, maxY); // Calcula los límites de la forma actual
    posX += desplazamientoX;
    posY += desplazamientoY;

    // Ajusta la posición para que el barco no salga de los límites
    if (posX + minX < 0) posX = -minX;
    if (posY + minY < 0) posY = -minY;
    if (posX + maxX >= ANCHO_MATRIZ) posX = ANCHO_MATRIZ - maxX - 1;
    if (posY + maxY >= ALTO_MATRIZ) posY = ALTO_MATRIZ - maxY - 1;
}

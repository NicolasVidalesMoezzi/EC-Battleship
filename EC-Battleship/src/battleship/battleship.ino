#include <Adafruit_NeoPixel.h>
#include <Vector.h>
#include "battleship_class.hpp"

#define PIN 6                                  // Pin digital
#define PIN_2 12                               // Pin digital
#define MATRIX_WIDTH 16                        // Numero de columnas
#define MATRIX_HEIGHT 16                       // Numero de filas
#define NUM_PIXELS MATRIX_WIDTH *MATRIX_HEIGHT // Numero de pixeles
#define BRIGHTNESS 50                          // Brillo de los leds

#define SHIP_WIDTH 1  // Este numero corresponde a la anchura de los barcos
#define SHIP_HEIGHT 1 // Este numero corresponde a la altura de los barcos
#define NUM_BARCOS 6  // Numero de barcos
#define TAM 3

#define COLOCACION 0
#define TURNO_JUGADOR_1 1
#define TURNO_JUGADOR_2 2
#define FIN_PARTIDA 3

#define BIT1 0x01
#define BIT2 0x02
#define BIT3 0x03

#define EV_FIN_COLOCACION BIT1
#define EV_FIN_TURNO BIT2
#define EV_FIN_JUEGO BIT3

#define SET(ev) (eventos |= (ev))
#define RESET(ev) (eventos &= ~(ev))
#define TEST(ev) (eventos & (ev))

#define PANTALLA_J1 0
#define PANTALLA_J2 1

#define der true
#define izq false

Adafruit_NeoPixel led_matrix = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int A_BUTTON = 0;
const int B_BUTTON = 1;
const int SELECT_BUTTON = 2;
const int START_BUTTON = 3;
const int UP_BUTTON = 4;
const int DOWN_BUTTON = 5;
const int LEFT_BUTTON = 6;
const int RIGHT_BUTTON = 7;

// Maquina de estados
unsigned char estado;
unsigned char eventos;

// NES

byte nesRegister[2] = {0, 0}; // jugador1, jugador2
int data[2] = {4, 10};        // Pin de datos del controlador NES verde
int clk = 2;                  // Pin de clock del controlador NES blanco
int latch[2] = {3, 9};        // Pin de latch del controlador NES negro

/*
byte nesRegister2 = 0;
int data2 = 10;
int latch2 = 9;
*/

// Barcos

unsigned cont_derribados[2] = {0, 0}; // El numero de barcos que tiene derribados el jugador 1 (0) y jugador 2 (1)

void inicializarX(ejes &valor, int inicio)
{
  for (int i = 0; i < 3; ++i)
  {
    valor.push_back(inicio);
    // Serial.println(valor[i]);
  }
}

void mover_puntero(unsigned jugador, int &puntero[2]) // puntero[0] -> eje X ,  puntero[1] -> eje Y
{
  // puntero[0] = 0;
  // puntero[1] = 0;
  // Movimientos de la cruceta
  while (bitRead(nesRegister[jugador], A_BUTTON) != 0)
  {
    if (bitRead(nesRegister[jugador], UP_BUTTON) == 0 && puntero[1] > 0)
    { // límite superior
      --puntero[1];
      Serial.println("arriba");
    }
    if (bitRead(nesRegister[jugador], DOWN_BUTTON) == 0 && puntero[1] < MATRIX_HEIGHT - 1)
    { // límite inferior
      ++puntero[1];
      Serial.println("abajo");
    }
    if (bitRead(nesRegister[jugador], RIGHT_BUTTON) == 0 && puntero[0] < MATRIX_WIDTH - 1)
    { // límite derecha
      ++puntero[0];
      Serial.println("derecha");
    }
    if (bitRead(nesRegister[jugador], LEFT_BUTTON) == 0 && puntero[0] > 0)
    { // límite izquierda
      --puntero[0];
      Serial.println("izquierda");
    }
  }
}

void inicializarY(ejes &valor, int inicio)
{
  for (int i = 0; i < 3; ++i)
  {
    valor.push_back(i + inicio);
    // Serial.println(valor[i]);
  }
}

const int ELEMENT_COUNT_MAX = 30;
int storage_array[ELEMENT_COUNT_MAX];
ejes ejeX(storage_array);
ejes ejeY(storage_array);

Battleship barcos[2][NUM_BARCOS];
// Posicion inicial de los leds. Cambia a arrays si los tamaños de los datos son distintos a uno
// const unsigned tam = barcos.get_size();

// Jugador1, Jugador2
int ledX[2][NUM_BARCOS][TAM];
int ledY[2][NUM_BARCOS][TAM];

// Esquina superior izquierda.

// Color del led en formato RGB
uint32_t colorLed = led_matrix.Color(255, 0, 0); // Rojo puro
// Funciones con barcos
// 0 <= jugador <= 1;
void colocar_barcos(unsigned jugador, unsigned &id_barco)
{
  // Movimientos de la cruceta
  if (bitRead(nesRegister[jugador], UP_BUTTON) == 0 && ledY[jugador][id_barco][0] > 0)
  { // límite superior
    barcos[jugador][id_barco].mover(0, -1);
    Serial.println("arriba");
  }
  if (bitRead(nesRegister[jugador], DOWN_BUTTON) == 0 && ledY[jugador][id_barco][3] < MATRIX_HEIGHT - 1 - 1)
  { // límite inferior
    barcos[jugador][id_barco].mover(0, 1);
    Serial.println("abajo");
  }
  if (bitRead(nesRegister[jugador], RIGHT_BUTTON) == 0 && ledX[jugador][id_barco][2] > 0)
  { // límite derecha
    barcos[jugador][id_barco].mover(1, 0);
    Serial.println("derecha");
  }
  if (bitRead(nesRegister[jugador], LEFT_BUTTON) == 0 && ledX[jugador][id_barco][0] < MATRIX_WIDTH - 1)
  { // límite izquierda
    barcos[jugador][id_barco].mover(-1, 0);
    Serial.println("izquierda");
  }
  if (bitRead(nesRegister[jugador], B_BUTTON) == 0)
  {
    barcos[jugador][id_barco] = barcos[jugador][id_barco] ^ der;
    Serial.println("rotar");
  }
  if (bitRead(nesRegister[jugador], A_BUTTON) == 0)
  {
    id_barco++;
  }
}

void atacar(unsigned jugador)
{                 // el jugador atacante seleccionará una casilla hasta que pulse el botón A. En ese momento, se comprobará si ha tocado un barco o no, con sus respectivas ramificaciones
  int puntero[2]; // almacena el lugar de la matriz donde se está apuntando
  mover_puntero(jugador, &puntero);
  for (unsigned i = 0; i < NUM_BARCOS; i++)
  {
    if (barcos[jugador][i].find(puntero))
    {
      // Cambia de color el led
      // Hay que implementarlo.
      if (barcos[jugador][i].get_derribado())
        cont_derribados[jugador]++;
      return; // Cuando encuentre una coincidencia se sale de la función
    }
  }
}

unsigned

    void
    display(unsigned jugador, unsigned id_barco)
{
  unsigned tam = barcos[jugador][id_barco].get_size();
  Vector<int> posX = barcos[jugador][id_barco].get_ejeX();
  Vector<int> posY = barcos[jugador][id_barco].get_ejeY();

  for (unsigned i = 0; i < tam; ++i)
  {
    ledX[jugador][id_barco][i] = posX[i];
    ledY[jugador][id_barco][i] = posY[i];
  }
  return;
}

// Función para convertir las coordenadas (X, Y) en índice considerando el formato de "serpiente"
int obtenerIndiceMatriz(int x, int y)
{
  if (y % 2 == 0)
  {
    // Si la fila es par, el índice va de izquierda a derecha
    return y * MATRIX_WIDTH + x;
  }
  else
  {
    // Si la fila es impar, el índice va de izquierda a derecha para evitar inversión
    return y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
  }
}

// Función para actualizar el LED activo en la matriz
void actualizarMatriz(unsigned jugador, unsigned id_barco)
{
  led_matrix.clear(); // Limpia todos los LEDs
  int pixelIndex[TAM];
  for (int i = 0; i < TAM; ++i)
  {
    pixelIndex[i] = obtenerIndiceMatriz(ledX[jugador][id_barco][i], ledY[jugador][id_barco][i]); // Convierte las coordenadas (X, Y) al índice en la matriz en formato serpiente
    led_matrix.setPixelColor(pixelIndex[i], colorLed);                                           // Establece el color del LED en la posición actual
    led_matrix.show();                                                                           // Muestra los cambios en la matriz
  }
}

// int ledX[2][NUM_BARCOS];
// int ledY[2][NUM_BARCOS];

void setup()
{
  Serial.begin(9600);

  pinMode(data[0], INPUT);
  pinMode(clk, OUTPUT);
  pinMode(latch[0], OUTPUT);
  pinMode(data[1], INPUT);
  pinMode(latch[1], OUTPUT);
  digitalWrite(clk, LOW);
  digitalWrite(latch[0], LOW);
  digitalWrite(latch[1], LOW);

  // Inicializacion de la matriz
  led_matrix.begin();
  led_matrix.setBrightness(BRIGHTNESS);
  led_matrix.show();

  inicializarX(ejeX, 7);
  inicializarY(ejeY, 7);
  estado = COLOCACION;
  ledX[2][NUM_BARCOS];
  ledY[2][NUM_BARCOS];
  unsigned i = 0;
  unsigned j = 0;
}

void loop()
{

  nesRegister[0] = readNesController(0);
  nesRegister[1] = readNesController(1);

  switch (estado)
  {
  case COLOCACION:
    static unsigned i = 0;
    static unsigned j = 0;
    while (i < NUM_BARCOS || j < NUM_BARCOS)
    {
      colocar_barcos(0, i);
      colocar_barcos(1, j);
    }
    estado = TURNO_JUGADOR_1;
    break;
  case TURNO_JUGADOR_1:
    atacar(1);                            // ataca al jugador 2
    if (cont_derribados[1] >= NUM_BARCOS) // Comprueba cuantos tiene derribados el jugador 2, si tiene el mismo numero que barcos entonces se pasa al fin de la partida
      estado = FIN_PARTIDA;
    else
      estado = TURNO_JUGADOR_2;
    break;
  case TURNO_JUGADOR_2:
    atacar(0);
    if (cont_derribados[0] >= NUM_BARCOS)
      estado = FIN_PARTIDA;
    else
      estado = TURNO_JUGADOR_1;
    break;
  case FIN_PARTIDA:
    while(bitRead(nesRegister[0], SELECT_BUTTON) != 0 && bitRead(nesRegister[1], SELECT_BUTTON) != 0);
    estado = COLOCACION;
    break;
  default:
    Serial.println("Error en la maquina de Estado...");
    return;
    break;
  }

  /*display(barcos, ledX[0], ledY[0]);
  display(barcos, ledX[1], ledY[1]);
  actualizarMatriz();*/
  delay(100);
}

byte readNesController(int jugador)
{
  byte tempData = 0xFF;

  // Pulso en el latch para leer el estado de los botones
  digitalWrite(latch[jugador], HIGH);
  delayMicroseconds(2);
  digitalWrite(latch[jugador], LOW);

  for (int i = 0; i < 8; i++)
  {
    if (digitalRead(data[jugador]) == LOW)
      bitClear(tempData, i);

    digitalWrite(clk, HIGH);
    delayMicroseconds(2);
    digitalWrite(clk, LOW);
  }

  return tempData;
}

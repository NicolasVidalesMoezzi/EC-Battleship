#include <Adafruit_NeoPixel.h>
#include <Vector.h>
#include "battleship_class.hpp"

#define PIN 6                                  // Pin digital
#define MATRIX_WIDTH 16                        // Numero de columnas
#define MATRIX_HEIGHT 16                       // Numero de filas
#define NUM_PIXELS MATRIX_WIDTH *MATRIX_HEIGHT // Numero de pixeles
#define BRIGHTNESS 50                          // Brillo de los leds

#define SHIP_WIDTH 1  // Este numero corresponde a la anchura de los barcos
#define SHIP_HEIGHT 1 // Este numero corresponde a la altura de los barcos
#define NUM_BARCOS 1  // Numero de barcos
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

byte nesRegister = 0;
int data = 4;  // Pin de datos del controlador NES verde
int clk = 2;   // Pin de clock del controlador NES blanco
int latch = 3; // Pin de latch del controlador NES negro

// Barcos

void inicializarX(ejes &valor, int inicio)
{
  for (int i = 0; i < 3; ++i)
  {
    valor.push_back(inicio);
    // Serial.println(valor[i]);
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

// Posicion inicial de los leds. Cambia a arrays si los tamaños de los datos son distintos a uno
// const unsigned tam = barcos.get_size();
int ledX[TAM] = {0, 0, 0};
int ledY[TAM] = {7, 8, 9};
// Esquina superior izquierda.

// Color del led en formato RGB
uint32_t colorLed = led_matrix.Color(255, 0, 0); // Rojo puro
// Funciones con barcos

void colocar_barcos(unsigned id_barco)
{
  // Movimientos de la cruceta
  if (bitRead(nesRegister, UP_BUTTON) == 0 && ledY[0] > 0)
  { // límite superior
    barcos[id_barco].mover(0, -1);
    Serial.println("arriba");
  }
  if (bitRead(nesRegister, DOWN_BUTTON) == 0 && ledY[3] < MATRIX_HEIGHT - 1 - 1)
  { // límite inferior
    barcos[id_barco].mover(0, 1);
    Serial.println("abajo");
  }
  if (bitRead(nesRegister, RIGHT_BUTTON) == 0 && ledX[2] > 0)
  { // límite derecha
    barcos[id_barco].mover(1, 0);
    Serial.println("derecha");
  }
  if (bitRead(nesRegister, LEFT_BUTTON) == 0 && ledX[0] < MATRIX_WIDTH - 1)
  { // límite izquierda
    barcos[id_barco].mover(-1, 0);
    Serial.println("izquierda");
  }
}

void atacar(Battleship atacante, Battleship defensor);

void display(Battleship barcos[])
{
  unsigned tam = barcos->get_size();
  Vector<int> posX = barcos->get_ejeX();
  Vector<int> posY = barcos->get_ejeY();

  for (unsigned i = 0; i < tam; ++i)
  {
    ledX[i] = posX[i];
    ledY[i] = posY[i];
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
void actualizarMatriz()
{
  led_matrix.clear(); // Limpia todos los LEDs
  int pixelIndex[TAM];
  for (int i = 0; i < TAM; ++i)
  {
    pixelIndex[i] = obtenerIndiceMatriz(ledX[i], ledY[i]); // Convierte las coordenadas (X, Y) al índice en la matriz en formato serpiente
    led_matrix.setPixelColor(pixelIndex[i], colorLed);     // Establece el color del LED en la posición actual
    led_matrix.show();                                     // Muestra los cambios en la matriz
  }
}

Vector<Battleship> barcos;

void setup()
{
  Serial.begin(9600);

  pinMode(data, INPUT);
  pinMode(clk, OUTPUT);
  pinMode(latch, OUTPUT);
  digitalWrite(clk, LOW);
  digitalWrite(latch, LOW);

  // Inicializacion de la matriz
  led_matrix.begin();
  led_matrix.setBrightness(BRIGHTNESS);
  led_matrix.show();

  inicializarX(ejeX, 7);
  // Serial.println(ejeX.size());
  /*for(unsigned i = 0; i < 3; i++)
    Serial.println(ejeX[i]);*/
  inicializarY(ejeY, 7);
  /*for(unsigned i = 0; i < 3; i++)
    Serial.println(ejeY[i]);*/
  barcos = new Battleship(ejeX, ejeY);
  estado = COLOCACION;
}

void loop()
{

  nesRegister = readNesController();

  switch (estado)
  {
  case COLOCACION:

    break;
  case TURNO_JUGADOR_1:

    break;
  case TURNO_JUGADOR_2:

    break;
  case FIN_PARTIDA:

    break;
  default:
    Serial.println("Error en la maquina de Estado...");
    return;
    break;
  }

  display(barcos);
  actualizarMatriz();
  delay(100);
}

byte readNesController()
{
  byte tempData = 0xFF;

  // Pulso en el latch para leer el estado de los botones
  digitalWrite(latch, HIGH);
  delayMicroseconds(2);
  digitalWrite(latch, LOW);

  for (int i = 0; i < 8; i++)
  {
    if (digitalRead(data) == LOW)
      bitClear(tempData, i);

    digitalWrite(clk, HIGH);
    delayMicroseconds(2);
    digitalWrite(clk, LOW);
  }

  return tempData;
}

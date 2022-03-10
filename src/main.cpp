#include <Arduino.h>
#include <BotaoI2C.h>
#include <ArduinoJson.h>
#include <NeoSWSerial.h>

#define S 22
#define L 19
#define T 12
#define BLANK 10

DynamicJsonDocument doc(1024);

BotaoI2C botao;
NeoSWSerial neo(15, 16);
String json;
void setup()
{

  Serial.begin(9600);
  neo.begin(9600);
  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);
  botao = BotaoI2C(0x22, 0, 0x20);
}

void printone(bool *array);

bool *gerartabela(int dezena, int unidade, int seta)
{
  //{0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1};

  bool *array = malloc(13 * sizeof(bool));
  // 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1
  array[0] = unidade & B00000001;
  array[1] = unidade & B00000010;
  array[2] = unidade & B00000100;
  array[3] = unidade & B00001000;
  array[4] = unidade & B00010000;
  array[5] = dezena & B00000001;
  array[6] = dezena & B00000010;
  array[7] = dezena & B00000100;
  array[8] = dezena & B00001000;
  array[9] = dezena & B00010000;
  array[10] = seta & B00000001;
  array[11] = seta & B00000010;
  array[12] = seta & B00000100;

  return array;
}

int a = 0;
unsigned long tempo = 0;
bool block = false;
bool lendo = false;
int contErros = 0;
int pos = 0;
int direcao = 0;

void loop()
{
  while (neo.available())
  {

    char a = neo.read();
    if (isAscii(a))
    {
      if (a == "{" && lendo == false)
      {
        lendo = true;
        json = "";
        json += a;
      }
      if (lendo)
      {
        json += a;
      }
      if (a == "}" && lendo == true)
      {
        lendo = false;
        json += a;
        doc.clear();
      }
    }
    else
    {
      contErros++;
      if (contErros > 3)
      {
        contErros = 0;
        break;
      }
    }
  }
  if (json.length > 10)
  {
    if (deserializeJson(doc, json) == DeserializationError::Ok)
    {
      pos = doc["POS"];
      direcao = doc["DIR"];
    }
  }

  int dezena = pos / 10;
  int unidade = pos % 10;

  if (pos == 0)
  {
    dezena = BLANK;
    unidade = T;
  }
  if (pos == 1)
  {
    dezena = S;
    unidade = L;
  }
  int i = botao.ler();
  if(i > 0)
  {
    neo.print("p "+String(i-1)+ ";");
  }

  printone(gerartabela(dezena, unidade, direcao));
}

// Serial.println(i);

void printone(bool *array)
{
  // digitalWrite(14, LOW);
  // delayMicroseconds(3);

  for (int k = 0; k < 10; k++)
  {
    int i = botao.ler();
    if (i == 0)
    {
      i = botao.ler();
    }

    if (i > 0)
    {
      a = i;
    }

    if (a != 0 && !block)
    {
      Serial.println(i);
      tempo = millis();
      block = true;
    }

    if (block && (millis() - tempo) > 5000)
    {
      block = false;
      botao.apagarLed(a);
      a = 0;
    }

    for (int i = 0; i < 13; i++)
    {
      if (array[i])
      {

        digitalWrite(14, 1);
        delay(3);
        digitalWrite(14, 0);
        delay(3);
        digitalWrite(14, 0);
        delay(3);
        digitalWrite(14, 0);
        delay(3);
      }
      else
      {
        digitalWrite(14, 1);
        delay(3);
        digitalWrite(14, 1);
        delay(3);
        digitalWrite(14, 1);
        delay(3);
        digitalWrite(14, 0);
        delay(3);
      }
    }
    for (int i = 0; i < 5; i++)
    {
      digitalWrite(14, 1);
      delay(3);
    }
    for (int i = 0; i < 5; i++)
    {
      digitalWrite(14, 0);
      delay(3);
    }
  }

  // digitalWrite(14, HIGH);
}

// 26 11 57 = 19
// 26 10 57 = 177
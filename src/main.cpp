#include <Arduino.h>
#include <ButtonPCF8574.h>
#include <ArduinoJson.h>
#include <NeoSWSerial.h>
#include <TimerOne.h>

#define S 22
#define L 19
#define T 12
#define BLANK 10

#define LED_PIN 13
DynamicJsonDocument doc(1024);

ButtonPCF8574 botao;
ButtonPCF8574 botao2;

NeoSWSerial neo(15, 16);
String json;

int i = 0;
int a = 0;
unsigned long tempo = 0;
bool block = false;

void ler()
{
  i = botao.read();

  if (i > -1)
  {
    Serial.print("p " + String(i - 1) + ";");
  }
  else
  {
    i = botao2.read();
    if (i > -1)
    {
      Serial.print("p " + String(i) + ";");
    }
  }

  if (i > -1)
  {
    a = i;
    i = 0;
  }

  if (a != 0 && !block)
  {
    // Serial.println(i);
    tempo = millis();
    block = true;
  }

  if (block && (millis() - tempo) > 5000)
  {
    block = false;
    if (a > 7)
    {

      botao2.turnoff(a);
    }
    else
    {
      botao.turnoff(a);
    }
    a = 0;
  }
}
void setup()
{

  Serial.begin(9600);
  neo.begin(9600);
  pinMode(14, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(14, HIGH);

  botao = ButtonPCF8574(0x22, 0, 0x20, 1);
  botao2 = ButtonPCF8574(0x23, 0, 0x21, 8);

  Serial.print(F("Status Botão 1: "));
  Serial.println(botao.enabledBots);
  Serial.print(F("Status Botão 2: "));
  Serial.println(botao2.enabledBots);
  Serial.print(F("Status Led 1: "));
  Serial.println(botao.enabledLeds);
  Serial.print(F("Status Led 2: "));
  Serial.println(botao2.enabledLeds);
}

void printone(bool *array);

bool array[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

bool *gerartabela(int dezena, int unidade, int seta)
{
  //{0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1};

  for (size_t i = 0; i < 13; i++)
  {
    array[i] = 0;
  }

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

bool lendo = false;
int contErros = 0;
int pos = 0;
int direcao = 0;

void loop()
{
  while (Serial.available())
  {

    char a = Serial.read();
    if (isAscii(a))
    {
      if (a == '{' && lendo == false)
      {
        lendo = true;
        json = "";
        // json += a;
      }
      if (lendo)
      {
        json += a;
      }
      if (a == '}' && lendo == true)
      {
        lendo = false;
        // json += a;
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
  if (json.length() > 5)
  {

    if (deserializeJson(doc, json) == DeserializationError::Ok)
    {

      if (doc.containsKey("POS"))
      {
        pos = doc["POS"];
      }
      if (doc.containsKey("DIRECAO"))
      {
        direcao = doc["DIRECAO"];
      }
      if (doc.containsKey("apagar"))
      {

        botao.turnoff(doc["apagar"]);
      }
    }
    doc.clear();
  }

  int dezena = pos / 10;
  int unidade = pos % 10;

  if (unidade == 0 && dezena == 0)
  {
    unidade = BLANK;
  }
  if (dezena == 0)
  {
    dezena = BLANK;
  }
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

  printone(gerartabela(dezena, unidade, direcao));
}

// Serial.println(i);

void printone(bool *array)
{
  // digitalWrite(14, LOW);
  // delayMicroseconds(3);

  for (int k = 0; k < 10; k++)
  {

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
      ler();
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

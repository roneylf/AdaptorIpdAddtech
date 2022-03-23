#include <ButtonPCF8574.h>

ButtonPCF8574::ButtonPCF8574() {}
ButtonPCF8574::ButtonPCF8574(int addressBot, int id)
{
  Wire.begin();
  Wire.beginTransmission(addressBot);
  Wire.write(B00000000);
  Wire.endTransmission();
  this->enabledLeds = false;
  this->addressBot = addressBot;
}

ButtonPCF8574::ButtonPCF8574(int addressBot, int id, int addressLed, int cont)
{
  Wire.begin();
  Wire.beginTransmission(addressBot);
  Wire.write(B11111111);
  this->enabledBots = (Wire.endTransmission() == 0);
  Serial.println();

  Wire.beginTransmission(addressLed);
  Wire.write(B00000000);
  this->enabledLeds = Wire.endTransmission() == 0;
  this->addressBot = addressBot;
  this->addressLed = addressLed;
  this->cont = cont;
}

ButtonPCF8574::~ButtonPCF8574() {}

int ButtonPCF8574::read()
{
  Wire.requestFrom(this->addressBot, 1); // 1 ByteF
  if (Wire.available())
  {
    byte a = Wire.read();

    // B11111110;
    a = ~a;

    for (int i = 0; i < 8; i++)
    {
      if (((a & (B00000001 << i)) == (B00000001 << i)) != this->pins[i])
      {
        this->pins[i] = ((a & (B00000001 << i)) == (B00000001 << i));
        if (this->pins[i] == 1)
        {
          ButtonPCF8574::lightup(i);
          return i + cont;
        }
      }
    }
  }
  return -1;
}

void ButtonPCF8574::lightup(int id)
{
  if (this->enabledLeds)
  {
    this->ledbuffer |= (B00000001 << id);
    Wire.beginTransmission(this->addressLed);
    Wire.write(ledbuffer);
    Wire.endTransmission();
  }
}

void ButtonPCF8574::turnoff(int id)
{
  if (this->enabledLeds)
  {
    this->ledbuffer = this->ledbuffer ^ (B00000001 << (id - cont));
    Wire.beginTransmission(this->addressLed);
    Wire.write(ledbuffer);
    Wire.endTransmission();
  }
}

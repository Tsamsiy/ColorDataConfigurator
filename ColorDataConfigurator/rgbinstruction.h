#ifndef RGBINSTRUCTION_H
#define RGBINSTRUCTION_H

#include <cstdint>
#include <QString>

//definitions
#define StartDelim '{'
#define StopDelim '}'
#define BlockDelim ';'
#define CommandDelim ':'
#define ParamDelim ','

//cstring utility:
uint8_t indxOf(const char* str, uint8_t len, char val, bool mode = 0);
void strRotRgt(char* str, uint8_t len);
uint8_t strLen(const char* str, uint8_t len = 0xFF);
void strCpy(const char* strA, char* strB, uint8_t nA, uint8_t lenB);

//#####################################################################################################################################################################################
class RGBInstruction
{
public:
    RGBInstruction();
    RGBInstruction(const QString& str);

    //3byte rgb value
    struct HEXColor
    {
      uint8_t r = 0x00;
      uint8_t g = 0x00;
      uint8_t b = 0x00;

      HEXColor& operator=(uint8_t rhs[3])
      {
          this->r = rhs[0];
          this->g = rhs[1];
          this->b = rhs[2];

          return *this;
      }
    };

    //list of all possible commands
    enum CommandID
    {
      C,
      F,
      L
    };

    //encodes how the LEDs are controlled
    //11bytes total (no compression)
    uint8_t com = C;      //which command. determines which parameters are relevant.
    HEXColor c1;      //color parameter
    HEXColor c2;      //optional color parameter
    uint16_t x = 0;    //number of skips on loopback
    uint16_t t = 0;   //delay value

    bool parse(const QString& str, QString& error);     //calls parseCommand after converting QString to char array
    QString print() const;                    //converts char array from exportCommand into QString

private:
    bool parseCommand(const char* str, uint8_t len, char* errStr, uint8_t errStrLen);    //converts string to RGBInstruction object
    void exportCommand(char* str, uint8_t len) const;         //converts RGBInstruction object to string

    bool HEXtoBIN(bool mode, char c, uint8_t& b);
    void BINtoHEX(bool mode, char& c, uint8_t b) const;
    bool StrToInt(const char* str, uint8_t len, uint16_t& num);
    void IntToStr(char* str, uint8_t len, uint16_t num) const;
    bool parseColor(const char* str, HEXColor& c);
    void exportColor(char* str, const HEXColor& c) const;
};

#endif // RGBINSTRUCTION_H

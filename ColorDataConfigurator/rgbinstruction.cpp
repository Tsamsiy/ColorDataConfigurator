#include "rgbinstruction.h"

#include <string.h>

#define DEBUG
#ifdef DEBUG
  #include <QDebug>
#endif

//#####################################################################################################################################################################################
RGBInstruction::RGBInstruction()//------------------------------------------------------------
{

}

RGBInstruction::RGBInstruction(const QString& str)//------------------------------------------------------------
{
    QString err;        //error is discarded
    this->parse(str, err);
    #ifdef DEBUG
        if(!err.isEmpty()) qDebug() << "RGBInstruction() bad argument: " << err;
    #endif
}

//#####################################################################################################################################################################################
bool RGBInstruction::parse(const QString& str, QString& error)//------------------------------------------------------------
{
    char errStr[64];
    errStr[0] = '\0';
    bool res = false;

    std::string cnvStr = str.toStdString();
    res =  this->parseCommand(cnvStr.c_str(), cnvStr.length(), errStr, sizeof(errStr));
    error = errStr;
    return res;
}

QString RGBInstruction::print() const//------------------------------------------------------------
{
    //maximum strLen if all parameters are printed: "-:rrggbb,rrggbb,65535,65535;" -> 28 chars + 1 termination
    char str[32];       //exportCommand(adds termination)
    this->exportCommand(str, sizeof(str));
    QString cnvStr = str;
    return cnvStr;
}

//expects a complete command block sequence, including delimiter at the end, without formatting
bool RGBInstruction::parseCommand(const char* str, uint8_t len, char* errStr, uint8_t errStrLen)//------------------------------------------------------------
{
    //get critical values
    uint8_t blockDelimIndex = indxOf(str, len, BlockDelim);
    uint8_t commandDelimIndex = indxOf(str, len, CommandDelim);
    uint8_t paramsDelimIndex[2];
    uint8_t numParamDelims = 0;

    //check if delimiters are present
    //indxOf returns 0 if not found, at index 0, command identifier is expected, at index 1 command delimiter
    if((commandDelimIndex != 1) || (blockDelimIndex == 0))
    {
        #ifdef DEBUG
            qDebug() << "no(indx | cmd)";
        #endif
        strCpy("no(indx | cmd)", errStr, errStrLen, errStrLen);

        return false;
    }

    //find all parameter delimiter indexes
    uint8_t tmpIndx = commandDelimIndex + 1;
    for(uint8_t i = 0; i < 2; i++)
    {
      paramsDelimIndex[i] = 0;

      uint8_t indx = indxOf(&str[tmpIndx], len, ParamDelim);
      if(indx > 0)
      {
        paramsDelimIndex[i] = indx + tmpIndx;
        tmpIndx += indx + 1;

        numParamDelims++;
      }
    }

    //identify command char
    switch(str[0])
    {
      case 'C':   //C:rrggbb,t;
        //check for length
        if(blockDelimIndex < 10)
        {
          #ifdef DEBUG
            qDebug() << "invl blckLen";
          #endif
          strCpy("invl blckLen", errStr, errStrLen, errStrLen);

          return false;
        }

        //check number of parameters
        if(numParamDelims != 1)
        {
          #ifdef DEBUG
            qDebug() << "invl numPrms";
          #endif
          strCpy("invl numPrms", errStr, errStrLen, errStrLen);

          return false;
        }

        this->com = C;

        //color parameter
        if(!parseColor(&str[commandDelimIndex + 1], this->c1))
        {
          #ifdef DEBUG
            qDebug() << str[0] << ":!c1";
          #endif
          errStr[0] = str[0];
          strCpy(":!c1", &errStr[1], errStrLen, errStrLen);

          return false;
        }

        //time parameter
        if(!StrToInt(&str[paramsDelimIndex[0] + 1], len - paramsDelimIndex[0] + 1, this->t))
        {
          #ifdef DEBUG
            qDebug() << str[0] << ":!t";
          #endif
          errStr[0] = str[0];
          strCpy(":!t", &errStr[1], errStrLen, errStrLen);

          return false;
        }
      break;

      case 'F':   //F:rrggbb,rrggbb,t;
        //check for length
        if(blockDelimIndex < 17)
        {
          #ifdef DEBUG
            qDebug() << "invl blckLen";
          #endif
          strCpy("invl blckLen", errStr, errStrLen, errStrLen);

          return false;
        }

        //check number of parameters
        if(numParamDelims != 2)
        {
          #ifdef DEBUG
            qDebug() << "invl numPrms";
          #endif
          strCpy("invl numPrms", errStr, errStrLen, errStrLen);

          return false;
        }

        this->com = F;

        //first color parameter
        if(!parseColor(&str[commandDelimIndex + 1], this->c1))
        {
          #ifdef DEBUG
            qDebug() << str[0] << ":!c1";
          #endif
          errStr[0] = str[0];
          strCpy(":!c1", &errStr[1], errStrLen, errStrLen);

          return false;
        }

        //second color parameter
        if(!parseColor(&str[paramsDelimIndex[0] + 1], this->c2))
        {
          #ifdef DEBUG
            qDebug() << str[0] << ":!c2";
          #endif
          errStr[0] = str[0];
          strCpy(":!c2", &errStr[1], errStrLen, errStrLen);

          return false;
        }

        //time parameter
        if(!StrToInt(&str[paramsDelimIndex[1] + 1], len - paramsDelimIndex[1] + 1, this->t))
        {
          #ifdef DEBUG
            qDebug() << str[0] << ":!t";
          #endif
          errStr[0] = str[0];
          strCpy(":!t", &errStr[1], errStrLen, errStrLen);

          return false;
        }
      break;

      case 'L':   //L:x,t;
        //check for length
        if(blockDelimIndex < 5)
        {
          #ifdef DEBUG
            qDebug() << "invl blckLen";
          #endif
          strCpy("invl blckLen", errStr, errStrLen, errStrLen);

          return false;
        }

        //check number of parameters
        if(numParamDelims != 1)
        {
          #ifdef DEBUG
            qDebug() << "invl numPrms";
          #endif
          strCpy("invl numPrms", errStr, errStrLen, errStrLen);

          return false;
        }

        this->com = L;

        //skip parameter
        uint16_t tempX;
        if(!StrToInt(&str[commandDelimIndex + 1], paramsDelimIndex[0] - (commandDelimIndex + 1), tempX))
        {
          #ifdef DEBUG
            qDebug() << str[0] << ":!x";
          #endif
          errStr[0] = str[0];
          strCpy(":!x", &errStr[1], errStrLen, errStrLen);
          return false;
        }
        this->x = tempX;

        //time parameter
        if(!StrToInt(&str[paramsDelimIndex[0] + 1], len - paramsDelimIndex[0] + 1, this->t))
        {
          #ifdef DEBUG
            qDebug() << str[0] << ":!t";
          #endif
          errStr[0] = str[0];
          strCpy(":!t", &errStr[1], errStrLen, errStrLen);

          return false;
        }
      break;

      default: //unknown command
        #ifdef DEBUG
          qDebug() << "ukwn cmd";
        #endif
        strCpy("ukwn cmd", errStr, errStrLen, errStrLen);

        return false;
      break;
    }

    return true;
}

//exports an object as a complete command block sequence with delimiter, without formating
void RGBInstruction::exportCommand(char* str, uint8_t len) const//------------------------------------------------------------
{
    uint8_t strPtr = 0;
    char tmpNum[6];      //max(uint16_t) = 65535 -> 5 chars + 1 termination
    uint8_t nTmpNum = 0;

    switch(this->com)
    {
        case C:
            str[strPtr] = 'C';
            strPtr++;

            str[strPtr] = CommandDelim;
            strPtr++;

            this->exportColor(&str[strPtr], this->c1);
            strPtr += 2 * sizeof(HEXColor);     //2 chars per Hex byte

            str[strPtr] = ParamDelim;
            strPtr++;

            IntToStr(tmpNum, sizeof(tmpNum), this->t);
            nTmpNum = strLen(tmpNum, sizeof(tmpNum));
            strCpy(tmpNum, &str[strPtr], nTmpNum, (len - strPtr));
            strPtr += nTmpNum;

            str[strPtr] = BlockDelim;
            strPtr++;
        break;

        case F:
            str[strPtr] = 'F';
            strPtr++;

            str[strPtr] = CommandDelim;
            strPtr++;

            this->exportColor(&str[strPtr], this->c1);
            strPtr += 2 * sizeof(HEXColor);     //2 chars per Hex byte

            str[strPtr] = ParamDelim;
            strPtr++;

            this->exportColor(&str[strPtr], this->c2);
            strPtr += 2 * sizeof(HEXColor);     //2 chars per Hex byte

            str[strPtr] = ParamDelim;
            strPtr++;

            IntToStr(tmpNum, sizeof(tmpNum), this->t);
            nTmpNum = strLen(tmpNum, sizeof(tmpNum));
            strCpy(tmpNum, &str[strPtr], nTmpNum, (len - strPtr));
            strPtr += nTmpNum;

            str[strPtr] = BlockDelim;
            strPtr++;
        break;

        case L:
            str[strPtr] = 'L';
            strPtr++;

            str[strPtr] = CommandDelim;
            strPtr++;

            IntToStr(tmpNum, sizeof(tmpNum), this->x);
            nTmpNum = strLen(tmpNum, sizeof(tmpNum));
            strCpy(tmpNum, &str[strPtr], nTmpNum, (len - strPtr));
            strPtr += nTmpNum;

            str[strPtr] = ParamDelim;
            strPtr++;

            IntToStr(tmpNum, sizeof(tmpNum), this->t);
            nTmpNum = strLen(tmpNum, sizeof(tmpNum));
            strCpy(tmpNum, &str[strPtr], nTmpNum, (len - strPtr));
            strPtr += nTmpNum;

            str[strPtr] = BlockDelim;
            strPtr++;
        break;

        default:
            //nope
        break;
    }

    //add final termination
    str[strPtr] = '\0';
}

//converts a char HEX code to binary with selectable low or high nibble. only overwrites the selected part. returns false if invalid char
bool RGBInstruction::HEXtoBIN(bool mode, char c, uint8_t& b)//------------------------------------------------------------
{
    //check if provided character is ok
    bool ok = false;
    if((c >= 'A') && (c <= 'F'))
    {
          c = c - 'A' + 10;
          ok = true;
    }
    if((c >= 'a') && (c <= 'f'))
    {
          c = c - 'a' + 10;
          ok = true;
    }
    if((c >= '0') && (c <= '9'))
    {
          c = c - '0';
          ok = true;
    }

    //convert and save if char is ok
    if(ok)
    {
        if(mode)
        {
            //erase only upper 4 bits
            b &= 0x0F;
            //set bits as needed
            b |= c << 4;
        }
        else
        {
            //erase only lower 4 bits
            b &= 0xF0;
            //set bits as needed
            b |= c;
        }
    }

    return ok;
}

//converts selected high or low nibble to a HEX code char
void RGBInstruction::BINtoHEX(bool mode, char& c, uint8_t b) const//------------------------------------------------------------
{
    if(mode)
    {
        //erase only lower 4 bits
        b &= 0xF0;
        b >>= 4;
    }
    else
    {
        //erase only upper 4 bits
        b &= 0x0F;
    }
    //number or letter
    if(b > 9)
    {
        c = b - 10 + 'A';
    }
    else
    {
        c = b + '0';
    }
}

//converts a string of numbers . returns false if invalid sequence. terminates if null, a non-number character or len is reached
bool RGBInstruction::StrToInt(const char* str, uint8_t len, uint16_t& num)//------------------------------------------------------------
{
    bool ok = false;

    for(uint8_t i = 0; i < len; i++)
    {
        //check if is number    //null also is a non-number char
        if((str[i] >= '0') && (str[i] <= '9'))
        {
            //only overwrites if number is found
            if(!ok)
            {
                num = 0;
                ok = true;
            }

            num *= 10;
            num += str[i] - '0';
        }
        else
        {
            return ok;
        }
    }
    return ok;
}

//creates null terminated left bounded number string in provided buffer. terminates if buffer is full or number ended
void RGBInstruction::IntToStr(char* str, uint8_t len, uint16_t num) const//------------------------------------------------------------
{
    bool didConv = false;
    str[0] = '\0';

    //itterate through buffer
    for(uint8_t i = 0; ((i < len) && (num > 0)); i++)
    {
        strRotRgt(str, len);
        str[0] = num % 10 + '0';
        num /= 10;

        didConv = true;
    }

    //need this if num is 0
    if(!didConv)
    {
        strRotRgt(str, len);
        str[0] = '0';
    }
}

//converts string HEX RGB code to binary. excluding delimiters. expects 2 chars per color, 6 total
bool RGBInstruction::parseColor(const char* str, RGBInstruction::HEXColor& c)//------------------------------------------------------------
{
    //convert chars to their value
    bool ok = true;
    for(uint8_t i = 0; i < 2; i++)
    {
        uint8_t mode = (i + 1) % 2;
        //clear bit if conversion fails
        ok &= HEXtoBIN(mode, str[i + 0], c.r);
        ok &= HEXtoBIN(mode, str[i + 2], c.g);
        ok &= HEXtoBIN(mode, str[i + 4], c.b);
    }
    return ok;
}

//converts a color object to string, without delimiter. buffer is expected to be 6 long
void RGBInstruction::exportColor(char* str, const HEXColor& c) const//------------------------------------------------------------
{
    for(uint8_t i = 0; i < 2; i++)
    {
        uint8_t mode = (i + 1) % 2;

        BINtoHEX(mode, str[i + 0], c.r);
        BINtoHEX(mode, str[i + 2], c.g);
        BINtoHEX(mode, str[i + 4], c.b);
    }
}

//#####################################################################################################################################################################################
//returns the index of given character, 0 if not found. switchable search direction
uint8_t indxOf(const char* str, uint8_t len, char val, bool mode)//------------------------------------------------------------
{
    uint8_t lastIndx = 0;

    for(uint8_t i = 0; i < len; i++)
    {
        //match
        if(*str == val)
        {
            if(!mode)   //indexOf()
            {
              return i;
            }
            else    //lastIndexOf()  less efficient like this, but less flash
            {
              lastIndx = i;
            }
        }
        str++;
    }
    return lastIndx;
}

//rotates string one position to right in the given buffer length with no regard for termination
void strRotRgt(char* str, uint8_t len)//------------------------------------------------------------
{
    char temp = str[len - 1];
    for(uint8_t i = len - 1; i > 0; i--)
    {
        str[i] = str [i - 1];
    }
    str[0] = temp;
}

//returns the length of a string within an array. It terminates if null is found or len is reached
uint8_t strLen(const char* str, uint8_t len)//###########################################################################################################################################################################################################
{
    for(uint8_t i = 0; i < len; i++)
    {
        if(*str == NULL)
        {
            return i;
        }
        str++;
    }
    return len;
}

//copies contents of strA into strB. adds a null terminator. nA is how many chars should be copied, lenB is buffer size of strB. terminates if lenB is reached or null in strA is found
void strCpy(const char* strA, char* strB, uint8_t nA, uint8_t lenB)//###########################################################################################################################################################################################################
{
    for(uint8_t i = 0; i < nA; i++)
    {
        *strB = *strA;

        strA++;
        strB++;

        //buffer full
        if(((i + 1) == lenB) || (*strA == '\0'))
        {
            break;
        }
    }

    //add a null terminator
    //strB++;   //is already moved there
    *strB = '\0';
}

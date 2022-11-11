/*  PS/2 to keyboard matrix for the TRS-80 model I
    based on PS2KeyMap and PS2KeyAdvanced libraries, 
    Written by Paul Carpenter, PC Services <sales@pcserviceselectronics.co.uk>

    This code is written by Fred Jan Kraan, fjkraan@electrickery.nl
    Like the original libraries this is under LGPL license.

    The ASCII code produced by PS2KeyMap is combined with the keyboard
    code from PS2KeyAdvanced to get a complete set of required key-presses
    (both visible as modifier keys).
*/

#define VERSION 0.5
   
#include <PS2KeyAdvanced.h>
// Include all mappings
#include <PS2KeyMap.h>

/* Keyboard constants  Change to suit your Arduino
   define pins used for data and clock from keyboard */
#define DATAPIN 2
#define IRQPIN  3

// MT8816 pins
#define MT_RESET A0
#define MT_DATA  A1
#define MT_CS    11
#define MT_STB   12

#define MT_ADDRPINCOUNT 7
//     correct order                          AX0 AX1 AX2 AX3 AY0 AY1 AY2
const uint8_t mt_addrPins[MT_ADDRPINCOUNT] = { 4,  5,  6,  7,  8,  9,  10 };

//   wrong order                              AY0 AY1 AY2 AX0 AX1 AX2 AX3
//const uint8_t mt_addrPins[MT_ADDRPINCOUNT] = { 8,  9,  10, 4,  5,  6,  7  };

#define MT_XRANGE 16

#define CT_RESET A2

//  Fixing pin-order bug in MT8816. This is only the AX-nibble, the AY nibble is not mangled.
//  Only X0 to X7 is used.        X0    X1    X2    X3    X4    X5    X6    X7
uint8_t mt8816Map[MT_XRANGE] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x08, 0x09, 
//                                X8    X9    X10   X11   X12   X13   X14   X15 
                                 0x0A, 0x0B, 0x0C, 0x0D, 0x06, 0x07, 0x0E, 0x0F}; 

PS2KeyAdvanced keyboard;
PS2KeyMap keymap;

uint16_t kcode;
uint16_t code;

#include "ascii2trs80m1keymap.h"

#define DEBUG 1


#define SERIALBUFSIZE         10
char serialBuffer[SERIALBUFSIZE];
byte setBufPointer = 0;



void setup() {
    Serial.begin(115200);
    Serial.print("PS2Keyboard to Aster CT-80 matrix V");
    Serial.print(VERSION);
    Serial.println();
    // Start keyboard setup while outputting
    keyboard.begin(DATAPIN, IRQPIN);
    // Disable Break codes (key release) from PS2KeyAdvanced
    keyboard.setNoBreak(0);
    // and set no repeat on CTRL, ALT, SHIFT, GUI while outputting
    keyboard.setNoRepeat(0);
    
    // clearing internal matrix array
    for (uint8_t i = 0; i < ROWCOUNT; i++) {
      rows[i] = 0xFF;
    }
    // initialise the key pressed table
    for (byte i = 0; i < KEYPRESSEDMAX; i++) {
      keysPressed[i] = 0;
    }
    // setting the pins MT signals are active high
    digitalWrite(MT_RESET, LOW);
    pinMode(MT_RESET, OUTPUT);
    digitalWrite(MT_DATA, LOW);
    pinMode(MT_DATA, OUTPUT);
    digitalWrite(MT_CS, LOW);
    pinMode(MT_CS, OUTPUT);
    digitalWrite(MT_STB, LOW);
    pinMode(MT_STB, OUTPUT);
  
    setAddrPinsOutput();
    resetMT();
}


void loop() {
    bool release = 0;
    bool keyPressed = keyboard.available();
    if (keyPressed > 0) {
        bool modHandled = 0;
        bool specialHandled = 0;
        kcode = keyboard.read();
        release = 0;
        if (kcode > PS2RELEASE) {
            release = 1;
        }
        Serial.print("kcode: ");
        Serial.print(kcode, HEX);
        code = keymap.remapKey(kcode) & 0xFF;
        Serial.print(", code: ");
        Serial.print(code, HEX );
        Serial.print(", ");
        
        modHandled = modifierKeyHandler(kcode);  // Shift (and later Control)
        if (modHandled) {
            Serial.print("modifier handled key ");
        }
        specialHandled = specialKeyHandler(kcode);   // Esc, Tab, Backspace, Enter, ....
        if (specialHandled) {
            Serial.print("special handled key");
        }
        if (code >= 0x21 && code <= 0x7F) {
            uint16_t matrixPos = a2miMap(code);
            if (matrixPos == 0xFF) {
                Serial.print("not a valid key");
            } else {    // it is a valid key
                Serial.print(" ( ");
                Serial.write(code & 0xFF);
                Serial.print(" ) ");
                if (release) {
                    clearKey(matrixPos);
                } else {
                    setKey(matrixPos);
                }
            }
        }        
        Serial.println();
    }  // if (keyPressed > 0)
//    delay(KEYSCANRATE);  
    commandCollector();
}

bool modifierKeyHandler(uint16_t kcode) {
    // press
    if (kcode == PS2LSHIFT) {
        setKey(MXLSHIFT);
        return 1;
    }
    if (kcode == PS2RSHIFT) {
        setKey(MXLSHIFT);
        return 1;
    }
    // release
    if (kcode == PS2LSHIFT_R) {
        clearKey(MXLSHIFT);
        return 1;
    }
    if (kcode == PS2RSHIFT_R) {
        clearKey(MXLSHIFT);
        return 1;
    }
    return 0;
}
    
bool specialKeyHandler(uint16_t kcode) {
    if (kcode == PS2ENTER || kcode == PS2NUMENT ) {
        setKey(MXENTER);
        return 1;
    }
    if (kcode == PS2ENTER_R || kcode == PS2NUMENT_R) {
        clearKey(MXENTER);
        return 1;
    }
    if (kcode == PS2HOME) {
        setKey(MXCLEAR);
        return 1;
    }
    if (kcode == PS2HOME_R) {
        clearKey(MXCLEAR);
        return 1;
    }
    if (kcode == PS2END) {
        setKey(MXBREAK);
        return 1;
    }
    if (kcode == PS2END_R) {
        clearKey(MXBREAK);
        return 1;
    }
    if (kcode == PS2BACKSP) {
        setKey(MXLEFT);
        return 1;
    }
    if (kcode == PS2BACKSP_R) {
        clearKey(MXLEFT);
        return 1;
    }
    if (kcode == PS2LEFT) {
        setKey(MXLEFT);
        return 1;
    }
    if (kcode == PS2LEFT_R) {
        clearKey(MXLEFT);
        return 1;
    }
    if (kcode == PS2RIGHT) {
        setKey(MXRIGHT);
        return 1;
    }
    if (kcode == PS2RIGHT_R) {
        clearKey(MXRIGHT);
        return 1;
    }
    if (kcode == PS2UP) {
        setKey(MXUP);
        return 1;
    }
    if (kcode == PS2UP_R) {
        clearKey(MXUP);
        return 1;
    }
    if (kcode == PS2DOWN) {
        setKey(MXDOWN);
        return 1;
    }
    if (kcode == PS2DOWN_R) {
        clearKey(MXDOWN);
        return 1;
    }
    if (kcode == PS2SPACE) {
        setKey(MXSPACE);
        return 1;
    }
    if (kcode == PS2SPACE_R) {
        clearKey(MXSPACE);
        return 1;
    }
    if (kcode == PS2AT) {
        clearKey(MXLSHIFT);
        setKey(MXAT);
        return 1;
    }
    if (kcode == PS2AT_R) {
        clearKey(MXAT);
        return 1;
    }
    
    return 0;
}

// mapping visible characters to matrix coordinates (row, bit)
uint8_t a2miMap(uint16_t code) {
    uint16_t mapCode = code - 0x20; 
    if (DEBUG) {
      Serial.print(", a2km[");
      Serial.print(mapCode, HEX);
      Serial.print("]= ");
    }
    if (mapCode >= A2KMSIZE) {
      if (DEBUG) Serial.print("- ");
      return 0; // code > 128, not ASCII
    }
    uint8_t mxCode = a2km[mapCode];
    if (DEBUG) Serial.print(mxCode, HEX);    
    return mxCode;
}

// MT8816 chip interface

void setAddrPinsOutput() {
    for (uint8_t i = 0; i < MT_ADDRPINCOUNT; i++) {
        pinMode(mt_addrPins[i], OUTPUT);
    }
}

void setAddr(uint8_t addr) {  // Original code where nibbles are reversed. Don't know where yet.
    uint8_t axNibble = addr & 0x0F;         // lower nibble
    uint8_t ayNibble = (addr & 0xF0) >> 4;  // upper nibble
    
    Serial.print(" ad: ");
    printBin(ayNibble, 3);
    Serial.print(".");
    printBin(axNibble, 4);
    Serial.print(", ");
    
    uint8_t mtFixAxNib = mt8816Map[axNibble]; // fix out of order AX? addresses
    uint8_t mxAddr     = (ayNibble << 4) | mtFixAxNib;  
    
    Serial.print(" mx: ");
    printBin(ayNibble, 3);
    Serial.print(".");
    printBin(mtFixAxNib,4);
//    Serial.print(" [");
//    Serial.print(mxAddr, BIN);
//    Serial.print("]");
    
    // order is LSb - MSb: AX0, AX1, AX2, AX3, AY0, AY1, AY2
    for (uint8_t i = 0; i < MT_ADDRPINCOUNT; i++) {
        bool addrBit = mxAddr & (1 << i);
        digitalWrite(mt_addrPins[i], addrBit);
    }
    Serial.print(", ");
} 

void setKey(uint8_t addr) {
    changeKey(addr, 1);
}

void clearKey(uint8_t addr) {
    changeKey(addr, 0);
}

void changeKey(uint8_t addr, bool pressed) {
    // set CS
    digitalWrite(MT_CS, HIGH);
    // set address
    setAddr(addr);
    // set STROBE
    digitalWrite(MT_STB, HIGH);
    // set DATA
    digitalWrite(MT_DATA, pressed ? HIGH : LOW);
    // clear STROBE
    digitalWrite(MT_STB, LOW); // This is the moment data is latched
    // clear CS
    digitalWrite(MT_CS, LOW);
    
    Serial.print(" data: ");
    Serial.print(pressed, BIN);
    Serial.print(" ");
}

void resetMT() {
    // set CS
    digitalWrite(MT_CS, HIGH);
    // set STROBE
    digitalWrite(MT_STB, HIGH);
    // set RESET
    digitalWrite(MT_RESET, HIGH);
    delayMicroseconds(1); // 40 - 100 ns
    // clear RESET
    digitalWrite(MT_RESET, LOW);
    // clear CS
    digitalWrite(MT_CS, LOW);
    // clear STROBE
    digitalWrite(MT_STB, LOW);    
}

void printBin(uint8_t value, uint8_t size) {
    for (uint8_t i = size; i > 0; i--) {
        bool bit = value & 1 << i-1;
        Serial.print(bit);
    }
}



void clearSerialBuffer() {
  byte i;
  for (i = 0; i < SERIALBUFSIZE; i++) {
    serialBuffer[i] = 0;
  }
}

void commandCollector() {
  if (Serial.available() > 0) {
    int inByte = Serial.read();
    switch(inByte) {
    case '.':
//    case '\r':
    case '\n':
      commandInterpreter();
      clearSerialBuffer();
      setBufPointer = 0;
      break;
    case '\r':
      break;  // ignore carriage return
    default:
      serialBuffer[setBufPointer] = inByte;
      setBufPointer++;
      if (setBufPointer >= SERIALBUFSIZE) {
        Serial.println("Serial buffer overflow. Cleanup.");
        clearSerialBuffer();
        setBufPointer = 0;
      }
    }
  }
}

void commandInterpreter() {
  byte bufByte = serialBuffer[0];
  
  switch(bufByte) {
    case 'C':
    case 'c':
      closeXPoint();
      break;
    case 'H':
    case 'h':
    case '?':
      usage();
      break;
    case 'O':
    case 'o':
      openXPoint();
      break;
    case 'R':
    case 'r':
      resetMT();
      Serial.println("R: MT8816 reset");
      break;
    default:
      Serial.print(bufByte);
      Serial.print(" ");
      Serial.println("unsupported");
      return;
  }
}

void closeXPoint() {
    unsigned int address;
    if (setBufPointer == 3) {
        address = get8BitValue(1);
        Serial.print("C");
        if (address < 0x10) Serial.print("0");
        Serial.print(address, HEX);
        Serial.print(":");
        setKey(address);
    } else {
        Serial.println("unsupported"); 
        return;
    }
    Serial.println();
}

void openXPoint() {
    unsigned int address;
    if (setBufPointer == 3) {
        address = get8BitValue(1);
        Serial.print("O");
        if (address < 0x10) Serial.print("0");
        Serial.print(address, HEX);
        Serial.print(":");
        clearKey(address);
    } else {
        Serial.println("unsupported"); 
        return;
    }  
    Serial.println();
}

byte get8BitValue(byte index) {
  byte i = index;
  byte data;
  data  = getNibble(serialBuffer[i++]) * (1 << 4);
  data += getNibble(serialBuffer[i++]);
  return data;
}

int getNibble(unsigned char myChar) {
  int nibble = myChar;
  if (nibble > 'F') nibble -= ' ';  // lower to upper case
  nibble -= '0';
  if (nibble > 9) nibble -= 7; // offset 9+1 - A
  return nibble;
}

void usage() {
  Serial.print("PS/2 keyboard to MT8816 interface version V");
  Serial.println(VERSION, 1);
  Serial.println("Usage:");
  Serial.println(" Cnn - close crosspoint nn");
  Serial.println(" Onn - open crosspoint nn");
  Serial.println(" H   - this help");
  Serial.println(" R   - reset MT8816");
}

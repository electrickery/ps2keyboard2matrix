/* 
 *  ASCII 0x20-0x7E:
 *   !"#$%&'()*+,-./0123456789:;<=>?
 *  @ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_
 *  `abcdefghijklmnopqrstuvwxyz{|}~
 *  
 *  
 *  Model 1 (1 based!):
 * bits:  1 2 3 4 5 6 7 8                              TRS-80 MI address
 * row 1: @ A B C D E F G                              0x3801
 * row 2: H I J K L M N O                              0x3802
 * row 3: P Q R S T U V W                              0x3804
 * row 4: X Y Z                                        0x3808
 * row 5: 0  1! 2" 3# 4$ 5% 6& 7/                      0x3810
 * row 6: 8( 9) :* ;+ ,< -= .> /?                      0x3820
 * row 7: ENTER CLEAR BREAK UP DOWN LEFT RIGHT SPACE   0x3840
 * row 8: SHIFT                                        0x3880
 */

#define A2KMSIZE 96
// Mapping of ASCII 0x20 - 0x7E to the matrix. Upper nibble is the address row, 
// lower nibble is the data column bit on that row. Both are 1 based, value 0xFF is not used.
// Not all key values in the table are actually used, SPACE is for instance a special key.

const //                   SPACE !     "     #     $     %     &     '      // index  ASCII
uint8_t a2km[A2KMSIZE] = { 0x76, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0xFF,  // 00-07 (20-27)
//                         (     )     *     +     ,     -     .     /
                           0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,  // 08-0F (28-2F)
//                         0     1     2     3     4     5     6     7
                           0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,  // 10-17 (30-37)
//                         8     9     :     ;     <     =     >     ?
                           0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,  // 18-1F 
//                         @     A     B     C     D     E     F     G
                           0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,  // 20-27 (40-47)
//                         H     I     J     K     L     M     N     O
                           0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,  // 28-2F 
//                         P     Q     R     S     T     U     V     W
                           0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,  // 30-37 (50-57)
//                         X     Y     Z     [     \     ]     ^     _
                           0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0xFF,  // 38-3F 
//                         `     a     b     c     d     e     f     g
                           0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,  // 40-47 (60-67)
//                         h     i     j     k     l     m     n     o
                           0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,  // 48-4F 
//                         p     q     r     s     t     u     v     w
                           0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,  // 50-57 (70-77)
//                         x     y     z     {     |     }     ~
                           0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0xFF   // 58-5F (78-7F)
//                   
};

// matrix special keys (non-character)
const uint8_t MXENTER  = 0x60;
const uint8_t MXCLEAR  = 0x61;
const uint8_t MXBREAK  = 0x62;
const uint8_t MXUP     = 0x63;
const uint8_t MXDOWN   = 0x64;
const uint8_t MXLEFT   = 0x65;
const uint8_t MXRIGHT  = 0x66;
const uint8_t MXSPACE  = 0x67;
const uint8_t MXLSHIFT = 0x70;
const uint8_t MXRSHIFT = 0x70;
const uint8_t MXAT     = 0x00;

// The internal matrix array for debugging
#define ROWCOUNT 8
uint8_t rows[ROWCOUNT];

// delay. Might be optional.
#define KEYSCANRATE 50

// keyboard mask for modifier keys
#define PS2RELEASE 0x8000
#define PS2SHIFT   0x4000
#define PS2CONTROL 0x2000

// keyboard codes for non-printables. These are attack codes.
#define PS2LSHIFT 0x4106
#define PS2RSHIFT 0x4107
#define PS2LCTRL  0x2108
#define PS2RCTRL  0x2109
#define PS2LALT   0x090A
#define PS2RALT   0x050B
#define PS2ENTER  0x011E
#define PS2HOME   0x0111 // for CLEAR
#define PS2BREAK  0x6    //  Break is a toggle key 
#define PS2END    0x0112 // for BREAK
#define PS2BACKSP 0x11C
#define PS2LEFT   0x0115
#define PS2RIGHT  0x0116
#define PS2UP     0x0117
#define PS2DOWN   0x0118
#define PS2NUMENT 0x2B
#define PS2SPACE  0x011F
#define PS2AT     0x4032

// keyboard codes for non-printables. These are release codes. 
#define PS2LSHIFT_R 0x8106
#define PS2RSHIFT_R 0x8107
#define PS2LCTRL_R  0x8108
#define PS2RCTRL_R  0x8109
#define PS2LALT_R   0x8109
#define PS2RALT_R   0x810B
#define PS2ENTER_R  0x811E
#define PS2HOME_R   0x8111 // for CLEAR
#define PS2BREAK_R  0x6    // Break is a toggle key 
#define PS2END_R    0x8112 // for BREAK
#define PS2BACKSP_R 0x811C
#define PS2LEFT_R   0x8115
#define PS2RIGHT_R  0x8116
#define PS2UP_R     0x8117
#define PS2DOWN_R   0x8118
#define PS2NUMENT_R 0x802B
#define PS2SPACE_R  0x811F
#define PS2AT_R     0xC032


#define KEYPRESSEDMAX 6
uint16_t keysPressed[KEYPRESSEDMAX];

// functions
bool modKeyHandler(uint16_t kcode);
bool specialKeyHandler(uint16_t kcode);

void setRow(uint8_t matrixPos);
void clearRow(uint8_t matrixPos);
void showRows();

void removeKey (uint16_t kcode);
void addKey (uint16_t kcode);
bool isPressed(uint16_t code);

void setAllPinsInput();
void setDataPinsOutput();
void setAddrPinsOutput();
void setData(uint8_t data);
void setAddr(uint8_t addr);
void writeRow(uint8_t row);
void readRAM();
void fillRAM();

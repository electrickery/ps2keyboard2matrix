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
 * row 5: 0  1! 2" 3# 4$ 5% 6& 7'                      0x3810
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
                           0x50, 0x51, 0x52, 0x53, 0x54, 0x5F, 0x56, 0x57,  // 18-1F 
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
#define MXDEL     0x37
#define MXQUOTE   0x47
#define MXIS      0x55
#define MXCOLON   0x52
#define MXENTER   0x60
#define MXCLEAR   0x61
#define MXBREAK   0x62
#define MXUP      0x63
#define MXDOWN    0x64
#define MXLEFT    0x65
#define MXRIGHT   0x66
#define MXSPACE   0x67
#define MXLSHIFT  0x70
#define MXRSHIFT  0x70
#define MXAT      0x00
#define MXESC     0x71
#define MXF0      0x72
#define MXF1      0x73
#define MXCONT    0x74
#define MXF2      0x75
#define MXF3      0x76
#define MXF4      0x77

// delay. Might be optional.
#define KEYSCANRATE 50

// Press codes

// keyboard mask for modifier keys
#define PS2RELEASE 0x8000
#define PS2SHIFT   0x4000
#define PS2CONTROL 0x2000

// keyboard codes for non-printable keys. These are attack codes.
#define PS2LSHIFT 0x4106
#define PS2RSHIFT 0x4107
#define PS2LCTRL  0x2108
#define PS2RCTRL  0x2109
#define PS2LALT   0x090A
#define PS2RALT   0x050B
#define PS2ENTER  0x011E
#define PS2HOME   0x0111 // for CLEAR
#define PS2BREAK  0x06   //  Break is a toggle key 
#define PS2END    0x0112 // for BREAK
#define PS2DEL    0x011A
#define PS2INS    0x0119 // for Cont
#define PS2BACKSP 0x11C
#define PS2LEFT   0x0115
#define PS2RIGHT  0x0116
#define PS2UP     0x0117
#define PS2DOWN   0x0118
#define PS2NUMENT 0x2B
#define PS2ESC    0x011B
#define PS2FUNC1  0x0161
#define PS2FUNC2  0x0162
#define PS2FUNC3  0x0163
#define PS2FUNC4  0x0164
#define PS2FUNC10 0x016A

// keyboard codes for special-case printables.
#define PS2SPACE  0x011F
#define PS2AT     0x4032
#define PS2QUOTE  0x3A
#define PS2SHFT0  0x4030
#define PS2IS     0x5F
#define PS2COLON  0x405B

// Release codes

// keyboard codes for modifier. These are release codes. 
#define PS2LSHIFT_R 0x8106
#define PS2RSHIFT_R 0x8107
#define PS2LCTRL_R  0x8108
#define PS2RCTRL_R  0x8109

// keyboard release codes for special-case printables.
#define PS2AT_R     0xC032
#define PS2QUOTE_R  0x803A
#define PS2IS_R     0x805F
#define PS2COLON_R  0xC05B
#define PS2FUNC1_R  0x8161
#define PS2FUNC2_R  0x8162
#define PS2FUNC3_R  0x8163
#define PS2FUNC4_R  0x8164

// normal special keys; plain translation of PS/2 keycode to MX8816 code.

#define SPECSIZE 18 * 2
const 
uint16_t special[SPECSIZE] = {
  PS2ENTER,  MXENTER,
  PS2NUMENT, MXENTER,
  PS2HOME,   MXCLEAR,
  PS2END,    MXBREAK,
  PS2BACKSP, MXLEFT,
  PS2LEFT,   MXLEFT,
  PS2RIGHT,  MXRIGHT,
  PS2UP,     MXUP,
  PS2DOWN,   MXDOWN,
  PS2SPACE,  MXSPACE,
  PS2ESC,    MXESC,
  PS2FUNC10, MXF0,
  PS2FUNC1,  MXF1,
  PS2INS,    MXCONT,
  PS2DEL,    MXDEL,
  PS2FUNC2,  MXF2,
  PS2FUNC3,  MXF3,
  PS2FUNC4,  MXF4
};


/*
    PS2AT:    clearKey(MXLSHIFT);
    PS2QUOTE: setKey(MXLSHIFT);
    PS2IS:    setKey(MXLSHIFT);
    PS2COLON: clearKey(MXLSHIFT);
*/
// special unshifting keys; MX8816 codes requiring a press or release of the SHIFT key.
//#define SPSHSIZE 4 * 3
//const uint16_t specshft[SPSHSIZE] = {
//  PS2AT,     MXAT,    0,
//  PS2QUOTE,  MXQUOTE, 1,
//  PS2IS,     MXIS,    1,
//  PS2COLON,  MXCOLON, 0
//};

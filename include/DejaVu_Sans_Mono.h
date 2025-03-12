
#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
// Created by http://oleddisplay.squix.ch/ Consider a donation
// In case of problems make sure that you are using the font file with the correct version!
const uint8_t DejaVu_Sans_Mono_20Bitmaps[] PROGMEM = {

	// Bitmap Data:
	0x00, // ' '
	0xFF,0xFF,0xFC,0x3C, // '!'
	0xCF,0x3C,0xF3,0xCC, // '"'
	0x06,0x60,0x44,0x0C,0xC0,0xCC,0x7F,0xF7,0xFF,0x08,0x81,0x98,0x19,0x8F,0xFE,0xFF,0xE3,0x30,0x33,0x02,0x20,0x22,0x00, // '#'
	0x08,0x04,0x0F,0x8F,0xEE,0x96,0x43,0x20,0xD0,0x3E,0x05,0x82,0x61,0x38,0xBF,0xF9,0xF8,0x10,0x08,0x04,0x00, // '$'
	0x78,0x19,0x83,0x30,0x66,0x0C,0xC0,0xF1,0x80,0xC0,0x30,0x18,0x0C,0x78,0x19,0x83,0x30,0x66,0x0C,0xC0,0xF0, // '%'
	0x1F,0x07,0xE0,0xC0,0x18,0x03,0x00,0x30,0x06,0x03,0xE0,0x6E,0x78,0xCF,0x0D,0xE1,0xEE,0x18,0xFD,0x8F,0x38, // '&'
	0xFF,0xC0, // '''
	0x32,0x66,0x4C,0xCC,0xCC,0xCC,0xC6,0x66,0x23, // '('
	0xC4,0x66,0x63,0x33,0x33,0x33,0x36,0x66,0x4C, // ')'
	0x08,0x04,0x22,0x2D,0x61,0xC0,0xE1,0xAD,0x11,0x08,0x04,0x00, // '*'
	0x0C,0x03,0x00,0xC0,0x30,0xFF,0xFF,0xF0,0xC0,0x30,0x0C,0x03,0x00, // '+'
	0x6D,0xBD,0x80, // ','
	0xFF,0xC0, // '-'
	0xFC, // '.'
	0x00,0xC0,0x60,0x18,0x0C,0x03,0x01,0x80,0x60,0x30,0x0C,0x06,0x01,0x80,0xC0,0x30,0x18,0x06,0x03,0x00, // '/'
	0x3C,0x3F,0x98,0xDC,0x7C,0x1E,0x0F,0x37,0x9B,0xC1,0xE0,0xF0,0x7C,0x76,0x33,0xF8,0xF0, // '0'
	0x38,0xF8,0xD8,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xFF,0xFF, // '1'
	0x7E,0x7F,0xA0,0xE0,0x30,0x18,0x0C,0x0E,0x06,0x06,0x06,0x06,0x06,0x06,0x07,0xFF,0xFE, // '2'
	0x7E,0x7F,0xA0,0xE0,0x30,0x18,0x1C,0x7C,0x3C,0x03,0x00,0xC0,0x60,0x38,0x3F,0xF9,0xF8, // '3'
	0x07,0x01,0xC0,0xF0,0x2C,0x1B,0x04,0xC3,0x31,0x8C,0x63,0x30,0xCF,0xFF,0xFF,0x03,0x00,0xC0,0x30, // '4'
	0x7F,0x3F,0x98,0x0C,0x06,0x03,0xF1,0xFC,0x87,0x01,0x80,0xC0,0x60,0x38,0x37,0xF9,0xF0, // '5'
	0x1E,0x1F,0x9C,0x4C,0x0C,0x06,0x03,0x79,0xFE,0xE3,0xE0,0xF0,0x78,0x36,0x3B,0xF8,0xF8, // '6'
	0xFF,0xFF,0xC0,0xC0,0x60,0x30,0x30,0x18,0x0C,0x0C,0x06,0x03,0x03,0x01,0x80,0xC0,0xC0, // '7'
	0x3E,0x3F,0xB8,0xF8,0x3C,0x1B,0x18,0xF8,0xFE,0x63,0x60,0xF0,0x78,0x3E,0x3B,0xF8,0xF8, // '8'
	0x3E,0x3F,0xB8,0xD8,0x3C,0x1E,0x0F,0x8E,0xFF,0x3D,0x80,0xC0,0x60,0x64,0x73,0xF0,0xF0, // '9'
	0xFC,0x03,0xF0, // ':'
	0x6D,0x80,0x03,0x6D,0xEC, // ';'
	0x00,0x40,0xF0,0xF0,0xF0,0xE0,0x38,0x03,0xC0,0x3C,0x03,0xC0,0x10, // '<'
	0xFF,0xFF,0xF0,0x00,0x00,0xFF,0xFF,0xF0, // '='
	0x80,0x3C,0x03,0xC0,0x3C,0x01,0xC0,0x70,0xF0,0xF0,0xF0,0x20,0x00, // '>'
	0x7C,0xFE,0x87,0x03,0x03,0x07,0x0E,0x1C,0x38,0x30,0x30,0x30,0x00,0x30,0x30, // '?'
	0x0F,0x0C,0x66,0x0D,0x83,0x47,0xF1,0x3C,0xCF,0x33,0xCC,0xF3,0x3C,0xCF,0x13,0x67,0xD8,0x03,0x00,0xE0,0x0F,0x00, // '@'
	0x0C,0x03,0x01,0xE0,0x78,0x1E,0x0F,0xC3,0x30,0xCC,0x33,0x1C,0xE7,0xF9,0xFE,0x61,0xB0,0x3C,0x0C, // 'A'
	0xFE,0x3F,0xCC,0x3B,0x06,0xC1,0xB0,0xEF,0xF3,0xFC,0xC1,0xB0,0x3C,0x0F,0x03,0xC1,0xFF,0xEF,0xF0, // 'B'
	0x0F,0x8F,0xF7,0x05,0x80,0xC0,0x30,0x0C,0x03,0x00,0xC0,0x30,0x0C,0x01,0x80,0x70,0x4F,0xF0,0xF8, // 'C'
	0xFC,0x3F,0xCC,0x3B,0x06,0xC0,0xF0,0x3C,0x0F,0x03,0xC0,0xF0,0x3C,0x0F,0x06,0xC3,0xBF,0xCF,0xC0, // 'D'
	0xFF,0xFF,0xFC,0x03,0x00,0xC0,0x30,0x0F,0xFF,0xFF,0xC0,0x30,0x0C,0x03,0x00,0xC0,0x3F,0xFF,0xFC, // 'E'
	0xFF,0xFF,0xFC,0x03,0x00,0xC0,0x30,0x0F,0xFB,0xFE,0xC0,0x30,0x0C,0x03,0x00,0xC0,0x30,0x0C,0x00, // 'F'
	0x1F,0x0F,0xE7,0x09,0x80,0xC0,0x30,0x0C,0x03,0x0F,0xC3,0xF0,0x3C,0x0D,0x83,0x70,0xCF,0xF1,0xF0, // 'G'
	0xC0,0xF0,0x3C,0x0F,0x03,0xC0,0xF0,0x3F,0xFF,0xFF,0xC0,0xF0,0x3C,0x0F,0x03,0xC0,0xF0,0x3C,0x0C, // 'H'
	0xFF,0xFF,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xFF,0xFF, // 'I'
	0x3F,0x3F,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x87,0xFE,0x7C, // 'J'
	0xC0,0xD8,0x33,0x0C,0x63,0x0C,0xC1,0xB0,0x3E,0x07,0xE0,0xEC,0x18,0xC3,0x1C,0x61,0x8C,0x19,0x83,0xB0,0x30, // 'K'
	0xC0,0x30,0x0C,0x03,0x00,0xC0,0x30,0x0C,0x03,0x00,0xC0,0x30,0x0C,0x03,0x00,0xC0,0x3F,0xFF,0xFC, // 'L'
	0xE1,0xF8,0x7E,0x1F,0xCF,0xD2,0xF4,0xBD,0xEF,0x33,0xCC,0xF3,0x3C,0x0F,0x03,0xC0,0xF0,0x3C,0x0C, // 'M'
	0xE0,0xF8,0x3F,0x0F,0xC3,0xD8,0xF6,0x3C,0x8F,0x33,0xC4,0xF1,0xBC,0x6F,0x0F,0xC3,0xF0,0x7C,0x1C, // 'N'
	0x1E,0x1F,0xE6,0x1B,0x87,0xC0,0xF0,0x3C,0x0F,0x03,0xC0,0xF0,0x3C,0x0F,0x87,0x61,0x9F,0xE1,0xE0, // 'O'
	0xFF,0x3F,0xEC,0x1F,0x03,0xC0,0xF0,0x3C,0x1F,0xFE,0xFF,0x30,0x0C,0x03,0x00,0xC0,0x30,0x0C,0x00, // 'P'
	0x1E,0x1F,0xE6,0x1B,0x87,0xC0,0xF0,0x3C,0x0F,0x03,0xC0,0xF0,0x3C,0x0F,0x87,0x61,0x8F,0xC1,0xE0,0x0C,0x01,0x80,0x40, // 'Q'
	0xFF,0x0F,0xF8,0xC1,0xCC,0x0C,0xC0,0xCC,0x1C,0xFF,0x8F,0xF0,0xC3,0x8C,0x18,0xC1,0xCC,0x0C,0xC0,0xEC,0x06,0xC0,0x70, // 'R'
	0x3F,0x1F,0xEE,0x0B,0x00,0xC0,0x38,0x07,0xC0,0xFC,0x03,0x80,0x30,0x0C,0x03,0x81,0xFF,0xE7,0xF0, // 'S'
	0xFF,0xFF,0xFF,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00, // 'T'
	0xC0,0xF0,0x3C,0x0F,0x03,0xC0,0xF0,0x3C,0x0F,0x03,0xC0,0xF0,0x3C,0x0F,0x03,0xE1,0xDF,0xE3,0xF0, // 'U'
	0xC0,0xF0,0x36,0x19,0x86,0x61,0x98,0x63,0x30,0xCC,0x33,0x0F,0xC1,0xE0,0x78,0x1E,0x03,0x00,0xC0, // 'V'
	0xC0,0x3C,0x03,0xC0,0x36,0x06,0x66,0x66,0x66,0x6F,0x66,0xF6,0x69,0x66,0x96,0x39,0xC3,0x9C,0x39,0xC3,0x0C,0x30,0xC0, // 'W'
	0xE1,0xD8,0x67,0x38,0xCC,0x1E,0x07,0x80,0xC0,0x30,0x1E,0x07,0x83,0x70,0xCC,0x63,0x98,0x6C,0x1C, // 'X'
	0xE0,0x76,0x06,0x30,0xC3,0x0C,0x19,0x81,0xF8,0x0F,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00,0x60,0x06,0x00, // 'Y'
	0xFF,0xFF,0xF0,0x18,0x0E,0x03,0x01,0x80,0xE0,0x30,0x1C,0x06,0x03,0x01,0xC0,0x60,0x3F,0xFF,0xFC, // 'Z'
	0xFF,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xFF, // '['
	0xC0,0x18,0x06,0x00,0xC0,0x30,0x06,0x01,0x80,0x30,0x0C,0x01,0x80,0x60,0x0C,0x03,0x00,0x60,0x18,0x03, // '\'
	0xFF,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0xFF, // ']'
	0x0E,0x03,0x60,0xC6,0x30,0x6C,0x06, // '^'
	0xFF,0xFF,0xFF, // '_'
	0xC3,0x0C,0x30, // '`'
	0x3E,0x3F,0x90,0xE0,0x33,0xFB,0xFF,0x87,0x83,0xC3,0xFF,0xCF,0x60, // 'a'
	0xC0,0x60,0x30,0x18,0x0D,0xE7,0xFB,0x8D,0x83,0xC1,0xE0,0xF0,0x78,0x3E,0x37,0xFB,0x78, // 'b'
	0x1E,0x7F,0x61,0xC0,0xC0,0xC0,0xC0,0xC0,0x61,0x7F,0x1E, // 'c'
	0x01,0x80,0xC0,0x60,0x33,0xDB,0xFD,0x8F,0x83,0xC1,0xE0,0xF0,0x78,0x36,0x3B,0xFC,0xF6, // 'd'
	0x1E,0x3F,0x98,0xF8,0x3F,0xFF,0xFF,0x01,0x80,0x60,0xBF,0xC7,0xC0, // 'e'
	0x0F,0x8F,0xC6,0x03,0x0F,0xFF,0xFC,0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x80,0xC0,0x60, // 'f'
	0x3D,0xBF,0xD8,0xF8,0x3C,0x1E,0x0F,0x07,0x83,0x63,0xBF,0xCF,0x60,0x34,0x3B,0xF8,0xF8, // 'g'
	0xC0,0x60,0x30,0x18,0x0C,0xE7,0xFB,0x8F,0x83,0xC1,0xE0,0xF0,0x78,0x3C,0x1E,0x0F,0x06, // 'h'
	0x0C,0x03,0x00,0x00,0x00,0x7C,0x1F,0x00,0xC0,0x30,0x0C,0x03,0x00,0xC0,0x30,0x0C,0x3F,0xFF,0xFC, // 'i'
	0x0C,0x30,0x00,0x7D,0xF0,0xC3,0x0C,0x30,0xC3,0x0C,0x30,0xC3,0x0F,0xEF,0x00, // 'j'
	0xC0,0x60,0x30,0x18,0x0C,0x3E,0x3B,0x39,0xB8,0xF8,0x7C,0x3B,0x19,0xCC,0x66,0x1B,0x0E, // 'k'
	0xFC,0x3F,0x00,0xC0,0x30,0x0C,0x03,0x00,0xC0,0x30,0x0C,0x03,0x00,0xC0,0x30,0x0E,0x01,0xF0,0x3C, // 'l'
	0xDB,0xBF,0xFC,0xCF,0x33,0xCC,0xF3,0x3C,0xCF,0x33,0xCC,0xF3,0x3C,0xCC, // 'm'
	0xCE,0x7F,0xB8,0xF8,0x3C,0x1E,0x0F,0x07,0x83,0xC1,0xE0,0xF0,0x60, // 'n'
	0x3E,0x3F,0x98,0xD8,0x3C,0x1E,0x0F,0x07,0x83,0x63,0x3F,0x8F,0x80, // 'o'
	0xDE,0x7F,0xB8,0xD8,0x3C,0x1E,0x0F,0x07,0x83,0xE3,0x7F,0xB7,0x98,0x0C,0x06,0x03,0x00, // 'p'
	0x3D,0xBF,0xD8,0xF8,0x3C,0x1E,0x0F,0x07,0x83,0x63,0xBF,0xCF,0x60,0x30,0x18,0x0C,0x06, // 'q'
	0xCE,0xDF,0xF1,0xE0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0, // 'r'
	0x3F,0x3F,0xF0,0x38,0x0F,0xE1,0xF8,0x0E,0x03,0x83,0xFF,0x9F,0x80, // 's'
	0x18,0x0C,0x06,0x1F,0xFF,0xF8,0xC0,0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0xF8,0x7C, // 't'
	0xC1,0xE0,0xF0,0x78,0x3C,0x1E,0x0F,0x07,0x83,0xE3,0xBF,0xCE,0x60, // 'u'
	0xC1,0xF1,0xD8,0xCC,0x67,0x71,0xB0,0xD8,0x6C,0x1C,0x0E,0x07,0x00, // 'v'
	0xC0,0x3C,0x03,0x60,0x66,0x66,0x66,0x66,0x66,0x3F,0xC3,0x9C,0x39,0xC3,0x9C,0x30,0xC0, // 'w'
	0xE3,0xB1,0x8D,0x87,0xC1,0xC0,0xE0,0x70,0x7C,0x36,0x31,0xB8,0xE0, // 'x'
	0xC1,0xB1,0x98,0xCC,0x63,0x61,0xB0,0xF8,0x38,0x1C,0x06,0x06,0x03,0x01,0x83,0x81,0xC0, // 'y'
	0xFF,0xFF,0xC1,0xC0,0xC0,0xE0,0xE0,0xE0,0x60,0x60,0x7F,0xFF,0xE0, // 'z'
	0x0F,0x1F,0x18,0x18,0x18,0x18,0x18,0x18,0xF0,0xF0,0x38,0x18,0x18,0x18,0x18,0x18,0x1F,0x0F, // '{'
	0xFF,0xFF,0xFF,0xFF,0xFF, // '|'
	0xF0,0xF8,0x18,0x18,0x18,0x18,0x18,0x18,0x0F,0x0F,0x1C,0x18,0x18,0x18,0x18,0x18,0xF8,0xF0 // '}'
};
const GFXglyph DejaVu_Sans_Mono_20Glyphs[] PROGMEM = {
// bitmapOffset, width, height, xAdvance, xOffset, yOffset
	  {     0,   1,   1,  13,    0,   -1 }, // ' '
	  {     1,   2,  15,  13,    5,  -15 }, // '!'
	  {     5,   6,   5,  13,    3,  -15 }, // '"'
	  {     9,  12,  15,  13,    0,  -15 }, // '#'
	  {    32,   9,  18,  13,    2,  -15 }, // '$'
	  {    53,  11,  15,  13,    0,  -15 }, // '%'
	  {    74,  11,  15,  13,    1,  -15 }, // '&'
	  {    95,   2,   5,  13,    5,  -15 }, // '''
	  {    97,   4,  18,  13,    4,  -15 }, // '('
	  {   106,   4,  18,  13,    4,  -15 }, // ')'
	  {   115,   9,  10,  13,    1,  -15 }, // '*'
	  {   127,  10,  10,  13,    1,  -11 }, // '+'
	  {   140,   3,   6,  13,    4,   -3 }, // ','
	  {   143,   5,   2,  13,    3,   -7 }, // '-'
	  {   145,   2,   3,  13,    5,   -3 }, // '.'
	  {   146,  10,  16,  13,    1,  -15 }, // '/'
	  {   166,   9,  15,  13,    1,  -15 }, // '0'
	  {   183,   8,  15,  13,    3,  -15 }, // '1'
	  {   198,   9,  15,  13,    1,  -15 }, // '2'
	  {   215,   9,  15,  13,    1,  -15 }, // '3'
	  {   232,  10,  15,  13,    1,  -15 }, // '4'
	  {   251,   9,  15,  13,    1,  -15 }, // '5'
	  {   268,   9,  15,  13,    1,  -15 }, // '6'
	  {   285,   9,  15,  13,    1,  -15 }, // '7'
	  {   302,   9,  15,  13,    1,  -15 }, // '8'
	  {   319,   9,  15,  13,    1,  -15 }, // '9'
	  {   336,   2,  10,  13,    5,  -10 }, // ':'
	  {   339,   3,  13,  13,    4,  -10 }, // ';'
	  {   344,  10,  10,  13,    1,  -11 }, // '<'
	  {   357,  10,   6,  13,    1,   -9 }, // '='
	  {   365,  10,  10,  13,    1,  -11 }, // '>'
	  {   378,   8,  15,  13,    3,  -15 }, // '?'
	  {   393,  10,  17,  13,    1,  -14 }, // '@'
	  {   415,  10,  15,  13,    1,  -15 }, // 'A'
	  {   434,  10,  15,  13,    1,  -15 }, // 'B'
	  {   453,  10,  15,  13,    1,  -15 }, // 'C'
	  {   472,  10,  15,  13,    1,  -15 }, // 'D'
	  {   491,  10,  15,  13,    1,  -15 }, // 'E'
	  {   510,  10,  15,  13,    1,  -15 }, // 'F'
	  {   529,  10,  15,  13,    1,  -15 }, // 'G'
	  {   548,  10,  15,  13,    1,  -15 }, // 'H'
	  {   567,   8,  15,  13,    2,  -15 }, // 'I'
	  {   582,   8,  15,  13,    1,  -15 }, // 'J'
	  {   597,  11,  15,  13,    1,  -15 }, // 'K'
	  {   618,  10,  15,  13,    1,  -15 }, // 'L'
	  {   637,  10,  15,  13,    1,  -15 }, // 'M'
	  {   656,  10,  15,  13,    1,  -15 }, // 'N'
	  {   675,  10,  15,  13,    1,  -15 }, // 'O'
	  {   694,  10,  15,  13,    1,  -15 }, // 'P'
	  {   713,  10,  18,  13,    1,  -15 }, // 'Q'
	  {   736,  12,  15,  13,    1,  -15 }, // 'R'
	  {   759,  10,  15,  13,    1,  -15 }, // 'S'
	  {   778,  12,  15,  13,    0,  -15 }, // 'T'
	  {   801,  10,  15,  13,    1,  -15 }, // 'U'
	  {   820,  10,  15,  13,    1,  -15 }, // 'V'
	  {   839,  12,  15,  13,    0,  -15 }, // 'W'
	  {   862,  10,  15,  13,    1,  -15 }, // 'X'
	  {   881,  12,  15,  13,    0,  -15 }, // 'Y'
	  {   904,  10,  15,  13,    1,  -15 }, // 'Z'
	  {   923,   4,  18,  13,    5,  -15 }, // '['
	  {   932,  10,  16,  13,    1,  -15 }, // '\'
	  {   952,   4,  18,  13,    4,  -15 }, // ']'
	  {   961,  11,   5,  13,    1,  -15 }, // '^'
	  {   968,  12,   2,  13,    0,    3 }, // '_'
	  {   971,   5,   4,  13,    3,  -16 }, // '`'
	  {   974,   9,  11,  13,    1,  -11 }, // 'a'
	  {   987,   9,  15,  13,    1,  -15 }, // 'b'
	  {  1004,   8,  11,  13,    1,  -11 }, // 'c'
	  {  1015,   9,  15,  13,    1,  -15 }, // 'd'
	  {  1032,   9,  11,  13,    1,  -11 }, // 'e'
	  {  1045,   9,  15,  13,    2,  -15 }, // 'f'
	  {  1062,   9,  15,  13,    1,  -11 }, // 'g'
	  {  1079,   9,  15,  13,    1,  -15 }, // 'h'
	  {  1096,  10,  15,  13,    1,  -15 }, // 'i'
	  {  1115,   6,  19,  13,    2,  -15 }, // 'j'
	  {  1130,   9,  15,  13,    1,  -15 }, // 'k'
	  {  1147,  10,  15,  13,    0,  -15 }, // 'l'
	  {  1166,  10,  11,  13,    1,  -11 }, // 'm'
	  {  1180,   9,  11,  13,    1,  -11 }, // 'n'
	  {  1193,   9,  11,  13,    1,  -11 }, // 'o'
	  {  1206,   9,  15,  13,    1,  -11 }, // 'p'
	  {  1223,   9,  15,  13,    1,  -11 }, // 'q'
	  {  1240,   8,  11,  13,    4,  -11 }, // 'r'
	  {  1251,   9,  11,  13,    1,  -11 }, // 's'
	  {  1264,   9,  14,  13,    1,  -14 }, // 't'
	  {  1280,   9,  11,  13,    1,  -11 }, // 'u'
	  {  1293,   9,  11,  13,    1,  -11 }, // 'v'
	  {  1306,  12,  11,  13,    0,  -11 }, // 'w'
	  {  1323,   9,  11,  13,    1,  -11 }, // 'x'
	  {  1336,   9,  15,  13,    1,  -11 }, // 'y'
	  {  1353,   9,  11,  13,    1,  -11 }, // 'z'
	  {  1366,   8,  18,  13,    2,  -15 }, // '{'
	  {  1384,   2,  20,  13,    5,  -15 }, // '|'
	  {  1389,   8,  18,  13,    2,  -15 } // '}'
};
const GFXfont DejaVu_Sans_Mono_20 PROGMEM = {
(uint8_t  *)DejaVu_Sans_Mono_20Bitmaps,(GFXglyph *)DejaVu_Sans_Mono_20Glyphs,0x20, 0x7E, 24};
/*************************************************
*                                                *
*  EasyBMP Cross-Platform Windows Bitmap Library * 
*                                                *
*  Author: Paul Macklin                          *
*   email: macklin01@users.sourceforge.net       *
* support: http://easybmp.sourceforge.net        *
*                                                *
*          file: EasyBMP_DataStructures.h        *
*    date added: 05-02-2005                      *
* date modified: 03-31-2006                      *
*       version: 1.01                            *
*                                                *
*   License: BSD (revised/modified)              *
* Copyright: 2005-6 by the EasyBMP Project       * 
*                                                *
* description: Defines basic data structures for *
*              the BMP class                     *
*                                                *
*************************************************/

#ifndef _EasyBMP_Custom_Math_Functions_
#define _EasyBMP_Custom_Math_Functions_
inline double Square( double number )
{ return number*number; }

inline int IntSquare( int number )
{ return number*number; }
#endif

int IntPow( int base, int exponent );

#ifndef _WINGDI_H
#ifndef _SELF_DEFINED_WINGDI
#define _SELF_DEFINED_WINGDI
 typedef unsigned char BYTE;
 typedef unsigned short WORD;
 typedef unsigned long DWORD;
#endif
#endif 

#ifndef _EasyBMP_DataStructures_h_
#define _EasyBMP_DataStructures_h_

inline bool IsBigEndian()
{
 short word = 0x0001;
 if((*(char *)& word) != 0x01 )
 { return true; }
 return false;
}

inline WORD FlipWORD( WORD in )
{ return ( (in >> 8) | (in << 8) ); }

inline DWORD FlipDWORD( DWORD in )
{
 return ( ((in&0xFF000000)>>24) | ((in&0x000000FF)<<24) | 
          ((in&0x00FF0000)>>8 ) | ((in&0x0000FF00)<<8 )   );
}

// it's easier to use a struct than a class
// because we can read/write all four of the bytes 
// at once (as we can count on them being continuous 
// in memory

typedef struct RGBApixel {
	BYTE	Blue;
	BYTE	Green;
	BYTE	Red;
	BYTE	Alpha;
} RGBApixel; 

class BMFH{
public:
 WORD	bfType;
 DWORD	bfSize;
 WORD	bfReserved1;
 WORD	bfReserved2;
 DWORD	bfOffBits; 

 BMFH();
 void display( void );
 void SwitchEndianess( void );
};

class BMIH{
public:
 DWORD biSize;
 DWORD biWidth;
 DWORD biHeight;
 WORD biPlanes;
 WORD biBitCount;
 DWORD biCompression;
 DWORD biSizeImage;
 DWORD biXPelsPerMeter;
 DWORD biYPelsPerMeter;
 DWORD biClrUsed;
 DWORD biClrImportant;

 BMIH();
 void display( void );
 void SwitchEndianess( void );
};

#endif

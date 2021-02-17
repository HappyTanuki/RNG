#ifndef _PNG_H
#define _PNG_H

#ifndef _INC_STDIO
#include <stdio.h>
#endif

#ifndef _INC_STDLIB
#include <stdlib.h>
#endif

#ifndef _INC_STRING
#include <string.h>
#endif

#include <WinSock2.h>
#include <windows.h>

#define RET_SUCCESS 0;
#define RET_FAIL 1;

#pragma pack(push, 1)
typedef struct{
	union {
		unsigned int length;
		unsigned char length_c[4];
	};
	unsigned char cnkType[4];
	unsigned char* cnkData;
	union {
		unsigned int CRC;
		unsigned char CRC_c[4];
	};
} CNK;

typedef union {
	struct {
		int Width;
		int Height;
		unsigned char bitDepth;
		unsigned char colorType;
		unsigned char compressionMethod;
		unsigned char filterMethod;
		unsigned char interlaceMethod;
	};
	unsigned char IHDRBuffer[13];
} IHDR;

#pragma pack(pop)

typedef struct _Node {
	_Node* prev;
	CNK data;
	_Node* next;
}Node;

class PNG {
protected:
	unsigned char _FSIG[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
public:
	PNG();
	bool writePNG(FILE*);
	bool pushCNK(CNK*);
	bool popCNK();
	~PNG();
private:
	unsigned long update_crc(unsigned long crc, unsigned char* buf, int len);
	unsigned long crc(unsigned char* buf, int len);

	unsigned long crc_table[256];
	Node* dummyHead;
	Node* nodePtr;
};

#endif;
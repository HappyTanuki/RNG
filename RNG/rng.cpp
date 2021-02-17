#include <stdio.h>
#include "png.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib,"ws2_32")

int main(int argc, char* argv[]) {
	int i;
	char tmp;
	double times = 0;

	PNG png;
	CNK newChunk = {0};
	IHDR IHDRChunk;

	FILE* fp;
	fopen_s(&fp, "algorithm test image.png", "wb");
	if (fp == NULL) {
		perror("FILEOPEN");
		exit(1);
	}

	times = atoi(argv[1]);

	timeBeginPeriod(1);
	srand(timeGetTime());
	timeEndPeriod(1);

	newChunk.length = htonl(13);
	memcpy_s(newChunk.cnkType, 4, "IHDR", 4);
	newChunk.cnkData = (unsigned char *)malloc(ntohl(newChunk.length));
	IHDRChunk.Width = htonl(16);
	IHDRChunk.Height = htonl(16);
	IHDRChunk.bitDepth = 1;
	IHDRChunk.colorType = 0;
	IHDRChunk.compressionMethod = 0;
	IHDRChunk.filterMethod = 0;
	IHDRChunk.interlaceMethod = 0;
	memcpy_s(newChunk.cnkData, 13, IHDRChunk.IHDRBuffer, 13);
	png.pushCNK(&newChunk);
	free(newChunk.cnkData);

	newChunk.length = htonl(256);
	memcpy_s(newChunk.cnkType, 4, "IDAT", 4);
	newChunk.cnkData = (unsigned char*)malloc(ntohl(newChunk.length));
	for (i = 0; i < 256; i++) {
		if (i%2 == 0)
			newChunk.cnkData[i] = 0xFF;
		else
			newChunk.cnkData[i] = 0x00;
	}
	png.pushCNK(&newChunk);
	free(newChunk.cnkData);

	newChunk.length = htonl(0);
	memcpy_s(newChunk.cnkType, 4, "IEND", 4);
	newChunk.cnkData = (unsigned char*)malloc(0);
	png.pushCNK(&newChunk);
	free(newChunk.cnkData);

	png.writePNG(fp);
	if (argc > 1) {
		for (i = 0; i < times; i++) {
			tmp = '0' + rand() % 10;
			putchar(tmp);
		}
		putchar('\n');
	}

	if (fp != NULL)
		fclose(fp);
}
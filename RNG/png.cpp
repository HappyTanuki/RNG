#include "png.h"

PNG::PNG() {
	unsigned long c;
	int i, n, k;

	for (n = 0; n < 256; n++) {
		c = (unsigned long)n;
		for (k = 0; k < 8; k++) {
			if (c & 1)
				c = 0xedb88320L ^ (c >> 1);
			else
				c = c >> 1;
		}
		crc_table[n] = c;
	}

	dummyHead								= (Node*)malloc(sizeof(Node));
	if (dummyHead == NULL) {
		fprintf(stderr, "Insufficient memory");
		exit(EXIT_FAILURE);
	}

	dummyHead->prev = NULL;
	for (i = 0; i < 4; i++)
		dummyHead->data.cnkType[i]			= 0x00;
	dummyHead->data.length					= 0x00;
	dummyHead->data.CRC						= 0x00;
	dummyHead->data.cnkData					= 0x00;
	dummyHead->next							= (Node*)malloc(sizeof(Node));
	if (dummyHead->next == NULL) {
		fprintf(stderr, "Insufficient memory");
		exit(EXIT_FAILURE);
	}

	dummyHead->next->prev					= dummyHead;
	for (i = 0; i < 4; i++)
		dummyHead->next->data.cnkType[i]	= 0x00;
	dummyHead->next->data.length			= 0x00;
	dummyHead->next->data.CRC				= 0x00;
	dummyHead->next->data.cnkData			= 0x00;
	dummyHead->next->next					= NULL;

	nodePtr = dummyHead;
}
bool PNG::writePNG(FILE* fp) {
	fwrite(_FSIG, 8, 1, fp);

	nodePtr = dummyHead->next;
	while (nodePtr->data.cnkData != NULL) {
		fwrite(nodePtr->data.length_c, 4, 1, fp);
		fwrite(nodePtr->data.cnkType, 4, 1, fp);
		fwrite(nodePtr->data.cnkData, ntohl(nodePtr->data.length), 1, fp);
		fwrite(nodePtr->data.CRC_c, 4, 1, fp);
		nodePtr = nodePtr->next;
	}
	return RET_SUCCESS;
}
bool PNG::pushCNK(CNK* chunk) {
	unsigned char* data4CRC;
	Node* newCNK;

	newCNK = (Node*)malloc(sizeof(Node));
	if (newCNK == NULL) {
		fprintf(stderr, "Insufficient memory");
		return RET_FAIL;
	}
	newCNK->prev = nodePtr;
	newCNK->data.cnkData = (unsigned char*)malloc(ntohl(chunk->length));
	newCNK->data.length = chunk->length;
	memcpy_s(newCNK->data.cnkType, 4, chunk->cnkType, 4);
	memcpy_s(newCNK->data.cnkData, ntohl(chunk->length), chunk->cnkData, ntohl(chunk->length));
	newCNK->next = nodePtr->next;
	nodePtr->next = newCNK;

	nodePtr = newCNK;

	data4CRC = (unsigned char*)malloc(ntohl(chunk->length) + 4);
	if (data4CRC == NULL) {
		fprintf(stderr, "Insufficient memory");
		return RET_FAIL;
	}

	memcpy_s(data4CRC, ntohl(chunk->length) + 4, chunk->cnkType, 4);
	memcpy_s(data4CRC + 4, ntohl(chunk->length) + 4, chunk->cnkData, ntohl(chunk->length));

	newCNK->data.CRC = htonl(crc(data4CRC, ntohl(chunk->length) + 4));

	free(data4CRC);
	return RET_SUCCESS;
}
bool PNG::popCNK() {
	Node* delChunk = nodePtr;

	nodePtr->prev->next = nodePtr->next;
	nodePtr->next->prev = nodePtr->prev;

	nodePtr = nodePtr->prev;
	free(delChunk->data.cnkData);
	free(delChunk);
	return RET_SUCCESS;
}
unsigned long PNG::update_crc(unsigned long crc, unsigned char* buf, int len) {
	unsigned long c = crc;
	int n;

	for (n = 0; n < len; n++) {
		c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
	}
	return c;
}
unsigned long PNG::crc(unsigned char* buf, int len) {
     return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
}
PNG::~PNG() {
	Node* delChunk;

	nodePtr = dummyHead;
	delChunk = nodePtr;
	nodePtr = nodePtr->next;
	free(delChunk->data.cnkData);
	free(delChunk);
	if (nodePtr == NULL) {
		fprintf(stderr, "Insufficient memory");
		exit(EXIT_FAILURE);
	}
	while (nodePtr->data.cnkData != NULL) {
		delChunk = nodePtr;
		nodePtr = nodePtr->next;
		free(delChunk->data.cnkData);
		free(delChunk);
	}
	delChunk = nodePtr;
	free(delChunk->data.cnkData);
	free(delChunk);
}
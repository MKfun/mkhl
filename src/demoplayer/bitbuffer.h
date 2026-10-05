#ifndef DEMOPLAYER_BITBUFFER_H
#define DEMOPLAYER_BITBUFFER_H

class BitBuffer
{
public:
	BitBuffer();
	BitBuffer(void *newData, unsigned int size);
	BitBuffer(unsigned int size);
	virtual ~BitBuffer();

	bool Resize(unsigned int size);
	void Clear();
	void FastClear();
	void Reset();
	void Free();
	void SetBuffer(void *buffer, int size);

	int CurrentBit();
	int CurrentSize();
	int SpaceLeft();
	void AlignByte();

	void StartBitMode();
	void EndBitMode();

	int ReadBit();
	unsigned int ReadBits(int n);
	unsigned int PeekBits(int numbits);
	int ReadChar();
	int ReadByte();
	int ReadShort();
	int ReadWord();
	unsigned int ReadLong();
	float ReadFloat();
	bool ReadBuf(int iSize, void *pbuf);
	char *ReadString();
	char *ReadStringLine();
	char *ReadBitString();
	float ReadAngle();
	float ReadHiresAngle();
	float ReadBitAngle(int numbits);
	int ReadSBits(int numbits);
	float ReadCoord();
	float ReadBitCoord();
	void ReadBitVec3Coord(float *fa);
	int ReadBitData(void *dest, int length);

	void WriteBit(int c);
	void WriteBits(unsigned int bits, int n);
	void WriteSBits(int bits, int numbits);
	void WriteChar(int c);
	void WriteByte(int c);
	void WriteShort(int c);
	void WriteWord(int c);
	void WriteLong(unsigned int c);
	void WriteFloat(float f);
	void WriteString(const char *s);
	void WriteBitString(const char *s);
	void WriteAngle(float f);
	void WriteHiresAngle(float f);
	void WriteBitAngle(float fAngle, int numbits);
	void WriteCoord(float f);
	void WriteBuf(const void *buf, int iSize);
	void WriteBuf(BitBuffer *stream, int iSize);
	void WriteBitData(void *src, int length);

	void SkipBytes(int n);
	void SkipBits(int n);
	int SkipString();
	void ConcatBuffer(BitBuffer *buffer);

public:
	bool sizeError;
	unsigned char *data;
	unsigned char *currentByte;
	int currentBit;
	int maxSize;
	bool littleEndian;
	bool ownData;
};

#endif // DEMOPLAYER_BITBUFFER_H

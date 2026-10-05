#include "bitbuffer.h"
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdint>

static inline int32_t LittleLong(int32_t val)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return __builtin_bswap32(val);
#else
	return val;
#endif
}

static const unsigned int ROWBITTABLE[33] = {
	0x00000000, 0x00000001, 0x00000003, 0x00000007,
	0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
	0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
	0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
	0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
	0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
	0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
	0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
	0xffffffff
};

static const unsigned char BITTABLE[8] = {
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
};

static const unsigned char INVBITTABLE[8] = {
	0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f
};

static const unsigned char masks[8] = {
	0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

static const unsigned char inv_masks[8] = {
	0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe
};

BitBuffer::BitBuffer()
{
	sizeError = false;
	data = nullptr;
	currentByte = nullptr;
	currentBit = 0;
	maxSize = 0;
	littleEndian = true;
	ownData = false;
}

BitBuffer::BitBuffer(void *newData, unsigned int size)
{
	sizeError = false;
	data = (unsigned char *)newData;
	currentByte = (unsigned char *)newData;
	currentBit = 0;
	maxSize = size;
	littleEndian = true;
	ownData = false;
}

BitBuffer::BitBuffer(unsigned int size)
{
	sizeError = false;
	data = nullptr;
	currentByte = nullptr;
	currentBit = 0;
	maxSize = 0;
	littleEndian = true;
	ownData = false;

	unsigned char *mem = (unsigned char *)malloc(size + 4);
	if (mem)
	{
		memset(mem, 0, size + 4);
		data = mem;
		currentByte = mem;
		maxSize = size;
		ownData = true;
	}
}

BitBuffer::~BitBuffer()
{
	Free();
}

bool BitBuffer::Resize(unsigned int size)
{
	if (data && ownData)
		free(data);

	sizeError = false;
	data = nullptr;
	currentByte = nullptr;
	currentBit = 0;
	maxSize = 0;
	littleEndian = true;
	ownData = false;

	unsigned char *mem = (unsigned char *)malloc(size + 4);
	if (mem)
	{
		memset(mem, 0, size + 4);
		data = mem;
		currentByte = mem;
		maxSize = size;
		ownData = true;
		return true;
	}

	return false;
}

void BitBuffer::Clear()
{
	if (data && maxSize > 0)
		memset(data, 0, maxSize);

	currentByte = data;
	currentBit = 0;
	sizeError = false;
	littleEndian = true;
}

void BitBuffer::FastClear()
{
	if (data)
	{
		int clearSize = (currentByte - data) + (currentBit != 0 ? 1 : 0) + 4;
		if (clearSize > maxSize)
			clearSize = maxSize;
		if (clearSize > 0)
			memset(data, 0, clearSize);
	}

	currentByte = data;
	currentBit = 0;
	sizeError = false;
	littleEndian = true;
}

void BitBuffer::Reset()
{
	currentByte = data;
	currentBit = 0;
	sizeError = false;
	littleEndian = true;
}

void BitBuffer::Free()
{
	if (data && ownData)
		free(data);

	data = nullptr;
	currentByte = nullptr;
	currentBit = 0;
	maxSize = 0;
	sizeError = false;
	littleEndian = true;
	ownData = false;
}

void BitBuffer::SetBuffer(void *buffer, int size)
{
	if (data && ownData)
		free(data);

	data = (unsigned char *)buffer;
	currentByte = (unsigned char *)buffer;
	currentBit = 0;
	maxSize = size;
	sizeError = false;
	littleEndian = true;
	ownData = false;
}

int BitBuffer::CurrentBit()
{
	return currentBit + 8 * (currentByte - data);
}

int BitBuffer::CurrentSize()
{
	return (currentBit != 0 ? 1 : 0) + (currentByte - data);
}

int BitBuffer::SpaceLeft()
{
	return maxSize + (data - currentByte);
}

void BitBuffer::AlignByte()
{
	if (currentBit != 0)
	{
		currentByte++;
		currentBit = 0;
	}
}

void BitBuffer::StartBitMode()
{
	if (currentBit != 0)
		sizeError = true;
}

void BitBuffer::EndBitMode()
{
	if (currentBit != 0)
	{
		currentByte++;
		currentBit = 0;
	}
}

int BitBuffer::ReadBit()
{
	if (currentByte - data >= maxSize)
	{
		sizeError = true;
		return -1;
	}

	if (littleEndian)
	{
		if (currentBit == 7)
		{
			currentBit = 0;
			unsigned char b = *currentByte++;
			return (b >> 7) & 1;
		}
		else
		{
			int bit = (*currentByte >> currentBit) & 1;
			currentBit++;
			return bit;
		}
	}
	else
	{
		if (currentBit == 7)
		{
			currentBit = 0;
			unsigned char b = *currentByte++;
			return b & 1;
		}
		else
		{
			int bit = (*currentByte >> (7 - currentBit)) & 1;
			currentBit++;
			return bit;
		}
	}
}

unsigned int BitBuffer::ReadBits(int n)
{
	if (n <= 0)
		return 0;

	if (littleEndian)
	{
		if (currentByte - data >= maxSize)
		{
			sizeError = true;
			return (unsigned int)-1;
		}

		if (currentBit + n <= 32)
		{
			unsigned int dwordVal = *(unsigned int *)currentByte;
			unsigned int res = (dwordVal >> currentBit) & ROWBITTABLE[n];
			unsigned char *nextByte = currentByte + (n >> 3);
			int nextBit = (n & 7) + currentBit;
			currentByte = nextByte;
			currentBit = nextBit;
			if (nextBit > 7)
			{
				currentBit = nextBit & 7;
				currentByte = nextByte + 1;
			}
			return res;
		}
		else
		{
			unsigned int lowDword = *(unsigned int *)currentByte;
			unsigned int highDword = *(unsigned int *)(currentByte + 4);
			int shift = currentBit;
			int nextBit = (currentBit + n) & 7;
			currentByte += 4;
			currentBit = nextBit;
			unsigned int mask = ROWBITTABLE[nextBit];
			return (lowDword >> shift) | ((highDword & mask) << (32 - shift));
		}
	}
	else
	{
		unsigned int res = 0;
		int count = n;
		while (count > 0)
		{
			count--;
			if (ReadBit())
				res |= (1 << count);
		}
		return res;
	}
}

unsigned int BitBuffer::PeekBits(int numbits)
{
	int savedBit = currentBit;
	unsigned char *savedByte = currentByte;
	unsigned int result = ReadBits(numbits);
	currentBit = savedBit;
	currentByte = savedByte;
	return result;
}

int BitBuffer::ReadChar()
{
	return (int)ReadBits(8);
}

int BitBuffer::ReadByte()
{
	return (int)ReadBits(8);
}

int BitBuffer::ReadShort()
{
	return (int)ReadBits(16);
}

int BitBuffer::ReadWord()
{
	return (int)ReadBits(16);
}

unsigned int BitBuffer::ReadLong()
{
	return ReadBits(32);
}

float BitBuffer::ReadFloat()
{
	uint32_t val = ReadBits(32);
	int32_t swapped = LittleLong((int32_t)val);
	float result;
	memcpy(&result, &swapped, sizeof(result));
	return result;
}

bool BitBuffer::ReadBuf(int iSize, void *pbuf)
{
	if (!pbuf || iSize <= 0)
		return false;

	if (iSize + (currentByte - data) > maxSize)
	{
		sizeError = true;
		return false;
	}

	char *dst = (char *)pbuf;
	if (currentBit != 0)
	{
		int dwords = iSize / 4;
		for (int i = 0; i < dwords; i++)
		{
			unsigned int bits = ReadBits(32);
			memcpy(dst + i * 4, &bits, 4);
		}
		int remainder = iSize % 4;
		for (int i = 0; i < remainder; i++)
		{
			dst[dwords * 4 + i] = (char)ReadBits(8);
		}
		return true;
	}
	else
	{
		memcpy(pbuf, currentByte, iSize);
		currentByte += iSize;
		return true;
	}
}

char *BitBuffer::ReadString()
{
	static char stringBuf[8192];
	int i;
	for (i = 0; i < 8191; i++)
	{
		unsigned int c = ReadBits(8);
		if (c == 0 || (int)c == -1)
			break;
		stringBuf[i] = (char)c;
	}
	stringBuf[i] = '\0';
	return stringBuf;
}

char *BitBuffer::ReadStringLine()
{
	static char lineBuf[2048];
	int i;
	for (i = 0; i < 2047; i++)
	{
		unsigned int c = ReadBits(8);
		if (c == '\n' || c == 0 || (int)c == -1)
			break;
		lineBuf[i] = (char)c;
	}
	lineBuf[i] = '\0';
	return lineBuf;
}

char *BitBuffer::ReadBitString()
{
	static char bitStrBuf[2048];
	char *dst = bitStrBuf;
	unsigned int c;
	do
	{
		c = ReadBits(8);
		*dst++ = (char)c;
	} while (c != 0 && (int)c != -1 && dst < bitStrBuf + 2047);
	*dst = '\0';
	return bitStrBuf;
}

float BitBuffer::ReadAngle()
{
	return (float)((int)ReadBits(8)) * 1.40625f;
}

float BitBuffer::ReadHiresAngle()
{
	return (float)((int)ReadBits(16)) * 0.0054931641f;
}

float BitBuffer::ReadBitAngle(int numbits)
{
	return (float)ReadBits(numbits) * (360.0f / (float)(1 << numbits));
}

int BitBuffer::ReadSBits(int numbits)
{
	int sign = ReadBit();
	int val = (int)ReadBits(numbits - 1);
	if (sign)
		return -val;
	return val;
}

float BitBuffer::ReadCoord()
{
	return (float)((int)ReadBits(16)) * 0.125f;
}

float BitBuffer::ReadBitCoord()
{
	int intflag = ReadBit();
	int fracflag = ReadBit();
	if (!intflag && !fracflag)
		return 0.0f;

	int signflag = ReadBit();
	int intval = 0;
	if (intflag)
		intval = (int)ReadBits(12);

	float fracval = 0.0f;
	if (fracflag)
		fracval = (float)((int)ReadBits(3)) * 0.125f;

	float val = (float)intval + fracval;
	if (signflag)
		val = -val;
	return val;
}

void BitBuffer::ReadBitVec3Coord(float *fa)
{
	int has_x = ReadBit();
	int has_y = ReadBit();
	int has_z = ReadBit();

	if (has_x)
		fa[0] = ReadBitCoord();
	if (has_y)
		fa[1] = ReadBitCoord();
	if (has_z)
		fa[2] = ReadBitCoord();
}

int BitBuffer::ReadBitData(void *dest, int length)
{
	if (length > 0 && dest)
	{
		unsigned char *d = (unsigned char *)dest;
		for (int i = 0; i < length; i++)
			d[i] = (unsigned char)ReadBits(8);
	}
	return length;
}

void BitBuffer::WriteBit(int c)
{
	if (currentByte - data >= maxSize)
	{
		sizeError = true;
		return;
	}

	if (littleEndian)
	{
		if (currentBit == 7)
		{
			if (!c)
				*currentByte &= ~0x80u;
			else
				*currentByte |= 0x80u;
			currentByte++;
			currentBit = 0;
		}
		else
		{
			if (!c)
				*currentByte &= INVBITTABLE[currentBit];
			else
				*currentByte |= BITTABLE[currentBit];
			currentBit++;
		}
	}
	else
	{
		if (c)
			*currentByte |= masks[currentBit];
		else
			*currentByte &= inv_masks[currentBit];
		currentBit++;
		if (currentBit == 8)
		{
			currentBit = 0;
			currentByte++;
		}
	}
}

void BitBuffer::WriteBits(unsigned int bits, int n)
{
	if (sizeError || n <= 0)
		return;

	if (littleEndian)
	{
		if (currentByte - data + (n >> 8) > maxSize)
		{
			sizeError = true;
			return;
		}

		if (currentBit + n > 32)
		{
			unsigned int masked = bits & ROWBITTABLE[n];
			*(unsigned int *)currentByte |= (masked << currentBit);
			int prevBit = currentBit;
			currentBit = (currentBit + n) & 7;
			currentByte += 4;
			*(unsigned int *)currentByte |= (masked >> (32 - prevBit));
		}
		else
		{
			*(unsigned int *)currentByte |= ((bits & ROWBITTABLE[n]) << currentBit);
			int nextBit = currentBit + (n & 7);
			unsigned char *nextByte = currentByte + (n >> 3);
			currentByte = nextByte;
			currentBit = nextBit;
			if (nextBit > 7)
			{
				currentBit = nextBit & 7;
				currentByte = nextByte + 1;
			}
		}
	}
	else
	{
		unsigned int v = bits;
		if (n <= 31 && bits >= (1u << n) && bits != 0xffffffff)
			v = (1u << n) - 1;

		int count = n;
		while (count > 0)
		{
			count--;
			if (currentByte - data >= maxSize)
			{
				sizeError = true;
				return;
			}
			WriteBit((v >> count) & 1);
		}
	}
}

void BitBuffer::WriteSBits(int bits, int numbits)
{
	int v = bits;
	int shift = (numbits > 31) ? 30 : numbits - 1;

	int maxVal = (1 << shift) - 1;
	int minVal = 1 - (1 << shift);

	if (v > maxVal)
		v = maxVal;
	else if (v < minVal)
		v = minVal;

	if (currentByte - data >= maxSize)
	{
		sizeError = true;
		return;
	}

	if (v < 0)
		WriteBit(1);
	else
		WriteBit(0);

	WriteBits((unsigned int)std::abs(v), shift);
}

void BitBuffer::WriteChar(int c)
{
	WriteBits((unsigned int)c, 8);
}

void BitBuffer::WriteByte(int c)
{
	WriteBits((unsigned int)c, 8);
}

void BitBuffer::WriteShort(int c)
{
	WriteBits((unsigned int)c, 16);
}

void BitBuffer::WriteWord(int c)
{
	WriteBits((unsigned int)c, 16);
}

void BitBuffer::WriteLong(unsigned int c)
{
	WriteBits(c, 32);
}

void BitBuffer::WriteFloat(float f)
{
	int32_t raw;
	memcpy(&raw, &f, sizeof(raw));
	int32_t swapped = LittleLong(raw);
	WriteBits((unsigned int)swapped, 32);
}

void BitBuffer::WriteString(const char *s)
{
	if (!s)
	{
		WriteBits(0, 8);
		return;
	}

	int len = (int)strlen(s) + 1;
	WriteBuf(s, len);
}

void BitBuffer::WriteBitString(const char *s)
{
	if (!s)
	{
		WriteBits(0, 8);
		return;
	}

	for (const char *p = s; *p; p++)
		WriteBits((unsigned char)*p, 8);
	WriteBits(0, 8);
}

void BitBuffer::WriteAngle(float f)
{
	float v = f * 256.0f / 360.0f;
	WriteBits((unsigned char)(int)v, 8);
}

void BitBuffer::WriteHiresAngle(float f)
{
	float v = f * 65536.0f / 360.0f;
	WriteBits((unsigned short)(int)v, 16);
}

void BitBuffer::WriteBitAngle(float fAngle, int numbits)
{
	if (numbits <= 31)
	{
		double v = (double)fAngle * (double)(1 << numbits);
		WriteBits(((1 << numbits) - 1) & ((int)v / 360), numbits);
	}
	else
	{
		sizeError = true;
	}
}

void BitBuffer::WriteCoord(float f)
{
	float fa = f * 8.0f;
	WriteBits((unsigned int)(int)fa, 16);
}

void BitBuffer::WriteBuf(const void *buf, int iSize)
{
	if (!buf || sizeError || iSize <= 0)
		return;

	if (iSize + (currentByte - data) > maxSize)
	{
		sizeError = true;
		return;
	}

	const char *src = (const char *)buf;
	if (currentBit != 0)
	{
		int dwords = iSize / 4;
		for (int i = 0; i < dwords; i++)
		{
			unsigned int bits;
			memcpy(&bits, src + i * 4, 4);
			WriteBits(bits, 32);
		}
		int remainder = iSize % 4;
		for (int i = 0; i < remainder; i++)
		{
			WriteBits((unsigned char)src[dwords * 4 + i], 8);
		}
	}
	else
	{
		memcpy(currentByte, src, iSize);
		currentByte += iSize;
	}
}

void BitBuffer::WriteBuf(BitBuffer *stream, int iSize)
{
	if (!stream || !stream->currentByte || sizeError || iSize <= 0)
		return;

	if (iSize + (currentByte - data) > maxSize)
	{
		sizeError = true;
		return;
	}

	WriteBuf(stream->currentByte, iSize);

	if (iSize + (stream->currentByte - stream->data) > stream->maxSize)
		stream->sizeError = true;
	stream->currentByte += iSize;
}

void BitBuffer::WriteBitData(void *src, int length)
{
	if (length > 0 && src)
	{
		unsigned char *s = (unsigned char *)src;
		for (int i = 0; i < length; i++)
			WriteBits(s[i], 8);
	}
}

void BitBuffer::SkipBytes(int n)
{
	if (n + (currentByte - data) > maxSize)
		sizeError = true;
	currentByte += n;
}

void BitBuffer::SkipBits(int n)
{
	if (n <= 0)
		return;

	if (littleEndian)
	{
		if (currentByte - data >= maxSize)
		{
			sizeError = true;
			return;
		}

		if (currentBit + n > 32)
		{
			currentByte += 4;
			currentBit = (currentBit + n) & 7;
		}
		else
		{
			int nextBit = (n & 7) + currentBit;
			unsigned char *nextByte = currentByte + (n >> 3);
			currentByte = nextByte;
			currentBit = nextBit;
			if (nextBit > 7)
			{
				currentBit = nextBit & 7;
				currentByte = nextByte + 1;
			}
		}
	}
	else
	{
		int count = n;
		while (count > 0)
		{
			count--;
			if (currentBit == 7)
			{
				currentByte++;
				currentBit = 0;
			}
			else
			{
				currentBit++;
			}
		}
	}
}

int BitBuffer::SkipString()
{
	int i;
	for (i = 1; i < 8192; i++)
	{
		unsigned int c = ReadBits(8);
		if (c == 0 || (int)c == -1)
			break;
	}
	return i;
}

void BitBuffer::ConcatBuffer(BitBuffer *buffer)
{
	if (!buffer || !buffer->data || sizeError)
		return;

	int len = (buffer->currentByte - buffer->data) + (buffer->currentBit != 0 ? 1 : 0);
	if (len <= 0)
		return;

	WriteBuf(buffer->data, len);
}

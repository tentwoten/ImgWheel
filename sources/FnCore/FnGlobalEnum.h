#ifndef _FNGLOBALENUM_H_
#define _FNGLOBALENUM_H_

enum class PtrType
{
	S8=0,
	U8=1,
	S16=2,
	U16=3,
	S32=4,
	U32=5,
	S64=6,
	U64=7,
	F32=8,
	F64=9
};

enum class PixelForm
{
	//U8_RGB = 1,
	//U8_RGBA = 2,
	//U8 = 3,
	//U16 = 4,
	//S16 = 5,
	//F32 = 6,
	//U8_BGRA = 7,
	//U32 = 8,
	//U16_RGB = 9,
	//U64 = 10,
	//F64 = 11,
	//S8 = 12,
	//S32 = 13
	S8 = 0,
	U8 = 1,
	S8_RGB = 2,
	U8_RGB = 3,
	S8_RGBA = 4,
	U8_RGBA = 5,
	S8_BGRA = 6,
	U8_BGRA = 7,


	S16 = 8,
	U16 = 9,
	S16_RGB = 10,
	U16_RGB = 11,

	S32 = 12,
	U32 = 13,
	S32_RGB = 14,
	U32_RGB = 15,

	S64 = 16,
	U64 = 17,
	S64_RGB = 18,
	U64_RGB = 19,

	F32 = 20,
	F32_RGB = 21,

	F64 = 22,
	F64_RGB = 23
};

enum class ColorMapStyle
{
	COMMON = 0,
	AUTUMN = 1,
	BONE = 2,
	CIVIDIS = 3,
	COOL = 4,
	HOT = 5,
	HSV = 6,
	INFERNO = 7,
	JET = 8,
	MAGMA = 9,
	OCEAN = 10,
	PARULA = 11,
	PINK = 12,
	PLASMA = 13,
	RAINBOW = 14,
	SPRING = 15,
	SUMMER = 16,
	TURBO = 17,
	TWILIGHT = 18,
	TWILIGHTSHIFTED = 19,
	VIRIDIS = 20,
	WINTER = 21
};

enum class PaddType
{
	EMPTY=0,
	SAME=1,
	MIRROR=2
};

#endif // !_ENUM_H_
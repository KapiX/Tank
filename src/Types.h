#ifndef _TYPES_H_
#define _TYPES_H_

#if defined(_MSC_VER)
	typedef	__int8				s8;
	typedef	unsigned __int8		u8;
	typedef	__int16				s16;
	typedef unsigned __int16	u16;
	typedef __int32				s32;
	typedef unsigned __int32	u32;
	typedef __int64				s64;
	typedef unsigned __int64	u64;
	typedef char				c8;
	typedef float				f32;
	typedef double				f64;
	typedef wchar_t				uc8; // unicode char
	typedef unsigned char       uchar;
#elif defined(__GNUC__)
	typedef signed char			s8;
	typedef unsigned char		u8;
	typedef signed short		s16;
	typedef unsigned short		u16;
	typedef signed int			s32;
	typedef unsigned int		u32;
	typedef signed long long	s64;
	typedef unsigned long long	u64;
	typedef char				c8;
	typedef float				f32;
	typedef double				f64;
	typedef wchar_t				uc8; // unicode char;
	typedef unsigned char		uchar;
#endif

#endif
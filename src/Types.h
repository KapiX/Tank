/*
	Copyright 2011 Kacper Kasper

	This file is part of Tank.

    Tank is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tank is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Tank.  If not, see <http://www.gnu.org/licenses/>.
*/

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
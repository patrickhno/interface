
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#ifndef ARGB32_H
#define ARGB32_H

#include <posix/sys/types.h>

class ARGB32{
public:			ARGB32();
				ARGB32(const uint8 A, const uint8 R, const uint8 G, const uint8 B);
				ARGB32(const uint8 R, const uint8 G, const uint8 B);
				ARGB32(const ARGB32 *);
				~ARGB32();
		ARGB32	&operator= (const ARGB32 &);
		ARGB32	&operator+=(const ARGB32 &);
		ARGB32	&operator-=(const ARGB32 &);
		ARGB32	&operator*=(const ARGB32 &);
		ARGB32	&operator|=(const ARGB32 &);
		ARGB32	operator+(const ARGB32 &) const;
		ARGB32	operator-(const ARGB32 &) const;
		ARGB32	operator*(const float) const;
		ARGB32	operator*(const ARGB32 &) const;
		ARGB32	operator/(const float) const;
		union{	uint8	A,Alpha; };
		union{	uint8	R,Red;   };
		union{	uint8	G,Green; };
		union{	uint8	B,Blue;  };
};

inline ARGB32::ARGB32(){
	A=255; R=G=B=0;
}

inline ARGB32::ARGB32(const uint8 _A, const uint8 _R, const uint8 _G, const uint8 _B){
	A=_A;R=_R;G=_G;B=_B;
}

inline ARGB32::ARGB32(const uint8 _R, const uint8 _G, const uint8 _B){
	A=255;R=_R;G=_G;B=_B;
}

inline ARGB32::ARGB32(const ARGB32 *col){
	A=col->A; R=col->R; G=col->G; B=col->B;
}

inline ARGB32::~ARGB32(){}

inline ARGB32 &ARGB32::operator=(const ARGB32 &col){
	A=col.A;R=col.R;G=col.G;B=col.B;
	return *this;
}

inline ARGB32 &ARGB32::operator+=(const ARGB32 &col){
	A+=col.A;R+=col.R;G+=col.G;B+=col.B;
	return *this;
}

inline ARGB32 &ARGB32::operator-=(const ARGB32 &col){
	A-=col.A;R-=col.R;G-=col.G;B-=col.B;
	return *this;
}

inline ARGB32 &ARGB32::operator*=(const ARGB32 &col){
	A*=col.A;R*=col.R;G*=col.G;B*=col.B;
	return *this;
}

inline ARGB32 &ARGB32::operator|=(const ARGB32 &col){
	A|=col.A;R|=col.R;G|=col.G;B|=col.B;
	return *this;
}

inline ARGB32 ARGB32::operator+(const ARGB32 &col) const{
	return(ARGB32(A+col.A,R+col.R,G+col.G,B+col.B));
}

inline ARGB32 ARGB32::operator-(const ARGB32 &col) const{
	return(ARGB32(A-col.A,R-col.R,G-col.G,B-col.B));
}

inline ARGB32 ARGB32::operator*(const float s) const{
	int i = int(s*256.0f*256.0f);
	return(ARGB32((A*i)>>16,(R*i)>>16,(G*i)>>16,(B*i)>>16));
}

inline ARGB32 ARGB32::operator*(const ARGB32 &col) const{
	return(ARGB32(A*col.A,R*col.R,G*col.G,B*col.B));
}

inline ARGB32 ARGB32::operator/(const float i) const{
	int val = int((1.0f/i)*256.0f*256.0f);
	return(ARGB32((A*val)>>16,(R*val)>>16,(G*val)>>16,(B*val)>>16));
}

#endif


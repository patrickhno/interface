
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#ifndef ARGB_H
#define ARGB_H

// All values: 0.0f to 1.0f

class ARGB{
public:			ARGB();
				ARGB(const float A, const float R, const float G, const float B);
				ARGB(const float R, const float G, const float B);
				ARGB(const ARGB *);
				~ARGB();
		ARGB	&operator=(const ARGB &);
		ARGB	&operator+=(const ARGB &);
		ARGB	&operator-=(const ARGB &);
		ARGB	&operator*=(const ARGB &);
		ARGB	&operator|=(const ARGB &);
		ARGB	operator+(const ARGB &) const;
		ARGB	operator-(const ARGB &) const;
		ARGB	operator*(const float) const;
		ARGB	operator*(const ARGB &) const;
		ARGB	operator/(const float) const;
		union{
			float	A,Alpha;
		};
		union{
			float	R,Red;
		};
		union{
			float	G,Green;
		};
		union{
			float	B,Blue;
		};
};

inline ARGB::ARGB(){
	A=1; R=G=B=0;
}

inline ARGB::ARGB(const float _A, const float _R, const float _G, const float _B){
	A=_A;R=_R;G=_G;B=_B;
}

inline ARGB::ARGB(const float _R, const float _G, const float _B){
	A=1;R=_R;G=_G;B=_B;
}

inline ARGB::ARGB(const ARGB *col){
	A=col->A; R=col->R; G=col->G; B=col->B;
}

inline ARGB::~ARGB(){}

inline ARGB &ARGB::operator=(const ARGB &col){
	A=col.A;
	R=col.R;
	G=col.G;
	B=col.B;
	return *this;
}

inline ARGB &ARGB::operator+=(const ARGB &col){
	A+=col.A;
	R+=col.R;
	G+=col.G;
	B+=col.B;
	return *this;
}

inline ARGB &ARGB::operator-=(const ARGB &col){
	A-=col.A;
	R-=col.R;
	G-=col.G;
	B-=col.B;
	return *this;
}

inline ARGB &ARGB::operator*=(const ARGB &col){
	A*=col.A;
	R*=col.R;
	G*=col.G;
	B*=col.B;
	return *this;
}

inline ARGB &ARGB::operator|=(const ARGB &col){
	int	t1,t2;
	t1=int(    A*float(0x7fffffff));
	t2=int(col.A*float(0x7fffffff));
	A=float(t1|t2)/float(0x7fffffff);
	t1=int(    R*float(0x7fffffff));
	t2=int(col.R*float(0x7fffffff));
	R=float(t1|t2)/float(0x7fffffff);
	t1=int(    G*float(0x7fffffff));
	t2=int(col.G*float(0x7fffffff));
	G=float(t1|t2)/float(0x7fffffff);
	t1=int(    B*float(0x7fffffff));
	t2=int(col.B*float(0x7fffffff));
	B=float(t1|t2)/float(0x7fffffff);
	return *this;
}

inline ARGB ARGB::operator+(const ARGB &col) const{
	return(ARGB(A+col.A,R+col.R,G+col.G,B+col.B));
}

inline ARGB ARGB::operator-(const ARGB &col) const{
	return(ARGB(A-col.A,R-col.R,G-col.G,B-col.B));
}

inline ARGB ARGB::operator*(const float i) const{
	return(ARGB(A*i,R*i,G*i,B*i));
}

inline ARGB ARGB::operator*(const ARGB &col) const{
	return(ARGB(A*col.A,R*col.R,G*col.G,B*col.B));
}

inline ARGB ARGB::operator/(const float i) const{
	return(ARGB(A/i,R/i,G/i,B/i));
}

#endif


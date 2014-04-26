
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <posix/stdlib.h>
#include <posix/stdio.h>
#include <Interface/ARGB.h>
#include <Interface/ARGB32.h>
#include <Interface/Bitmap.h>
#include <Interface/Bitmap32.h>
#include <Interface/Rect.h>
#include <Interface/DynamicArray.h>
#include <Interface/String.h>
#include <assert.h>

static DynamicArray *Bitmap32Cache = 0;
struct BmCache{
	String		Name;
	Bitmap32	*Bitmap;
};

Bitmap32::Bitmap32(const String &name, const int _format) : Bitmap32Driver(){

	Name = name;

	// Filename caching
	Cache = 0;
	CacheCount = 0;

if(!Bitmap32Cache) Bitmap32Cache = new DynamicArray();

	int cnt = Bitmap32Cache->GetCount();
	for(int n=0; n<cnt; n++){
		BmCache *c = (BmCache*)Bitmap32Cache->GetItem(n);
		if(c->Name==name){
			c->Bitmap->AddCache();
			Cache = c->Bitmap;
			Width = Cache->Width;
			Height = Cache->Height;
			Frames = 0;
			frame = 0;
			return;
		}
	}

	BmCache *c = new BmCache;
	c->Name = name;
	c->Bitmap = this;
	Bitmap32Cache->Add(c);
//printf("1\n");
	Bitmap *bm = new Bitmap(name.GetString(),_format);
//printf("2\n");
	Width     = bm->GetWidth();
	Height    = bm->GetHeight();
	Frames = new DynamicArray();
	frame = 0;
	cnt = bm->GetFrameCount();
//printf("3\n");
	for(n=0; n<cnt; n++){
//printf("a\n");
		ARGB   *s = bm->LoadBuffer(n);
//printf("b\n");
		ARGB32 *Buffer    = (ARGB32*)malloc(Width*Height*sizeof(ARGB32)); //new ARGB32[Width*Height];
		Frames->Add(Buffer);
		ARGB32 *b = Buffer;
		for(int n=0; n<Width*Height; n++){
			ARGB col = *s++;
			*b++ = ARGB32(col.A*255.0f,col.R*255.0f,col.G*255.0f,col.B*255.0f);
		}
		bm->UnloadBuffer();
	}
//printf("4\n");
	format = bm->GetFormat();
//printf("6\n");
	Bitmap32Driver::SetOwner(this);
//printf("7\n");
	delete bm;
Invalidate();
}

Bitmap32::Bitmap32(class Bitmap *bm) : Bitmap32Driver(){

	Cache = 0;
	CacheCount = 0;
	Name = "";

	Width     = bm->GetWidth();
	Height    = bm->GetHeight();
	ARGB   *s = bm->LoadBuffer(0);
	Frames = new DynamicArray();
	frame = 0;
	ARGB32 *Buffer    = (ARGB32*)malloc(Width*Height*sizeof(ARGB32)); //new ARGB32[Width*Height];
	Frames->Add(Buffer);
	ARGB32 *b = Buffer;
	for(int n=0; n<Width*Height; n++){
		ARGB col = *s++;
		*b++ = ARGB32(col.A*255.0f,col.R*255.0f,col.G*255.0f,col.B*255.0f);
		//*b++ = (uint32(col.A*255.0f)<<24) | (uint32(col.R*255.0f)<<16) | (uint16(col.G*255.0f)<<8) | uint8(col.B*255.0f);
		//*b++=uint8(col.A*255.0f);
		//*b++=uint8(col.R*255.0f);
		//*b++=uint8(col.G*255.0f);
		//*b++=uint8(col.B*255.0f);
	}
	bm->UnloadBuffer();
	format = bm->GetFormat();
	Bitmap32Driver::SetOwner(this);
Invalidate();
}

Bitmap32::Bitmap32(class Bitmap32 *bm) : Bitmap32Driver(){ //, int f){

	Cache = 0; // fixme
	CacheCount = 0;
	Name = "";

	Width     = bm->GetWidth();
	Height    = bm->GetHeight();
	ARGB32 *s = bm->LoadBuffer(0);
	Frames = new DynamicArray();
	frame = 0;
	ARGB32 *Buffer    = (ARGB32*)malloc(Width*Height*sizeof(ARGB32)); //new ARGB32[Width*Height];
	Frames->Add(Buffer);
	ARGB32 *b = Buffer;
	for(int n=0; n<Width*Height; n++) *b++ = *s++;
	bm->UnloadBuffer();
	format    = bm->format;
	Bitmap32Driver::SetOwner(this);
Invalidate();
}

Bitmap32::Bitmap32(const class Rect &rect, int f) : Bitmap32Driver(){

	Cache = 0;
	CacheCount = 0;
	Name = "";

	Width   = rect.Width;
	Height  = rect.Height;
	Frames = new DynamicArray();
	frame = 0;
	ARGB32 *Buffer    = (ARGB32*)malloc(Width*Height*sizeof(ARGB32)); //new ARGB32[Width*Height];
	Frames->Add(Buffer);
	format	= f;
	Bitmap32Driver::SetOwner(this);
Invalidate();
}

Bitmap32::~Bitmap32(){

//assert(CacheCount==0);
	if(CacheCount) return;

	if(Bitmap32Cache){
		int cnt = Bitmap32Cache->GetCount();
		for(int n=0; n<cnt; n++){
			BmCache *c = (BmCache*)Bitmap32Cache->GetItem(n);
			if(c->Bitmap==this){
				Bitmap32Cache->Remove(n);
				delete c;
				break;
			}
		}
	}

	if(Frames){
		int cnt = Frames->GetCount();
		for(int n=0; n<cnt; n++) free(Frames->GetItem(n));
	}
	if(Cache) Cache->SubCache();
}

int Bitmap32::GetFrameCount(void) const{
	//assert(Cache==0);
	if(Cache) return Cache->GetFrameCount();
	else return Frames->GetCount();
}

ARGB32 *Bitmap32::LoadBuffer(const int f) const{
	//assert(Cache==0);
	if(Cache) return (ARGB32*)Cache->Frames->GetItem(f);
	else      return (ARGB32*)Frames->GetItem(f);
}

Bitmap32 &Bitmap32::operator*=(const ARGB &col){
	assert(Cache==0);

	DisableCache();

	uint16 am = uint16(col.A*256.0f);
	uint16 rm = uint16(col.R*256.0f);
	uint16 gm = uint16(col.G*256.0f);
	uint16 bm = uint16(col.B*256.0f);

	ARGB32 *b = (ARGB32*)Frames->GetItem(frame);
	for(int n=0; n<Width*Height; n++){
		ARGB32 c = *b;
		uint16 ca = c.A;
		uint16 cr = c.R;
		uint16 cg = c.G;
		uint16 cb = c.B;
		*b++=ARGB32(uint8((ca*am)>>8),uint8((cr*rm)>>8),uint8((cg*gm)>>8),uint8((cb*bm)>>8));
	}

	return *this;
}

Bitmap32 &Bitmap32::operator*=(const class Bitmap32 &from){
	assert(Cache==0);

	DisableCache();

	ARGB32 *fbm = from.LoadBuffer(from.GetFrame());
	ARGB32 *Buffer = (ARGB32*)Frames->GetItem(frame);
	int w = GetWidth();
	int h = GetHeight();
	int fw = from.GetWidth();
	int fh = from.GetHeight();
	if(w>fw) w=fw;
	if(h>fh) h=fh;
	for(int y=0; y<h; y++){
		int ywidth = y*Width;
		int yfw = y*fw;
		for(int x=0; x<w; x++){
			ARGB32 col1 = Buffer[ywidth+x];
			ARGB32 col2 = fbm[yfw+x];
			uint16 a = uint16(col1.A)*uint16(col2.A);
			uint16 r = uint16(col1.R)*uint16(col2.R);
			uint16 g = uint16(col1.G)*uint16(col2.G);
			uint16 b = uint16(col1.B)*uint16(col2.B);
			#if 1
			Buffer[ywidth+x] = ARGB32(a>>8,r>>8,g>>8,b>>8);
			#else
			uint32 res =
				((uint16(col1.A)+uint16(col2.A))<<27)|
				((uint16(col1.R)+uint16(col2.R))<<18)|
				((uint16(col1.G)+uint16(col2.G))<<9)|
				 (uint16(col1.B)+uint16(col2.B));
			res|=((res&0x4020100)*0xff)>>8;
			Buffer[ywidth+x] = ARGB32((res>>27)&0xff,(res>>18)&0xff,(res>>9)&0xff,res&0xff);
			#endif
		}
//		for(x=w; x<(GetWidth()>from.GetWidth()?GetWidth():from.GetWidth()); x++) Buffer[ywidth+x] = ARGB32(0,0,0,0);
	}
//	for(y=h; y<(GetHeight()>from.GetHeight()?GetHeight():from.GetHeight()); y++){
//		int ywidth = y*Width;
//		for(int x=0; x<(GetWidth()>from.GetWidth()?GetWidth():from.GetWidth()); x++) Buffer[ywidth+x] = ARGB32(0,0,0,0);
//	}
	from.UnloadBuffer();

	return *this;
}

Bitmap32 &Bitmap32::operator*=(class Bitmap &from){
	assert(Cache==0);

	DisableCache();

	ARGB *fbm = from.LoadBuffer(from.GetFrame());
	ARGB32 *Buffer = (ARGB32*)Frames->GetItem(frame);
	int w = GetWidth();
	int h = GetHeight();
	int fw = from.GetWidth();
	int fh = from.GetHeight();
	if(w>fw) w=fw;
	if(h>fh) h=fh;
	for(int y=0; y<h; y++){
		int ywidth = y*Width;
		int yfw = y*fw;
		for(int x=0; x<w; x++){
			ARGB32 col1 = Buffer[ywidth+x];
			ARGB col2 = fbm[yfw+x];
			uint8 a = uint8(float(col1.A)*col2.A);
			uint8 r = uint8(float(col1.R)*col2.R);
			uint8 g = uint8(float(col1.G)*col2.G);
			uint8 b = uint8(float(col1.B)*col2.B);
			Buffer[ywidth+x] = ARGB32(a,r,g,b);
			/*
			uint32 res =
				((uint16(col1.A)+uint16(col2.A))<<27)|
				((uint16(col1.R)+uint16(col2.R))<<18)|
				((uint16(col1.G)+uint16(col2.G))<<9)|
				 (uint16(col1.B)+uint16(col2.B));
			res|=((res&0x4020100)*0xff)>>8;
			Buffer[ywidth+x] = ARGB32((res>>27)&0xff,(res>>18)&0xff,(res>>9)&0xff,res&0xff);
			*/
		}
	}
	from.UnloadBuffer();

	return *this;
}

void Bitmap32::Multiply(const ARGB &col, const Rect &rect){
	assert(Cache==0);

	DisableCache();

	uint16 am = uint16(col.A*256.0f);
	uint16 rm = uint16(col.R*256.0f);
	uint16 gm = uint16(col.G*256.0f);
	uint16 bm = uint16(col.B*256.0f);

	int t = int(rect.y);
	int l = int(rect.x);
	int b = int(rect.ly);
	int r = int(rect.rx);
	int w = int(rect.Width);

	ARGB32 *Buffer = (ARGB32*)Frames->GetItem(frame);
	for(int y=t; y<b; y++){
		for(int x=l; x<r; x++){
			ARGB32 c = Buffer[y*w+x];
			uint16 ca = c.A;
			uint16 cr = c.R;
			uint16 cg = c.G;
			uint16 cb = c.B;
			Buffer[y*w+x]=ARGB32(uint8((ca*am)>>8),uint8((cr*rm)>>8),uint8((cg*gm)>>8),uint8((cb*bm)>>8));
		}
	}
}

void Bitmap32::Clear(void){
	assert(Cache==0);

	DisableCache();

	ARGB32 *b = (ARGB32*)Frames->GetItem(frame);
	for(int n=0; n<Width*Height; n++) *b++=ARGB32(0,0,0,0);
}

void Bitmap32::Add(Bitmap32 *from){

	assert(Cache==0);

	DisableCache();

//	ARGB32 *b = Buffer;
//	for(int n=0; n<Width*Height; n++) *b++=ARGB32(0,50,0,0);
//	return;

//	if(isfile){
//		LoadBuffer(0);
//		isfile = false;
//		count=0;
//	}
	ARGB32 *fbm = from->LoadBuffer(0);
	ARGB32 *Buffer = (ARGB32*)Frames->GetItem(frame);
	int w = GetWidth();
	int h = GetHeight();
	int fw = from->GetWidth();
	int fh = from->GetHeight();
	if(w>fw) w=fw;
	if(h>fh) h=fh;
	for(int y=0; y<h; y++){
		int ywidth = y*Width;
		int yfw = y*fw;
		for(int x=0; x<w; x++){
			ARGB32 col1 = Buffer[ywidth+x];
			ARGB32 col2 = fbm[yfw+x];
			uint16 a = uint16(col1.A)+uint16(col2.A);
			uint16 r = uint16(col1.R)+uint16(col2.R);
			uint16 g = uint16(col1.G)+uint16(col2.G);
			uint16 b = uint16(col1.B)+uint16(col2.B);
			if(a>255) a = 255;
			if(r>255) r = 255;
			if(g>255) g = 255;
			if(b>255) b = 255;
			Buffer[ywidth+x] = ARGB32(a,r,g,b);
			/*
			uint32 res =
				((uint16(col1.A)+uint16(col2.A))<<27)|
				((uint16(col1.R)+uint16(col2.R))<<18)|
				((uint16(col1.G)+uint16(col2.G))<<9)|
				 (uint16(col1.B)+uint16(col2.B));
			res|=((res&0x4020100)*0xff)>>8;
			Buffer[ywidth+x] = ARGB32((res>>27)&0xff,(res>>18)&0xff,(res>>9)&0xff,res&0xff);
			*/
		}
	}
	from->UnloadBuffer();
}

void Bitmap32::Scale(const Rect &rect){
	assert(Cache==0);

	DisableCache();

	ARGB32 *obm = (ARGB32*)Frames->GetItem(frame);
	int nw = int(rect.Width);
	int nh = int(rect.Height);
	ARGB32 *Buffer = (ARGB32*)malloc(nw*nh*sizeof(ARGB32)); //new ARGB32[nw*nh]; //(ARGB32*)malloc(nw*nh*sizeof(ARGB32));
	Frames->SetItem(frame,Buffer);
	int w = nw;//GetWidth()  > nw ? nw : GetWidth();
	int h = nh;//GetHeight() > nh ? nh : GetHeight();
	int sx = int((float(GetWidth())/rect.Width)*255.0f);
	int sy = int((float(GetHeight())/rect.Height)*255.0f);
	for(int y=0; y<h; y++){
		for(int x=0; x<w; x++) Buffer[y*nw+x]=obm[((y*sy)>>8)*Width+((x*sx)>>8)];
	}
	Width  = nw;
	Height = nh;
	free(obm); //delete Buffer; //free(obm);
}

void Bitmap32::SetSize(const Rect &rect){
	assert(Cache==0);

	DisableCache();

	ARGB32 *obm = (ARGB32*)Frames->GetItem(frame);
	int nw = int(rect.Width);
	int nh = int(rect.Height);
	ARGB32 *Buffer = (ARGB32*)malloc(nw*nh*sizeof(ARGB32)); //new ARGB32[nw*nh]; //(ARGB32*)malloc(nw*nh*sizeof(ARGB32));
	Frames->SetItem(frame,Buffer);
	int w = nw;//GetWidth()  > nw ? nw : GetWidth();
	int h = nh;//GetHeight() > nh ? nh : GetHeight();
	//int sx = int((float(GetWidth())/rect.Width)*255.0f);
	//int sy = int((float(GetHeight())/rect.Height)*255.0f);
	if(w>Width) w = Width;
	if(h>Height) h = Height;
	h<<=8;
	w<<=8;
	h-=256;
	w-=256;
	for(int y=128; y<h; y+=256){
		int um = 256-(y-128);
		int lm = (y-128);
		for(int x=128; x<w; x+=256){
			int lm = 256-(x-128);
			int rm = (x-128);
			ARGB32 ul = obm[(y>>8)*Width+(x>>8)];
			ARGB32 ur = obm[(y>>8)*Width+(x>>8)+1];
			uint32 uA = ul.A*lm + ur.A*rm;
			uint32 uR = ul.R*lm + ur.R*rm;
			uint32 uG = ul.G*lm + ur.G*rm;
			uint32 uB = ul.B*lm + ur.B*rm;
			ARGB32 ll = obm[((y>>8)+1)*Width+(x>>8)];
			ARGB32 lr = obm[((y>>8)+1)*Width+(x>>8)+1];
			uint32 lA = ll.A*lm + lr.A*rm;
			uint32 lR = ll.R*lm + lr.R*rm;
			uint32 lG = ll.G*lm + lr.G*rm;
			uint32 lB = ll.B*lm + lr.B*rm;
			Buffer[(y>>8)*nw+(x>>8)]=ARGB32((uA*um+lA*lm)>>16,(uR*um+lR*lm)>>16,(uG*um+lG*lm)>>16,(uB*um+lB*lm)>>16);
		}
	}
	Width  = nw;
	Height = nh;
	free(obm); //delete Buffer; //free(obm);
}

void Bitmap32::CopyRect(Bitmap32 *frombm, const Rect &from, const Rect &to){
	assert(Cache==0);

	DisableCache();

	ARGB32 *fbm = frombm->LoadBuffer(0);
	ARGB32 *Buffer = (ARGB32*)Frames->GetItem(frame);
	int tox   = int(to.x);
	int toy   = int(to.y);
	int toly  = int(to.ly);
	int torx  = int(to.rx);
	int fromx = int(from.x);
	int fromy = int(from.y);
	//int fh_over_th = int((from.Height/to.Height)*256.0f);
	//int fw_over_tw = int((from.Width/to.Width)*256.0f);
	int fw = from.Width;
	int fh = from.Height;
	if(fh<0){
		fh = -fh;
		fromy-=fh;
		int h = (toly-toy);
		toy+=h;
		toly-=h;
	}
	int tw = to.Width;
	int th = to.Height;
	if(th<0){
		th=-th;
		for(int y=toly; y<toy; y++){
			for(int x=tox; x<torx; x++){
				Buffer[y*Width+x]=
	//				fbm[(fromy+((y-toy)*fh_over_th)>>8)*frombm->Width+
	//				     fromx+((x-tox)*fw_over_tw)>>8];
					fbm[int(fromy+((toy-y-1)*fh/th))*frombm->Width+
						int(fromx+ (x-tox)*fw/tw)];
			}
		}
	}else{
		for(int y=toy; y<toly; y++){
			for(int x=tox; x<torx; x++){
				Buffer[y*Width+x]=
	//				fbm[(fromy+((y-toy)*fh_over_th)>>8)*frombm->Width+
	//				     fromx+((x-tox)*fw_over_tw)>>8];
					fbm[int(fromy+((y-toy)*fh/th))*frombm->Width+
						int(fromx+ (x-tox)*fw/tw)];
			}
		}
	}
	frombm->UnloadBuffer();
//	Invalidate();
}


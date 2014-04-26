
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <posix/stdlib.h>
#include <posix/stdio.h>
#include <posix/string.h>
#include <Interface/Bitmap32.h>
#include "Kernel.h"
#include "Bitmap.h"
#include "Rect.h"
#include "DynamicArray.h"
#include "Font.h"
#include <assert.h>

/*
class Font{
public:			Font(char *name);
				~Font();
		Bitmap	*GetString(char *);
private:
struct	FInfo	*FontInfo;
		Bitmap	*orgbm;
};
*/

inline uint32 BigEndian(uint32 v){
	return(((v&0xff000000)>>24)|((v&0xff0000)>>8)|((v&0xff00)<<8)|((v&0xff)<<24));
}

//#include <Interface/ARGB32.h>

#include <Interface/String.h>

static DynamicArray *FontCache = 0;
struct FCache{
	String		Name;
	Font		*font;
};

Font::Font(const char *name){

	// Filename caching
	Cache = 0;
	CacheCount = 0;

	Size = 1.0f/80.0f;

if(!FontCache) FontCache = new DynamicArray();

	int cnt = FontCache->GetCount();
	for(int n=0; n<cnt; n++){
		FCache *c = (FCache*)FontCache->GetItem(n);
		if(c->Name==name){
			c->font->AddCache();
			Cache = c->font;
//			Width = Cache->Width;
//			Height = Cache->Height;
//			Frames = 0;
//			frame = 0;
//BitmapDriver::SetOwner(this);
			return;
		}
	}

	FCache *c = new FCache;
	c->Name = name;
	c->font = this;
	FontCache->Add(c);

	FontInfo = new FInfo[256];
	for(n=0; n<256; n++){
		FontInfo[n].x = -1;
	}
	char finame[1024];
	char fname[1024];
	sprintf(finame,"%s_info",name);
	sprintf(fname,"%s",name);
	FILE *fi = fopen(finame,"rb");
//printf("A\n");
	orgbm = new Bitmap(String(fname),Bitmap::cARGB);
//printf("B2\n");
	orgbm32 = new Bitmap32(String(fname),Bitmap32::cARGB); //orgbm);
//printf("C\n");
	fread(&cnt,1,sizeof(uint32),fi);
	cnt = BigEndian(cnt);
	//FILE *foo=fopen("debug.txt","wb");
	//fprintf(foo,"%x\n",cnt);
//puts("Hey");
	for(n=0; n<cnt; n++){
		uint32 c,x,y,w,h,b;
		fread(&c,1,sizeof(uint32),fi);
		fread(&x,1,sizeof(uint32),fi);
		fread(&y,1,sizeof(uint32),fi);
		fread(&w,1,sizeof(uint32),fi);
		fread(&h,1,sizeof(uint32),fi);
		fread(&b,1,sizeof(uint32),fi);
//printf("%c:%x,%d,%d,%d,%d,%d\n",BigEndian(c),BigEndian(c),BigEndian(x),BigEndian(y),BigEndian(w),BigEndian(h),BigEndian(b));
		c = BigEndian(c)&0xff;
//	printf("%c",c);
		//if((c>=0)&&(c<256)){
			FontInfo[c].x = BigEndian(x);
			FontInfo[c].y = BigEndian(y);
			FontInfo[c].Width = BigEndian(w);
			FontInfo[c].Height = BigEndian(h);
			FontInfo[c].Baseline = BigEndian(b);
		//}
	}
	//fclose(foo);
	fclose(fi);
	CharData = 0;
}

Font::~Font(){
assert(CacheCount==0);
	if(!Cache){
		delete orgbm;
		delete orgbm32;
		delete []FontInfo;
	}

	if(FontCache){
		int cnt = FontCache->GetCount();
		for(int n=0; n<cnt; n++){
			FCache *c = (FCache*)FontCache->GetItem(n);
			if(c->font==this){
				FontCache->Remove(n);
				delete c;
				break;
			}
		}
	}
	if(Cache) Cache->SubCache();
}

int Font::GetHeight(void){
	if(Cache) return Cache->FontInfo['\n'].Height;
	else return(FontInfo['\n'].Height);
}

Bitmap *Font::GetString(const char *_str){
//	assert(Cache==0);
	if(Cache) return Cache->GetString(_str);
	else{
		const char *str = _str;
		int len = strlen(str);
		int Width=0;
		int Height=0;
		int lines = 1;
		int lineWidth=0;
		for(int n=0; n<len; n++){
			unsigned char c=*str++;
			if(c=='\n'){
				if(lineWidth>Width) Width=lineWidth;
				lines++;
				lineWidth=0;
			}
			else lineWidth+=int(float(FontInfo[c].Width)*1.2f);
		}
		if(lineWidth>Width) Width=lineWidth;
		str = _str;
		Height = FontInfo['\n'].Height*lines;
		int bl = 35;
//printf("%s,%d,%d\n",_str,Width,Height);
		Bitmap *bm = new Bitmap(Rect(float(Width),float(Height)),Bitmap::cARGB);
		int x=0,y=0;
	//	FILE *foo=fopen("debug.txt","wb");
		bm->Clear();
//bm->CopyRect(orgbm,Rect(0,0,1,1),Rect(0,0,1,1));

		for(n=0; n<len; n++){
			unsigned char c=*str++;
			if(c=='\n'){
				x=0;
				y+=FontInfo['\n'].Height;
			}else{
	//			printf("%f,%f,%f,%f-%f,%f,%f,%f\n",float(FontInfo[c].x),float(FontInfo[c].y),float(FontInfo[c].x+FontInfo[c].Width),float(FontInfo[c].y+FontInfo[c].Height),float(x),float(y+bl-FontInfo[c].Height-FontInfo[c].Baseline),float(x+FontInfo[c].Width),float(y+bl-FontInfo[c].Baseline));

				bm->CopyRect(orgbm,
					Rect(float(FontInfo[c].x),float(FontInfo[c].y),float(FontInfo[c].x+FontInfo[c].Width),float(FontInfo[c].y+FontInfo[c].Height)),
					Rect(float(x),5.0f+float(y+bl-FontInfo[c].Height-FontInfo[c].Baseline),float(x+FontInfo[c].Width),5.0f+float(y+bl-FontInfo[c].Baseline)));
				x+=int(float(FontInfo[c].Width)*1.2f);
			}
		}

	//	fclose(foo);
		return bm;
	}
}

Bitmap32 *Font::GetString32(const char *_str){
//	assert(Cache==0);
	if(Cache) return Cache->GetString32(_str);
	else{
		const char *str = _str;
		int len = strlen(str);
		int Width=0;
		int Height=0;
		int lines = 1;
		int lineWidth=0;
		for(int n=0; n<len; n++){
			unsigned char c=*str++;
			if(c=='\n'){
				if(lineWidth>Width) Width=lineWidth;
				lines++;
				lineWidth=0;
			}
			else lineWidth+=int(float(FontInfo[c].Width)*1.2f);
		}
		if(lineWidth>Width) Width=lineWidth;
		str = _str;
		Height = FontInfo['\n'].Height*lines;
		int bl = 35;
	//printf("%s,%d,%d\n",_str,Width,Height);
		Bitmap32 *bm = new Bitmap32(Rect(float(Width),float(Height)),Bitmap::cARGB);
		int x=0,y=0;
	//	FILE *foo=fopen("debug.txt","wb");
		bm->Clear();
	//bm->CopyRect(orgbm,Rect(0,0,1,1),Rect(0,0,1,1));
		for(n=0; n<len; n++){
			unsigned char c=*str++;
			if(c=='\n'){
				x=0;
				y+=FontInfo['\n'].Height;
			}else{
	//			printf("%f,%f,%f,%f-%f,%f,%f,%f\n",float(FontInfo[c].x),float(FontInfo[c].y),float(FontInfo[c].x+FontInfo[c].Width),float(FontInfo[c].y+FontInfo[c].Height),float(x),float(y+bl-FontInfo[c].Height-FontInfo[c].Baseline),float(x+FontInfo[c].Width),float(y+bl-FontInfo[c].Baseline));

				bm->CopyRect(orgbm32,
					Rect(float(FontInfo[c].x),float(FontInfo[c].y),float(FontInfo[c].x+FontInfo[c].Width),float(FontInfo[c].y+FontInfo[c].Height)),
					Rect(float(x),5.0f+float(y+bl-FontInfo[c].Height-FontInfo[c].Baseline),float(x+FontInfo[c].Width),5.0f+float(y+bl-FontInfo[c].Baseline)));
				x+=int(float(FontInfo[c].Width)*1.2f);
			}
		}
	//	fclose(foo);

	//ARGB32 *buf = bm->LoadBuffer(0);
	//for(n=0; n<bm->GetWidth()*bm->GetHeight(); n++){
	//	ARGB32 col = *buf++;
	//	printf("%d,%d,%d,%d\n",col.A,col.R,col.G,col.B);
	//}
	//bm->UnloadBuffer();

		return bm;
	}
}

Bitmap *Font::GetBitmap(void){
	assert(Cache==0);
	return(orgbm);
}

FInfo *Font::GetFontInfo(void){
	assert(Cache==0);
	return(FontInfo);
}

float Font::GetSize(void){
//	assert(Cache==0);
	return Size;
}

void Font::SetSize(float s){
	Size = s;
}

float Font::GetWidth(const char *str){
	if(Cache){
		int len = strlen(str);
		float lineWidth = 0;
		float Width = 0;
		for(int n=0; n<len; n++){
			char c=*str++;
			if(c=='\n'){
				if(lineWidth>Width) Width=lineWidth;
				lineWidth=0;
			}else if(c!=' '){
	if(Cache->FontInfo[int(c)&0xff].Width>0&&Cache->FontInfo[int(c)&0xff].Width<50)
				lineWidth+=int(float(Cache->FontInfo[int(c)&0xff].Width));//*1.2f);
	else printf("Insane width of character %c (%f)\n",c,float(Cache->FontInfo[int(c)&0xff].Width));
				lineWidth+=Cache->FontInfo[' '].Width*.05f;
			}else lineWidth+=int(float(Cache->FontInfo[int(c)&0xff].Width));//*1.2f);
		}
		if(lineWidth>Width) Width=lineWidth;
	//printf("!!!!!!!!! %f\n",Width/GetHeight()*Size);
		return Width/GetHeight()*Size;
	}else{
		int len = strlen(str);
		float lineWidth = 0;
		float Width = 0;
		for(int n=0; n<len; n++){
			char c=*str++;
			if(c=='\n'){
				if(lineWidth>Width) Width=lineWidth;
				lineWidth=0;
			}else if(c!=' '){
	if(FontInfo[int(c)&0xff].Width>0&&FontInfo[int(c)&0xff].Width<50)
				lineWidth+=int(float(FontInfo[int(c)&0xff].Width));//*1.2f);
	else printf("Insane width of character %c (%f)\n",c,float(FontInfo[int(c)&0xff].Width));
				lineWidth+=FontInfo[' '].Width*.05f;
			}else lineWidth+=int(float(FontInfo[int(c)&0xff].Width));//*1.2f);
		}
		if(lineWidth>Width) Width=lineWidth;
	//printf("!!!!!!!!! %f\n",Width/GetHeight()*Size);
		return Width/GetHeight()*Size;
	}
}

#define __MSC__
#include <Interface/ARGB.h>
#include <glide.h>
#include <math.h>

struct CharInfo{
	float		Width,Height,Baseline;
	GrTexInfo	TexInfo;
	GrTexInfo	AlphaInfo;
};

DynamicArray *Font::GetCharData(void){
//printf("111\n");
	if(Cache) return Cache->GetCharData();
	else{
//printf("222\n");
	//return 0;
		// Convert font to 656
		if(CharData) return CharData;
//printf("333\n");
		CharData = new DynamicArray();
	//	Cache->CharData = CharData;
		FInfo *inf = GetFontInfo();
		Bitmap *bitm = GetBitmap();
		int bmw = bitm->GetWidth();
		int bmh = bitm->GetHeight();
//printf("444\n");
		ARGB *bm = bitm->LoadBuffer(0);
//printf("555\n");
		for(int n=0; n<256; n++){
//printf("a: %d\n",n);
			if((inf[n].x>=0&&/*inf[n].Width>0&&*/inf[n].Height>0)||n==' '){
//printf("b\n");
				int xofs = inf[n].x;
				int yofs = inf[n].y;
				uint16 *tex = new uint16[32*32];		// fixme: Where is these freed.
				uint8 *alpha = new uint8[32*32];
				CharInfo *cinfo = new CharInfo;
				cinfo->Width  = float(inf[n].Width);
				cinfo->Height = float(inf[n].Height);
				cinfo->Baseline = float(inf[n].Baseline);
	cinfo->TexInfo.smallLod		 = GR_LOD_32;
	cinfo->TexInfo.largeLod		 = GR_LOD_32;
	cinfo->TexInfo.aspectRatio	 = GR_ASPECT_1x1;
	cinfo->TexInfo.format		 = GR_TEXFMT_RGB_565;
	cinfo->TexInfo.data			 = tex;
	cinfo->AlphaInfo.smallLod	 = GR_LOD_32;
	cinfo->AlphaInfo.largeLod	 = GR_LOD_32;
	cinfo->AlphaInfo.aspectRatio = GR_ASPECT_1x1;
	cinfo->AlphaInfo.format		 = GR_TEXFMT_RGB_565;
	cinfo->AlphaInfo.data		 = alpha;
//printf("c\n");
				#ifdef _DEBUG
				CharData->Add(cinfo,__FILE__,__LINE__);
				#else
				CharData->Add(cinfo);
				#endif
//printf("d\n");
				//for(int y=0; y<inf[n].Height; y++){
				//	for(int x=0; x<inf[n].Width; x++){
				//		data[y*inf[n].Width+x]=0;
				//	}
				//}
				// fixme: Need bilinear filtering
				ARGB *ch = &bm[yofs*bmw+xofs];
				int cW = inf[n].Width;
				int cH = inf[n].Height;
//printf("e (%x)\n",tex);
		//{
		//for(int y=0; y<16; y++){
		//	for(int x=0; x<16; x++){
		//		tex[y*16+x]=0;
		//	}
		//}
		//}
				for(int y=0; y<32; y++){
					if(y==0||y==31) for(int x=0; x<32; x++) tex[y*32+x] = 0;//x1f<<5;
					else for(int x=0; x<32; x++){
#if 1
						if(x==0||x==31) tex[y*32+x] = 0;//x1f<<5;
						else{
							float xf = float(x-1)*float(cW-1)/(32.0f-2.0f)+.5f;
							float yf = float(y-1)*float(cH-1)/(32.0f-2.0f)+.5f;
							float fx = xf-floor(xf);
							float fy = yf-floor(yf);
							//printf("%f,%f\n",xf,fx);
							ARGB vx0 = ch[int(yf  )*bmw+int(xf)]*fx+ch[int(yf  )*bmw+int(xf)]*(1.0f-fx);
							ARGB vx1 = ch[int(yf+1)*bmw+int(xf)]*fx+ch[int(yf+1)*bmw+int(xf)]*(1.0f-fx);
							ARGB vxy = vx1*fy+vx0*(1.0f-fy);
							tex[y*32+x] = (int(vxy.R*31.1f)<<11) | (int(vxy.G*63.1f)<<5) | int(vxy.B*31.1f);
	/*
							tex[y*16+x]=
								(int(ch[int(yf)*bmw+int(xf)].R*31.1f)<<11) |
								(int(ch[int(yf)*bmw+int(xf)].G*63.1f)<<5) |
								 int(ch[int(yf)*bmw+int(xf)].B*31.1f);
	*/	//						alpha[y*256+x]=int(ch[(y*256/cH)*256+x*256/cW].A*255.1f);
						}
#endif
					}
				}
//printf("f\n");
			}else{
				#ifdef _DEBUG
				CharData->Add(0,__FILE__,__LINE__);
				#else
				CharData->Add(0);
				#endif
				//if(n==':'){
				//	printf("!!!\n");
				//	printf("%d,%d,%d,%d\n",
				//		inf[n].x,
				//		inf[n].y,
				//		inf[n].Width,
				//		inf[n].Height);
				//}
			}
		}
//printf("666\n");
		bitm->UnloadBuffer();
//printf("777\n");
		return CharData;
	}
}


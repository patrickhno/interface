
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include "Debug.h"

#include "BitmapLoader.h"
#include <Interface/Loaders/PngLoader.h>
#include "DynamicArray.h"
#include <string.h>
#include <posix/stdio.h>
#include <assert.h>

#define USE_CACHE

#ifdef USE_CACHE
static DynamicArray *BitmapCache = 0;
static int loadercount = 0;

struct BmCache{
	char	*Name;
	void	*Cache;
	int		Count;
};
#endif

BitmapLoader::BitmapLoader(){
	_Open			= png_Open;
	__Deinitialize	= png_Deinitialize;
	_GetWidth		= png_GetWidth;
	_GetHeight		= png_GetHeight;
	_GetFrameCount	= png_GetFrameCount;
	_LoadFrame		= png_LoadFrame;
	_UnloadFrame	= png_UnloadFrame;
	Cache=0;
	#ifdef USE_CACHE
	if(!BitmapCache) BitmapCache = new DynamicArray();
	loadercount++;
	#endif
}

BitmapLoader::~BitmapLoader(){
//if(!Cache) return;
	#ifdef USE_CACHE
	if(Cache){
		int cnt = BitmapCache->GetCount();
		bool cont=true;
		for(int n=0; (n<cnt)&&cont; n++){
			BmCache *bc = (BmCache*)BitmapCache->GetItem(n);
			if(bc->Cache==Cache){
				if(--bc->Count==0){
					#ifdef _DEBUG
					BitmapCache->Remove(n,__FILE__,__LINE__);
					#else
					BitmapCache->Remove(n);
					#endif
					delete bc->Name;
					if(bc->Cache) __Deinitialize(bc->Cache);
					delete bc;
					//
					//loadercount--;
					//if(loadercount==1){
					//	printf("!!!!! %d\n",BitmapCache->GetCount());
					//	assert(BitmapCache->GetCount()==0);
						if(!BitmapCache->GetCount()){
							delete BitmapCache;
							BitmapCache = 0;
						}
					//}
				}
				cont=false;
			}
		}
		if(cont==true) printf("Warning: Failed to destruct bitmap.\n");
	}
/*
	loadercount--;
	if(!loadercount){
		printf("%d\n",BitmapCache->GetCount());
		assert(BitmapCache->GetCount()==0);
		delete BitmapCache;
		BitmapCache = 0;
	}
*/
	#else
	if(Cache) __Deinitialize(Cache);
	#endif
}

bool BitmapLoader::OpenBitmapFile(char *name){
	if(Cache) __Deinitialize(Cache);		// fixme?
	#ifdef USE_CACHE
	// Seek in cache
	int cnt = BitmapCache->GetCount();
	for(int n=0; n<cnt; n++){
		BmCache *bc = (BmCache*)BitmapCache->GetItem(n);
		if(!strcmp(bc->Name,name)){
			Cache = bc->Cache;
			bc->Count++;
			//return(Cache!=0);
			return false;
		}
	}
	#endif
	//
	Cache=_Open(name);
	assert(Cache!=0);
	#ifdef USE_CACHE
	BmCache *bc = new BmCache;
	bc->Name = new char[strlen(name)+1];
	sprintf(bc->Name,"%s",name);
	bc->Cache = Cache;
	bc->Count = 1;
	#ifdef _DEBUG
	BitmapCache->Add(bc,__FILE__,__LINE__);
	#else
	BitmapCache->Add(bc);
	#endif
	#endif
	//return(Cache!=0);
	return true;
}
/*
bool BitmapLoader::OpenBitmapFile(BitmapLoader &bm){
	if(Cache) __Deinitialize(Cache);		// fixme?
	#ifdef USE_CACHE
	// Seek in cache
	int cnt = BitmapCache->GetCount();
	for(int n=0; n<cnt; n++){
		BmCache *bc = (BmCache*)BitmapCache->GetItem(n);
		if(!strcmp(bc->Name,name)){
			Cache = bc->Cache;
			bc->Count++;
			//return(Cache!=0);
			return false;
		}
	}
	#endif
	//
	Cache=_Open(name);
	#ifdef USE_CACHE
	BmCache *bc = new BmCache;
	bc->Name = new char[strlen(name)+1];
	sprintf(bc->Name,"%s",name);
	bc->Cache = Cache;
	bc->Count = 1;
	BitmapCache->Add(bc);
	#endif
	//return(Cache!=0);
	return true;
}
*/
int BitmapLoader::GetBitmapWidth(void){
	return(_GetWidth(Cache));
}
int BitmapLoader::GetBitmapHeight(void){
	return(_GetHeight(Cache));
}
int BitmapLoader::GetFrameCount(void){
	return(_GetFrameCount(Cache));
}
class ARGB *BitmapLoader::LoadFrame(int frame){
	return(_LoadFrame(Cache,frame));
}
void BitmapLoader::UnloadFrame(ARGB *buf){
	_UnloadFrame(Cache,buf);
}


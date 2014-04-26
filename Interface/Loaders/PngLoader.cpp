
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <posix/stdio.h>
#include <png.h>
#include <Interface/ARGB.h>
#include <Interface/DynamicArray.h>
//#include <memory.h>
//#include "malloc.h"
//#include "stdio.h"
#include "PngLoader.h"
#include <posix/stdlib.h>
#include <assert.h>

//#include "Debug.h"

#define PNG_BYTES_TO_CHECK 4

char	*MakeFilename(int, char*, char*);

class PngImgData{
public:
		PngImgData(){};
		~PngImgData(){ delete ImgName; }
		char	*ImgName;
		int		width;
		int		height;
};

static foo = 0;

extern "C" int posix_printf(const char *,...);

void *png_Open(char *name){
	FILE			*fp;
	png_infop		info_ptr;
	png_structp		png_ptr;
	png_uint_32		width, height;
	unsigned int	sig_read = 0;
	int				bit_depth, color_type, interlace_type;
//	int				MaxPics = 3;
	char			buf[PNG_BYTES_TO_CHECK];
	char			*filename;
	PngImgData		*pid;

	DynamicArray* ptrCache=new DynamicArray(DARRAY_GENID);	// fixme: GENID
	int i=-1;
	
	while(true){
redo:	filename=MakeFilename(i,name, ".png");		// fixme: Free filenames
		if((fp = fopen(filename,"rb")) == NULL){
			free(filename);
			if(i==-1){ i++; goto redo;}
			if(ptrCache){ 
				if(png_GetFrameCount(ptrCache)>0) return (void*)ptrCache;
				png_Deinitialize(ptrCache);
			}
			posix_printf("File \"%s\" not found!\n",filename); // File couldn't be opened.
			assert(false);
			return NULL;
		}
//_printf("File \"%s\" OK!\n",filename);

		if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK){
			free(filename);
			fclose(fp);
			if (ptrCache){ 
				if (png_GetFrameCount(ptrCache)>0);
					return (void*)ptrCache;
				png_Deinitialize(ptrCache);
				}
			assert(false);
			return NULL;// Not a valid PNG file.
		}
		
		// This is a pngfile. Read its infostruct
		// FIXME:
		
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		
		if (png_ptr == NULL){
			free(filename);
			fclose(fp);
			if (ptrCache){ 
				if (png_GetFrameCount(ptrCache)>0);
					return (void*)ptrCache;
				png_Deinitialize(ptrCache);
				}
			assert(false);
			return NULL;
		}

		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL){
			free(filename);
			fclose(fp);
			png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
			if (ptrCache){ 
				if (png_GetFrameCount(ptrCache)>0);
					return (void*)ptrCache;
				png_Deinitialize(ptrCache);
				}
			assert(false);
			return NULL;
		}
		
		if (setjmp(png_ptr->jmpbuf)){
			free(filename);
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			fclose(fp);
			if (ptrCache){ 
				if (png_GetFrameCount(ptrCache)>0);
					return (void*)ptrCache;
				png_Deinitialize(ptrCache);
				}
			assert(false);
			return NULL;
		}
		png_init_io(png_ptr, fp);	
		png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);
		
		png_read_info(png_ptr, info_ptr);
		png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
			&interlace_type, NULL, NULL);
png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		
		// This is a file we want. Make a ptrCache for it and store it away!		
fclose(fp);		
		pid = new PngImgData();
		pid->ImgName=filename;
		pid->width=info_ptr->width; 
		pid->height=info_ptr->height;
		#ifdef _DEBUG
		ptrCache->Add(pid,__FILE__,__LINE__);
		#else
		ptrCache->Add(pid);
		#endif
		i++;
foo++;
	}
	return (void*)ptrCache;	//return pointer to cache;
}

// Frees the cache
void png_Deinitialize(void *ptrCache){
	DynamicArray	*daPtr;
	int				cnt;
	if(ptrCache){
		daPtr=(DynamicArray*)ptrCache;
//		printf("Deinitializing!\n");
		cnt=daPtr->GetCount(); //
		for(int i=0; i<cnt; i++)
			if(daPtr->GetItem(i)){
				delete (PngImgData*)(daPtr->GetItem(i));
				foo--;
			}
		delete daPtr;
	}
//	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!! %d\n",foo);
}

int png_GetWidth(void *ptrCache){
	PngImgData *pidPtr;
	DynamicArray *daPtr;
	daPtr=(DynamicArray*)ptrCache;
	pidPtr=(PngImgData*)daPtr->GetItem(0);
//	printf("Getting width!\n");
	return pidPtr->width;
}

int png_GetHeight(void *ptrCache){
	PngImgData *pidPtr;
	DynamicArray *daPtr;
	daPtr=(DynamicArray*)ptrCache;
	pidPtr=(PngImgData*)daPtr->GetItem(0);
//	printf("Getting height!\n");
	return pidPtr->height;
}

int png_GetFrameCount(void *ptrCache){
	DynamicArray *daPtr;
	daPtr=(DynamicArray*)ptrCache;
	return int(daPtr->GetCount());
}

// Allocate buffers, load picture[index] and return a ptr to the bitmap
#if 1
ARGB *png_LoadFrame(void *ptrCache, int index){
	//printf("æasdgnlhæskljgælSDKJGÆKLSjdgljfg\n");
	png_structp		png_ptr;
	png_infop		info_ptr;
	png_uint_32		width, height;
	int				bit_depth, color_type, interlace_type;
	FILE			*fp;
	char			*name;

//posix_printf("png Loading frame %d\n",index);
	
	DynamicArray*	daPtr;
	PngImgData*		pidPtr;

	daPtr=(DynamicArray*)ptrCache;
	pidPtr=(PngImgData*)daPtr->GetItem(index);
	name=pidPtr->ImgName;
//posix_printf("LoadFrame imagename: %s\n", pidPtr->ImgName);
	
	if ((fp = fopen(/*"C:\\Games\\Game\\Textures\\Rock0000.png"*/name, "rb")) == NULL)
      return NULL;

//posix_printf("1\n");
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
//posix_printf("2\n");
	if (png_ptr == NULL){
		fclose(fp);
		return NULL;
	}
	
//posix_printf("3\n");
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL){
		fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return NULL;
	}
	
//posix_printf("4\n");
	if (setjmp(png_ptr->jmpbuf)){
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		return NULL;
	}

//posix_printf("5\n");
	png_init_io(png_ptr, fp);	
	
//posix_printf("6\n");
	png_read_info(png_ptr, info_ptr);
	
//posix_printf("7\n");
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		&interlace_type, NULL, NULL);

//posix_printf("8\n");
if (bit_depth==16){ 
		printf("Stripping to 8 bits\n");
		png_set_strip_16(png_ptr);
	}
	/* expand paletted or RGB images with transparency to full alpha channels
    * so the data will be available as RGBA quartets */{


//posix_printf("9\n");
if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
//		printf("Expanding tRNS\n");	
		png_set_expand(png_ptr);
	}

//posix_printf("10\n");
if (color_type == PNG_COLOR_TYPE_PALETTE){
//		posix_printf("Expanding palette\n");	
		int channels=png_get_channels(png_ptr, info_ptr);
//		posix_printf("Channels: %d\n", channels);
		png_set_expand(png_ptr);
	}


//posix_printf("11\n");
if ((color_type == PNG_COLOR_TYPE_RGB) && (bit_depth==8)){
		posix_printf("Bit depth==8 og RGB\n");
		png_set_filler(png_ptr, 0xff, PNG_FILLER_BEFORE);
	}

//posix_printf("12\n");
if (color_type==PNG_COLOR_TYPE_RGB_ALPHA){
//	posix_printf("RGB w/ Alpha. Switching to ARGB\n");
	png_set_swap_alpha(png_ptr);
}

//posix_printf("13\n");
if (bit_depth<8){
	posix_printf("Bitdepth < 8\n");
	png_set_packing(png_ptr);
}

//printf("-----   %d\n",color_type);

	
//posix_printf("14\n");
	png_read_update_info(png_ptr, info_ptr);
//posix_printf("15\n");

	/* allocate the memory to hold the image using the fields of info_ptr. */
	/* the easiest way to read the image */
	
	png_bytep *row_pointers = (png_bytep*)malloc(sizeof(png_bytep)*height);
		
	for (int row=0; row<height; row++){
//posix_printf("row %d, bytes %d\n",row,png_get_rowbytes(png_ptr, info_ptr));
png_bytep foo = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
//posix_printf("%x\n",foo);
		row_pointers[row] = foo;
	}

//posix_printf("15\n");
	png_read_image(png_ptr, row_pointers);
//posix_printf("16\n");
	png_read_end(png_ptr, info_ptr);
	
	png_bytep rowPtr;
	
	ARGB *argbPtr=(ARGB*)malloc(sizeof(ARGB)*width*height);
	ARGB *ptr = argbPtr;

	for (int i=0; i<height; i++){
		rowPtr=row_pointers[i];
		for (int j=0; j<width; j++){
			ARGB col;
			if (color_type==PNG_COLOR_TYPE_PALETTE)
				col.A=1.0f;
			else
				col.A = ((float)*rowPtr++)/256.0f;
			col.R = ((float)*rowPtr++)/256.0f;
			col.G = ((float)*rowPtr++)/256.0f;
			col.B = ((float)*rowPtr++)/256.0f;
			*ptr++=col;
			//printf("Row: %d A: %f R: %f G: %f B: %f\n", i, argbPtr->A, argbPtr->R, argbPtr->G, argbPtr->B);
			//argbPtr++;	
		}
	}

	for(row=0; row<height; row++){
		free(row_pointers[row]);
	}

//posix_printf("17\n");
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	fclose(fp);
	
//posix_printf("18\n");
	free(row_pointers);
	return argbPtr;
}
#endif

void png_UnloadFrame(void *, ARGB *buf){
	free(buf);
}

char *MakeFilename(int num,char *name,char *ext){
	if(num==-1){
		char *filename = (char *) malloc(strlen(name)+1+4);
		sprintf(filename,"%s%s",name,ext);
		return(filename);
	}else{
		char *filename = (char *) malloc(strlen(name)+9);
		
		     if(num<   10){ sprintf(filename ,"%s000%d%s",name,num,ext); return filename;}
		else if(num<  100){ sprintf(filename , "%s00%d%s",name,num,ext); return filename;}
		else if(num< 1000){ sprintf(filename ,  "%s0%d%s",name,num,ext); return filename;}
		else if(num<10000){ sprintf(filename ,   "%s%d%s",name,num,ext); return filename;}
		return NULL;
	}
}
/*
void main(void){
	ARGB* argbPtr;
	void *ptr;
	
	//printf("Getwidth returned:");
	
	//printf("%d\n", png_GetHeight(png_Open("testimage.png"))); 
	//printf("%d\n", png_GetFrameCount(png_Open("testimage.png"))); 
	//printf("Loading frame 0:");
	ptr=png_Open("testimage");
	printf("Width: %d\n", png_GetWidth(ptr)); 
	argbPtr = png_LoadFrame(ptr, 1);
	
	png_Deinitialize(ptr);
	return;
}
*/

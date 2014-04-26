
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <posix/stdio.h>
#include <windows.h>
#include <assert.h>
#include <crtdbg.h>
#include "Debug.h"
#include "Semaphore.h"
#include "rme_Memory.h"
#include <Interface/DynamicArray.h>
#include <posix/unistd.h>
#include <process.h>
#include <posix/sys/types.h>

#define HEAP_SIZE  (1024*1024*4)
#define BLOCK_SIZE (1024*8)
//#define WARNING_SIZE (HEAP_SIZE*5)

class Heap{
public:
	int		GetBlock(void *);
	Heap	*Next;
	HANDLE	File;
	char	Mask[HEAP_SIZE/BLOCK_SIZE/8];
	char	Type[HEAP_SIZE/BLOCK_SIZE/8];
	int		Size[HEAP_SIZE/BLOCK_SIZE];
	void	*Blocks;
};

int Heap::GetBlock(void *data){
	return (int(data)-int(Blocks))/BLOCK_SIZE;
}

struct Fragment{
	Fragment	*Next,*Prev;
};

static Heap *first_heap;
static Semaphore *heap_sem,*thread_sem;
static Fragment **Fragments = 0;
static bool initialized = false;

static void DumpStuff(void){

	int bytes = 0;
	for(int n=0; n<32; n++){
		Fragment *frag = Fragments[n];
		while(frag){
			bytes+=1<<n;
			frag=frag->Prev;
		}
	}
	printf("%d bytes of unused memory in fragments\n",bytes);
}

static int heap_bytes = 0;
static int heap_used = 0;

struct MemHeader{
	#ifdef MEMORY_STATS
	int				line;
	char			*file;
	MemHeader		*listent,*Next;
	DynamicArray	*history;
	#endif
	int		size;
};

#ifdef _DEBUG
//struct LineInfo{
//	LineInfo	*Next;
//	char		*Name;
//	int			Bytes;
//};
struct BufferInfo{
	BufferInfo	*Next;
	char		*Name;
	int			Bytes;
	void		*Buffer;
	int			AllocLine;
};
//static LineInfo *info = 0;
static BufferInfo *info = 0;

//#define MEMORY_STATS

#ifdef MEMORY_STATS
static DynamicArray *total_used_history = 0;
static DynamicArray *total_size_history = 0;
static DynamicArray *total_frag_history = 0;
static DynamicArray *total_frags_history = 0;
static DynamicArray *total_blocks_history = 0;
#endif

#ifdef MEMORY_STATS
static struct MemHeader *list = 0;
/*
static int get_frag_count(void){
	int frags = 0;
	for(int n=0; n<32; n++){
		Fragment *frag = Fragments[n];
		if(frag){
			while(frag->Prev) frag = frag->Prev;
			while(frag){
				frags++;
				frag = frag->Next;
			}
		}
	}
	return frags;
}
*/
static void ticker(void*){
	thread_sem->Release();
	while(true){
		sleep(60000);
		heap_sem->Acquire();

		int frags_used = 0;
		int frag_count = 0;
		for(int n=0; n<32; n++){
			Fragment *frag = Fragments[n];
			if(frag){
				while(frag->Prev) frag = frag->Prev;
				while(frag){
					frags_used+=(1<<n)+1;
					frag_count++;
					frag = frag->Next;
				}
			}
		}

		Heap *heap = first_heap;
		int blocks = 0;
		while(heap){
			for(int n=0; n<HEAP_SIZE/BLOCK_SIZE; n++){
				if(!(heap->Mask[n/8]&(1<<(n&7)))) blocks++;
			}
			heap = heap->Next;
		}
		blocks*=BLOCK_SIZE;

		MemHeader *ent = list;
		while(ent){
			if(!ent->history) ent->history = new DynamicArray();
			#ifdef _DEBUG
			ent->history->Add((void*)ent->size,__FILE__,__LINE__);
			#else
			ent->history->Add((void*)ent->size);
			#endif
			ent=ent->Next;
		}

		#ifdef _DEBUG
		total_size_history->Add((void*)heap_bytes,__FILE__,__LINE__);
		total_used_history->Add((void*)heap_used,__FILE__,__LINE__);
		total_frag_history->Add((void*)frags_used,__FILE__,__LINE__);
		total_frags_history->Add((void*)frag_count,__FILE__,__LINE__);
		total_blocks_history->Add((void*)blocks,__FILE__,__LINE__);
		#else
		total_size_history->Add(heap_bytes);
		total_used_history->Add(heap_used);
		total_frag_history->Add(frags_used);
		total_frags_history->Add((void*)frag_count,__FILE__,__LINE__);
		total_blocks_history->Add((void*)blocks,__FILE__,__LINE__);
		#endif

		//if(!heap_used_history) heap_used_history = new DynamicArray();
		//heap_used_history->Add((void*)heap_used);
		//if(!heap_size_history) heap_size_history = new DynamicArray();
		//heap_size_history->Add((void*)heap_bytes);
		heap_sem->Release();
	}
}
#endif

#endif

static void DumpStats(void){
	#ifdef _DEBUG
	printf("Shared memory stats:\n");
	BufferInfo *i = info;
	int bytes = 0;
	while(i){
		if(i->Bytes){
			printf("%d bytes in %s, line %d\n",i->Bytes,i->Name,i->AllocLine);
			bytes+=i->Bytes;
		}
		i=i->Next;
	}
	printf("A total of %d\n",bytes);

	#ifdef MEMORY_STATS
	FILE *file = fopen("SharedHeap.csv","wb");
	MemHeader *ent = list;
	while(ent){
		fprintf(file,"%s(%d);",ent->file,ent->line);
		if(ent->history){
			int cnt = ent->history->GetCount();
			for(int n=0; n<cnt; n++) fprintf(file,"%d;",ent->history->GetItem(n));
		}
		fprintf(file,"\n");
		ent = ent->Next;
	}
	fprintf(file,"Total size;");
	int cnt = total_size_history->GetCount();
	for(int n=0; n<cnt; n++) fprintf(file,"%d;",total_size_history->GetItem(n));
	fprintf(file,"\nTotal used;");
	cnt = total_used_history->GetCount();
	for(n=0; n<cnt; n++) fprintf(file,"%d;",total_used_history->GetItem(n));
	fprintf(file,"\nFrags used;");
	cnt = total_frag_history->GetCount();
	for(n=0; n<cnt; n++) fprintf(file,"%d;",total_frag_history->GetItem(n));
	fprintf(file,"\nFrag count;");
	cnt = total_frags_history->GetCount();
	for(n=0; n<cnt; n++) fprintf(file,"%d;",total_frags_history->GetItem(n));
	fprintf(file,"\nBlock count;");
	cnt = total_blocks_history->GetCount();
	for(n=0; n<cnt; n++) fprintf(file,"%d;",total_blocks_history->GetItem(n));
	fclose(file);
	#endif

//	while(true) Sleep(500);
	#endif
}

static Heap *GetHeap(void *data){
	int tmp = int(data);
	Heap *heap = first_heap;
	while(heap){
		int start = int(heap->Blocks);
		int stop  = start+HEAP_SIZE;
		if(tmp>=start&&tmp<stop) return heap;
		heap=heap->Next;
	}
	//assert(false);
	printf("GetHeap: Warning: No heap for buffer %x!\n",data);
	return 0;
}

static int allocated = 0;

#ifdef _DEBUG
static void *internal_Allocate(int size, int flags, char *file, int line);
static void internal_Free(void *, char *file, int line);
#else
static void *internal_Allocate(int size, int flags);
static void internal_Free(void *);
#endif

#ifdef _DEBUG

static void AddToFile(void *buf, char *file, int line, int bytes){
	if(file){
		if(!info){
			info = (BufferInfo*)internal_Allocate(sizeof(BufferInfo),0,0,0);
			info->Name = file;
			info->AllocLine = line;
			info->Next = 0;
			info->Bytes = bytes;
			info->Buffer = buf;
			return;
		}
		BufferInfo *i=info,*l=0;
		while(i){
			if(i->Buffer==buf){
				i->Bytes+=bytes;
				if(i->Bytes <= 0){
					if(l) l->Next = i->Next;
					else info = 0;
					internal_Free(i,0,0);
				}
				return;
			}
			l=i;
			i=i->Next;
		}
		i = (BufferInfo*)internal_Allocate(sizeof(BufferInfo),0,0,0);
		l->Next = i;
		i->Next = 0;
		i->Name = file;
		i->AllocLine = line;
		i->Bytes = bytes;
		i->Buffer = buf;
	}
}

#else
#endif

#ifdef _DEBUG
static void *internal_Allocate(int size, int flags, char *file, int line){
#else
static void *internal_Allocate(int size, int flags){
#endif

//	assert(size!=48);

//int log=1;
//int tmp = size-1;
//while((tmp>>=1)!=0) log++;
//printf("AllocateSize %d,%d\n",size,log);
	if(!initialized){
		heap_sem = new Semaphore();
		heap_sem->Acquire();

		#ifdef MEMORY_STATS
		thread_sem = new Semaphore();
		thread_sem->Acquire();
unsigned long dwThreadId;
CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ticker, 0, 0, &dwThreadId);
//		_beginthread(&ticker,4096,0);
		thread_sem->Acquire();
//		Sleep(100000);
		#endif

		HANDLE file = CreateFileMapping((HANDLE)-1,0,PAGE_READWRITE,0,HEAP_SIZE+sizeof(Heap),0);
printf("Total shared heap space allocated = %d bytes\n",heap_bytes+=HEAP_SIZE);
		Heap *h = (Heap*)MapViewOfFile(file,FILE_MAP_WRITE,0,0,0);
		assert(h);
		first_heap = h;
		h->Next = 0;
		h->File = file;
		memset(h->Mask,0,HEAP_SIZE/BLOCK_SIZE/8);
		memset(h->Type,0,HEAP_SIZE/BLOCK_SIZE/8);
		memset(h->Size,-1,HEAP_SIZE/BLOCK_SIZE*sizeof(int));
		h->Blocks = (void*)(int(h)+sizeof(Heap));
		initialized = true;		// Assume we get no errors
		#ifdef _DEBUG
		void *block = internal_Allocate(BLOCK_SIZE-sizeof(MemHeader),flags,0,0);
		#else
		void *block = internal_Allocate(BLOCK_SIZE-sizeof(MemHeader),flags);
		#endif
		assert(block);
		if(!block){
			heap_sem->Release();
			printf("Failed!\n");
			return 0;
		}
		int _size = 32*sizeof(Fragment*);			// fixme: not 32 bits
		int log=1;
		int tmp = _size-1;
		while((tmp>>=1)!=0) log++;

		h = GetHeap(block);
		int blockn = h->GetBlock(block);
		h->Size[blockn] = size;
		h->Type[blockn/8]|=1<<(blockn&7);

		Fragments = (Fragment**)block;
		memset(Fragments,0,32*sizeof(Fragment*));	// fixme: not 32 bits

		Fragments[log] = (Fragment*)(int(block)+(2<<log));
		Fragments[log]->Next = 0;
		Fragments[log]->Prev = 0;

		for(int n=3; n<(BLOCK_SIZE-sizeof(MemHeader))>>log; n++){
			Fragment *frag = (Fragment*)(int(block)+(n<<log));
			Fragments[log]->Next = frag;
			frag->Next = 0;
			frag->Prev = Fragments[log];
			Fragments[log] = frag;
		}
		heap_sem->Release();
		#ifdef MEMORY_STATS
		total_used_history = (DynamicArray*)internal_Allocate(sizeof(DynamicArray),flags,0,0);
		total_size_history = (DynamicArray*)internal_Allocate(sizeof(DynamicArray),flags,0,0);
		total_frag_history = (DynamicArray*)internal_Allocate(sizeof(DynamicArray),flags,0,0);
		total_frags_history = (DynamicArray*)internal_Allocate(sizeof(DynamicArray),flags,0,0);
		total_blocks_history = (DynamicArray*)internal_Allocate(sizeof(DynamicArray),flags,0,0);
		total_used_history->DynamicArray::DynamicArray(DARRAY_SHARED_MEMORY);
		total_size_history->DynamicArray::DynamicArray(DARRAY_SHARED_MEMORY);
		total_frag_history->DynamicArray::DynamicArray(DARRAY_SHARED_MEMORY);
		total_frags_history->DynamicArray::DynamicArray(DARRAY_SHARED_MEMORY);
		total_blocks_history->DynamicArray::DynamicArray(DARRAY_SHARED_MEMORY);
		#endif
	}
	heap_sem->Acquire();
	int orgsize = size;
	size+=sizeof(MemHeader);
	if(size<sizeof(Fragment)) size = sizeof(Fragment);

	if(size<=BLOCK_SIZE/2){


		int log=1;
		int tmp = size-1;
		while((tmp>>=1)!=0) log++;

//printf("Frag %d!\n",log);
		if(!Fragments[log]){
//printf("Frag %d empty!\n",log);
			#ifdef _DEBUG
			void *block = internal_Allocate(BLOCK_SIZE-sizeof(MemHeader),flags,0,0);
			#else
			void *block = internal_Allocate(BLOCK_SIZE-sizeof(MemHeader),flags);
			#endif
			assert(block);
			if(!block){
				heap_sem->Release();
				assert(false);
				return 0;
			}
			Heap *h = GetHeap(block);
			int blockn = h->GetBlock(block);
			h->Size[blockn] = log;
//printf("h->Size[3] = %d\n",h->Size[3]);
			h->Type[blockn/8]|=1<<(blockn&7);

			Fragments[log] = (Fragment*)block;
			Fragments[log]->Next = 0;
			Fragments[log]->Prev = 0;

			for(int n=2; n<(BLOCK_SIZE-sizeof(MemHeader))>>log; n++){
				Fragment *frag = (Fragment*)(int(block)+(n<<log));
				Fragments[log]->Next = frag;
				frag->Next = 0;
				frag->Prev = Fragments[log];
				Fragments[log] = frag;
			}
		}

//		int frags = get_frag_count();

		MemHeader *header = (MemHeader*)Fragments[log];
//printf("Alloc: %d\n",log);
	if(Fragments[log]->Prev) Fragments[log]->Prev->Next = Fragments[log]->Next;
	if(Fragments[log]->Next) Fragments[log]->Next->Prev = Fragments[log]->Prev;
		Fragments[log] = Fragments[log]->Prev;
//DumpStuff();
//GetHeap(data);
		//#ifdef MEMORY_STATISTICS
//		heap_used+=orgsize;
		//#endif

//		frags = frags-get_frag_count();
		//printf("Alloc: %d\n",frags);
//		assert(frags==1);

		#ifdef _DEBUG
		AddToFile(header,file,line,orgsize); //1<<log);
		#endif
//printf("Alloc at %x\n",data);
//printf("Alloc(%d) %x\n",orgsize,header);
		header->size = orgsize;
//printf("a\n");
		#ifdef MEMORY_STATS
		if(file&&line){
			header->line = line;
			if(file){
				header->file = (char*)internal_Allocate(strlen(file)+1,flags,0,0);
				sprintf(header->file,"%s",file);
			}else header->file = 0;

//printf("b\n");
			MemHeader *ent = list;
			while(ent){
				if(ent->line==header->line){
					if(ent->file&&header->file){
						if(!strcmp(ent->file,header->file)){
							header->listent = ent;
							ent->size+=orgsize;
							break;
						}
					}else if(!ent->file&&!!header->file){
						header->listent = ent;
						ent->size+=orgsize;
						break;
					}
				}
				ent = ent->Next;
			}
//printf("c\n");
			if(!ent){
				MemHeader *ent = (MemHeader*)internal_Allocate(sizeof(MemHeader),flags,0,0);
				ent->size = orgsize;
				ent->file = file;
				ent->line = line;
				ent->history = (DynamicArray*)internal_Allocate(sizeof(DynamicArray),flags,0,0);
				ent->history->DynamicArray::DynamicArray(DARRAY_SHARED_MEMORY);
				header->listent = ent;
				ent->Next = list;
				list = ent;
			}
		}else header->listent = 0;
		#endif

//printf("d\n");
		heap_used+=orgsize;
//printf("Allocated %d bytes!\n",orgsize);
		heap_sem->Release();
//printf("e\n");
		return (void*)(int(header)+sizeof(MemHeader));
	}else{
//int bs = BLOCK_SIZE;
		int blocks = (size+BLOCK_SIZE-1)/BLOCK_SIZE;
		assert(blocks);
		//#ifdef WARNING_SIZE
		//allocated+=blocks*BLOCK_SIZE;
		//printf("%d bytes allocated\n",allocated);
		//if(allocated>=WARNING_SIZE) DumpStats();
		//#endif
		Heap *heap = first_heap;
		while(heap){
			int found = 0;
			for(int n=0; n<HEAP_SIZE/BLOCK_SIZE; n++){
				if(!(heap->Mask[n/8]&(1<<(n&7)))) found++;
				else found=0;
				if(found==blocks){
					n=n-blocks+1;
					MemHeader *header = (MemHeader*)(int(heap->Blocks)+n*BLOCK_SIZE);
					int tmp=blocks;
					while(tmp--){ heap->Mask[n/8]|=(1<<(n&7)); n++; }
					Heap *h = GetHeap(header);
					int block = h->GetBlock(header);
					h->Type[block/8]&=~(1<<(block&7));
					h->Size[block]=blocks;
//printf("h->Size[3] = %d\n",h->Size[3]);
					//#ifdef MEMORY_STATISTICS
//					heap_used+=orgsize;
					//#endif
		#ifdef _DEBUG
		AddToFile(header,file,line,orgsize); //blocks*BLOCK_SIZE);
		#endif
//printf("Alloc at %x\n",data);
//printf("Alloc(%d) %x\n",orgsize,header);
					header->size = orgsize;

					#ifdef MEMORY_STATS
					if(file&&line){
						header->line = line;
						if(file){
							header->file = (char*)internal_Allocate(strlen(file)+1,flags,0,0);
							sprintf(header->file,"%s",file);
						}else header->file = 0;

						MemHeader *ent = list;
						while(ent){
							if(ent->line==header->line){
								if(ent->file&&header->file){
									if(!strcmp(ent->file,header->file)){
										header->listent = ent;
										ent->size+=orgsize;
										break;
									}
								}else if(!ent->file&&!!header->file){
									header->listent = ent;
									ent->size+=orgsize;
									break;
								}
							}
							ent = ent->Next;
						}
						if(!ent){
							MemHeader *ent = (MemHeader*)internal_Allocate(sizeof(MemHeader),flags,0,0);
							ent->size = orgsize;
							ent->file = file;
							ent->line = line;
							ent->history = (DynamicArray*)internal_Allocate(sizeof(DynamicArray),flags,0,0);
							ent->history->DynamicArray::DynamicArray(DARRAY_SHARED_MEMORY);
							header->listent = ent;
							ent->Next = list;
							list = ent;
						}
					}else header->listent = 0;
					#endif

					heap_used+=orgsize;
//printf("Allocated %d bytes!\n",orgsize);
					heap_sem->Release();
					return (void*)(int(header)+sizeof(MemHeader));
				}
			}
			if(!heap->Next){
				HANDLE file = CreateFileMapping((HANDLE)-1,0,PAGE_READWRITE,0,HEAP_SIZE+sizeof(Heap),0);
printf("Total shared heap space allocated = %d bytes, %d used\n",heap_bytes+=HEAP_SIZE,heap_used);
DumpStuff();
printf("%d\n",size);
//if(heap_bytes>=WARNING_SIZE) DumpStats();
				Heap *h = (Heap*)MapViewOfFile(file,FILE_MAP_WRITE,0,0,0);
				assert(h);
				if(!h) return 0;
				h->Next = 0;
				h->File = file;
				memset(h->Mask,0,HEAP_SIZE/BLOCK_SIZE/8);
				memset(h->Type,0,HEAP_SIZE/BLOCK_SIZE/8);
				memset(h->Size,-1,HEAP_SIZE/BLOCK_SIZE);
				h->Blocks = (void*)(int(h)+sizeof(Heap));
				heap->Next = h;
			}
			heap=heap->Next;
		}
	}
	assert(false);
	heap_sem->Release();
	assert(false);
	return 0;
}

#ifdef _DEBUG
static void internal_Free(void *data, char *file, int line){
#else
static void internal_Free(void *data){
#endif

//printf("Free at %x\n",data);
	// fixme: buffer should be checked.
	heap_sem->Acquire();
	MemHeader *header = (MemHeader*)(int(data)-sizeof(MemHeader));
	Heap *h = GetHeap(header);
	if(!h){
		heap_sem->Release();
		printf("Free: Warning: Failed to get heap for buffer %x\n",data);
		return;
	}
	int block = h->GetBlock(header);

	//#if defined(_DEBUG)||defined(WARNING_SIZE)
	//int size=0;
	//if(h->Type[block/8]&(1<<(block&7))){
	//}else{
	//	size = h->Size[block]*BLOCK_SIZE;
	//}
	//#endif
	//#ifdef WARNING_SIZE
	//allocated-=size;
	//if(allocated>=WARNING_SIZE) DumpStats();
	//#endif

//printf("Free(%x)(%d)\n",header,header->size);
	#ifdef _DEBUG
	AddToFile(header,file,line,-header->size);
	#endif

	#ifdef MEMORY_STATS
/*
	if(file&&line){
		MemHeader *ent = list;
		while(ent){
			if(ent->line==header->line){
				if(ent->file&&header->file){
					if(!strcmp(ent->file,header->file)){
						header->listent = ent;
						ent->size-=header->size;
						break;
					}
				}else if(!ent->file&&!!header->file){
					header->listent = ent;
					ent->size-=header->size;
					break;
				}
			}
			ent = ent->Next;
		}
	}
*/
	if(header->listent) header->listent->size-=header->size;
	#endif


	//#ifdef MEMORY_STATISTICS
	heap_used-=header->size;
//printf("Freed %d bytes!\n",header->size);
	//#endif

	#ifdef MEMORY_STATISTICS
	if(file&&line) internal_Free(header->file,0,0);
	#endif

	if(h->Type[block/8]&(1<<(block&7))){
		// Fragment block

		int log=1;
		int tmp = header->size+sizeof(MemHeader);
		if(tmp<sizeof(Fragment)) tmp = sizeof(Fragment);
		tmp-=1;
		while((tmp>>=1)!=0) log++;

//printf("Free: %d\n",log);
		Fragment *frag = (Fragment*)header;
		if(Fragments[log]){
			//assert(Fragments[log]->Next==0);
			//printf("%x,%x\n",Fragments[log]->Prev,Fragments[log]->Next);

//			int frags = get_frag_count();

			frag->Next = Fragments[log]->Next;
			Fragments[log]->Next = frag;
			//frag->Next = 0;
			frag->Prev = Fragments[log];
			Fragments[log] = frag;

//			frags = get_frag_count()-frags;
			//printf("Free: %d\n",frags);
//			assert(frags==1);

		}else{
			Fragments[log] = frag;
			frag->Next = 0;
			frag->Prev = 0;
		}
		// fixme: Keep size info, and free blocks.
	}else{
//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		// One or more blocks
		int blocks = h->Size[block];//BLOCK_SIZE;
		while(blocks--){
			h->Mask[block/8]&=~(1<<(block&7));
			//h->Type[block/8]&=~(1<<(block&7));
			h->Size[block++]=-1;
		}
	}
	heap_sem->Release();
}

#ifdef _DEBUG
static void *internal_ReAllocate(void *buf, int size, char *file, int line){
#else
static void *internal_ReAllocate(void *buf, int size){
#endif
//printf("ReAlloc at %x\n",buf);
	heap_sem->Acquire();
	MemHeader *header = (MemHeader*)(int(buf)-sizeof(MemHeader));
	Heap *h = GetHeap(header);
	int block = h->GetBlock(header);
	int oldsize;
	if(h->Type[block/8]&(1<<(block&7))){
		oldsize = header->size;//Size[block]; //1<<h->Size[block];
	}else{
		oldsize = header->size;//Size[block]; //*BLOCK_SIZE;
	}
	heap_sem->Release();

//printf("realoc(%x,%d,%d)\n",buf,oldsize,size);

	#ifdef _DEBUG
	void *nbuf = internal_Allocate(size,0,file,line);
	memcpy(nbuf,buf,size>oldsize ? oldsize : size);
	internal_Free(buf,file,line);
	#else
	void *nbuf = internal_Allocate(size,0);
	memcpy(nbuf,buf,size>oldsize ? oldsize : size);
	internal_Free(buf);
	#endif

//printf("Alloc to %x\n",nbuf);
	return nbuf;
}

#ifdef _DEBUG
void *(*Allocate)(int,int, char *file, int line) = 0;
void  (*Free)(void*, char *file, int line) = 0;
void *(*ReAllocate)(void*,int, char *file, int line) = 0;
#else
void *(*Allocate)(int,int) = 0;
void  (*Free)(void*) = 0;
void *(*ReAllocate)(void*,int) = 0;
#endif

void init_Memory(void){
	Allocate = internal_Allocate;
	Free = internal_Free;
	ReAllocate = internal_ReAllocate;
}

#ifdef _DEBUG
void init_Memory(
	void *(*_Allocate)(int,int, char *file, int line),
	void  (*_Free)(void*, char *file, int line),
	void *(*_ReAllocate)(void*,int, char *file, int line),
	void *_list){
	#ifdef MEMORY_STATS
//	list = (MemHeader*)_list;
	#endif
#else
void init_Memory(
	void *(*_Allocate)(int,int),
	void  (*_Free)(void*),
	void *(*_ReAllocate)(void*,int)){
#endif
	Allocate = _Allocate;
	Free = _Free;
	ReAllocate = _ReAllocate;
}

void deinit_Memory(void){
	Allocate = internal_Allocate;
	Free = internal_Free;
	ReAllocate = internal_ReAllocate;
	DumpStats();
}


// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

// Array of pointers

#define DARRAY_GENID			1
#define DARRAY_NOSHRINK			2
#define DARRAY_SHARED_MEMORY	4

class DynamicArray{
public:				DynamicArray();
					DynamicArray(int flags);
					#ifdef _DEBUG
					DynamicArray(DynamicArray *, char * = __FILE__":DynamicArray(DynamicArray*)", int = __LINE__);
					#else
					DynamicArray(DynamicArray *);
					#endif
					~DynamicArray();
		void		New(void);
		int			GetCount(void) const{ return Count; };			// Returns number of items in the array
		#ifdef	_DEBUG
		int			Add(void *Item, char *, int);
		void		Remove(void *Item, int ID, char *, int);
		void		Remove(int index, char *, int);
		int			Add(void *Item){ return Add(Item,__FILE__,__LINE__); }
		void		Remove(void *Item, int ID){ Remove(Item,ID,__FILE__,__LINE__); }
		void		Remove(int index){ Remove(index,__FILE__,__LINE__); }
		#else
		int			Add(void *Item);
		void		Remove(void *Item, int ID);
		void		Remove(int index);
		#endif
		void		Clear(void);
		void		*GetItem(const int Index) const{ return Array[Index]; }
		void		SetItem(const int Index, void *data){ Array[Index]=data; };
		void		*GetBuffer(void);
//		void		SetBuffer(void *,int);
		int			GetItemID(int Index);
		void		Sort(void);
//virtual	bool	DoSwap(void *ent1, void *ent2);
virtual	bool		Swap(void *, void *);
private:
		int			Count;
		int			BufferCount;
		int			flags;
		void		**Array;
		int			*IDArray;
//class	Semaphore	*sem;
};

#endif


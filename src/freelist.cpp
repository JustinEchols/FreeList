#include "../base_common.h"
#include "../base_macros.h"
#include "../base_memory.cpp"

struct int_link
{
	int Data;
	int_link *Next;
};

struct int_free_list
{
	int_link *List;
	int_link *Free;
};

struct int_record
{
	memory_arena Arena;
	int_free_list IntFreeList;
};

internal void
FreeListAdd(int_free_list *FreeList, memory_arena *Arena, int A)
{
	if(!FreeList->Free)
	{
		FreeList->Free = PushStruct(Arena, int_link);
		FreeList->Free->Next = 0;
	}

	int_link *Link = FreeList->Free;
	FreeList->Free = Link->Next;
	Link->Next = FreeList->List;
	FreeList->List = Link;
	Link->Data = A;
}

internal void
FreeListAddIfUnique(int_free_list *FreeList, memory_arena *Arena, int A)
{
	for(int_link *Link = FreeList->List; Link; Link = Link->Next)
	{
		if(Link->Data == A)
		{
			return;
		}
	}

	FreeListAdd(FreeList, Arena, A);
}

internal void
FreeListPrint(int_free_list *FreeList)
{
	for(int_link *Link = FreeList->List; Link; Link = Link->Next)
	{
		printf("%d\n", Link->Data);
	}
}

inline void *
MemoryAlloc(memory_index Size)
{
	void *Memory = calloc(Size, sizeof(u8));
	return(Memory);
}

inline void
MemoryFree(void *Memory)
{
	if(Memory)
	{
		free(Memory);
	}
}

int main(int Argc, char **Argv)
{
	void *Memory = MemoryAlloc(Megabyte(1));
	if(!Memory)
	{
		return(0);
	}

	int_record Record;
	ArenaInitialize(&Record.Arena, Memory, Kilobyte(64));

	for(int Index = 0; Index < 10; ++Index)
	{
		FreeListAdd(&Record.IntFreeList, &Record.Arena, Index);
	}

	for(int Index = 0; Index < 11; ++Index)
	{
		FreeListAddIfUnique(&Record.IntFreeList, &Record.Arena, Index);
	}

	FreeListPrint(&Record.IntFreeList);
	MemoryFree(Memory);

	return(0);
}

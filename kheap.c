#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

//==================================================================//
//==================================================================//
//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)//
//==================================================================//
//==================================================================//

void initialize_dyn_block_system()
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] initialize_dyn_block_system
	// your code is here, remove the panic and write your code
	//kpanic_into_prompt("initialize_dyn_block_system() is not implemented yet...!!");

	//[1] Initialize two lists (AllocMemBlocksList & FreeMemBlocksList) [Hint: use LIST_INIT()]

	LIST_INIT(&AllocMemBlocksList);
	LIST_INIT(&FreeMemBlocksList);
	int size_Of_Heap;
	//LIST_INIT(&AvailableMemBlocksList);

#if STATIC_MEMBLOCK_ALLOC
	//DO NOTHING
#else
	/*[2] Dynamically allocate the array of MemBlockNodes
	 * 	remember to:
	 * 		1. set MAX_MEM_BLOCK_CNT with the chosen size of the array
	 * 		2. allocation should be aligned on PAGE boundary
	 * 	HINT: can use alloc_chunk(...) function
	 */

		MAX_MEM_BLOCK_CNT = NUM_OF_KHEAP_PAGES;
		MemBlockNodes = (struct MemBlock*)KERNEL_HEAP_START;
		size_Of_Heap = (sizeof(struct MemBlock)) * NUM_OF_KHEAP_PAGES;
		allocate_chunk(ptr_page_directory, KERNEL_HEAP_START , size_Of_Heap , PERM_USED | PERM_WRITEABLE);
#endif
	//[3] Initialize AvailableMemBlocksList by filling it with the MemBlockNodes
		initialize_MemBlocksList(MAX_MEM_BLOCK_CNT);

	//[4] Insert a new MemBlock with the remaining heap size into the FreeMemBlocksList

		struct MemBlock *New_MemBlock = LIST_FIRST(&AvailableMemBlocksList);
		New_MemBlock->size = KERNEL_HEAP_MAX - KERNEL_HEAP_START - ROUNDUP(size_Of_Heap , PAGE_SIZE);
		New_MemBlock->sva  = KERNEL_HEAP_START + ROUNDUP(size_Of_Heap , PAGE_SIZE);
		LIST_REMOVE((&(AvailableMemBlocksList)), New_MemBlock);
		LIST_INSERT_HEAD((&FreeMemBlocksList) , New_MemBlock);

}

void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kmalloc
	// your code is here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");
	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy

	int FF = isKHeapPlacementStrategyFIRSTFIT();
	int BF = isKHeapPlacementStrategyBESTFIT();

	unsigned int S = ROUNDUP(size , PAGE_SIZE);
	int exst;
	struct MemBlock *returnVal;
	struct FrameInfo * ptr_Frame;
	struct FrameInfo * ptr_returned;

	if(FF == 1){
		returnVal = alloc_block_FF(S);
		if(returnVal != NULL){

		    exst = allocate_chunk(ptr_page_directory, returnVal->sva , returnVal->size ,  PERM_WRITEABLE);
		    //LIST_INSERT_HEAD((&AllocMemBlocksList) , returnVal);
		    insert_sorted_allocList(returnVal);
		}
	}

	if(BF == 1){
		returnVal = alloc_block_BF(S);
		if(returnVal != NULL){

			 exst = allocate_chunk(ptr_page_directory, returnVal->sva , returnVal->size ,  PERM_WRITEABLE);
			 //LIST_INSERT_HEAD((&AllocMemBlocksList) , returnVal);
			 insert_sorted_allocList(returnVal);
		}
	}

if(returnVal != NULL)
	{return (void *)returnVal->sva;}
else
	{return (void *)NULL;}
}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kfree
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");

	uint32 Va = (uint32)virtual_address;
	Va = ROUNDUP(Va,PAGE_SIZE);
	struct MemBlock *returnVal ;
	returnVal = find_block(&AllocMemBlocksList, Va);

	if(returnVal != NULL){
		LIST_REMOVE((&(AllocMemBlocksList)), returnVal);

		for(uint32 i = Va ; i < (returnVal->sva+returnVal->size) ; i+=PAGE_SIZE){
			unmap_frame(ptr_page_directory , i);
		}

		insert_sorted_with_merge_freeList(returnVal);
	}
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kheap_virtual_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");
	struct FrameInfo * ptr = to_frame_info(physical_address);
	return ptr->va;
	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details
	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kheap_physical_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");

	int retPh_address = virtual_to_physical(ptr_page_directory , virtual_address);

	return retPh_address;
	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details
}


void kfreeall()
{
	panic("Not implemented!");

}

void kshrink(uint32 newSize)
{
	panic("Not implemented!");
}

void kexpand(uint32 newSize)
{
	panic("Not implemented!");
}




//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT MS2 - BONUS] [KERNEL HEAP] krealloc
	// Write your code here, remove the panic and write your code
	panic("krealloc() is not implemented yet...!!");
}

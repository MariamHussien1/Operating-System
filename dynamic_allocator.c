
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"
uint32 tempsva = 0;
int check = 0;

void print_mem_block_lists()
{
	cprintf("\n=========================================\n");
	struct MemBlock* blk ;
	struct MemBlock* lastBlk = NULL ;
	cprintf("\nFreeMemBlocksList:\n");
	uint8 sorted = 1 ;
	LIST_FOREACH(blk, &FreeMemBlocksList)
	{
		if (lastBlk && blk->sva < lastBlk->sva + lastBlk->size)
			sorted = 0 ;
		cprintf("[%x, %x)-->", blk->sva, blk->sva + blk->size) ;
		lastBlk = blk;
	}
	if (!sorted)	cprintf("\nFreeMemBlocksList is NOT SORTED!!\n") ;
	cprintf("\nAvailableMemBlocksList:\n");
		LIST_FOREACH(blk, &AvailableMemBlocksList)
		{
			if (lastBlk && blk->sva < lastBlk->sva + lastBlk->size)
				sorted = 0 ;
			cprintf("[%x, %x)-->", blk->sva, blk->sva + blk->size) ;
			lastBlk = blk;
		}
	lastBlk = NULL ;
	cprintf("\nAllocMemBlocksList:\n");
	sorted = 1 ;
	LIST_FOREACH(blk, &AllocMemBlocksList)
	{
		if (lastBlk && blk->sva < lastBlk->sva + lastBlk->size)
			sorted = 0 ;
		cprintf("[%x, %x)-->", blk->sva, blk->sva + blk->size) ;
		lastBlk = blk;
	}
	if (!sorted)	cprintf("\nAllocMemBlocksList is NOT SORTED!!\n") ;
	cprintf("\n=========================================\n");

}

//===============================
// [1] INITIALIZE AVAILABLE LIST:DONE
//===============================
void initialize_MemBlocksList(uint32 numOfBlocks)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] initialize_MemBlocksList
	LIST_INIT(&AvailableMemBlocksList);
	for (int i=0 ; i<numOfBlocks ; ++i){
		LIST_INSERT_HEAD(&AvailableMemBlocksList, &(MemBlockNodes[i]));
	}
}

//===============================
// [2] FIND BLOCK:DONE
//===============================
struct MemBlock *find_block(struct MemBlock_List *blockList, uint32 va)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] find_block
	struct MemBlock *Loop_Block;
	    LIST_FOREACH(Loop_Block, (blockList))
	    {
	        if (va == Loop_Block->sva){
	           return Loop_Block;
	        }
	    }
	    return NULL;
}

//=========================================
// [3] INSERT BLOCK IN ALLOC LIST [SORTED]:
//=========================================
void insert_sorted_allocList(struct MemBlock *blockToInsert)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] insert_sorted_allocList
	//Case_one AllocMemBlocksList is empty
	struct MemBlock* Felement = LIST_FIRST(&AllocMemBlocksList);
	if(Felement==NULL){
		LIST_INSERT_HEAD(&(AllocMemBlocksList), blockToInsert);
	}
	else{
		struct MemBlock *element;
		LIST_FOREACH(element, &(AllocMemBlocksList))
		{
			struct MemBlock *NextElement = LIST_NEXT(element);
			if(blockToInsert->sva < element->sva){
				LIST_INSERT_BEFORE(&(AllocMemBlocksList),element,blockToInsert);
				break;
			}
			else if(NextElement != NULL && blockToInsert->sva > element->sva && blockToInsert->sva < NextElement->sva){
				LIST_INSERT_AFTER(&(AllocMemBlocksList),element,blockToInsert);
				break;
			}
			else if(blockToInsert->sva > element->sva && NextElement == NULL){
				LIST_INSERT_AFTER(&(AllocMemBlocksList),element,blockToInsert);
				break;
			}
		}
	}
}

//=========================================
// [4] ALLOCATE BLOCK BY FIRST FIT:DONE
//=========================================
struct MemBlock *alloc_block_FF(uint32 size)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] alloc_block_FF
	struct MemBlock *element;
	struct MemBlock *returnvalue=NULL;
	LIST_FOREACH(element, &(FreeMemBlocksList))
	{
		if(element->size == size){
			LIST_REMOVE(&FreeMemBlocksList, element);
			returnvalue=element;
			break;
		}
		//GettingFirstFit
		else if(element->size > size){
			uint32 actualSize;
			uint32 freeSize;
			actualSize=element->size;
			freeSize=actualSize-size;
			//New Element from AvailableMemBlocksList
			struct MemBlock * element1 = LIST_LAST(&AvailableMemBlocksList);
			LIST_REMOVE(&AvailableMemBlocksList, element1);
			//returning new element after updating its size
			element1->size=size;
			element1->sva=element->sva;
			returnvalue=element1;
			//updating exiting element
			element->size=freeSize;
			element->sva=element->sva+size;
			break;
		}
	}
	return returnvalue;
}

//=========================================
// [5] ALLOCATE BLOCK BY BEST FIT:DONE
//=========================================
struct MemBlock *alloc_block_BF(uint32 size)
{
	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] alloc_block_BF
	struct MemBlock *element;
	struct MemBlock *bestfitBlock=NULL;
	uint32 tempsize= 4294967295;
	LIST_FOREACH(element, &(FreeMemBlocksList))
	{
		if(element->size == size){
			LIST_REMOVE(&FreeMemBlocksList, element);
			return element;
		}
		//GettingBestSize
		else if(tempsize > element->size && element->size > size){
			tempsize = element->size;
			bestfitBlock = element;
		}
	}
	if(bestfitBlock != NULL){
		struct MemBlock * element1 = LIST_LAST(&AvailableMemBlocksList);
		LIST_REMOVE(&AvailableMemBlocksList, element1);
		element1->sva = bestfitBlock->sva;
		element1->size = size;
		bestfitBlock->sva = bestfitBlock->sva + size;
		bestfitBlock->size = bestfitBlock->size - size;
		return element1;
	}
	return NULL;
}
//=========================================
// [7] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
struct MemBlock *alloc_block_NF(uint32 size)
{
	//TODO: [PROJECT MS1 - BONUS] [DYNAMIC ALLOCATOR] alloc_block_NF
	struct MemBlock *returnvalue=NULL;
	//FirstFit
	if(tempsva == 0 && check == 0){
		struct MemBlock *element;
		LIST_FOREACH(element, &(FreeMemBlocksList))
		{
			if(element->size == size){
				LIST_REMOVE(&FreeMemBlocksList, element);
				returnvalue=element;
				tempsva=element->sva+element->size;
				check=1;
				break;
			}
			//GettingFirstFit
			else if(element->size > size){
				uint32 actualSize;
				uint32 freeSize;
				actualSize=element->size;
				freeSize=actualSize-size;
				//New Element from AvailableMemBlocksList
				struct MemBlock * element1 = LIST_LAST(&AvailableMemBlocksList);
				LIST_REMOVE(&AvailableMemBlocksList, element1);
				//returning new element after updating its size
				element1->size=size;
				element1->sva=element->sva;
				returnvalue=element1;
				//updating exiting element
				element->size=freeSize;
				element->sva=element->sva+size;
				tempsva=element1->sva+element1->size;
				check=1;
				break;
			}
		}
	}
	else if(check == 1){

		void loop(){
			struct MemBlock *element;
			LIST_FOREACH(element, &(FreeMemBlocksList))
			{
				struct MemBlock* next = LIST_NEXT(element);
				if(element->size == size && tempsva <= element->sva){
					LIST_REMOVE(&FreeMemBlocksList, element);
					returnvalue=element;
					tempsva=element->sva+element->size;
					check=1;
					break;
				}
				else if(element->size > size && tempsva <= element->sva){
					uint32 actualSize;
					uint32 freeSize;
					actualSize=element->size;
					freeSize=actualSize-size;
					//New Element from AvailableMemBlocksList
					struct MemBlock * element1 = LIST_LAST(&AvailableMemBlocksList);
					LIST_REMOVE(&AvailableMemBlocksList, element1);
					//returning new element after updating its size
					element1->size=size;
					element1->sva=element->sva;
					returnvalue=element1;
					//updating exiting element
					element->size=freeSize;
					element->sva=element->sva+size;
					tempsva=element1->sva+element1->size;
					check=1;
					print_mem_block_lists();
					break;
				}
				else if(next == NULL){
					tempsva = 0;
					element= LIST_FIRST(&(FreeMemBlocksList));
					loop();
				}
			}
		}
		loop();
	}
	return returnvalue;
}

//===================================================
// [8] INSERT BLOCK (SORTED WITH MERGE) IN FREE LIST:
//===================================================
void insert_sorted_with_merge_freeList(struct MemBlock *blockToInsert)
{
	//cprintf("BEFORE INSERT with MERGE: insert [%x, %x)\n=====================\n", blockToInsert->sva, blockToInsert->sva + blockToInsert->size);
	//print_mem_block_lists();

	//TODO: [PROJECT MS1] [DYNAMIC ALLOCATOR] insert_sorted_with_merge_freeList
	struct MemBlock *Loop_Free;
	struct MemBlock *TempFree = LIST_FIRST(&FreeMemBlocksList);
	////////////////////////
	struct MemBlock *e;
	LIST_FOREACH(e, &AvailableMemBlocksList)
	{
		if (e->sva != 0 && e->size != 0){
			e->sva = 0;
			e->size = 0;
			//cprintf("[%x, %x)-->", e->sva, e->sva + e->size) ;
		}
	}
	////////////////////////////
	if(TempFree == NULL){
		LIST_INSERT_HEAD(&(FreeMemBlocksList), blockToInsert);
	}
	else{
		LIST_FOREACH(Loop_Free,&(FreeMemBlocksList))
		{
			struct MemBlock *NextElement = LIST_NEXT(Loop_Free);
			//MergeWithNextAndPrevious
			if(NextElement != NULL && Loop_Free->sva + Loop_Free->size == blockToInsert->sva && blockToInsert->sva + blockToInsert->size ==  NextElement->sva){
				Loop_Free->size = Loop_Free->size + blockToInsert->size + NextElement->size;
				LIST_REMOVE(&(FreeMemBlocksList), NextElement);
				blockToInsert->size = 0;
				blockToInsert->sva = 0;
				NextElement->size = 0;
				NextElement->sva = 0;
				//cprintf("insert [%x, %x)\n=====================\n", NextElement->sva, NextElement->sva + NextElement->size);
				//cprintf("insert [%x, %x)\n=====================\n", blockToInsert->sva, blockToInsert->sva + blockToInsert->size);
				LIST_INSERT_HEAD(&(AvailableMemBlocksList) , NextElement);
				LIST_INSERT_HEAD(&(AvailableMemBlocksList) , blockToInsert);
				break;
			}
			//MergeWithPrevious
			else if(Loop_Free->sva + Loop_Free->size == blockToInsert->sva){
				Loop_Free->size = Loop_Free->size + blockToInsert->size ;
				blockToInsert->size = 0;
				blockToInsert->sva  = 0;
				//cprintf("insert [%x, %x)\n=====================\n", blockToInsert->sva, blockToInsert->sva + blockToInsert->size);
				LIST_INSERT_TAIL(&(AvailableMemBlocksList) , blockToInsert);
				break;
			}
			//MergeWithNext
			else if(blockToInsert->sva + blockToInsert->size == Loop_Free->sva){
				Loop_Free->size = Loop_Free->size + blockToInsert->size;
				Loop_Free->sva  = blockToInsert->sva;
				blockToInsert->size = 0;
				blockToInsert->sva  = 0;
				//cprintf("insert [%x, %x)\n=====================\n", blockToInsert->sva, blockToInsert->sva + blockToInsert->size);
				LIST_INSERT_HEAD(&(AvailableMemBlocksList) , blockToInsert);
				break;
			}
			//NoMerge
			else if(blockToInsert->sva < Loop_Free->sva){
				LIST_INSERT_BEFORE(&(FreeMemBlocksList),Loop_Free,blockToInsert);
				break;
			}
			else if(NextElement != NULL && blockToInsert->sva > Loop_Free->sva && blockToInsert->sva < NextElement->sva && NextElement->sva != blockToInsert->sva +blockToInsert->size){
				LIST_INSERT_AFTER(&(FreeMemBlocksList),Loop_Free,blockToInsert);
				break;
			}
			else if(blockToInsert->sva > Loop_Free->sva && NextElement == NULL){
				LIST_INSERT_AFTER(&(FreeMemBlocksList),Loop_Free,blockToInsert);
				break;
			}
			else{
				continue;
			}
		}
	}
		//cprintf("\nAFTER INSERT with MERGE:\n=====================\n");
		//print_mem_block_lists();
}

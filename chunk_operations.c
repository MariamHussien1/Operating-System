/*
 * chunk_operations.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include <kern/trap/fault_handler.h>
#include <kern/disk/pagefile_manager.h>
#include "kheap.h"
#include "memory_manager.h"


/******************************/
/*[1] RAM CHUNKS MANIPULATION */
/******************************/

//===============================
// 1) CUT-PASTE PAGES IN RAM:
//===============================
//This function should cut-paste the given number of pages from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int cut_paste_pages(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 num_of_pages)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] cut_paste_pages
	// Write your code here, remove the panic and write your code
	//panic("cut_paste_pages() is not implemented yet...!!");
	uint32 *ptr = NULL;
	uint32 *ptr2 = NULL;
	uint32 dest = ROUNDDOWN(dest_va , PAGE_SIZE);
	uint32 source = ROUNDDOWN(source_va , PAGE_SIZE);
	int retDest_PT = get_page_table(page_directory , dest , &ptr);
	int retSource_PT = get_page_table(page_directory , source , &ptr2);
	struct FrameInfo * ptr_dest_info = get_frame_info(page_directory , dest , &ptr);
	struct FrameInfo * ptr_source_info = get_frame_info(page_directory , source , &ptr2);
	uint32 Permissions = pt_get_page_permissions(page_directory , source);
	for(int j=0 ; j<num_of_pages ; ++j){
	   if(ptr_dest_info != NULL){
			return -1;
	   }
	   uint32 T = dest_va + PAGE_SIZE;
	   ptr_dest_info = get_frame_info(page_directory , T , &ptr);
	}

   for(int i=0 ; i<num_of_pages ; ++i){
	   get_page_table(page_directory , dest , &ptr);
	   if(retDest_PT == 1 ){
		   create_page_table(page_directory , dest);
	   }
	   else{
		   if(ptr_dest_info != NULL){
			   return -1;
			}
		   Permissions = pt_get_page_permissions(page_directory , source);
		   //get_page_table(page_directory , dest , &ptr);
		   map_frame(page_directory , ptr_source_info , dest , Permissions);
		   unmap_frame(page_directory , source);
		   source_va = source_va + PAGE_SIZE;
		   source = ROUNDDOWN(source_va , PAGE_SIZE);
		   ptr_source_info = get_frame_info(page_directory , source , &ptr2);
		   dest_va = dest_va + PAGE_SIZE;
		   dest = ROUNDDOWN(dest_va , PAGE_SIZE);
		   ptr_dest_info = get_frame_info(page_directory , dest , &ptr);
	   }
   }
	   return 0;
}

//===============================
// 2) COPY-PASTE RANGE IN RAM:
//===============================
//This function should copy-paste the given size from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int copy_paste_chunk(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 size)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] copy_paste_chunk
	// Write your code here, remove the panic and write your code
	//panic("copy_paste_chunk() is not implemented yet...!!");

	int permissions;
	int PermissionsDest;
	struct FrameInfo *ptr_Frame_Info;
	struct FrameInfo *ptr_Frame;
	uint32 *ptr_page_table = NULL;
	uint8 *ptrsource = (uint8*)source_va;
	uint8 *ptrdest= (uint8*)dest_va;

	for(uint32 va = dest_va; va < (dest_va + size); va += PAGE_SIZE){
		ptr_Frame_Info = get_frame_info(page_directory, va, &ptr_page_table);
		permissions = pt_get_page_permissions(page_directory, va);
		if(ptr_Frame_Info != NULL && (permissions & PERM_WRITEABLE) == 0){
			return -1;
		}
	}

	for(uint32 va = dest_va; va < (dest_va + size); va += PAGE_SIZE){
		ptr_Frame_Info = get_frame_info(page_directory, va, &ptr_page_table);
		if(ptr_Frame_Info == NULL){
			PermissionsDest = pt_get_page_permissions(page_directory, source_va);
			PermissionsDest = (PermissionsDest | PERM_WRITEABLE);
			allocate_frame(&ptr_Frame);
			map_frame(page_directory, ptr_Frame, va, PermissionsDest);
		}
		get_page_table(page_directory, va, &ptr_page_table);
		if(ptr_page_table == NULL){
			create_page_table(page_directory, va);
		}
		source_va+=PAGE_SIZE;
	}

	for(int i = source_va; i < (source_va + size); i++){
		*ptrdest= *ptrsource;
		ptrdest++;
		ptrsource++;
	}
	return 0;
}

//===============================
// 3) SHARE RANGE IN RAM:
//===============================
//This function should share the given size from dest_va with the source_va
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int share_chunk(uint32* page_directory, uint32 source_va,uint32 dest_va, uint32 size, uint32 perms)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] share_chunk
	// Write your code here, remove the panic and write your code
	//panic("share_chunk() is not implemented yet...!!");
	int check = 0;
	uint32 sourceVAstart = ROUNDDOWN(source_va, PAGE_SIZE);
	uint32 sourceVAend = ROUNDUP(source_va + size, PAGE_SIZE);
	uint32 DestVAstart = ROUNDDOWN(dest_va, PAGE_SIZE);
	uint32 DestVAend = ROUNDUP(dest_va + size, PAGE_SIZE);
	uint32 *ptr_page_table = NULL;
	uint32 *ptr_page_tabledest = NULL;
	struct FrameInfo *ptr_Frame_Info;
	for(uint32 va = DestVAstart; va < DestVAend; va += PAGE_SIZE){
		ptr_Frame_Info = get_frame_info(page_directory, va, &ptr_page_table);
		if(ptr_Frame_Info != NULL){
			check=1;
			break;
		}
	}
	if(check == 1){
		return -1;
	}
	else{
		for(uint32 va = sourceVAstart; va <= sourceVAend; va += PAGE_SIZE){
			ptr_Frame_Info = get_frame_info(page_directory, va, &ptr_page_table);
			get_page_table(page_directory, DestVAstart, &ptr_page_tabledest);
			if(ptr_page_tabledest != NULL){
				map_frame(page_directory,ptr_Frame_Info,DestVAstart,perms);
			}
			else{
				create_page_table(page_directory,DestVAstart);
				map_frame(page_directory,ptr_Frame_Info,DestVAstart,perms);
			}
			DestVAstart = DestVAstart + PAGE_SIZE;
			if(DestVAstart >= DestVAend){
				break;
			}
		}
	}
	return 0;
}
//===============================
// 4) ALLOCATE CHUNK IN RAM:
//===============================
//This function should allocate in RAM the given range [va, va+size)
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int allocate_chunk(uint32* page_directory, uint32 va, uint32 size, uint32 perms)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] allocate_chunk
	// Write your code here, remove the panic and write your code
	//panic("allocate_chunk() is not implemented yet...!!");
	uint32 Start_address = ROUNDDOWN(va , PAGE_SIZE);
	uint32 Destination_address = ROUNDUP(va + size , PAGE_SIZE);
	struct FrameInfo *ptr_freeframe_info;
	uint32 *ptrI = NULL;
	for(uint32 i = Start_address ; i < Destination_address ; i += PAGE_SIZE){
		ptr_freeframe_info = get_frame_info(page_directory , i , &ptrI);
		if(ptr_freeframe_info != NULL){
			return -1;
		}
		else if(ptrI == NULL){
			create_page_table(page_directory , i);
		}
	}

	for(uint32 j = Start_address ; j < Destination_address ; j += PAGE_SIZE){
		ptr_freeframe_info = get_frame_info(page_directory , j , &ptrI);
		int m = allocate_frame(&ptr_freeframe_info);
		if(m == E_NO_MEM){
			return -1;
		}
		map_frame(page_directory , ptr_freeframe_info , j , perms);
		ptr_freeframe_info->va=j;
	}
	return 0;
}

/*BONUS*/
//=====================================
// 5) CALCULATE ALLOCATED SPACE IN RAM:
//=====================================
void calculate_allocated_space(uint32* page_directory, uint32 sva, uint32 eva, uint32 *num_tables, uint32 *num_pages)
{
	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_allocated_space
	// Write your code here, remove the panic and write your code
	//panic("calculate_allocated_space() is not implemented yet...!!");

	uint32 start = ROUNDDOWN  (sva , PAGE_SIZE);
	uint32 endT  = ROUNDUP  (eva , PAGE_SIZE*1024);
	uint32 endP  = ROUNDUP  (eva , PAGE_SIZE);
	uint32 ctr_pg  = 0;
	uint32 ctr_tbl = 0;

	for (uint32 i = start ; i < endT ; i += PAGE_SIZE*1024){

		uint32 *ptr ;
		int retVal = get_page_table(page_directory , i , &ptr);

		if (retVal == TABLE_IN_MEMORY){
			ctr_tbl++;

		}
		*num_tables = ctr_tbl;
	}

	for (uint32 i = start ; i < endP ; i += PAGE_SIZE){

		uint32 *ptr2;
		struct FrameInfo * ptr_frame_info = get_frame_info(page_directory , i , &ptr2);

		if (ptr_frame_info != NULL){
			ctr_pg++;
		}
		*num_pages  = ctr_pg;
	}

	//cprintf("Number Of PageTables =  %d\n" , *num_tables);
	//cprintf("Number Of Pages      =  %d\n" , *num_pages);
}

/*BONUS*/
//=====================================
// 6) CALCULATE REQUIRED FRAMES IN RAM:
//=====================================
// calculate_required_frames:
// calculates the new allocation size required for given address+size,
// we are not interested in knowing if pages or tables actually exist in memory or the page file,
// we are interested in knowing whether they are allocated or not.
uint32 calculate_required_frames(uint32* page_directory, uint32 sva, uint32 size)
{
	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_required_frames
	// Write your code here, remove the panic and write your code
	//panic("calculate_required_frames() is not implemented yet...!!");

	uint32 start = ROUNDDOWN(sva , PAGE_SIZE);
	uint32 endT  = ROUNDUP  (sva + size , PAGE_SIZE*1024);
	uint32 endP  = ROUNDUP  (sva + size , PAGE_SIZE);
	uint32 Cntr_pages = 0 , Cntr_tables = 0;

	for(int i = start ; i < endT ; i += PAGE_SIZE*1024){

		uint32 *ptr;
		int retVal = get_page_table(page_directory , i , &ptr);
		if ( retVal == TABLE_NOT_EXIST){
			Cntr_tables++;
		}
	}

	for(int i = start ; i < endP ; i += PAGE_SIZE){

		uint32 *ptr_f;
		struct FrameInfo * ptr_frame_info = get_frame_info(page_directory , i , &ptr_f);

		if ( ptr_frame_info == NULL){
			Cntr_pages++;
		}
	}

	uint32 Res = Cntr_pages + Cntr_tables;
	return Res;
}

//=================================================================================//
//===========================END RAM CHUNKS MANIPULATION ==========================//
//=================================================================================//

/*******************************/
/*[2] USER CHUNKS MANIPULATION */
/*******************************/

//======================================================
/// functions used for USER HEAP (malloc, free, ...)
//======================================================

//=====================================
// 1) ALLOCATE USER MEMORY:
//=====================================
void allocate_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	// Write your code here, remove the panic and write your code
	panic("allocate_user_mem() is not implemented yet...!!");
}

//=====================================
// 2) FREE USER MEMORY:
//=====================================
void free_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	//TODO: [PROJECT MS3] [USER HEAP - KERNEL SIDE] free_user_mem
	// Write your code here, remove the panic and write your code
	//panic("free_user_mem() is not implemented yet...!!");
	//This function should:
	//1. Free ALL pages of the given range from the Page File
	//2. Free ONLY pages that are resident in the working set from the memory
	//3. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
	uint32 end = virtual_address + size;
	uint32 va  = virtual_address;

	for(uint32 i = virtual_address ; i < end ; i += PAGE_SIZE){
		pf_remove_env_page(e , i);
	}

	for(uint32 i = 0 ; i < e->page_WS_max_size ; i ++){
		uint32 ret = env_page_ws_get_virtual_address(e,i);
		if (ret >= virtual_address && ret < virtual_address+size){
			unmap_frame(e->env_page_directory,ret);
			env_page_ws_clear_entry(e,i);
		}
	}

	for(uint32 i = virtual_address ; i < virtual_address+size ; i += PAGE_SIZE){
		uint32 *ptr = NULL;
		get_page_table(e->env_page_directory , i , &ptr);
		int f = 1;
		if(ptr != NULL){
			for (int i=0 ; i<1024 ; ++i){
				if(ptr[i] != 0){
					f = 0;
					break;
				}
			}
			if(f == 1){
				kfree((void*)ptr);
				pd_clear_page_dir_entry((void*)e,(uint32)va);
			}
		}
	}
	//tlbflush();
}
//=====================================
// 2) FREE USER MEMORY (BUFFERING):
//=====================================
void __free_user_mem_with_buffering(struct Env* e, uint32 virtual_address, uint32 size)
{
	// your code is here, remove the panic and write your code
	panic("__free_user_mem_with_buffering() is not implemented yet...!!");

	//This function should:
	//1. Free ALL pages of the given range from the Page File
	//2. Free ONLY pages that are resident in the working set from the memory
	//3. Free any BUFFERED pages in the given range
	//4. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
}

//=====================================
// 3) MOVE USER MEMORY:
//=====================================
void move_user_mem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
{
	//TODO: [PROJECT MS3 - BONUS] [USER HEAP - KERNEL SIDE] move_user_mem
	//your code is here, remove the panic and write your code
	panic("move_user_mem() is not implemented yet...!!");

	// This function should move all pages from "src_virtual_address" to "dst_virtual_address"
	// with the given size
	// After finished, the src_virtual_address must no longer be accessed/exist in either page file
	// or main memory

	/**/
}

//=================================================================================//
//========================== END USER CHUNKS MANIPULATION =========================//
//=================================================================================//


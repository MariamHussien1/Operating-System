/*
 * paging_helpers.c
 *
 *  Created on: Sep 30, 2022
 *      Author: HP
 */
#include "memory_manager.h"

/*[2.1] PAGE TABLE ENTRIES MANIPULATION */
inline void pt_set_page_permissions(uint32* page_directory, uint32 virtual_address, uint32 permissions_to_set, uint32 permissions_to_clear)
{
	//TODO: [PROJECT MS2] [PAGING HELPERS] pt_set_page_permissions
	// Write your code here, remove the panic and write your code
	//panic("pt_set_page_permissions() is not implemented yet...!!");
	uint32 *ptr_PT = NULL;
	get_page_table(page_directory, virtual_address, &ptr_PT);
	if (ptr_PT==NULL){
		panic("Invalid va");
		}
	else if (ptr_PT != NULL){
		if (permissions_to_set !=0 && permissions_to_clear ==0){
			ptr_PT[PTX(virtual_address)]= ptr_PT[PTX(virtual_address)] | (permissions_to_set);
		}
		else if (permissions_to_set == 0 && permissions_to_clear !=0){
			ptr_PT[PTX(virtual_address)]= ptr_PT[PTX(virtual_address)] & (~permissions_to_clear);
		}
		else if (permissions_to_set != 0 && permissions_to_clear !=0){
			ptr_PT[PTX(virtual_address)]= ptr_PT[PTX(virtual_address)] | (permissions_to_set);
			ptr_PT[PTX(virtual_address)]= ptr_PT[PTX(virtual_address)] & (~permissions_to_clear);
		}
	}
	tlb_invalidate((void *)NULL, (void *)virtual_address);
}

inline int pt_get_page_permissions(uint32* page_directory, uint32 virtual_address )
{
	//TODO: [PROJECT MS2] [PAGING HELPERS] pt_get_page_permissions
	// Write your code here, remove the panic and write your code
	//panic("pt_get_page_permissions() is not implemented yet...!!");
	uint32 *ptr_page_table = NULL;
	get_page_table(page_directory, virtual_address, &ptr_page_table);
	if (ptr_page_table != NULL){
		uint32 PageTable_entry = ptr_page_table [PTX(virtual_address)];
		uint32 perm = PageTable_entry << 20;
		perm = perm >> 20;
		return perm;
	}
	else{
		return -1;
	}
}

inline void pt_clear_page_table_entry(uint32* page_directory, uint32 virtual_address)
{
	//TODO: [PROJECT MS2] [PAGING HELPERS] pt_clear_page_table_entry
	// Write your code here, remove the panic and write your code
	//panic("pt_clear_page_table_entry() is not implemented yet...!!");
	uint32 *ptr_page_table=NULL;
	get_page_table(page_directory,virtual_address,&ptr_page_table);
	if(ptr_page_table==NULL){
		panic("Invalid va");
	}
	ptr_page_table[PTX(virtual_address)] = 0;
	tlb_invalidate((void *)NULL, (void *)virtual_address);
}

/***********************************************************************************************/

/*[2.2] ADDRESS CONVERTION*/
inline int virtual_to_physical(uint32* page_directory, uint32 virtual_address)
{
	//TODO: [PROJECT MS2] [PAGING HELPERS] virtual_to_physical
	// Write your code here, remove the panic and write your code
	//panic("virtual_to_physical() is not implemented yet...!!");
	uint32 *ptr_page_table = NULL;
	get_page_table(page_directory, virtual_address, &ptr_page_table);
	if (ptr_page_table != NULL){
		uint32 PageTable_entry = ptr_page_table[PTX(virtual_address)];
		uint32 frameNum = PageTable_entry >> 12;
		uint32 physical_address = frameNum * PAGE_SIZE;
		return physical_address;
	}
	else{
		return -1;
	}
}

/***********************************************************************************************/

/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/

///============================================================================================
/// Dealing with page directory entry flags

inline uint32 pd_is_table_used(uint32* page_directory, uint32 virtual_address)
{
	return ( (page_directory[PDX(virtual_address)] & PERM_USED) == PERM_USED ? 1 : 0);
}

inline void pd_set_table_unused(uint32* page_directory, uint32 virtual_address)
{
	page_directory[PDX(virtual_address)] &= (~PERM_USED);
	tlb_invalidate((void *)NULL, (void *)virtual_address);
}

inline void pd_clear_page_dir_entry(uint32* page_directory, uint32 virtual_address)
{
	page_directory[PDX(virtual_address)] = 0 ;
	tlbflush();
}

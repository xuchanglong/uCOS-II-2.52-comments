/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                            MEMORY MANAGEMENT
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : OS_MEM.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE
#include "includes.h"
#endif

#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)
/*
*********************************************************************************************************
*                                        CREATE A MEMORY PARTITION
*
* Description : Create a fixed-sized memory partition that will be managed by uC/OS-II.
*
* Arguments   : addr     is the starting address of the memory partition
*
*               nblks    is the number of memory blocks to create from the partition.
*
*               blksize  is the size (in bytes) of each block in the memory partition.
*
*               err      is a pointer to a variable containing an error message which will be set by
*                        this function to either:
*
*                        OS_NO_ERR            if the memory partition has been created correctly.
*                        OS_MEM_INVALID_ADDR  you are specifying an invalid address for the memory 
*                                             storage of the partition.
*                        OS_MEM_INVALID_PART  no free partitions available
*                        OS_MEM_INVALID_BLKS  user specified an invalid number of blocks (must be >= 2)
*                        OS_MEM_INVALID_SIZE  user specified an invalid block size
*                                             (must be greater than the size of a pointer)
* Returns    : != (OS_MEM *)0  is the partition was created
*              == (OS_MEM *)0  if the partition was not created because of invalid arguments or, no
*                              free partition is available.
*********************************************************************************************************
*/

OS_MEM  *OSMemCreate (void *addr, INT32U nblks, INT32U blksize, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                           /* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr;
#endif    
//	存放内存控制块首地址
    OS_MEM    *pmem;
    INT8U     *pblk;
    void     **plink;
    INT32U     i;


#if OS_ARG_CHK_EN > 0
//	内存分区首地址不能为空
    if (addr == (void *)0) 
	{                          /* Must pass a valid address for the memory part. */
        *err = OS_MEM_INVALID_ADDR;
        return ((OS_MEM *)0);
    }
//	每个分区的内存块的个数至少2个
    if (nblks < 2) 
	{                                  /* Must have at least 2 blocks per partition      */
        *err = OS_MEM_INVALID_BLKS;
        return ((OS_MEM *)0);
    }
//	每个内存块的大小至少能够存下一个指针
    if (blksize < sizeof(void *)) 
	{                   /* Must contain space for at least a pointer      */
        *err = OS_MEM_INVALID_SIZE;
        return ((OS_MEM *)0);
    }
#endif
//	打开中断
    OS_ENTER_CRITICAL();
//	从空闲内存块链表中得到1个内存控制块
    pmem = OSMemFreeList;                             /* Get next free memory partition                */
//	空闲内存块链表不能为空
//	若指针为0，说明当前Node即为链表最后一个Node
	if (OSMemFreeList != (OS_MEM *)0) 
	{               /* See if pool of free partitions was empty      */
        OSMemFreeList = (OS_MEM *)OSMemFreeList->OSMemFreeList;
    }
//	关闭中断
    OS_EXIT_CRITICAL();
//	看是否有空余的内存控制块
    if (pmem == (OS_MEM *)0) 
	{                        /* See if we have a memory partition             */
        *err = OS_MEM_INVALID_PART;
        return ((OS_MEM *)0);
    }
//	由于内存分区不能直接存放指针，故将内存分区首地址强转为2维指针，这样数组元素可以放下指针，
//	从而每个内存块的首字节可以放下下一个内存块的首地址

//	详情：https://blog.csdn.net/fanwei326/article/details/6127091

    plink = (void **)addr;                            /* Create linked list of free memory blocks      */
    pblk  = (INT8U *)addr + blksize;
    for (i = 0; i < (nblks - 1); i++) 
	{
	//	在当前内存块的首地址放入下一个内存块的首地址
        *plink = (void *)pblk;
        plink  = (void **)pblk;
        pblk   = pblk + blksize;
    }
//	最后一个节点的首字节放入0，即：链表的末端。 
    *plink              = (void *)0;                  /* Last memory block points to NULL              */
    pmem->OSMemAddr     = addr;                       /* Store start address of memory partition       */
    pmem->OSMemFreeList = addr;                       /* Initialize pointer to pool of free blocks     */
    pmem->OSMemNFree    = nblks;                      /* Store number of free blocks in MCB            */
    pmem->OSMemNBlks    = nblks;
    pmem->OSMemBlkSize  = blksize;                    /* Store block size of each memory blocks        */
    *err                = OS_NO_ERR;
    return (pmem);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          GET A MEMORY BLOCK
*
* Description : Get a memory block from a partition
*
* Arguments   : pmem    is a pointer to the memory partition control block
*
*               err     is a pointer to a variable containing an error message which will be set by this
*                       function to either:
*
*                       OS_NO_ERR           if the memory partition has been created correctly.
*                       OS_MEM_NO_FREE_BLKS if there are no more free memory blocks to allocate to caller
*                       OS_MEM_INVALID_PMEM if you passed a NULL pointer for 'pmem'
*
* Returns     : A pointer to a memory block if no error is detected
*               A pointer to NULL if an error is detected
*********************************************************************************************************
*/

void  *OSMemGet (OS_MEM *pmem, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                           /* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr;
#endif  
//	存储当前空闲内存块的首地址
    void      *pblk;


#if OS_ARG_CHK_EN > 0
    if (pmem == (OS_MEM *)0) 
	{                        /* Must point to a valid memory partition         */
        *err = OS_MEM_INVALID_PMEM;
        return ((OS_MEM *)0);
    }
#endif
//	关中断
    OS_ENTER_CRITICAL();
//	当前存在空闲内存块
    if (pmem->OSMemNFree > 0) 
	{                       /* See if there are any free memory blocks       */
	//	指向下一个内存块的首地址
        pblk                = pmem->OSMemFreeList;    /* Yes, point to next free memory block          */
	//	该代码是重点
	//	(void **)pblk  将当前指针转化为二级指针，即：*pblk存放着指针，
	//	该指针指向了下一个内存块的首地址
		pmem->OSMemFreeList = *(void **)pblk;         /*      Adjust pointer to new free list          */
	//	空闲内存块数量减一
		pmem->OSMemNFree--;                           /*      One less memory block in this partition  */
        OS_EXIT_CRITICAL();
        *err = OS_NO_ERR;                             /*      No error                                 */
        return (pblk);                                /*      Return memory block to caller            */
    }
//	开中断
    OS_EXIT_CRITICAL();
    *err = OS_MEM_NO_FREE_BLKS;                       /* No,  Notify caller of empty memory partition  */
    return ((void *)0);                               /*      Return NULL pointer to caller            */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                         RELEASE A MEMORY BLOCK
*
* Description : Returns a memory block to a partition
*
* Arguments   : pmem    is a pointer to the memory partition control block
*
*               pblk    is a pointer to the memory block being released.
*
* Returns     : OS_NO_ERR            if the memory block was inserted into the partition
*               OS_MEM_FULL          if you are returning a memory block to an already FULL memory 
*                                    partition (You freed more blocks than you allocated!)
*               OS_MEM_INVALID_PMEM  if you passed a NULL pointer for 'pmem'
*               OS_MEM_INVALID_PBLK  if you passed a NULL pointer for the block to release.
*********************************************************************************************************
*/

INT8U  OSMemPut (OS_MEM  *pmem, void *pblk)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    
    
#if OS_ARG_CHK_EN > 0
    if (pmem == (OS_MEM *)0) 
	{                   /* Must point to a valid memory partition             */
        return (OS_MEM_INVALID_PMEM);
    }
    if (pblk == (void *)0) 
	{                     /* Must release a valid block                         */
        return (OS_MEM_INVALID_PBLK);
    }
#endif
    OS_ENTER_CRITICAL();
//	防止重复性释放内存块
    if (pmem->OSMemNFree >= pmem->OSMemNBlks) 
	{  /* Make sure all blocks not already returned          */
        OS_EXIT_CRITICAL();
        return (OS_MEM_FULL);
    }
//	该代码是重点
//	将当前的空闲块的首地址赋值给待释放的内存块的首字节中
//	如此，待释放的内存块就变成了空闲块链表的首块
    *(void **)pblk      = pmem->OSMemFreeList;   /* Insert released block into free block list         */
//	将待释放的内存块赋值给空闲块链表的表头
	pmem->OSMemFreeList = pblk;
//	空闲块数量加一
	pmem->OSMemNFree++;                          /* One more memory block in this partition            */
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);                          /* Notify caller that memory block was released       */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          QUERY MEMORY PARTITION
*
* Description : This function is used to determine the number of free memory blocks and the number of
*               used memory blocks from a memory partition.
*
* Arguments   : pmem    is a pointer to the memory partition control block
*
*               pdata   is a pointer to a structure that will contain information about the memory
*                       partition.
*
* Returns     : OS_NO_ERR            If no errors were found.
*               OS_MEM_INVALID_PMEM  if you passed a NULL pointer for 'pmem'
*               OS_MEM_INVALID_PDATA if you passed a NULL pointer for the block to release.
*********************************************************************************************************
*/

#if OS_MEM_QUERY_EN > 0
INT8U  OSMemQuery (OS_MEM *pmem, OS_MEM_DATA *pdata)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    
    
#if OS_ARG_CHK_EN > 0
    if (pmem == (OS_MEM *)0) 
	{                   /* Must point to a valid memory partition             */
        return (OS_MEM_INVALID_PMEM);
    }
    if (pdata == (OS_MEM_DATA *)0) 
	{             /* Must release a valid storage area for the data     */
        return (OS_MEM_INVALID_PDATA);
    }
#endif
    OS_ENTER_CRITICAL();
    pdata->OSAddr     = pmem->OSMemAddr;
    pdata->OSFreeList = pmem->OSMemFreeList;
    pdata->OSBlkSize  = pmem->OSMemBlkSize;
    pdata->OSNBlks    = pmem->OSMemNBlks;
    pdata->OSNFree    = pmem->OSMemNFree;
    OS_EXIT_CRITICAL();
    pdata->OSNUsed    = pdata->OSNBlks - pdata->OSNFree;
    return (OS_NO_ERR);
}
#endif                                           /* OS_MEM_QUERY_EN                                    */
/*$PAGE*/
/*
*********************************************************************************************************
*                                    INITIALIZE MEMORY PARTITION MANAGER
*
* Description : This function is called by uC/OS-II to initialize the memory partition manager.  Your
*               application MUST NOT call this function.
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/

void  OS_MemInit (void)
{
//	当内存分区只有一个的时候
#if OS_MAX_MEM_PART == 1
    OSMemFreeList                = (OS_MEM *)&OSMemTbl[0]; /* Point to beginning of free list          */
//	由于只有一个分区，故最后一个节点为0
	OSMemFreeList->OSMemFreeList = (void *)0;              /* Initialize last node                     */
//	内存分区指针为0
	OSMemFreeList->OSMemAddr     = (void *)0;              /* Store start address of memory partition  */
//	当前可分配的内存块数量
	OSMemFreeList->OSMemNFree    = 0;                      /* No free blocks                           */
//	内存块数量为0
	OSMemFreeList->OSMemNBlks    = 0;                      /* No blocks                                */
//	内存块的长度
	OSMemFreeList->OSMemBlkSize  = 0;                      /* Zero size                                */
#endif

#if OS_MAX_MEM_PART >= 2
    OS_MEM  *pmem;
    INT16U   i;

//	指向内存控制块的首地址。
    pmem = (OS_MEM *)&OSMemTbl[0];                    /* Point to memory control block (MCB)           */
    for (i = 0; i < (OS_MAX_MEM_PART - 1); i++) 
	{     /* Init. list of free memory partitions          */
	//	串联内存控制块链表的指针
        pmem->OSMemFreeList = (void *)&OSMemTbl[i+1]; /* Chain list of free partitions                 */
	//	内存分区指针清零
		pmem->OSMemAddr     = (void *)0;              /* Store start address of memory partition       */
	//	当前无可用的内存块
		pmem->OSMemNFree    = 0;                      /* No free blocks                                */
	//	内存块数量为0
		pmem->OSMemNBlks    = 0;                      /* No blocks                                     */
	//	内存块长度为0
		pmem->OSMemBlkSize  = 0;                      /* Zero size                                     */
        pmem++;
    }
    pmem->OSMemFreeList = (void *)0;                  /* Initialize last node                          */
    pmem->OSMemAddr     = (void *)0;                  /* Store start address of memory partition       */
    pmem->OSMemNFree    = 0;                          /* No free blocks                                */
    pmem->OSMemNBlks    = 0;                          /* No blocks                                     */
    pmem->OSMemBlkSize  = 0;                          /* Zero size                                     */
//	指向空闲内存块链表的首地址
    OSMemFreeList       = (OS_MEM *)&OSMemTbl[0];     /* Point to beginning of free list               */
#endif
}
#endif                                           /* OS_MEM_EN                                          */

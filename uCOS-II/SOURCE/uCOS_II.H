/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                            (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : uCOS_II.H
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             MISCELLANEOUS
*												（杂项）
*********************************************************************************************************
*/
//	该值除以100即为OS版本号
#define  OS_VERSION              252                    /* Version of uC/OS-II (Vx.yy mult. by 100)    */
//	指明是否是全局的
#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif

#ifndef  FALSE
#define  FALSE                     0
#endif

#ifndef  TRUE
#define  TRUE                      1
#endif
//	指明是任务本身的优先级
#define  OS_PRIO_SELF           0xFF                    /* Indicate SELF priority                      */
//	指明是否创建统计任务
#if OS_TASK_STAT_EN > 0
//	系统任务的数量（空闲任务和统计任务）
#define  OS_N_SYS_TASKS            2                    /* Number of system tasks                      */
#else
//	空闲任务
#define  OS_N_SYS_TASKS            1
#endif
//	统计任务的优先级
#define  OS_STAT_PRIO       (OS_LOWEST_PRIO - 1)        /* Statistic task priority                     */
//	空闲任务的优先级
#define  OS_IDLE_PRIO       (OS_LOWEST_PRIO)            /* IDLE      task priority                     */
//	确定等待任务列表的字节数，即：每一位代表一个等待任务。
#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO) / 8 + 1)   /* Size of event table                         */
//	确定就绪任务列表的字节数，即：每一位代表一个就绪任务。
#define  OS_RDY_TBL_SIZE   ((OS_LOWEST_PRIO) / 8 + 1)   /* Size of ready table                         */

#define  OS_TASK_IDLE_ID       65535                    /* I.D. numbers for Idle and Stat tasks        */
#define  OS_TASK_STAT_ID       65534
//	所有的事件是否都使能
#define  OS_EVENT_EN       (((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0) || (OS_SEM_EN > 0) || (OS_MUTEX_EN > 0))

/*$PAGE*/
/*
*********************************************************************************************************
*                              TASK STATUS (Bit definition for OSTCBStat)
*							   （任务状态字，供OS_TCB中OSTCBStat使用）
*********************************************************************************************************
*/
//	就绪状态
#define  OS_STAT_RDY            0x00        /* Ready to run                                            */
//	等待信号量状态
#define  OS_STAT_SEM            0x01        /* Pending on semaphore                                    */
//	等待消息邮箱状态
#define  OS_STAT_MBOX           0x02        /* Pending on mailbox                                      */
//	等待队列状态
#define  OS_STAT_Q              0x04        /* Pending on queue                                        */
//	挂起状态
#define  OS_STAT_SUSPEND        0x08        /* Task is suspended                                       */
//	等待互斥量状态
#define  OS_STAT_MUTEX          0x10        /* Pending on mutual exclusion semaphore                   */
//	等待信号量标志组状态
#define  OS_STAT_FLAG           0x20        /* Pending on event flag group                             */

/*
*********************************************************************************************************
*                                        OS_EVENT types
*										 事件类型，供OS_EVENT中的OSEventType使用
*********************************************************************************************************
*/
//	暂时没有用到
#define  OS_EVENT_TYPE_UNUSED      0
//	消息邮箱
#define  OS_EVENT_TYPE_MBOX        1
//	队列
#define  OS_EVENT_TYPE_Q           2
//	信号量
#define  OS_EVENT_TYPE_SEM         3
//	互斥量
#define  OS_EVENT_TYPE_MUTEX       4
//	信号量标志组
#define  OS_EVENT_TYPE_FLAG        5

/*
*********************************************************************************************************
*                                         EVENT FLAGS
*										  信号量逻辑运算关系，供OS_FLAG_NODE中的OSFlagNodeWaitType使用
*********************************************************************************************************
*/
//	指定的信号量均是0
#define  OS_FLAG_WAIT_CLR_ALL      0        /* Wait for ALL    the bits specified to be CLR (i.e. 0)   */ 
#define  OS_FLAG_WAIT_CLR_AND      0
//	指定的信号量至少有一个是0
#define  OS_FLAG_WAIT_CLR_ANY      1        /* Wait for ANY of the bits specified to be CLR (i.e. 0)   */
#define  OS_FLAG_WAIT_CLR_OR       1
//	指定的信号量均是1
#define  OS_FLAG_WAIT_SET_ALL      2        /* Wait for ALL    the bits specified to be SET (i.e. 1)   */ 
#define  OS_FLAG_WAIT_SET_AND      2
//	指定的信号量至少有一个是1
#define  OS_FLAG_WAIT_SET_ANY      3        /* Wait for ANY of the bits specified to be SET (i.e. 1)   */
#define  OS_FLAG_WAIT_SET_OR       3

//	定义恢复信号量的标志位，若该位为1，则等待任务得到指定的信号量置位之后，
//	将这些标志位复位。
#define  OS_FLAG_CONSUME        0x80        /* Consume the flags if condition(s) satisfied             */

//	定义信号量的标志位置位时的数值
#define  OS_FLAG_CLR               0
#define  OS_FLAG_SET               1

/*
*********************************************************************************************************
*       Possible values for 'opt' argument of OSSemDel(), OSMboxDel(), OSQDel() and OSMutexDel()
*		定义删除事件时的情况，		
*********************************************************************************************************
*/
//	该事件没有等待任务时才删除
#define  OS_DEL_NO_PEND            0
//	该事件删除，将等待的任务恢复就绪状态
#define  OS_DEL_ALWAYS             1

/*
*********************************************************************************************************
*                                     OS???PostOpt() OPTIONS
*
* These #defines are used to establish the options for OSMboxPostOpt() and OSQPostOpt().
* 定义发送消息到消息邮箱和队列的情况。
*********************************************************************************************************
*/
//	发送消息到等待任务中优先级最高的任务
#define  OS_POST_OPT_NONE       0x00        /* Post to highest priority task waiting                   */
//	发送消息到所有的等待的任务
#define  OS_POST_OPT_BROADCAST  0x01        /* Broadcast message to ALL tasks waiting                  */  
//	发送消息到队列中最前面的任务
#define  OS_POST_OPT_FRONT      0x02        /* Post to highest priority task waiting                   */

/*
*********************************************************************************************************
*                                 TASK OPTIONS (see OSTaskCreateExt()) 
*********************************************************************************************************
*/
#define  OS_TASK_OPT_STK_CHK  0x0001        /* Enable stack checking for the task                      */
#define  OS_TASK_OPT_STK_CLR  0x0002        /* Clear the stack when the task is create                 */
#define  OS_TASK_OPT_SAVE_FP  0x0004        /* Save the contents of any floating-point registers       */

/*
*********************************************************************************************************
*                                             ERROR CODES
*												错误代码
*********************************************************************************************************
*/
#define OS_NO_ERR                 0

#define OS_ERR_EVENT_TYPE         1
#define OS_ERR_PEND_ISR           2
#define OS_ERR_POST_NULL_PTR      3
#define OS_ERR_PEVENT_NULL        4
#define OS_ERR_POST_ISR           5
#define OS_ERR_QUERY_ISR          6
#define OS_ERR_INVALID_OPT        7
#define OS_ERR_TASK_WAITING       8

#define OS_TIMEOUT               10
#define OS_TASK_NOT_EXIST        11

#define OS_MBOX_FULL             20

#define OS_Q_FULL                30

#define OS_PRIO_EXIST            40
#define OS_PRIO_ERR              41
#define OS_PRIO_INVALID          42

#define OS_SEM_OVF               50

#define OS_TASK_DEL_ERR          60
#define OS_TASK_DEL_IDLE         61
#define OS_TASK_DEL_REQ          62
#define OS_TASK_DEL_ISR          63

#define OS_NO_MORE_TCB           70

#define OS_TIME_NOT_DLY          80
#define OS_TIME_INVALID_MINUTES  81
#define OS_TIME_INVALID_SECONDS  82
#define OS_TIME_INVALID_MILLI    83
#define OS_TIME_ZERO_DLY         84

#define OS_TASK_SUSPEND_PRIO     90
#define OS_TASK_SUSPEND_IDLE     91

#define OS_TASK_RESUME_PRIO     100
#define OS_TASK_NOT_SUSPENDED   101

#define OS_MEM_INVALID_PART     110
#define OS_MEM_INVALID_BLKS     111
#define OS_MEM_INVALID_SIZE     112
#define OS_MEM_NO_FREE_BLKS     113
#define OS_MEM_FULL             114
#define OS_MEM_INVALID_PBLK     115
#define OS_MEM_INVALID_PMEM     116
#define OS_MEM_INVALID_PDATA    117
#define OS_MEM_INVALID_ADDR     118

#define OS_ERR_NOT_MUTEX_OWNER  120

#define OS_TASK_OPT_ERR         130

#define OS_ERR_DEL_ISR          140
#define OS_ERR_CREATE_ISR       141

#define OS_FLAG_INVALID_PGRP    150
#define OS_FLAG_ERR_WAIT_TYPE   151
#define OS_FLAG_ERR_NOT_RDY     152
#define OS_FLAG_INVALID_OPT     153
#define OS_FLAG_GRP_DEPLETED    154

/*$PAGE*/
/*
*********************************************************************************************************
*                                          EVENT CONTROL BLOCK
*											事件控制块
*********************************************************************************************************
*/

#if (OS_EVENT_EN > 0) && (OS_MAX_EVENTS > 0)
typedef struct {
//	事件类型
    INT8U   OSEventType;                   /* Type of event control block (see OS_EVENT_TYPE_???)      */
//	等待任务列表的任务组
	INT8U   OSEventGrp;                    /* Group corresponding to tasks waiting for event to occur  */
//	信号量计数器
	INT16U  OSEventCnt;                    /* Semaphore Count (not used if other EVENT type)           */
//	用来存放消息邮箱和消息队列的指针
	void   *OSEventPtr;                    /* Pointer to message or queue structure                    */
//	以优先级为顺序，记录等待的任务
	INT8U   OSEventTbl[OS_EVENT_TBL_SIZE]; /* List of tasks waiting for event to occur                 */
} OS_EVENT;
#endif


/*
*********************************************************************************************************
*                                       EVENT FLAGS CONTROL BLOCK
*										信号量集控制块
*********************************************************************************************************
*/

#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
//	信号量集的结构
typedef struct {                            /* Event Flag Group                                        */
//	信号量标志组的标志位，总是赋予OS_EVENT_TYPE_FLAG。
	INT8U         OSFlagType;               /* Should be set to OS_EVENT_TYPE_FLAG                     */
//	指向等待任务列表
	void         *OSFlagWaitList;           /* Pointer to first NODE of task waiting on event flag     */
//	记录各个标志位的值
	OS_FLAGS      OSFlagFlags;              /* 8, 16 or 32 bit flags                                   */
} OS_FLAG_GRP;    

    
//	信号量集的等待任务的链表的节点    
typedef struct {                            /* Event Flag Wait List Node                               */
//	指向下一个节点
	void         *OSFlagNodeNext;           /* Pointer to next     NODE in wait list                   */
//	指向前一个节点
	void         *OSFlagNodePrev;           /* Pointer to previous NODE in wait list                   */
//	指向等待的任务的控制块
	void         *OSFlagNodeTCB;            /* Pointer to TCB of waiting task                          */  
//	反向指向信号量集的控制块
	void         *OSFlagNodeFlagGrp;        /* Pointer to Event Flag Group                             */  
//	信号过滤器，即：确定该任务使用的信号
	OS_FLAGS      OSFlagNodeFlags;          /* Event flag to wait on                                   */  
//	定义信号量的逻辑运算的关系
	INT8U         OSFlagNodeWaitType;       /* Type of wait:                                           */
                                            /*      OS_FLAG_WAIT_AND                                   */
                                            /*      OS_FLAG_WAIT_ALL                                   */
                                            /*      OS_FLAG_WAIT_OR                                    */
                                            /*      OS_FLAG_WAIT_ANY                                   */
} OS_FLAG_NODE;
#endif


/*
*********************************************************************************************************
*                                          MESSAGE MAILBOX DATA
*											消息邮箱的数据
*********************************************************************************************************
*/

#if OS_MBOX_EN > 0
typedef struct {
//	指向邮箱中的消息
    void   *OSMsg;                         /* Pointer to message in mailbox                            */
//	以优先级为顺序，记录等待的任务
	INT8U   OSEventTbl[OS_EVENT_TBL_SIZE]; /* List of tasks waiting for event to occur                 */
//	记录等待的任务的组
	INT8U   OSEventGrp;                    /* Group corresponding to tasks waiting for event to occur  */
} OS_MBOX_DATA;
#endif

/*
*********************************************************************************************************
*                                     MEMORY PARTITION DATA STRUCTURES
*										内存分区的结构体
*********************************************************************************************************
*/

#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)
typedef struct {                       /* MEMORY CONTROL BLOCK                                         */
//	内存分区的首地址
	void   *OSMemAddr;                 /* Pointer to beginning of memory partition                     */
//	指向空闲内存块链表的首地址
	void   *OSMemFreeList;             /* Pointer to list of free memory blocks                        */
//	每一个内存块的大小
	INT32U  OSMemBlkSize;              /* Size (in bytes) of each block of memory                      */
//	该分区内存块的数量
	INT32U  OSMemNBlks;                /* Total number of blocks in this partition                     */
//	该分区的空闲的内存块的数量
	INT32U  OSMemNFree;                /* Number of memory blocks remaining in this partition          */
} OS_MEM;


typedef struct {
//	内存分区的首地址
	void   *OSAddr;                    /* Pointer to the beginning address of the memory partition     */
//	指向空闲内存块链表的首地址
	void   *OSFreeList;                /* Pointer to the beginning of the free list of memory blocks   */
//	内存块的大小
	INT32U  OSBlkSize;                 /* Size (in bytes) of each memory block                         */
//	分区中总共内存块的数量
	INT32U  OSNBlks;                   /* Total number of blocks in the partition                      */
//	分区中空闲的内存块的数量
	INT32U  OSNFree;                   /* Number of memory blocks free                                 */
//	分区中已用的内存块的数量
	INT32U  OSNUsed;                   /* Number of memory blocks used                                 */
} OS_MEM_DATA;
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                    MUTUAL EXCLUSION SEMAPHORE DATA
*										互斥性信号量（互斥量）结构体
*********************************************************************************************************
*/

#if OS_MUTEX_EN > 0
typedef struct {
//	以优先级为顺序，记录等待的任务
	INT8U   OSEventTbl[OS_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur                */
//	记录的等待任务的组
	INT8U   OSEventGrp;                     /* Group corresponding to tasks waiting for event to occur */
//	互斥量的值
	INT8U   OSValue;                        /* Mutex value (0 = used, 1 = available)                   */
//	保存占用互斥量的任务的优先级（OSEventCnt的低8位）
	INT8U   OSOwnerPrio;                    /* Mutex owner's task priority or 0xFF if no owner         */
//	保存继承的优先级（OSEventCnt的高8位）
	INT8U   OSMutexPIP;                     /* Priority Inheritance Priority or 0xFF if no owner       */
} OS_MUTEX_DATA;
#endif

/*
*********************************************************************************************************
*                                          MESSAGE QUEUE DATA
*											消息队列的结构
*********************************************************************************************************
*/

#if OS_Q_EN > 0
typedef struct os_q {                   /* QUEUE CONTROL BLOCK                                         */
//	指向下一个队列控制块
	struct os_q   *OSQPtr;              /* Link to next queue control block in list of free blocks     */
//	指向队列的开始位置
	void         **OSQStart;            /* Pointer to start of queue data                              */
//	指向队列的结束位置
	void         **OSQEnd;              /* Pointer to end   of queue data                              */
//	指向下一个消息将要插入到队列的位置
	void         **OSQIn;               /* Pointer to where next message will be inserted  in   the Q  */
//	指向下一个消息将要从队列提取的位置
	void         **OSQOut;              /* Pointer to where next message will be extracted from the Q  */
//	队列的大小
	INT16U         OSQSize;             /* Size of queue (maximum number of entries)                   */
//	队列中当前消息的数量
	INT16U         OSQEntries;          /* Current number of entries in the queue                      */
} OS_Q;


typedef struct {
//	指向下一个将要提取的消息
	void          *OSMsg;               /* Pointer to next message to be extracted from queue          */
//	队列中的消息的数量
	INT16U         OSNMsgs;             /* Number of messages in message queue                         */
//	队列的大小
	INT16U         OSQSize;             /* Size of message queue                                       */
//	以优先级为顺序，记录等待的任务		
	INT8U          OSEventTbl[OS_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur         */
//	等待任务的组
	INT8U          OSEventGrp;          /* Group corresponding to tasks waiting for event to occur     */
} OS_Q_DATA;
#endif

/*
*********************************************************************************************************
*                                           SEMAPHORE DATA
*											信号量结构
*********************************************************************************************************
*/

#if OS_SEM_EN > 0
typedef struct {
//	信号量的值
	INT16U  OSCnt;                          /* Semaphore count                                         */
//	以优先级为顺序，记录等待的任务
	INT8U   OSEventTbl[OS_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur                */
//	等待任务的组
	INT8U   OSEventGrp;                     /* Group corresponding to tasks waiting for event to occur */
} OS_SEM_DATA;
#endif

/*
*********************************************************************************************************
*                                            TASK STACK DATA
*												任务堆栈结构
*********************************************************************************************************
*/

#if OS_TASK_CREATE_EXT_EN > 0
typedef struct {
//	堆栈中空闲的字节的数量
	INT32U  OSFree;                    /* Number of free bytes on the stack                            */
//	堆栈中已用的字节的数量
	INT32U  OSUsed;                    /* Number of bytes used on the stack                            */
} OS_STK_DATA;
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          TASK CONTROL BLOCK
*********************************************************************************************************
*/

typedef struct os_tcb 
{
//	指向当前任务的栈顶
    OS_STK        *OSTCBStkPtr;        /* Pointer to current top of stack                              */

#if OS_TASK_CREATE_EXT_EN > 0
    void          *OSTCBExtPtr;        /* Pointer to user definable data for TCB extension             */
    OS_STK        *OSTCBStkBottom;     /* Pointer to bottom of stack                                   */
    INT32U         OSTCBStkSize;       /* Size of task stack (in number of stack elements)             */
    INT16U         OSTCBOpt;           /* Task options as passed by OSTaskCreateExt()                  */
    INT16U         OSTCBId;            /* Task ID (0..65535)                                           */
#endif
//	指向下一个任务的节点
    struct os_tcb *OSTCBNext;          /* Pointer to next     TCB in the TCB list                      */
//	指向上一个任务的节点
	struct os_tcb *OSTCBPrev;          /* Pointer to previous TCB in the TCB list                      */

#if ((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0) || (OS_SEM_EN > 0) || (OS_MUTEX_EN > 0)
//	指向事件控制块的首地址
	OS_EVENT      *OSTCBEventPtr;      /* Pointer to event control block                               */
#endif

#if ((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0)
//	存储从OSMboxPost、OSQPost接受来的消息
	void          *OSTCBMsg;           /* Message received from OSMboxPost() or OSQPost()              */
#endif

#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
#if OS_TASK_DEL_EN > 0
//	指向信号量集的等待任务节点的首地址
    OS_FLAG_NODE  *OSTCBFlagNode;      /* Pointer to event flag node                                   */
#endif   
//	记录信号量集中所使用的标志
    OS_FLAGS       OSTCBFlagsRdy;      /* Event flags that made task ready to run                      */
#endif
//	任务等待是时长（节拍数）
    INT16U         OSTCBDly;           /* Nbr ticks to delay task or, timeout waiting for event        */
//	任务的状态
	INT8U          OSTCBStat;          /* Task status                                                  */
//	任务的优先级
	INT8U          OSTCBPrio;          /* Task priority (0 == highest, 63 == lowest)                   */
//	存储任务优先级的低3位，即：X
    INT8U          OSTCBX;             /* Bit position in group  corresponding to task priority (0..7) */
//	存储任务优先级的高3位，即：Y
	INT8U          OSTCBY;             /* Index into ready table corresponding to task priority        */
//	存储就绪表中的位掩码
	INT8U          OSTCBBitX;          /* Bit mask to access bit position in ready table               */
//	存储就绪组中的位掩码
	INT8U          OSTCBBitY;          /* Bit mask to access bit position in ready group               */

#if OS_TASK_DEL_EN > 0
//	存储该任务是否需要删除自身，若其他任务要删除该任务，
//	则将该为变为 OS_TASK_DEL_REQ，该任务会在适当的时候
//	删除自身。
    BOOLEAN        OSTCBDelReq;        /* Indicates whether a task needs to delete itself              */
#endif
} OS_TCB;

/*$PAGE*/
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*												全局变量
*********************************************************************************************************
*/
//	作用：记录上下文切换的次数。
//	在OSInit中初始化为0。
//	在退出中断函数（OSIntExit）或者任务切换（OS_Sched）时，若就绪表中最高优先级的任务不是本任务，则进行切换，该值+1
OS_EXT  INT32U            OSCtxSwCtr;               /* Counter of number of context switches           */

#if (OS_EVENT_EN > 0) && (OS_MAX_EVENTS > 0)
//	指向空闲事件控制块链表的首地址
OS_EXT  OS_EVENT         *OSEventFreeList;          /* Pointer to list of free EVENT control blocks    */
//	事件控制块数组
OS_EXT  OS_EVENT          OSEventTbl[OS_MAX_EVENTS];/* Table of EVENT control blocks                   */
#endif

#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
//	信号量集组数组
OS_EXT  OS_FLAG_GRP       OSFlagTbl[OS_MAX_FLAGS];  /* Table containing event flag groups              */
//	指向空闲信号量集组链表的首地址
OS_EXT  OS_FLAG_GRP      *OSFlagFreeList;           /* Pointer to free list of event flag groups       */
#endif

#if OS_TASK_STAT_EN > 0
//	CPU使用率
OS_EXT  INT8S             OSCPUUsage;               /* Percentage of CPU used                          */
OS_EXT  INT32U            OSIdleCtrMax;             /* Max. value that idle ctr can take in 1 sec.     */
OS_EXT  INT32U            OSIdleCtrRun;             /* Val. reached by idle ctr at run time in 1 sec.  */
OS_EXT  BOOLEAN           OSStatRdy;                /* Flag indicating that the statistic task is rdy  */
OS_EXT  OS_STK            OSTaskStatStk[OS_TASK_STAT_STK_SIZE];      /* Statistics task stack          */
#endif
//	中断嵌套次数
OS_EXT  INT8U             OSIntNesting;             /* Interrupt nesting level                         */
//	记录中断函数退出时，最高优先级所在的组数
OS_EXT  INT8U             OSIntExitY;
//	调度器上锁的次数
OS_EXT  INT8U             OSLockNesting;            /* Multitasking lock nesting level                 */
//	当前任务的优先级
OS_EXT  INT8U             OSPrioCur;                /* Priority of current task                        */
//	优先级最高任务的优先级
OS_EXT  INT8U             OSPrioHighRdy;            /* Priority of highest priority task               */
//	就绪任务的组
OS_EXT  INT8U             OSRdyGrp;                        /* Ready list group                         */
//	就绪任务的表
OS_EXT  INT8U             OSRdyTbl[OS_RDY_TBL_SIZE];       /* Table of tasks which are ready to run    */
//	系统正在运行的标志
OS_EXT  BOOLEAN           OSRunning;                       /* Flag indicating that kernel is running   */
//	已创建的任务的数量
OS_EXT  INT8U             OSTaskCtr;                       /* Number of tasks created                  */
//	空闲任务执行次数
OS_EXT  volatile  INT32U  OSIdleCtr;                                 /* Idle counter                   */
//	空闲任务的堆栈
OS_EXT  OS_STK            OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE];      /* Idle task stack                */


OS_EXT  OS_TCB           *OSTCBCur;                        /* Pointer to currently running TCB         */
//	指向空任务控制块链表的首地址。
OS_EXT  OS_TCB           *OSTCBFreeList;                   /* Pointer to list of free TCBs             */
//	指向最高优先级的控制块
OS_EXT  OS_TCB           *OSTCBHighRdy;                    /* Pointer to highest priority TCB R-to-R   */
//	任务控制块链表，即：任务控制块都已绑定了任务
OS_EXT  OS_TCB           *OSTCBList;                       /* Pointer to doubly linked list of TCBs    */
//	以优先级为顺序，存储每一个任务控制块的首地址
OS_EXT  OS_TCB           *OSTCBPrioTbl[OS_LOWEST_PRIO + 1];/* Table of pointers to created TCBs        */
//	定义任务控制块数组，后两级包括空闲任务和统计任务。
OS_EXT  OS_TCB            OSTCBTbl[OS_MAX_TASKS + OS_N_SYS_TASKS];   /* Table of TCBs                  */

#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)
//	空闲内存分区链表
OS_EXT  OS_MEM           *OSMemFreeList;            /* Pointer to free list of memory partitions       */
//	内存分区数组
OS_EXT  OS_MEM            OSMemTbl[OS_MAX_MEM_PART];/* Storage for memory partition manager            */
#endif

#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)
//	空闲队列控制块链表
OS_EXT  OS_Q             *OSQFreeList;              /* Pointer to list of free QUEUE control blocks    */
//	队列控制块数组
OS_EXT  OS_Q              OSQTbl[OS_MAX_QS];        /* Table of QUEUE control blocks                   */
#endif

#if OS_TIME_GET_SET_EN > 0  
//	当前系统节拍总数
OS_EXT  volatile  INT32U  OSTime;                   /* Current value of system time (in ticks)         */
#endif
//	优先级 -->    位掩码
extern  INT8U  const      OSMapTbl[];               /* Priority->Bit Mask lookup table                 */
//	优先级 --> 最高优先级
//	索引表，用来指示最高优先级所在的位/组
extern  INT8U  const      OSUnMapTbl[];             /* Priority->Index    lookup table                 */

/*$PAGE*/
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                     (Target Independent Functions)
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         EVENT FLAGS MANAGEMENT
*********************************************************************************************************
*/

#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)

#if OS_FLAG_ACCEPT_EN > 0
OS_FLAGS      OSFlagAccept(OS_FLAG_GRP *pgrp, OS_FLAGS flags, INT8U wait_type, INT8U *err);
#endif

OS_FLAG_GRP  *OSFlagCreate(OS_FLAGS flags, INT8U *err);

#if OS_FLAG_DEL_EN > 0
OS_FLAG_GRP  *OSFlagDel(OS_FLAG_GRP *pgrp, INT8U opt, INT8U *err);
#endif

OS_FLAGS      OSFlagPend(OS_FLAG_GRP *pgrp, OS_FLAGS flags, INT8U wait_type, INT16U timeout, INT8U *err);
OS_FLAGS      OSFlagPost(OS_FLAG_GRP *pgrp, OS_FLAGS flags, INT8U operation, INT8U *err);

#if OS_FLAG_QUERY_EN > 0
OS_FLAGS      OSFlagQuery(OS_FLAG_GRP *pgrp, INT8U *err);
#endif
#endif

/*
*********************************************************************************************************
*                                        MESSAGE MAILBOX MANAGEMENT
*********************************************************************************************************
*/

#if OS_MBOX_EN > 0

#if OS_MBOX_ACCEPT_EN > 0
void         *OSMboxAccept(OS_EVENT *pevent);
#endif

OS_EVENT     *OSMboxCreate(void *msg);

#if OS_MBOX_DEL_EN > 0
OS_EVENT     *OSMboxDel(OS_EVENT *pevent, INT8U opt, INT8U *err);
#endif

void         *OSMboxPend(OS_EVENT *pevent, INT16U timeout, INT8U *err);

#if OS_MBOX_POST_EN > 0
INT8U         OSMboxPost(OS_EVENT *pevent, void *msg);
#endif

#if OS_MBOX_POST_OPT_EN > 0
INT8U         OSMboxPostOpt(OS_EVENT *pevent, void *msg, INT8U opt);
#endif

#if OS_MBOX_QUERY_EN > 0
INT8U         OSMboxQuery(OS_EVENT *pevent, OS_MBOX_DATA *pdata);
#endif
#endif

/*
*********************************************************************************************************
*                                           MEMORY MANAGEMENT
*********************************************************************************************************
*/

#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)

OS_MEM       *OSMemCreate(void *addr, INT32U nblks, INT32U blksize, INT8U *err);
void         *OSMemGet(OS_MEM *pmem, INT8U *err);
INT8U         OSMemPut(OS_MEM *pmem, void *pblk);

#if OS_MEM_QUERY_EN > 0
INT8U         OSMemQuery(OS_MEM *pmem, OS_MEM_DATA *pdata);
#endif

#endif

/*
*********************************************************************************************************
*                                MUTUAL EXCLUSION SEMAPHORE MANAGEMENT
*********************************************************************************************************
*/

#if OS_MUTEX_EN > 0

#if OS_MUTEX_ACCEPT_EN > 0
INT8U         OSMutexAccept(OS_EVENT *pevent, INT8U *err);
#endif

OS_EVENT     *OSMutexCreate(INT8U prio, INT8U *err);

#if OS_MUTEX_DEL_EN > 0
OS_EVENT     *OSMutexDel(OS_EVENT *pevent, INT8U opt, INT8U *err);
#endif

void          OSMutexPend(OS_EVENT *pevent, INT16U timeout, INT8U *err);
INT8U         OSMutexPost(OS_EVENT *pevent);

#if OS_MUTEX_QUERY_EN > 0
INT8U         OSMutexQuery(OS_EVENT *pevent, OS_MUTEX_DATA *pdata);
#endif

#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                         MESSAGE QUEUE MANAGEMENT
*********************************************************************************************************
*/

#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)

#if OS_Q_ACCEPT_EN > 0
void         *OSQAccept(OS_EVENT *pevent);
#endif

OS_EVENT     *OSQCreate(void **start, INT16U size);

#if OS_Q_DEL_EN > 0
OS_EVENT     *OSQDel(OS_EVENT *pevent, INT8U opt, INT8U *err);
#endif

#if OS_Q_FLUSH_EN > 0
INT8U         OSQFlush(OS_EVENT *pevent);
#endif

void         *OSQPend(OS_EVENT *pevent, INT16U timeout, INT8U *err);

#if OS_Q_POST_EN > 0
INT8U         OSQPost(OS_EVENT *pevent, void *msg);
#endif

#if OS_Q_POST_FRONT_EN > 0
INT8U         OSQPostFront(OS_EVENT *pevent, void *msg);
#endif

#if OS_Q_POST_OPT_EN > 0
INT8U         OSQPostOpt(OS_EVENT *pevent, void *msg, INT8U opt);
#endif

#if OS_Q_QUERY_EN > 0
INT8U         OSQQuery(OS_EVENT *pevent, OS_Q_DATA *pdata);
#endif

#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          SEMAPHORE MANAGEMENT
*********************************************************************************************************
*/

//ISR中不允许使用的函数如下：
//OSSemCreate、OSSemDel、OSSemPend

#if OS_SEM_EN > 0

#if OS_SEM_ACCEPT_EN > 0
INT16U        OSSemAccept(OS_EVENT *pevent);
#endif

OS_EVENT     *OSSemCreate(INT16U cnt);

#if OS_SEM_DEL_EN > 0
OS_EVENT     *OSSemDel(OS_EVENT *pevent, INT8U opt, INT8U *err);
#endif

void          OSSemPend(OS_EVENT *pevent, INT16U timeout, INT8U *err);
INT8U         OSSemPost(OS_EVENT *pevent);

#if OS_SEM_QUERY_EN > 0
INT8U         OSSemQuery(OS_EVENT *pevent, OS_SEM_DATA *pdata);
#endif

#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                            TASK MANAGEMENT
*********************************************************************************************************
*/
#if OS_TASK_CHANGE_PRIO_EN > 0
INT8U         OSTaskChangePrio(INT8U oldprio, INT8U newprio);
#endif

#if OS_TASK_CREATE_EN > 0
INT8U         OSTaskCreate(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT8U prio);
#endif

#if OS_TASK_CREATE_EXT_EN > 0
INT8U         OSTaskCreateExt(void  (*task)(void *pd),
                              void   *pdata,
                              OS_STK *ptos,
                              INT8U   prio,
                              INT16U  id,
                              OS_STK *pbos,
                              INT32U  stk_size,
                              void   *pext,
                              INT16U  opt);
#endif

#if OS_TASK_DEL_EN > 0
INT8U         OSTaskDel(INT8U prio);
INT8U         OSTaskDelReq(INT8U prio);
#endif

#if OS_TASK_SUSPEND_EN > 0
INT8U         OSTaskResume(INT8U prio);
INT8U         OSTaskSuspend(INT8U prio);
#endif

#if OS_TASK_CREATE_EXT_EN > 0
INT8U         OSTaskStkChk(INT8U prio, OS_STK_DATA *pdata);
#endif

#if OS_TASK_QUERY_EN > 0
INT8U         OSTaskQuery(INT8U prio, OS_TCB *pdata);
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                            TIME MANAGEMENT
*********************************************************************************************************
*/

void          OSTimeDly(INT16U ticks);

#if OS_TIME_DLY_HMSM_EN > 0
INT8U         OSTimeDlyHMSM(INT8U hours, INT8U minutes, INT8U seconds, INT16U milli);
#endif

#if OS_TIME_DLY_RESUME_EN > 0
INT8U         OSTimeDlyResume(INT8U prio);
#endif

#if OS_TIME_GET_SET_EN > 0
INT32U        OSTimeGet(void);
void          OSTimeSet(INT32U ticks);
#endif

void          OSTimeTick(void);

/*
*********************************************************************************************************
*                                             MISCELLANEOUS
*********************************************************************************************************
*/

void          OSInit(void);

void          OSIntEnter(void);
void          OSIntExit(void);

#if OS_SCHED_LOCK_EN > 0
void          OSSchedLock(void);
void          OSSchedUnlock(void);
#endif

void          OSStart(void);

void          OSStatInit(void);

INT16U        OSVersion(void);

/*$PAGE*/
/*
*********************************************************************************************************
*                                      INTERNAL FUNCTION PROTOTYPES
*                            (Your application MUST NOT call these functions)
*********************************************************************************************************
*/

#if OS_TASK_DEL_EN > 0
void          OS_Dummy(void);
#endif

#if ((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0) || (OS_SEM_EN > 0) || (OS_MUTEX_EN > 0)
INT8U         OS_EventTaskRdy(OS_EVENT *pevent, void *msg, INT8U msk);
void          OS_EventTaskWait(OS_EVENT *pevent);
void          OS_EventTO(OS_EVENT *pevent);
void          OS_EventWaitListInit(OS_EVENT *pevent);
#endif

#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
void          OS_FlagInit(void);
void          OS_FlagUnlink(OS_FLAG_NODE *pnode);
#endif

#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)
void          OS_MemInit(void);
#endif

#if OS_Q_EN > 0
void          OS_QInit(void);
#endif

void          OS_Sched(void);

void          OS_TaskIdle(void *data);

#if OS_TASK_STAT_EN > 0
void          OS_TaskStat(void *data);
#endif

INT8U         OS_TCBInit(INT8U prio, OS_STK *ptos, OS_STK *pbos, INT16U id, INT32U stk_size, void *pext, INT16U opt);

/*$PAGE*/
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                      (Target Specific Functions)
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void          OSInitHookBegin(void);
void          OSInitHookEnd(void);
#endif

void          OSIntCtxSw(void);

void          OSStartHighRdy(void);

void          OSTaskCreateHook(OS_TCB *ptcb);
void          OSTaskDelHook(OS_TCB *ptcb);

#if OS_VERSION >= 251
void          OSTaskIdleHook(void);
#endif

void          OSTaskStatHook(void);
OS_STK       *OSTaskStkInit(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT16U opt);
void          OSTaskSwHook(void);

#if OS_VERSION >= 204
void          OSTCBInitHook(OS_TCB *ptcb);
#endif

void          OSTimeTickHook(void);

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                  (Compiler Specific ISR prototypes)
*********************************************************************************************************
*/

#ifndef OS_ISR_PROTO_EXT
void          OSCtxSw(void);
void          OSTickISR(void);
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                   LOOK FOR MISSING #define CONSTANTS
*
* This section is used to generate ERROR messages at compile time if certain #define constants are 
* MISSING in OS_CFG.H.  This allows you to quickly determine the source of the error.
*
* You SHOULD NOT change this section UNLESS you would like to add more comments as to the source of the
* compile time error.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            EVENT FLAGS
*********************************************************************************************************
*/

#ifndef OS_FLAG_EN
#error  "OS_CFG.H, Missing OS_FLAG_EN: Enable (1) or Disable (0) code generation for Event Flags"
#else
    #ifndef OS_MAX_FLAGS
    #error  "OS_CFG.H, Missing OS_MAX_FLAGS: Max. number of Event Flag Groups in your application"
    #else
        #if     OS_MAX_FLAGS == 0
        #error  "OS_CFG.H, OS_MAX_FLAGS must be > 0"
        #endif
        #if     OS_MAX_FLAGS > 255
        #error  "OS_CFG.H, OS_MAX_FLAGS must be <= 255"
        #endif
    #endif

    #ifndef OS_FLAG_WAIT_CLR_EN
    #error  "OS_CFG.H, Missing OS_FLAG_WAIT_CLR_EN: Include code for Wait on Clear EVENT FLAGS"
    #endif

    #ifndef OS_FLAG_ACCEPT_EN
    #error  "OS_CFG.H, Missing OS_FLAG_ACCEPT_EN: Include code for OSFlagAccept()"
    #endif

    #ifndef OS_FLAG_DEL_EN
    #error  "OS_CFG.H, Missing OS_FLAG_DEL_EN: Include code for OSFlagDel()"
    #endif

    #ifndef OS_FLAG_QUERY_EN
    #error  "OS_CFG.H, Missing OS_FLAG_QUERY_EN: Include code for OSFlagQuery()"
    #endif
#endif

/*
*********************************************************************************************************
*                                           MESSAGE MAILBOXES
*********************************************************************************************************
*/

#ifndef OS_MBOX_EN
#error  "OS_CFG.H, Missing OS_MBOX_EN: Enable (1) or Disable (0) code generation for MAILBOXES"
#else
    #ifndef OS_MBOX_ACCEPT_EN
    #error  "OS_CFG.H, Missing OS_MBOX_ACCEPT_EN: Include code for OSMboxAccept()"
    #endif

    #ifndef OS_MBOX_DEL_EN
    #error  "OS_CFG.H, Missing OS_MBOX_DEL_EN: Include code for OSMboxDel()"
    #endif

    #ifndef OS_MBOX_POST_EN
    #error  "OS_CFG.H, Missing OS_MBOX_POST_EN: Include code for OSMboxPost()"
    #endif

    #ifndef OS_MBOX_POST_OPT_EN
    #error  "OS_CFG.H, Missing OS_MBOX_POST_OPT_EN: Include code for OSMboxPostOpt()"
    #endif

    #ifndef OS_MBOX_QUERY_EN
    #error  "OS_CFG.H, Missing OS_MBOX_QUERY_EN: Include code for OSMboxQuery()"
    #endif
#endif

/*
*********************************************************************************************************
*                                           MEMORY MANAGEMENT
*********************************************************************************************************
*/

#ifndef OS_MEM_EN
#error  "OS_CFG.H, Missing OS_MEM_EN: Enable (1) or Disable (0) code generation for MEMORY MANAGER"
#else
    #ifndef OS_MAX_MEM_PART
    #error  "OS_CFG.H, Missing OS_MAX_MEM_PART: Max. number of memory partitions"
    #else
        #if     OS_MAX_MEM_PART == 0
        #error  "OS_CFG.H, OS_MAX_MEM_PART must be > 0"
        #endif
        #if     OS_MAX_MEM_PART > 255
        #error  "OS_CFG.H, OS_MAX_MEM_PART must be <= 255"
        #endif
    #endif

    #ifndef OS_MEM_QUERY_EN
    #error  "OS_CFG.H, Missing OS_MEM_QUERY_EN: Include code for OSMemQuery()"
    #endif
#endif

/*
*********************************************************************************************************
*                                       MUTUAL EXCLUSION SEMAPHORES
*********************************************************************************************************
*/

#ifndef OS_MUTEX_EN
#error  "OS_CFG.H, Missing OS_MUTEX_EN: Enable (1) or Disable (0) code generation for MUTEX"
#else
    #ifndef OS_MUTEX_ACCEPT_EN
    #error  "OS_CFG.H, Missing OS_MUTEX_ACCEPT_EN: Include code for OSMutexAccept()"
    #endif

    #ifndef OS_MUTEX_DEL_EN
    #error  "OS_CFG.H, Missing OS_MUTEX_DEL_EN: Include code for OSMutexDel()"
    #endif

    #ifndef OS_MUTEX_QUERY_EN
    #error  "OS_CFG.H, Missing OS_MUTEX_QUERY_EN: Include code for OSMutexQuery()"
    #endif
#endif

/*
*********************************************************************************************************
*                                              MESSAGE QUEUES
*********************************************************************************************************
*/

#ifndef OS_Q_EN
#error  "OS_CFG.H, Missing OS_Q_EN: Enable (1) or Disable (0) code generation for QUEUES"
#else
    #ifndef OS_MAX_QS
    #error  "OS_CFG.H, Missing OS_MAX_QS: Max. number of queue control blocks"
    #else
        #if     OS_MAX_QS == 0
        #error  "OS_CFG.H, OS_MAX_QS must be > 0"
        #endif
        #if     OS_MAX_QS > 255
        #error  "OS_CFG.H, OS_MAX_QS must be <= 255"
        #endif
    #endif

    #ifndef OS_Q_ACCEPT_EN
    #error  "OS_CFG.H, Missing OS_Q_ACCEPT_EN: Include code for OSQAccept()"
    #endif

    #ifndef OS_Q_DEL_EN
    #error  "OS_CFG.H, Missing OS_Q_DEL_EN: Include code for OSQDel()"
    #endif

    #ifndef OS_Q_FLUSH_EN
    #error  "OS_CFG.H, Missing OS_Q_FLUSH_EN: Include code for OSQFlush()"
    #endif

    #ifndef OS_Q_POST_EN
    #error  "OS_CFG.H, Missing OS_Q_POST_EN: Include code for OSQPost()"
    #endif

    #ifndef OS_Q_POST_FRONT_EN
    #error  "OS_CFG.H, Missing OS_Q_POST_FRONT_EN: Include code for OSQPostFront()"
    #endif

    #ifndef OS_Q_POST_OPT_EN
    #error  "OS_CFG.H, Missing OS_Q_POST_OPT_EN: Include code for OSQPostOpt()"
    #endif

    #ifndef OS_Q_QUERY_EN
    #error  "OS_CFG.H, Missing OS_Q_QUERY_EN: Include code for OSQQuery()"
    #endif
#endif

/*
*********************************************************************************************************
*                                              SEMAPHORES
*********************************************************************************************************
*/

#ifndef OS_SEM_EN
#error  "OS_CFG.H, Missing OS_SEM_EN: Enable (1) or Disable (0) code generation for SEMAPHORES"
#else
    #ifndef OS_SEM_ACCEPT_EN
    #error  "OS_CFG.H, Missing OS_SEM_ACCEPT_EN: Include code for OSSemAccept()"
    #endif

    #ifndef OS_SEM_DEL_EN
    #error  "OS_CFG.H, Missing OS_SEM_DEL_EN: Include code for OSSemDel()"
    #endif

    #ifndef OS_SEM_QUERY_EN
    #error  "OS_CFG.H, Missing OS_SEM_QUERY_EN: Include code for OSSemQuery()"
    #endif
#endif

/*
*********************************************************************************************************
*                                             TASK MANAGEMENT
*********************************************************************************************************
*/

#ifndef OS_MAX_TASKS
#error  "OS_CFG.H, Missing OS_MAX_TASKS: Max. number of tasks in your application"
#else
    #if     OS_MAX_TASKS == 0
    #error  "OS_CFG.H,         OS_MAX_TASKS must be >= 2"
    #endif
    #if     OS_MAX_TASKS > 63
    #error  "OS_CFG.H,         OS_MAX_TASKS must be <= 63"
    #endif
#endif

#ifndef OS_TASK_IDLE_STK_SIZE
#error  "OS_CFG.H, Missing OS_TASK_IDLE_STK_SIZE: Idle task stack size"
#endif

#ifndef OS_TASK_STAT_EN
#error  "OS_CFG.H, Missing OS_TASK_STAT_EN: Enable (1) or Disable(0) the statistics task"
#endif

#ifndef OS_TASK_STAT_STK_SIZE
#error  "OS_CFG.H, Missing OS_TASK_STAT_STK_SIZE: Statistics task stack size"
#endif

#ifndef OS_TASK_CHANGE_PRIO_EN
#error  "OS_CFG.H, Missing OS_TASK_CHANGE_PRIO_EN: Include code for OSTaskChangePrio()"
#endif

#ifndef OS_TASK_CREATE_EN
#error  "OS_CFG.H, Missing OS_TASK_CREATE_EN: Include code for OSTaskCreate()"
#endif

#ifndef OS_TASK_CREATE_EXT_EN
#error  "OS_CFG.H, Missing OS_TASK_CREATE_EXT_EN: Include code for OSTaskCreateExt()"
#endif

#ifndef OS_TASK_DEL_EN
#error  "OS_CFG.H, Missing OS_TASK_DEL_EN: Include code for OSTaskDel()"
#endif

#ifndef OS_TASK_SUSPEND_EN
#error  "OS_CFG.H, Missing OS_TASK_SUSPEND_EN: Include code for OSTaskSuspend() and OSTaskResume()"
#endif

#ifndef OS_TASK_QUERY_EN
#error  "OS_CFG.H, Missing OS_TASK_QUERY_EN: Include code for OSTaskQuery()"
#endif

/*
*********************************************************************************************************
*                                             TIME MANAGEMENT
*********************************************************************************************************
*/

#ifndef OS_TICKS_PER_SEC
#error  "OS_CFG.H, Missing OS_TICKS_PER_SEC: Sets the number of ticks in one second"
#endif

#ifndef OS_TIME_DLY_HMSM_EN
#error  "OS_CFG.H, Missing OS_TIME_DLY_HMSM_EN: Include code for OSTimeDlyHMSM()"
#endif

#ifndef OS_TIME_DLY_RESUME_EN
#error  "OS_CFG.H, Missing OS_TIME_DLY_RESUME_EN: Include code for OSTimeDlyResume()"
#endif

#ifndef OS_TIME_GET_SET_EN
#error  "OS_CFG.H, Missing OS_TIME_GET_SET_EN: Include code for OSTimeGet() and OSTimeSet()"
#endif

/*
*********************************************************************************************************
*                                            MISCELLANEOUS
*********************************************************************************************************
*/

#ifndef OS_MAX_EVENTS
#error  "OS_CFG.H, Missing OS_MAX_EVENTS: Max. number of event control blocks in your application"
#else
    #if     OS_MAX_EVENTS == 0
    #error  "OS_CFG.H, OS_MAX_EVENTS must be > 0"
    #endif
    #if     OS_MAX_EVENTS > 255
    #error  "OS_CFG.H, OS_MAX_EVENTS must be <= 255"
    #endif
#endif

#ifndef OS_LOWEST_PRIO
#error  "OS_CFG.H, Missing OS_LOWEST_PRIO: Defines the lowest priority that can be assigned"
#endif

#ifndef OS_ARG_CHK_EN
#error  "OS_CFG.H, Missing OS_ARG_CHK_EN: Enable (1) or Disable (0) argument checking"
#endif

#ifndef OS_CPU_HOOKS_EN
#error  "OS_CFG.H, Missing OS_CPU_HOOKS_EN: uC/OS-II hooks are found in the processor port files when 1"
#endif

#ifndef OS_SCHED_LOCK_EN
#error  "OS_CFG.H, Missing OS_SCHED_LOCK_EN: Include code for OSSchedLock() and OSSchedUnlock()"
#endif

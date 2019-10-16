/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                             CORE FUNCTIONS
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : OS_CORE.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE
#define  OS_GLOBALS
#include "includes.h"
#endif

/*
*********************************************************************************************************
*                              MAPPING TABLE TO MAP BIT POSITION TO BIT MASK
*
* Note: Index into table is desired bit position, 0..7
*       Indexed value corresponds to bit mask
*********************************************************************************************************
*/

INT8U  const  OSMapTbl[]   = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

/*
*********************************************************************************************************
*                                       PRIORITY RESOLUTION TABLE
*
* Note: Index into table is bit pattern to resolve highest priority
*       Indexed value corresponds to highest priority bit position (i.e. 0..7)
*********************************************************************************************************
*/
//	索引表，用来指示最高优先级所在的位/组
INT8U  const  OSUnMapTbl[] = {
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x00 to 0x0F                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x10 to 0x1F                             */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x20 to 0x2F                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x30 to 0x3F                             */
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x40 to 0x4F                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x50 to 0x5F                             */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x60 to 0x6F                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x70 to 0x7F                             */
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x80 to 0x8F                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0x90 to 0x9F                             */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xA0 to 0xAF                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xB0 to 0xBF                             */
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xC0 to 0xCF                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xD0 to 0xDF                             */
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,       /* 0xE0 to 0xEF                             */
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0        /* 0xF0 to 0xFF                             */
};

/*
*********************************************************************************************************
*                                       FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void  OS_InitEventList(void);
static  void  OS_InitMisc(void);
static  void  OS_InitRdyList(void);
static  void  OS_InitTaskIdle(void);
static  void  OS_InitTaskStat(void);
static  void  OS_InitTCBList(void);

/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*
* Description: This function is used to initialize the internals of uC/OS-II and MUST be called prior to
*              creating any uC/OS-II object and, prior to calling OSStart().
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/
//	系统初始化
void  OSInit (void)
{
#if OS_VERSION >= 204
//	钩子函数，调用用户特定的初始化代码（通过一个接口函数实现用户要求的插件式进入系统中）（初始化开始）
    OSInitHookBegin();                                           /* Call port specific initialization code   */
#endif
//	初始化杂项变量
    OS_InitMisc();                                               /* Initialize miscellaneous variables       */
//	初始化任务就绪表
    OS_InitRdyList();                                            /* Initialize the Ready List                */
//	初始化空任务控制块链表
	OS_InitTCBList();                                            /* Initialize the free list of OS_TCBs      */
//	初始化空闲的事件控制块链表
	OS_InitEventList();                                          /* Initialize the free list of OS_EVENTs    */

#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
//	初始化空闲信号量标志组链表
	OS_FlagInit();                                               /* Initialize the event flag structures     */
#endif

#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)
//	初始化内存管理器
    OS_MemInit();                                                /* Initialize the memory manager            */
#endif

#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)
//	初始化消息队列
    OS_QInit();                                                  /* Initialize the message queue structures  */
#endif
//	初始化空闲任务：创建空闲任务OS_TaskIdle
    OS_InitTaskIdle();                                           /* Create the Idle Task                     */
#if OS_TASK_STAT_EN > 0
//	初始化统计任务：创建统计任务OS_TaskStat
    OS_InitTaskStat();                                           /* Create the Statistic Task                */
#endif

#if OS_VERSION >= 204
//	钩子函数，调用用户特定的初始化代码（通过一个接口函数实现用户要求的插件式进入系统中）（初始化结束）
    OSInitHookEnd();                                             /* Call port specific init. code            */
#endif
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                              ENTER ISR
*
* Description: This function is used to notify uC/OS-II that you are about to service an interrupt
*              service routine (ISR).  This allows uC/OS-II to keep track of interrupt nesting and thus
*              only perform rescheduling at the last nested ISR.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) This function should be called ith interrupts already disabled
*              2) Your ISR can directly increment OSIntNesting without calling this function because
*                 OSIntNesting has been declared 'global'.  
*              3) You MUST still call OSIntExit() even though you increment OSIntNesting directly.
*              4) You MUST invoke OSIntEnter() and OSIntExit() in pair.  In other words, for every call
*                 to OSIntEnter() at the beginning of the ISR you MUST have a call to OSIntExit() at the
*                 end of the ISR.
*              5) You are allowed to nest interrupts up to 255 levels deep.
*              6) I removed the OS_ENTER_CRITICAL() and OS_EXIT_CRITICAL() around the increment because
*                 OSIntEnter() is always called with interrupts disabled.
*********************************************************************************************************
*/
//	中断嵌套层数 + 1
void  OSIntEnter (void)
{
    if (OSRunning == TRUE) 
	{
        if (OSIntNesting < 255) 
		{
            OSIntNesting++;                      /* Increment ISR nesting level                        */
        }
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                               EXIT ISR
*
* Description: This function is used to notify uC/OS-II that you have completed serviving an ISR.  When
*              the last nested ISR has completed, uC/OS-II will call the scheduler to determine whether
*              a new, high-priority task, is ready to run.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) You MUST invoke OSIntEnter() and OSIntExit() in pair.  In other words, for every call
*                 to OSIntEnter() at the beginning of the ISR you MUST have a call to OSIntExit() at the
*                 end of the ISR.
*              2) Rescheduling is prevented when the scheduler is locked (see OS_SchedLock())
*********************************************************************************************************
*/
//	记录完成的ISR的数量，并判断是否需要进行任务调度。
void  OSIntExit (void)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    
//	系统正在运行
    if (OSRunning == TRUE) 
	{
        OS_ENTER_CRITICAL();
	//	中断嵌套层数 - 1
        if (OSIntNesting > 0) 
		{                            /* Prevent OSIntNesting from wrapping       */
            OSIntNesting--;
        }
	//	若没有ISR运行，并且可以执行调度器，则进行任务切换。
        if ((OSIntNesting == 0) && (OSLockNesting == 0)) 
		{ /* Reschedule only if all ISRs complete ... */
            OSIntExitY    = OSUnMapTbl[OSRdyGrp];          /* ... and not locked.                      */
            OSPrioHighRdy = (INT8U)((OSIntExitY << 3) + OSUnMapTbl[OSRdyTbl[OSIntExitY]]);
            if (OSPrioHighRdy != OSPrioCur) 
			{              /* No Ctx Sw if current task is highest rdy */
                OSTCBHighRdy  = OSTCBPrioTbl[OSPrioHighRdy];
                OSCtxSwCtr++;                              /* Keep track of the number of ctx switches */
                OSIntCtxSw();                              /* Perform interrupt level ctx switch       */
            }
        }
        OS_EXIT_CRITICAL();
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          PREVENT SCHEDULING
*
* Description: This function is used to prevent rescheduling to take place.  This allows your application
*              to prevent context switches until you are ready to permit context switching.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) You MUST invoke OSSchedLock() and OSSchedUnlock() in pair.  In other words, for every
*                 call to OSSchedLock() you MUST have a call to OSSchedUnlock().
*********************************************************************************************************
*/

#if OS_SCHED_LOCK_EN > 0
//	调度器上锁，使调用者能够保持对CPU的使用权。
void  OSSchedLock (void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    
//	OS已运行
    if (OSRunning == TRUE) 
	{                     /* Make sure multitasking is running                  */
        OS_ENTER_CRITICAL();
	//	调度器嵌套次数 + 1
        if (OSLockNesting < 255) 
		{               /* Prevent OSLockNesting from wrapping back to 0      */
            OSLockNesting++;                     /* Increment lock nesting level                       */
        }
        OS_EXIT_CRITICAL();
    }
}
#endif    

/*$PAGE*/
/*
*********************************************************************************************************
*                                          ENABLE SCHEDULING
*
* Description: This function is used to re-allow rescheduling.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) You MUST invoke OSSchedLock() and OSSchedUnlock() in pair.  In other words, for every
*                 call to OSSchedLock() you MUST have a call to OSSchedUnlock().
*********************************************************************************************************
*/

#if OS_SCHED_LOCK_EN > 0
//	调度器解锁
void  OSSchedUnlock (void)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif    
    
    
    if (OSRunning == TRUE) 
	{                                   /* Make sure multitasking is running    */
        OS_ENTER_CRITICAL();
        if (OSLockNesting > 0) 
		{                               /* Do not decrement if already 0        */
            OSLockNesting--;                                   /* Decrement lock nesting level         */
		//	调度器嵌套层数 - 1，并且没有中断函数运行，则进行任务切换。
			if ((OSLockNesting == 0) && (OSIntNesting == 0)) 
			{ /* See if sched. enabled and not an ISR */
                OS_EXIT_CRITICAL();
                OS_Sched();                                    /* See if a HPT is ready                */
            } 
			else 
			{
                OS_EXIT_CRITICAL();
            }
        } 
		else 
		{
            OS_EXIT_CRITICAL();
        }
    }
}
#endif    

/*$PAGE*/
/*
*********************************************************************************************************
*                                          START MULTITASKING
*
* Description: This function is used to start the multitasking process which lets uC/OS-II manages the
*              task that you have created.  Before you can call OSStart(), you MUST have called OSInit()
*              and you MUST have created at least one task.
*
* Arguments  : none
*
* Returns    : none
*
* Note       : OSStartHighRdy() MUST:
*                 a) Call OSTaskSwHook() then,
*                 b) Set OSRunning to TRUE.
*                 c) Load the context of the task pointed to by OSTCBHighRdy.
*                 d_ Execute the task.
*********************************************************************************************************
*/

void  OSStart (void)
{
    INT8U y;
    INT8U x;

//	开始运行OS
    if (OSRunning == FALSE) 
	{
	//	找到最高优先级的任务
        y             = OSUnMapTbl[OSRdyGrp];        /* Find highest priority's task priority number   */
        x             = OSUnMapTbl[OSRdyTbl[y]];
        OSPrioHighRdy = (INT8U)((y << 3) + x);
        OSPrioCur     = OSPrioHighRdy;
        OSTCBHighRdy  = OSTCBPrioTbl[OSPrioHighRdy]; /* Point to highest priority task ready to run    */
        OSTCBCur      = OSTCBHighRdy;
	//	执行制定优先级的任务。
		OSStartHighRdy();                            /* Execute target specific code to start task     */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        STATISTICS INITIALIZATION
*
* Description: This function is called by your application to establish CPU usage by first determining
*              how high a 32-bit counter would count to in 1 second if no other tasks were to execute
*              during that time.  CPU usage is then determined by a low priority task which keeps track
*              of this 32-bit counter every second but this time, with other tasks running.  CPU usage is
*              determined by:
*
*                                             OSIdleCtr
*                 CPU Usage (%) = 100 * (1 - ------------)
*                                            OSIdleCtrMax
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

#if OS_TASK_STAT_EN > 0
void  OSStatInit (void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    
    
    OSTimeDly(2);                                /* Synchronize with clock tick                        */
    OS_ENTER_CRITICAL();
    OSIdleCtr    = 0L;                           /* Clear idle counter                                 */
    OS_EXIT_CRITICAL();
    OSTimeDly(OS_TICKS_PER_SEC);                 /* Determine MAX. idle counter value for 1 second     */
    OS_ENTER_CRITICAL();
    OSIdleCtrMax = OSIdleCtr;                    /* Store maximum idle counter count in 1 second       */
    OSStatRdy    = TRUE;
    OS_EXIT_CRITICAL();
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                         PROCESS SYSTEM TICK
*
* Description: This function is used to signal to uC/OS-II the occurrence of a 'system tick' (also known
*              as a 'clock tick').  This function should be called by the ticker ISR but, can also be
*              called by a high priority task.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/
//函数功能：
//1、统计系统节拍数。
//2、对每一个带延时的任务进行延时减一。
//3、对延时减一的任务进行判断，若该任务不再挂起，则登记再任务就绪表中。
void  OSTimeTick (void)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif    
//	临时存放任务块的指针
    OS_TCB    *ptcb;


    OSTimeTickHook();                                      /* Call user definable hook                 */
#if OS_TIME_GET_SET_EN > 0   
    OS_ENTER_CRITICAL();                                   /* Update the 32-bit tick counter           */
//	更新系统节拍总数
    OSTime++;
    OS_EXIT_CRITICAL();
#endif
//	保证OS已经运行
    if (OSRunning == TRUE) 
	{    
	//	从任务块链表中获得首个任务块
        ptcb = OSTCBList;                                  /* Point at first TCB in TCB list           */
	//	遍历所有的用户任务
        while (ptcb->OSTCBPrio != OS_IDLE_PRIO) 
		{          /* Go through all TCBs in TCB list          */
            OS_ENTER_CRITICAL();
		//	若任务延时为0，则不再进行处理
            if (ptcb->OSTCBDly != 0) 
			{                     /* Delayed or waiting for event with TO     */
			//	延时-1，再判断其是否为0
                if (--ptcb->OSTCBDly == 0) 
				{               /* Decrement nbr of ticks to end of delay   */
				//	延时为0的情况下，在判断该任务是否已经挂起，若挂起则延迟一个周期再判断该状态
				//	若不再挂起，则在任务就绪表中注册该该任务
                    if ((ptcb->OSTCBStat & OS_STAT_SUSPEND) == OS_STAT_RDY) 
					{ /* Is task suspended?    */
                        OSRdyGrp               |= ptcb->OSTCBBitY; /* No,  Make task R-to-R (timed out)*/
                        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
                    }
					else 
					{                               /* Yes, Leave 1 tick to prevent ...         */
                        ptcb->OSTCBDly = 1;                /* ... loosing the task when the ...        */
                    }                                      /* ... suspension is removed.               */
                }
            }
		//	下一个任务
            ptcb = ptcb->OSTCBNext;                        /* Point at next TCB in TCB list            */
            OS_EXIT_CRITICAL();
        }
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                             GET VERSION
*
* Description: This function is used to return the version number of uC/OS-II.  The returned value
*              corresponds to uC/OS-II's version number multiplied by 100.  In other words, version 2.00
*              would be returned as 200.
*
* Arguments  : none
*
* Returns    : the version number of uC/OS-II multiplied by 100.
*********************************************************************************************************
*/
//	返回当前OS的版本
INT16U  OSVersion (void)
{
    return (OS_VERSION);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                            DUMMY FUNCTION
*
* Description: This function doesn't do anything.  It is called by OSTaskDel().
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

#if OS_TASK_DEL_EN > 0
void  OS_Dummy (void)
{
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                             MAKE TASK READY TO RUN BASED ON EVENT OCCURING
*
* Description: This function is called by other uC/OS-II services and is used to ready a task that was
*              waiting for an event to occur.
*
* Arguments  : pevent    is a pointer to the event control block corresponding to the event.
*
*              msg       is a pointer to a message.  This pointer is used by message oriented services
*                        such as MAILBOXEs and QUEUEs.  The pointer is not used when called by other
*                        service functions.
*
*              msk       is a mask that is used to clear the status byte of the TCB.  For example,
*                        OSSemPost() will pass OS_STAT_SEM, OSMboxPost() will pass OS_STAT_MBOX etc.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
#if OS_EVENT_EN > 0
//	将等待时间的任务置为就绪
INT8U  OS_EventTaskRdy (OS_EVENT *pevent, void *msg, INT8U msk)
{
    OS_TCB *ptcb;
    INT8U   x;
    INT8U   y;
    INT8U   bitx;
    INT8U   bity;
    INT8U   prio;

//	获得最高优先级所在的组
    y    = OSUnMapTbl[pevent->OSEventGrp];            /* Find highest prio. task waiting for message   */
//	组转换为位掩码
	bity = OSMapTbl[y];
//	获得组内最高优先级的数值
    x    = OSUnMapTbl[pevent->OSEventTbl[y]];
//	将组内最高优先级的数值转换为位掩码
    bitx = OSMapTbl[x];
//	计算最高优先级数
    prio = (INT8U)((y << 3) + x);                     /* Find priority of task getting the msg         */
//	将prio对应的任务从等待任务表中删除
	if ((pevent->OSEventTbl[y] &= ~bitx) == 0x00) 
	{   /* Remove this task from the waiting list        */
        pevent->OSEventGrp &= ~bity;                  /* Clr group bit if this was only task pending   */
    }
//	获得prio对应的任务控制块
    ptcb                 =  OSTCBPrioTbl[prio];       /* Point to this task's OS_TCB                   */
//	将任务的等待时间清零
	ptcb->OSTCBDly       =  0;                        /* Prevent OSTimeTick() from readying task       */
//	与事件控制块断开连接
	ptcb->OSTCBEventPtr  = (OS_EVENT *)0;             /* Unlink ECB from this task                     */
#if ((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0)
//	向等待的任务发送消息
	ptcb->OSTCBMsg       = msg;                       /* Send message directly to waiting task         */
#else
    msg                  = msg;                       /* Prevent compiler warning if not used          */
#endif
//	消除等待事件（信号量、互斥量等）的状态标志
    ptcb->OSTCBStat     &= ~msk;                      /* Clear bit associated with event type          */
//	若任务状态为就绪状态，则置位任务就绪表
	if (ptcb->OSTCBStat == OS_STAT_RDY) 
	{             /* See if task is ready (could be susp'd)        */
        OSRdyGrp        |=  bity;                     /* Put task in the ready to run list             */
        OSRdyTbl[y]     |=  bitx;
    }
//	返回任务的优先级
    return (prio);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                   MAKE TASK WAIT FOR EVENT TO OCCUR
*
* Description: This function is called by other uC/OS-II services to suspend a task because an event has
*              not occurred.
*
* Arguments  : pevent   is a pointer to the event control block for which the task will be waiting for.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
#if OS_EVENT_EN > 0
void  OS_EventTaskWait (OS_EVENT *pevent)
{
//	绑定任务控制块和事件控制块
    OSTCBCur->OSTCBEventPtr = pevent;            /* Store pointer to event control block in TCB        */
//	将任务控制块从任务就绪表中删除
	if ((OSRdyTbl[OSTCBCur->OSTCBY] &= ~OSTCBCur->OSTCBBitX) == 0x00) 
	{   /* Task no longer ready      */
        OSRdyGrp &= ~OSTCBCur->OSTCBBitY;        /* Clear event grp bit if this was only task pending  */
    }
//	在事件控制块的等待任务表中注册该任务
    pevent->OSEventTbl[OSTCBCur->OSTCBY] |= OSTCBCur->OSTCBBitX;          /* Put task in waiting list  */
    pevent->OSEventGrp                   |= OSTCBCur->OSTCBBitY;
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                              MAKE TASK READY TO RUN BASED ON EVENT TIMEOUT
*
* Description: This function is called by other uC/OS-II services to make a task ready to run because a
*              timeout occurred.
*
* Arguments  : pevent   is a pointer to the event control block which is readying a task.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
#if OS_EVENT_EN > 0
//	当任务等待事件超时时，调用该函数将该任务置位就绪
void  OS_EventTO (OS_EVENT *pevent)
{
//	在事件等待任务表中删除该任务
    if ((pevent->OSEventTbl[OSTCBCur->OSTCBY] &= ~OSTCBCur->OSTCBBitX) == 0x00) 
	{
        pevent->OSEventGrp &= ~OSTCBCur->OSTCBBitY;
    }
//	任务的状态置为就绪
    OSTCBCur->OSTCBStat     = OS_STAT_RDY;       /* Set status to ready                                */
//	解绑任务控制块和事件控制块
	OSTCBCur->OSTCBEventPtr = (OS_EVENT *)0;     /* No longer waiting for event                        */
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                 INITIALIZE EVENT CONTROL BLOCK'S WAIT LIST
*
* Description: This function is called by other uC/OS-II services to initialize the event wait list.
*
* Arguments  : pevent    is a pointer to the event control block allocated to the event.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
#if ((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0) || (OS_SEM_EN > 0) || (OS_MUTEX_EN > 0)
//	初始化事件控制块中的任务等待列表
void  OS_EventWaitListInit (OS_EVENT *pevent)
{
    INT8U  *ptbl;

//	清零OSEventGrp，即：任务组
    pevent->OSEventGrp = 0x00;                   /* No task waiting on event                           */
//	清零等待任务表
	ptbl               = &pevent->OSEventTbl[0];

#if OS_EVENT_TBL_SIZE > 0
    *ptbl++            = 0x00;
#endif

#if OS_EVENT_TBL_SIZE > 1
    *ptbl++            = 0x00;
#endif

#if OS_EVENT_TBL_SIZE > 2
    *ptbl++            = 0x00;
#endif

#if OS_EVENT_TBL_SIZE > 3
    *ptbl++            = 0x00;
#endif

#if OS_EVENT_TBL_SIZE > 4
    *ptbl++            = 0x00;
#endif

#if OS_EVENT_TBL_SIZE > 5
    *ptbl++            = 0x00;
#endif

#if OS_EVENT_TBL_SIZE > 6
    *ptbl++            = 0x00;
#endif

#if OS_EVENT_TBL_SIZE > 7
    *ptbl              = 0x00;
#endif
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                           INITIALIZE THE FREE LIST OF EVENT CONTROL BLOCKS
*
* Description: This function is called by OSInit() to initialize the free list of event control blocks.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/
//	初始化空闲事件等待链表
static  void  OS_InitEventList (void)
{
#if (OS_EVENT_EN > 0) && (OS_MAX_EVENTS > 0)
//	事件控制块的数量 > 1
#if (OS_MAX_EVENTS > 1)
    INT16U     i;
    OS_EVENT  *pevent1;
    OS_EVENT  *pevent2;


    pevent1 = &OSEventTbl[0];
    pevent2 = &OSEventTbl[1];
//	建立并初始化空事件控制块链表
    for (i = 0; i < (OS_MAX_EVENTS - 1); i++) 
	{                  /* Init. list of free EVENT control blocks  */
        pevent1->OSEventType = OS_EVENT_TYPE_UNUSED;
        pevent1->OSEventPtr  = pevent2;
        pevent1++;
        pevent2++;
    }
    pevent1->OSEventType = OS_EVENT_TYPE_UNUSED;
    pevent1->OSEventPtr  = (OS_EVENT *)0;
//	将OSEventFreeList指向空事件控制块链表的首地址。
    OSEventFreeList      = &OSEventTbl[0];
#else
//	将OSEventFreeList指向空事件控制块链表的首地址。
    OSEventFreeList              = &OSEventTbl[0];               /* Only have ONE event control block        */
    OSEventFreeList->OSEventType = OS_EVENT_TYPE_UNUSED;
    OSEventFreeList->OSEventPtr  = (OS_EVENT *)0;
#endif
#endif
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                    INITIALIZE MISCELLANEOUS VARIABLES
*
* Description: This function is called by OSInit() to initialize miscellaneous variables.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

static  void  OS_InitMisc (void)
{
#if OS_TIME_GET_SET_EN > 0   
    OSTime        = 0L;                                          /* Clear the 32-bit system clock            */
#endif
//	清零中断嵌套层数
    OSIntNesting  = 0;                                           /* Clear the interrupt nesting counter      */
//	清零临界代码段嵌套次数
	OSLockNesting = 0;                                           /* Clear the scheduling lock counter        */
//	清零当前任务数量
    OSTaskCtr     = 0;                                           /* Clear the number of tasks                */
//	指明当前操作系统没有运行。
    OSRunning     = FALSE;                                       /* Indicate that multitasking not started   */
//	清零上下文切换的次数
    OSCtxSwCtr    = 0;                                           /* Clear the context switch counter         */
//	清零空闲任务执行的次数。
	OSIdleCtr     = 0L;                                          /* Clear the 32-bit idle counter            */

#if (OS_TASK_STAT_EN > 0) && (OS_TASK_CREATE_EXT_EN > 0)
    OSIdleCtrRun  = 0L;
    OSIdleCtrMax  = 0L;
//	统计任务没有准备就绪
    OSStatRdy     = FALSE;                                       /* Statistic task is not ready              */
#endif
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                       INITIALIZE THE READY LIST
*
* Description: This function is called by OSInit() to initialize the Ready List.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

static  void  OS_InitRdyList (void)
{
    INT16U   i;
    INT8U   *prdytbl;

//	就绪任务组
    OSRdyGrp      = 0x00;                                        /* Clear the ready list                     */
//	任务就绪表的首地址。
	prdytbl       = &OSRdyTbl[0];
//	清零每一个任务就绪表的成员。
    for (i = 0; i < OS_RDY_TBL_SIZE; i++) 
	{
        *prdytbl++ = 0x00;
    }
//	当前任务的优先级
    OSPrioCur     = 0;
//	清零最高优先级任务的优先级
    OSPrioHighRdy = 0;
//	清零指向优先级最高的任务的控制块的指针
    OSTCBHighRdy  = (OS_TCB *)0;  
//	清零指向当前任务的控制块的指针
    OSTCBCur      = (OS_TCB *)0;
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                         CREATING THE IDLE TASK
*
* Description: This function creates the Idle Task.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

static  void  OS_InitTaskIdle (void)
{
#if OS_TASK_CREATE_EXT_EN > 0
    #if OS_STK_GROWTH == 1
    (void)OSTaskCreateExt(OS_TaskIdle,
                          (void *)0,                                 /* No arguments passed to OS_TaskIdle() */
                          &OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE - 1], /* Set Top-Of-Stack                     */
                          OS_IDLE_PRIO,                              /* Lowest priority level                */
                          OS_TASK_IDLE_ID,
                          &OSTaskIdleStk[0],                         /* Set Bottom-Of-Stack                  */
                          OS_TASK_IDLE_STK_SIZE,
                          (void *)0,                                 /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
    #else
    (void)OSTaskCreateExt(OS_TaskIdle,
                          (void *)0,                                 /* No arguments passed to OS_TaskIdle() */
                          &OSTaskIdleStk[0],                         /* Set Top-Of-Stack                     */
                          OS_IDLE_PRIO,                              /* Lowest priority level                */
                          OS_TASK_IDLE_ID,
                          &OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE - 1], /* Set Bottom-Of-Stack                  */
                          OS_TASK_IDLE_STK_SIZE,
                          (void *)0,                                 /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
    #endif
#else
    #if OS_STK_GROWTH == 1
    (void)OSTaskCreate(OS_TaskIdle,
                       (void *)0,
                       &OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE - 1],
                       OS_IDLE_PRIO);
    #else
    (void)OSTaskCreate(OS_TaskIdle,
                       (void *)0,
                       &OSTaskIdleStk[0],
                       OS_IDLE_PRIO);
    #endif
#endif
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                      CREATING THE STATISTIC TASK
*
* Description: This function creates the Statistic Task.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

#if OS_TASK_STAT_EN > 0
static  void  OS_InitTaskStat (void)
{
#if OS_TASK_CREATE_EXT_EN > 0
    #if OS_STK_GROWTH == 1
    (void)OSTaskCreateExt(OS_TaskStat,
                          (void *)0,                                   /* No args passed to OS_TaskStat()*/
                          &OSTaskStatStk[OS_TASK_STAT_STK_SIZE - 1],   /* Set Top-Of-Stack               */
                          OS_STAT_PRIO,                                /* One higher than the idle task  */
                          OS_TASK_STAT_ID,
                          &OSTaskStatStk[0],                           /* Set Bottom-Of-Stack            */
                          OS_TASK_STAT_STK_SIZE,
                          (void *)0,                                   /* No TCB extension               */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  /* Enable stack checking + clear  */
    #else
    (void)OSTaskCreateExt(OS_TaskStat,
                          (void *)0,                                   /* No args passed to OS_TaskStat()*/
                          &OSTaskStatStk[0],                           /* Set Top-Of-Stack               */
                          OS_STAT_PRIO,                                /* One higher than the idle task  */
                          OS_TASK_STAT_ID,
                          &OSTaskStatStk[OS_TASK_STAT_STK_SIZE - 1],   /* Set Bottom-Of-Stack            */
                          OS_TASK_STAT_STK_SIZE,
                          (void *)0,                                   /* No TCB extension               */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  /* Enable stack checking + clear  */
    #endif
#else
    #if OS_STK_GROWTH == 1
    (void)OSTaskCreate(OS_TaskStat,
                       (void *)0,                                      /* No args passed to OS_TaskStat()*/
                       &OSTaskStatStk[OS_TASK_STAT_STK_SIZE - 1],      /* Set Top-Of-Stack               */
                       OS_STAT_PRIO);                                  /* One higher than the idle task  */
    #else
    (void)OSTaskCreate(OS_TaskStat,
                       (void *)0,                                      /* No args passed to OS_TaskStat()*/
                       &OSTaskStatStk[0],                              /* Set Top-Of-Stack               */
                       OS_STAT_PRIO);                                  /* One higher than the idle task  */
    #endif
#endif
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                            INITIALIZE THE FREE LIST OF TASK CONTROL BLOCKS
*
* Description: This function is called by OSInit() to initialize the free list of OS_TCBs.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

static  void  OS_InitTCBList (void)
{
    INT8U    i;
    OS_TCB  *ptcb1;
    OS_TCB  *ptcb2;

//	初始化任务块链表指针。
    OSTCBList     = (OS_TCB *)0;                                 /* TCB Initialization                       */
//	初始化优先级列表。
	for (i = 0; i < (OS_LOWEST_PRIO + 1); i++) 
	{                 /* Clear the priority table                 */
        OSTCBPrioTbl[i] = (OS_TCB *)0;
    }
//	得到空任务控制块链表前两个节点。
    ptcb1 = &OSTCBTbl[0];
    ptcb2 = &OSTCBTbl[1];
//	建立空任务控制块链表
    for (i = 0; i < (OS_MAX_TASKS + OS_N_SYS_TASKS - 1); i++) 
	{  /* Init. list of free TCBs                  */
        ptcb1->OSTCBNext = ptcb2;
        ptcb1++;
        ptcb2++;
    }
    ptcb1->OSTCBNext = (OS_TCB *)0;                              /* Last OS_TCB                              */
//	将OSTCBFreeList指向空任务控制块链表的首地址。
	OSTCBFreeList    = &OSTCBTbl[0];
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                              SCHEDULER
*
* Description: This function is called by other uC/OS-II services to determine whether a new, high
*              priority task has been made ready to run.  This function is invoked by TASK level code
*              and is not used to reschedule tasks from ISRs (see OSIntExit() for ISR rescheduling).
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) This function is INTERNAL to uC/OS-II and your application should not call it.
*              2) Rescheduling is prevented when the scheduler is locked (see OS_SchedLock())
*********************************************************************************************************
*/
//	进行任务调度，执行优先级最高的任务。
void  OS_Sched (void)
{
#if OS_CRITICAL_METHOD == 3                            /* Allocate storage for CPU status register     */
    OS_CPU_SR  cpu_sr;
#endif    
    INT8U      y;


    OS_ENTER_CRITICAL();
//	中断嵌套为0，调度器嵌套为0
    if ((OSIntNesting == 0) && (OSLockNesting == 0)) 
	{ /* Sched. only if all ISRs done & not locked    */
	//	确定最高优先级所在的组
        y             = OSUnMapTbl[OSRdyGrp];          /* Get pointer to HPT ready to run              */
	//	确定最高优先级
		OSPrioHighRdy = (INT8U)((y << 3) + OSUnMapTbl[OSRdyTbl[y]]);
	//	若当前任务的优先级 == 最高优先级，则不切换任务。
		if (OSPrioHighRdy != OSPrioCur) 
		{              /* No Ctx Sw if current task is highest rdy     */
            OSTCBHighRdy = OSTCBPrioTbl[OSPrioHighRdy];
		//	记录上下文切换的次数
			OSCtxSwCtr++;                              /* Increment context switch counter             */
            OS_TASK_SW();                              /* Perform a context switch                     */
        }
    }
    OS_EXIT_CRITICAL();
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                              IDLE TASK
*
* Description: This task is internal to uC/OS-II and executes whenever no other higher priority tasks
*              executes because they are ALL waiting for event(s) to occur.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 1) OSTaskIdleHook() is called after the critical section to ensure that interrupts will be
*                 enabled for at least a few instructions.  On some processors (ex. Philips XA), enabling
*                 and then disabling interrupts didn't allow the processor enough time to have interrupts
*                 enabled before they were disabled again.  uC/OS-II would thus never recognize
*                 interrupts.
*              2) This hook has been added to allow you to do such things as STOP the CPU to conserve 
*                 power.
*********************************************************************************************************
*/
//	空闲任务
void  OS_TaskIdle (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    
    
    pdata = pdata;                               /* Prevent compiler warning for not using 'pdata'     */
    for (;;) 
	{
        OS_ENTER_CRITICAL();
		//	空闲任务执行的次数
        OSIdleCtr++;
        OS_EXIT_CRITICAL();
        OSTaskIdleHook();                        /* Call user definable HOOK                           */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                            STATISTICS TASK
*
* Description: This task is internal to uC/OS-II and is used to compute some statistics about the
*              multitasking environment.  Specifically, OS_TaskStat() computes the CPU usage.
*              CPU usage is determined by:
*
*                                          OSIdleCtr
*                 OSCPUUsage = 100 * (1 - ------------)     (units are in %)
*                                         OSIdleCtrMax
*
* Arguments  : pdata     this pointer is not used at this time.
*
* Returns    : none
*
* Notes      : 1) This task runs at a priority level higher than the idle task.  In fact, it runs at the
*                 next higher priority, OS_IDLE_PRIO-1.
*              2) You can disable this task by setting the configuration #define OS_TASK_STAT_EN to 0.
*              3) We delay for 5 seconds in the beginning to allow the system to reach steady state and
*                 have all other tasks created before we do statistics.  You MUST have at least a delay
*                 of 2 seconds to allow for the system to establish the maximum value for the idle
*                 counter.
*********************************************************************************************************
*/

#if OS_TASK_STAT_EN > 0
//	统计任务
void  OS_TaskStat (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    INT32U     run;
    INT32U     max;
    INT8S      usage;


    pdata = pdata;                               /* Prevent compiler warning for not using 'pdata'     */
    while (OSStatRdy == FALSE) 
	{
        OSTimeDly(2 * OS_TICKS_PER_SEC);         /* Wait until statistic task is ready                 */
    }
    max = OSIdleCtrMax / 100L;
    for (;;) 
	{
        OS_ENTER_CRITICAL();
        OSIdleCtrRun = OSIdleCtr;                /* Obtain the of the idle counter for the past second */
        run          = OSIdleCtr;
        OSIdleCtr    = 0L;                       /* Reset the idle counter for the next second         */
        OS_EXIT_CRITICAL();
        if (max > 0L) 
		{
            usage = (INT8S)(100L - run / max);
            if (usage >= 0) 
			{                    /* Make sure we don't have a negative percentage      */
                OSCPUUsage = usage;
            } 
			else 
			{
                OSCPUUsage = 0;
            }
        } 
		else 
		{
            OSCPUUsage = 0;
            max        = OSIdleCtrMax / 100L;
        }
        OSTaskStatHook();                        /* Invoke user definable hook                         */
        OSTimeDly(OS_TICKS_PER_SEC);             /* Accumulate OSIdleCtr for the next second           */
    }
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                            INITIALIZE TCB
*
* Description: This function is internal to uC/OS-II and is used to initialize a Task Control Block when
*              a task is created (see OSTaskCreate() and OSTaskCreateExt()).
*
* Arguments  : prio          is the priority of the task being created
*
*              ptos          is a pointer to the task's top-of-stack assuming that the CPU registers
*                            have been placed on the stack.  Note that the top-of-stack corresponds to a
*                            'high' memory location is OS_STK_GROWTH is set to 1 and a 'low' memory
*                            location if OS_STK_GROWTH is set to 0.  Note that stack growth is CPU
*                            specific.
*
*              pbos          is a pointer to the bottom of stack.  A NULL pointer is passed if called by
*                            'OSTaskCreate()'.
*
*              id            is the task's ID (0..65535)
*
*              stk_size      is the size of the stack (in 'stack units').  If the stack units are INT8Us
*                            then, 'stk_size' contains the number of bytes for the stack.  If the stack
*                            units are INT32Us then, the stack contains '4 * stk_size' bytes.  The stack
*                            units are established by the #define constant OS_STK which is CPU
*                            specific.  'stk_size' is 0 if called by 'OSTaskCreate()'.
*
*              pext          is a pointer to a user supplied memory area that is used to extend the task
*                            control block.  This allows you to store the contents of floating-point
*                            registers, MMU registers or anything else you could find useful during a
*                            context switch.  You can even assign a name to each task and store this name
*                            in this TCB extension.  A NULL pointer is passed if called by OSTaskCreate().
*
*              opt           options as passed to 'OSTaskCreateExt()' or,
*                            0 if called from 'OSTaskCreate()'.
*
* Returns    : OS_NO_ERR         if the call was successful
*              OS_NO_MORE_TCB    if there are no more free TCBs to be allocated and thus, the task cannot
*                                be created.
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
//prio		任务优先级
//ptos		任务堆栈首地址
//id		任务的ID
//stk_size	堆栈单元数量
//pext		用于拓展堆栈的大小，被OSTCBCreate调用时，该参数为NULL。
//opt		供OSTaskCreateExt使用，若被OSTCBCreate调用，该参数为NULL。
INT8U  OS_TCBInit (INT8U prio, OS_STK *ptos, OS_STK *pbos, INT16U id, INT32U stk_size, void *pext, INT16U opt)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif    
    OS_TCB    *ptcb;


    OS_ENTER_CRITICAL();
	//从空闲的TCB链表中得到1个空闲的TCB块
    ptcb = OSTCBFreeList;                                  /* Get a free TCB from the free TCB list    */
    if (ptcb != (OS_TCB *)0) 
	{
		//更新空闲TCB链表的首地址
        OSTCBFreeList        = ptcb->OSTCBNext;            /* Update pointer to free TCB list          */
        OS_EXIT_CRITICAL();
		//保存堆栈指针至TCB
        ptcb->OSTCBStkPtr    = ptos;                       /* Load Stack pointer in TCB              */
		//保存优先级至TCB
		ptcb->OSTCBPrio      = (INT8U)prio;                /* Load task priority into TCB              */
		//保存任务状态
		ptcb->OSTCBStat      = OS_STAT_RDY;                /* Task is ready to run                     */
		//任务延时
		ptcb->OSTCBDly       = 0;                          /* Task is not delayed                      */

#if OS_TASK_CREATE_EXT_EN > 0
        ptcb->OSTCBExtPtr    = pext;                       /* Store pointer to TCB extension           */
        ptcb->OSTCBStkSize   = stk_size;                   /* Store stack size                         */
        ptcb->OSTCBStkBottom = pbos;                       /* Store pointer to bottom of stack         */
        ptcb->OSTCBOpt       = opt;                        /* Store task options                       */
        ptcb->OSTCBId        = id;                         /* Store task ID                            */
#else
        pext                 = pext;                       /* Prevent compiler warning if not used     */
        stk_size             = stk_size;
        pbos                 = pbos;
        opt                  = opt;
        id                   = id;
#endif

#if OS_TASK_DEL_EN > 0
        ptcb->OSTCBDelReq    = OS_NO_ERR;
#endif
		//任务优先级位图的横坐标
        ptcb->OSTCBY         = prio >> 3;                  /* Pre-compute X, Y, BitX and BitY          */
		//任务优先级位图所占的位置
		ptcb->OSTCBBitY      = OSMapTbl[ptcb->OSTCBY];
		//任务优先级在每组的级数
		ptcb->OSTCBX         = prio & 0x07;
		//任务优先级在每组的位置
		ptcb->OSTCBBitX      = OSMapTbl[ptcb->OSTCBX];

#if OS_EVENT_EN > 0
        ptcb->OSTCBEventPtr  = (OS_EVENT *)0;              /* Task is not pending on an event          */
#endif

#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0) && (OS_TASK_DEL_EN > 0)
        ptcb->OSTCBFlagNode  = (OS_FLAG_NODE *)0;          /* Task is not pending on an event flag     */
#endif

#if (OS_MBOX_EN > 0) || ((OS_Q_EN > 0) && (OS_MAX_QS > 0))
        ptcb->OSTCBMsg       = (void *)0;                  /* No message received                      */
#endif

#if OS_VERSION >= 204
        OSTCBInitHook(ptcb);
#endif

        OSTaskCreateHook(ptcb);                            /* Call user defined hook                   */
        
        OS_ENTER_CRITICAL();
        OSTCBPrioTbl[prio] = ptcb;
        ptcb->OSTCBNext    = OSTCBList;                    /* Link into TCB chain                      */
        ptcb->OSTCBPrev    = (OS_TCB *)0;
        if (OSTCBList != (OS_TCB *)0) {
            OSTCBList->OSTCBPrev = ptcb;
        }
        OSTCBList               = ptcb;
        OSRdyGrp               |= ptcb->OSTCBBitY;         /* Make task ready to run                   */
        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
        OS_EXIT_CRITICAL();
        return (OS_NO_ERR);
    }
    OS_EXIT_CRITICAL();
    return (OS_NO_MORE_TCB);
}

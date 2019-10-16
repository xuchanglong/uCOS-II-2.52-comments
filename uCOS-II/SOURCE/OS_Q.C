/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                        MESSAGE QUEUE MANAGEMENT
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : OS_Q.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE
#include "includes.h"
#endif

#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)
/*
*********************************************************************************************************
*                                      ACCEPT MESSAGE FROM QUEUE
*
* Description: This function checks the queue to see if a message is available.  Unlike OSQPend(),
*              OSQAccept() does not suspend the calling task if a message is not available.
*
* Arguments  : pevent        is a pointer to the event control block
*
* Returns    : != (void *)0  is the message in the queue if one is available.  The message is removed
*                            from the so the next time OSQAccept() is called, the queue will contain
*                            one less entry.
*              == (void *)0  if the queue is empty or,
*                            if 'pevent' is a NULL pointer or,
*                            if you passed an invalid event type
*********************************************************************************************************
*/

#if OS_Q_ACCEPT_EN > 0
void  *OSQAccept (OS_EVENT *pevent)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
    void      *msg;
    OS_Q      *pq;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) 
	{               /* Validate 'pevent'                                  */
        return ((void *)0);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {/* Validate event block type                          */
        return ((void *)0);
    }
#endif
    OS_ENTER_CRITICAL();
//	获得队列控制块
    pq = (OS_Q *)pevent->OSEventPtr;             /* Point at queue control block                       */
//	若队列非空，则返回NULL
	if (pq->OSQEntries > 0) 
	{                    /* See if any messages in the queue                   */
	//	得到队列中最老的元素
		msg = *pq->OSQOut++;                     /* Yes, extract oldest message from the queue         */
	//	更新队列中元素的个数
		pq->OSQEntries--;                        /* Update the number of entries in the queue          */
	//	若提取元素的指针指向了队列最后一个元素的下一个位置，则将该指针指向队列第一个元素的首地址
		if (pq->OSQOut == pq->OSQEnd) 
		{          /* Wrap OUT pointer if we are at the end of the queue */
            pq->OSQOut = pq->OSQStart;
        }
    } 
	else 
	{
	//	队列为空，无消息可用。
        msg = (void *)0;                         /* Queue is empty                                     */
    }
    OS_EXIT_CRITICAL();
    return (msg);                                /* Return message received (or NULL)                  */
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                        CREATE A MESSAGE QUEUE
*
* Description: This function creates a message queue if free event control blocks are available.
*
* Arguments  : start         is a pointer to the base address of the message queue storage area.  The
*                            storage area MUST be declared as an array of pointers to 'void' as follows
*
*                            void *MessageStorage[size]
*
*              size          is the number of elements in the storage area
*
* Returns    : != (OS_EVENT *)0  is a pointer to the event control clock (OS_EVENT) associated with the
*                                created queue
*              == (OS_EVENT *)0  if no event control blocks were available or an error was detected
*********************************************************************************************************
*/

OS_EVENT  *OSQCreate (void **start, INT16U size)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
//	存储空闲的事件控制块
    OS_EVENT  *pevent;
//	存储空闲的队列控制块
    OS_Q      *pq;

//	中断不可用
    if (OSIntNesting > 0) 
	{                      /* See if called from ISR ...                         */
        return ((OS_EVENT *)0);                  /* ... can't CREATE from an ISR                       */
    }
    OS_ENTER_CRITICAL();
//	得到空闲的事件控制块
    pevent = OSEventFreeList;                    /* Get next free event control block                  */
//	更新空闲事件控制块链表首地址
	if (OSEventFreeList != (OS_EVENT *)0) 
	{      /* See if pool of free ECB pool was empty             */
        OSEventFreeList = (OS_EVENT *)OSEventFreeList->OSEventPtr;
    }
    OS_EXIT_CRITICAL();
//	判断该空闲块是否可用
    if (pevent != (OS_EVENT *)0) 
	{               /* See if we have an event control block              */
        OS_ENTER_CRITICAL();
	//	得到空闲的队列控制块
        pq = OSQFreeList;                        /* Get a free queue control block                     */
        if (pq != (OS_Q *)0) 
		{                   /* Were we able to get a queue control block ?        */
		//	更新空闲的队列控制块链表的首地址
            OSQFreeList         = OSQFreeList->OSQPtr;    /* Yes, Adjust free list pointer to next free*/
            OS_EXIT_CRITICAL();
		//	队列的首地址
            pq->OSQStart        = start;                  /*      Initialize the queue                 */
		//	队列的最后一个节点的下一个节点
			pq->OSQEnd          = &start[size];
		//	指向队列中插入节点的位置
            pq->OSQIn           = start;
		//	指向队列中提取节点的位置
            pq->OSQOut          = start;
		//	队列的总字节数
            pq->OSQSize         = size;
		//	队列中元素的个数
            pq->OSQEntries      = 0;
		//	确定事件类型是Q
            pevent->OSEventType = OS_EVENT_TYPE_Q;
		//	和Cnt无关，故清零。
            pevent->OSEventCnt  = 0;
		//	指向队列控制块
            pevent->OSEventPtr  = pq;
            OS_EventWaitListInit(pevent);                 /*      Initalize the wait list              */
        }
		else 
		{
		//	申请的空闲事件控制块重新放回空闲事件控制块链表中
            pevent->OSEventPtr = (void *)OSEventFreeList; /* No,  Return event control block on error  */
            OSEventFreeList    = pevent;
            OS_EXIT_CRITICAL();
		//	返回NULL，代表创建队列失败。
            pevent = (OS_EVENT *)0;
        }
    }
    return (pevent);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        DELETE A MESSAGE QUEUE
*
* Description: This function deletes a message queue and readies all tasks pending on the queue.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            queue.
*
*              opt           determines delete options as follows:
*                            opt == OS_DEL_NO_PEND   Delete the queue ONLY if no task pending
*                            opt == OS_DEL_ALWAYS    Deletes the queue even if tasks are waiting.
*                                                    In this case, all the tasks pending will be readied.
*
*              err           is a pointer to an error code that can contain one of the following values:
*                            OS_NO_ERR               The call was successful and the queue was deleted
*                            OS_ERR_DEL_ISR          If you tried to delete the queue from an ISR
*                            OS_ERR_INVALID_OPT      An invalid option was specified
*                            OS_ERR_TASK_WAITING     One or more tasks were waiting on the queue
*                            OS_ERR_EVENT_TYPE       If you didn't pass a pointer to a queue
*                            OS_ERR_PEVENT_NULL      If 'pevent' is a NULL pointer.
*
* Returns    : pevent        upon error
*              (OS_EVENT *)0 if the queue was successfully deleted.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the queue MUST check the return code of OSQPend().
*              2) OSQAccept() callers will not know that the intended queue has been deleted unless
*                 they check 'pevent' to see that it's a NULL pointer.
*              3) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the queue.
*              4) Because ALL tasks pending on the queue will be readied, you MUST be careful in
*                 applications where the queue is used for mutual exclusion because the resource(s)
*                 will no longer be guarded by the queue.
*              5) If the storage for the message queue was allocated dynamically (i.e. using a malloc()
*                 type call) then your application MUST release the memory storage by call the counterpart
*                 call of the dynamic allocation scheme used.  If the queue storage was created statically
*                 then, the storage can be reused.
*********************************************************************************************************
*/

#if OS_Q_DEL_EN > 0
OS_EVENT  *OSQDel (OS_EVENT *pevent, INT8U opt, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    BOOLEAN    tasks_waiting;
    OS_Q      *pq;

//	ISR中不可调用
    if (OSIntNesting > 0) 
	{                                /* See if called from ISR ...               */
        *err = OS_ERR_DEL_ISR;                             /* ... can't DELETE from an ISR             */
        return ((OS_EVENT *)0);
    }
#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) 
	{                         /* Validate 'pevent'                        */
        *err = OS_ERR_PEVENT_NULL;
        return (pevent);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) 
	{          /* Validate event block type                */
        *err = OS_ERR_EVENT_TYPE;
        return (pevent);
    }
#endif
    OS_ENTER_CRITICAL();
//	判断该事件是否有任务等待
    if (pevent->OSEventGrp != 0x00) 
	{                      /* See if any tasks waiting on queue        */
        tasks_waiting = TRUE;                              /* Yes                                      */
    } 
	else 
	{
        tasks_waiting = FALSE;                             /* No                                       */
    }
    switch (opt) 
	{
	//	仅当没有任务等待的时候，才能删除该事件。
        case OS_DEL_NO_PEND:                               /* Delete queue only if no task waiting     */
		//	无任务等待，删除事件控制块和队列控制块。
			 if (tasks_waiting == FALSE) 
			 {
			 //	将队列控制块归还到空闲队列控制块链表中
                 pq                  = (OS_Q *)pevent->OSEventPtr;  /* Return OS_Q to free list        */
                 pq->OSQPtr          = OSQFreeList;
                 OSQFreeList         = pq;
			//	将事件控制块置为无用。
                 pevent->OSEventType = OS_EVENT_TYPE_UNUSED;
			//	将该事件控制块归还到空闲事件控制块的链表中
				 pevent->OSEventPtr  = OSEventFreeList;    /* Return Event Control Block to free list  */
                 OSEventFreeList     = pevent;             /* Get next free event control block        */
                 OS_EXIT_CRITICAL();
                 *err = OS_NO_ERR;
                 return ((OS_EVENT *)0);                   /* Queue has been deleted                   */
             }
			//	有任务等待，则报错。
			 else 
			 {
                 OS_EXIT_CRITICAL();
                 *err = OS_ERR_TASK_WAITING;
                 return (pevent);
             }
		//	无论有没有等待的任务，均删除事件控制块和队列控制块
        case OS_DEL_ALWAYS:                                /* Always delete the queue                  */
		//	将等待该事件的任务均从等待事件矩阵中删除，并置位任务就绪矩阵。
			 while (pevent->OSEventGrp != 0x00) 
			 {          /* Ready ALL tasks waiting for queue        */
                 OS_EventTaskRdy(pevent, (void *)0, OS_STAT_Q);
             }
		//	将队列控制块归还到空闲队列控制块链表中
             pq                  = (OS_Q *)pevent->OSEventPtr;      /* Return OS_Q to free list        */
             pq->OSQPtr          = OSQFreeList;
             OSQFreeList         = pq;
		//	将事件控制块置为无用。
             pevent->OSEventType = OS_EVENT_TYPE_UNUSED;
		//	将该事件控制块归还到空闲事件控制块的链表中
             pevent->OSEventPtr  = OSEventFreeList;        /* Return Event Control Block to free list  */
             OSEventFreeList     = pevent;                 /* Get next free event control block        */
             OS_EXIT_CRITICAL();
		//	由于有等待任务被重置为就绪，故重新调度使优先级最高的任务执行，
             if (tasks_waiting == TRUE) 
			 {                  /* Reschedule only if task(s) were waiting  */
                 OS_Sched();                               /* Find highest priority task ready to run  */
             }
             *err = OS_NO_ERR;
             return ((OS_EVENT *)0);                       /* Queue has been deleted                   */

        default:
             OS_EXIT_CRITICAL();
             *err = OS_ERR_INVALID_OPT;
             return (pevent);
    }
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                           FLUSH QUEUE
*
* Description : This function is used to flush the contents of the message queue.
*
* Arguments   : none
*
* Returns     : OS_NO_ERR           upon success
*               OS_ERR_EVENT_TYPE   If you didn't pass a pointer to a queue
*               OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer
*********************************************************************************************************
*/

#if OS_Q_FLUSH_EN > 0
//清空消息队列
INT8U  OSQFlush (OS_EVENT *pevent)
{
#if OS_CRITICAL_METHOD == 3                           /* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr;
#endif
    OS_Q      *pq;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) 
	{                    /* Validate 'pevent'                             */
        return (OS_ERR_PEVENT_NULL);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) 
	{     /* Validate event block type                     */
        return (OS_ERR_EVENT_TYPE);
    }
#endif
    OS_ENTER_CRITICAL();
//	获得队列控制块的首地址
    pq             = (OS_Q *)pevent->OSEventPtr;      /* Point to queue storage structure              */
//	插入节点的指针和提取节点的指针均指向队列的首地址
	pq->OSQIn      = pq->OSQStart;
    pq->OSQOut     = pq->OSQStart;
//	队列中的元素个数
    pq->OSQEntries = 0;
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                     PEND ON A QUEUE FOR A MESSAGE
*
* Description: This function waits for a message to be sent to a queue
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for a message to arrive at the queue up to the amount of time
*                            specified by this argument.  If you specify 0, however, your task will wait
*                            forever at the specified queue or, until a message arrives.
*
*              err           is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                            OS_NO_ERR           The call was successful and your task received a
*                                                message.
*                            OS_TIMEOUT          A message was not received within the specified timeout
*                            OS_ERR_EVENT_TYPE   You didn't pass a pointer to a queue
*                            OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer
*                            OS_ERR_PEND_ISR     If you called this function from an ISR and the result
*                                                would lead to a suspension.
*
* Returns    : != (void *)0  is a pointer to the message received
*              == (void *)0  if no message was received or,
*                            if 'pevent' is a NULL pointer or,
*                            if you didn't pass a pointer to a queue.
*********************************************************************************************************
*/

void  *OSQPend (OS_EVENT *pevent, INT16U timeout, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
    void      *msg;
    OS_Q      *pq;

//	中断中不可用
    if (OSIntNesting > 0) 
	{                      /* See if called from ISR ...                         */
        *err = OS_ERR_PEND_ISR;                  /* ... can't PEND from an ISR                         */
        return ((void *)0);
    }
#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) 
	{               /* Validate 'pevent'                                  */
        *err = OS_ERR_PEVENT_NULL;
        return ((void *)0);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) 
	{/* Validate event block type                          */
        *err = OS_ERR_EVENT_TYPE;
        return ((void *)0);
    }
#endif
//	关中断
    OS_ENTER_CRITICAL();
//	获得队列控制块
    pq = (OS_Q *)pevent->OSEventPtr;             /* Point at queue control block                       */
//	若队列中有元素
	if (pq->OSQEntries > 0) 
	{                    /* See if any messages in the queue                   */
	//	提取一个消息
        msg = *pq->OSQOut++;                     /* Yes, extract oldest message from the queue         */
	//	队列中元素个数-1
		pq->OSQEntries--;                        /* Update the number of entries in the queue          */
        if (pq->OSQOut == pq->OSQEnd) 
		{          /* Wrap OUT pointer if we are at the end of the queue */
            pq->OSQOut = pq->OSQStart;
        }
        OS_EXIT_CRITICAL();
        *err = OS_NO_ERR;
        return (msg);                            /* Return message received                            */
    }
//	因为队列中没有消息，故将事件控制块置于等待队列消息的状态
    OSTCBCur->OSTCBStat |= OS_STAT_Q;            /* Task will have to pend for a message to be posted  */
//	设置等待超时
	OSTCBCur->OSTCBDly   = timeout;              /* Load timeout into TCB                              */
//	将当前任务从任务就绪矩阵中删除，置位事件等待矩阵。
	OS_EventTaskWait(pevent);                    /* Suspend task until event or timeout occurs         */
//	开中断
	OS_EXIT_CRITICAL();
//	任务调度，发现最高优先级的任务
	OS_Sched();                                  /* Find next highest priority task ready to run       */
//	任务返回，有可能是在timeout内发现队列中存在任务，或者是超时。
	OS_ENTER_CRITICAL();
//	获得当前消息
    msg = OSTCBCur->OSTCBMsg;
    if (msg != (void *)0) 
	{                      /* Did we get a message?                              */
        OSTCBCur->OSTCBMsg      = (void *)0;     /* Extract message from TCB (Put there by QPost)      */
        OSTCBCur->OSTCBStat     = OS_STAT_RDY;
        OSTCBCur->OSTCBEventPtr = (OS_EVENT *)0; /* No longer waiting for event                        */
        OS_EXIT_CRITICAL();
        *err                    = OS_NO_ERR;
        return (msg);                            /* Return message received                            */
    }
    OS_EventTO(pevent);                          /* Timed out                                          */
    OS_EXIT_CRITICAL();
    *err = OS_TIMEOUT;                           /* Indicate a timeout occured                         */
    return ((void *)0);                          /* No message received                                */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              msg           is a pointer to the message to send.  You MUST NOT send a NULL pointer.
*
* Returns    : OS_NO_ERR             The call was successful and the message was sent
*              OS_Q_FULL             If the queue cannot accept any more messages because it is full.
*              OS_ERR_EVENT_TYPE     If you didn't pass a pointer to a queue.
*              OS_ERR_PEVENT_NULL    If 'pevent' is a NULL pointer
*              OS_ERR_POST_NULL_PTR  If you are attempting to post a NULL pointer
*********************************************************************************************************
*/

#if OS_Q_POST_EN > 0
INT8U  OSQPost (OS_EVENT *pevent, void *msg)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
    OS_Q      *pq;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) 
	{                    /* Validate 'pevent'                             */
        return (OS_ERR_PEVENT_NULL);
    }
    if (msg == (void *)0) 
	{                           /* Make sure we are not posting a NULL pointer   */
        return (OS_ERR_POST_NULL_PTR);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) 
	{     /* Validate event block type                     */
        return (OS_ERR_EVENT_TYPE);
    }
#endif
    OS_ENTER_CRITICAL();
//	该队列有等待的任务，说明队列中的元素也是空的。
    if (pevent->OSEventGrp != 0x00) 
	{                 /* See if any task pending on queue              */
	//	在任务就绪列表中置位优先级在事件等待列表中最高的任务，并将消息传入任务控制块
        OS_EventTaskRdy(pevent, msg, OS_STAT_Q);      /* Ready highest priority task waiting on event  */
		OS_EXIT_CRITICAL();
        OS_Sched();                                   /* Find highest priority task ready to run       */
        return (OS_NO_ERR);
    }
//	因为无任务等待，故设置队列，存放待取走的消息。
//	获得该事件控制块的队列控制块
    pq = (OS_Q *)pevent->OSEventPtr;                  /* Point to queue control block                  */
//	若队列中消息已满，则报队列已满的错误。
	if (pq->OSQEntries >= pq->OSQSize) 
	{              /* Make sure queue is not full                   */
        OS_EXIT_CRITICAL();
        return (OS_Q_FULL);
    }
//	插入消息节点
    *pq->OSQIn++ = msg;                               /* Insert message into queue                     */
//	消息数量+1
	pq->OSQEntries++;                                 /* Update the nbr of entries in the queue        */
//	闭环
	if (pq->OSQIn == pq->OSQEnd) 
	{                    /* Wrap IN ptr if we are at end of queue         */
        pq->OSQIn = pq->OSQStart;
    }
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                   POST MESSAGE TO THE FRONT OF A QUEUE
*
* Description: This function sends a message to a queue but unlike OSQPost(), the message is posted at
*              the front instead of the end of the queue.  Using OSQPostFront() allows you to send
*              'priority' messages.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              msg           is a pointer to the message to send.  You MUST NOT send a NULL pointer.
*
* Returns    : OS_NO_ERR             The call was successful and the message was sent
*              OS_Q_FULL             If the queue cannot accept any more messages because it is full.
*              OS_ERR_EVENT_TYPE     If you didn't pass a pointer to a queue.
*              OS_ERR_PEVENT_NULL    If 'pevent' is a NULL pointer
*              OS_ERR_POST_NULL_PTR  If you are attempting to post to a non queue.
*********************************************************************************************************
*/

#if OS_Q_POST_FRONT_EN > 0
//发送消息到队列，但是与OSQPost不同，其发送到队列头部，即：发送的是优先消息。
INT8U  OSQPostFront (OS_EVENT *pevent, void *msg)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
    OS_Q      *pq;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) 
	{                    /* Validate 'pevent'                             */
        return (OS_ERR_PEVENT_NULL);
    }
    if (msg == (void *)0) 
	{                           /* Make sure we are not posting a NULL pointer   */
        return (OS_ERR_POST_NULL_PTR);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) 
	{     /* Validate event block type                     */
        return (OS_ERR_EVENT_TYPE);
    }
#endif
    OS_ENTER_CRITICAL();
//	有任务等待该事件。
    if (pevent->OSEventGrp != 0x00) 
	{                 /* See if any task pending on queue              */
	//	在任务就绪列表中置位优先级在事件等待列表中最高的任务，并将消息传入任务控制块
        OS_EventTaskRdy(pevent, msg, OS_STAT_Q);      /* Ready highest priority task waiting on event  */
        OS_EXIT_CRITICAL();
	//	任务调度，执行最高优先级任务。
        OS_Sched();                                   /* Find highest priority task ready to run       */
        return (OS_NO_ERR);
    }
//	无任务等待该事件
    pq = (OS_Q *)pevent->OSEventPtr;                  /* Point to queue control block                  */
//	队列中元素个数超过队列元素个数上限，则提示队列已满。
	if (pq->OSQEntries >= pq->OSQSize) 
	{              /* Make sure queue is not full                   */
        OS_EXIT_CRITICAL();
        return (OS_Q_FULL);
    }
//	为了实现将消息插入队列的前面，传统方法是在OSQOut指向不变的情况下，
//	需要将队列中每一个元素向后移动一个位置，但是这种方法就造成了运行
//	时间的不可控性。
//	为了解决这个问题，元素的位置不再移动，移动OSQOut指向，即：将元素
//	插入队列的前面，OSQOut向前移动一位，从而解决上述问题。同时这也需
//	要该队列是一个循环队列。
//	详情链接：https://www.cnblogs.com/dengxiaojun/p/4324911.html
    if (pq->OSQOut == pq->OSQStart) 
	{                 /* Wrap OUT ptr if we are at the 1st queue entry */
        pq->OSQOut = pq->OSQEnd;
    }
//	移动OSQOut向前移动一位，使插入的消息位于队列的前面。
    pq->OSQOut--;
//	在队列的前面插入消息
	*pq->OSQOut = msg;                                /* Insert message into queue                     */
//	队列中元素个数+1
	pq->OSQEntries++;                                 /* Update the nbr of entries in the queue        */
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                        POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue.  This call has been added to reduce code size
*              since it can replace both OSQPost() and OSQPostFront().  Also, this function adds the
*              capability to broadcast a message to ALL tasks waiting on the message queue.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              msg           is a pointer to the message to send.  You MUST NOT send a NULL pointer.
*
*              opt           determines the type of POST performed:
*                            OS_POST_OPT_NONE         POST to a single waiting task
*                                                     (Identical to OSQPost())
*                            OS_POST_OPT_BROADCAST    POST to ALL tasks that are waiting on the queue
*                            OS_POST_OPT_FRONT        POST as LIFO (Simulates OSQPostFront())
*
*                            Below is a list of ALL the possible combination of these flags:
*
*                                 1) OS_POST_OPT_NONE
*                                    identical to OSQPost()
*
*                                 2) OS_POST_OPT_FRONT
*                                    identical to OSQPostFront()
*
*                                 3) OS_POST_OPT_BROADCAST
*                                    identical to OSQPost() but will broadcast 'msg' to ALL waiting tasks
*
*                                 4) OS_POST_OPT_FRONT + OS_POST_OPT_BROADCAST  is identical to
*                                    OSQPostFront() except that will broadcast 'msg' to ALL waiting tasks
*
* Returns    : OS_NO_ERR             The call was successful and the message was sent
*              OS_Q_FULL             If the queue cannot accept any more messages because it is full.
*              OS_ERR_EVENT_TYPE     If you didn't pass a pointer to a queue.
*              OS_ERR_PEVENT_NULL    If 'pevent' is a NULL pointer
*              OS_ERR_POST_NULL_PTR  If you are attempting to post a NULL pointer
*
* Warning    : Interrupts can be disabled for a long time if you do a 'broadcast'.  In fact, the
*              interrupt disable time is proportional to the number of tasks waiting on the queue.
*********************************************************************************************************
*/

#if OS_Q_POST_OPT_EN > 0
INT8U  OSQPostOpt (OS_EVENT *pevent, void *msg, INT8U opt)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
    OS_Q      *pq;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) 
	{                    /* Validate 'pevent'                             */
        return (OS_ERR_PEVENT_NULL);
    }
    if (msg == (void *)0) 
	{                           /* Make sure we are not posting a NULL pointer   */
        return (OS_ERR_POST_NULL_PTR);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) 
	{     /* Validate event block type                     */
        return (OS_ERR_EVENT_TYPE);
    }
#endif
    OS_ENTER_CRITICAL();
//	有任务等待该事件。
    if (pevent->OSEventGrp != 0x00) 
	{                 /* See if any task pending on queue              */
	//	将消息广播到每一个等待该事件的任务，并唤醒这些任务。
        if ((opt & OS_POST_OPT_BROADCAST) != 0x00) 
		{  /* Do we need to post msg to ALL waiting tasks ? */
            while (pevent->OSEventGrp != 0x00) 
			{      /* Yes, Post to ALL tasks waiting on queue       */
                OS_EventTaskRdy(pevent, msg, OS_STAT_Q);
            }
        } 
	//	否则只将消息发给等待任务中优先级最高的任务。
		else 
		{
            OS_EventTaskRdy(pevent, msg, OS_STAT_Q);  /* No,  Post to HPT waiting on queue             */
        }
        OS_EXIT_CRITICAL();
	//	执行任务调度。
        OS_Sched();                                   /* Find highest priority task ready to run       */
        return (OS_NO_ERR);
    }
//	无等待的任务
//	获得该队列的控制块
    pq = (OS_Q *)pevent->OSEventPtr;                  /* Point to queue control block                  */
//	若队列中消息数量超过队列大小，提示队列已满。
	if (pq->OSQEntries >= pq->OSQSize) 
	{              /* Make sure queue is not full                   */
        OS_EXIT_CRITICAL();
        return (OS_Q_FULL);
    }
//	将消息插入队列的首部
    if ((opt & OS_POST_OPT_FRONT) != 0x00) 
	{          /* Do we post to the FRONT of the queue?         */
        if (pq->OSQOut == pq->OSQStart) 
		{             /* Yes, Post as LIFO, Wrap OUT pointer if we ... */
            pq->OSQOut = pq->OSQEnd;                  /*      ... are at the 1st queue entry           */
        }
        pq->OSQOut--;
        *pq->OSQOut = msg;                            /*      Insert message into queue                */
    } 
//	将消息插入队列的尾部
	else 
	{                                          /* No,  Post as FIFO                             */
        *pq->OSQIn++ = msg;                           /*      Insert message into queue                */
        if (pq->OSQIn == pq->OSQEnd) 
		{                /*      Wrap IN ptr if we are at end of queue    */
            pq->OSQIn = pq->OSQStart;
        }
    }
//	更新队列中消息的数量。
	pq->OSQEntries++;                                 /* Update the nbr of entries in the queue        */
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                        QUERY A MESSAGE QUEUE
*
* Description: This function obtains information about a message queue.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              pdata         is a pointer to a structure that will contain information about the message
*                            queue.
*
* Returns    : OS_NO_ERR           The call was successful and the message was sent
*              OS_ERR_EVENT_TYPE   If you are attempting to obtain data from a non queue.
*              OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer
*********************************************************************************************************
*/

#if OS_Q_QUERY_EN > 0
INT8U  OSQQuery (OS_EVENT *pevent, OS_Q_DATA *pdata)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif
    OS_Q      *pq;
    INT8U     *psrc;
    INT8U     *pdest;


#if OS_ARG_CHK_EN > 0
    if (pevent == (OS_EVENT *)0) 
	{                         /* Validate 'pevent'                        */
        return (OS_ERR_PEVENT_NULL);
    }
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) 
	{          /* Validate event block type                */
        return (OS_ERR_EVENT_TYPE);
    }
#endif
    OS_ENTER_CRITICAL();
    pdata->OSEventGrp = pevent->OSEventGrp;                /* Copy message queue wait list           */
    psrc              = &pevent->OSEventTbl[0];
    pdest             = &pdata->OSEventTbl[0];
#if OS_EVENT_TBL_SIZE > 0
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 1
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 2
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 3
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 4
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 5
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 6
    *pdest++          = *psrc++;
#endif

#if OS_EVENT_TBL_SIZE > 7
    *pdest            = *psrc;
#endif
    pq = (OS_Q *)pevent->OSEventPtr;
    if (pq->OSQEntries > 0) 
	{
	//	存储下一个要提取的消息。
        pdata->OSMsg = *pq->OSQOut;                        /* Get next message to return if available  */
    } 
	else 
	{
	//	若队列中无消息，则返回NULL。
        pdata->OSMsg = (void *)0;
    }
    pdata->OSNMsgs = pq->OSQEntries;
    pdata->OSQSize = pq->OSQSize;
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif                                                     /* OS_Q_QUERY_EN                            */

/*$PAGE*/
/*
*********************************************************************************************************
*                                      QUEUE MODULE INITIALIZATION
*
* Description : This function is called by uC/OS-II to initialize the message queue module.  Your
*               application MUST NOT call this function.
*
* Arguments   :  none
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
//将空闲控制块链表串联起来。
void  OS_QInit (void)
{
//	该队列仅仅1个元素
#if OS_MAX_QS == 1
    OSQFreeList         = &OSQTbl[0];            /* Only ONE queue!                                    */
    OSQFreeList->OSQPtr = (OS_Q *)0;
#endif
//	该队列至少2个元素
#if OS_MAX_QS >= 2
    INT16U  i;
    OS_Q   *pq1;
    OS_Q   *pq2;


    pq1 = &OSQTbl[0];
    pq2 = &OSQTbl[1];
    for (i = 0; i < (OS_MAX_QS - 1); i++) 
	{      /* Init. list of free QUEUE control blocks            */
        pq1->OSQPtr = pq2;
        pq1++;
        pq2++;
    }
    pq1->OSQPtr = (OS_Q *)0;
    OSQFreeList = &OSQTbl[0];
#endif
}
#endif                                                     /* OS_Q_EN                                  */

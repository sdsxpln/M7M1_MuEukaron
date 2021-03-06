/******************************************************************************
Filename    : platform_cmx.c
Author      : pry
Date        : 01/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The hardware abstraction layer for Cortex-M microcontrollers.
******************************************************************************/

/* Includes ******************************************************************/
#define __HDR_DEFS__
#include "Kernel/kernel.h"
#include "Kernel/kotbl.h"
#include "Kernel/captbl.h"
#include "Kernel/pgtbl.h"
#include "Kernel/prcthd.h"
#include "Kernel/siginv.h"
#include "Platform/CortexM/platform_cmx.h"
#undef __HDR_DEFS__

#define __HDR_STRUCTS__
#include "Platform/CortexM/platform_cmx.h"
#include "Kernel/captbl.h"
#include "Kernel/pgtbl.h"
#include "Kernel/prcthd.h"
#include "Kernel/siginv.h"
#undef __HDR_STRUCTS__

/* Private include */
#include "Platform/CortexM/platform_cmx.h"

#define __HDR_PUBLIC_MEMBERS__
#include "Kernel/kernel.h"
#include "Kernel/captbl.h"
#include "Kernel/pgtbl.h"
#include "Kernel/prcthd.h"
#include "Kernel/siginv.h"
#undef __HDR_PUBLIC_MEMBERS__
/* End Includes **************************************************************/

/* Begin Function:__RME_Comp_Swap *********************************************
Description : The compare-and-swap atomic instruction. If the *Old value is equal to
              *Ptr, then set the *Ptr as New and return 1; else set the *Old as *Ptr,
              and return 0.
              On Cortex-M there is only one core. There's basically no need to do
              anything special, and we just disable interrupt for a very short time.
Input       : ptr_t* Ptr - The pointer to the data.
              ptr_t* Old - The old value.
              ptr_t New - The new value.
Output      : ptr_t* Ptr - The pointer to the data.
              ptr_t* Old - The old value.
Return      : ptr_t - If successful, 1; else 0.
******************************************************************************/
ptr_t __RME_Comp_Swap(ptr_t* Ptr, ptr_t* Old, ptr_t New)
{
    __RME_Disable_Int();
    if(*Ptr==*Old)
    {
        *Ptr=New;
        __RME_Enable_Int();
        return 1;
    }
    *Old=*Ptr;
    __RME_Enable_Int();
    
    return 0;
}
/* End Function:__RME_Comp_Swap **********************************************/

/* Begin Function:__RME_Fetch_Add *********************************************
Description : The fetch-and-add atomic instruction. Increase the value that is 
              pointed to by the pointer, and return the value before addition.
              On Cortex-M there is only one core. There's basically no need to do
              anything special, and we just disable interrupt for a very short time.
Input       : ptr_t* Ptr - The pointer to the data.
              cnt_t Addend - The number to add.
Output      : ptr_t* Ptr - The pointer to the data.
Return      : ptr_t - The value before the addition.
******************************************************************************/
ptr_t __RME_Fetch_Add(ptr_t* Ptr, cnt_t Addend)
{
    ptr_t Old;
    
    __RME_Disable_Int();
    Old=*Ptr;
    *Ptr=Old+Addend;
    __RME_Enable_Int();
    
    return Old;
}
/* End Function:__RME_Fetch_Add **********************************************/

/* Begin Function:__RME_Fetch_And *********************************************
Description : The fetch-and-logic-and atomic instruction. Logic AND the pointer
              value with the operand, and return the value before logic AND.
              On Cortex-M there is only one core. There's basically no need to do
              anything special.
Input       : ptr_t* Ptr - The pointer to the data.
              cnt_t Operand - The number to logic AND with the destination.
Output      : ptr_t* Ptr - The pointer to the data.
Return      : ptr_t - The value before the AND operation.
******************************************************************************/
ptr_t __RME_Fetch_And(ptr_t* Ptr, ptr_t Operand)
{
    ptr_t Old;
    
    __RME_Disable_Int();
    Old=*Ptr;
    *Ptr=Old&Operand;
    __RME_Enable_Int();
    
    return Old;
}
/* End Function:__RME_Fetch_And **********************************************/

/* Begin Function:__RME_Putchar ***********************************************
Description : Output a character to console. In Cortex-M, under most circumstances, 
              we should use the ITM for such outputs.
Input       : char Char - The character to print.
Output      : None.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t __RME_Putchar(char Char)
{
    RME_CMX_PUTCHAR(Char);
    return 0;
}
/* End Function:__RME_Putchar ************************************************/

/* Begin Function:__RME_Low_Level_Init ****************************************
Description : Initialize the low-level hardware. Currently this function works on
              Cortex-M7 only.
Input       : None.
Output      : None.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t __RME_Low_Level_Init(void)
{
    RME_CMX_LOW_LEVEL_INIT();
    
    /* Enable the MPU */
    SCB->SHCSR&=~SCB_SHCSR_MEMFAULTENA_Msk;
    MPU->CTRL&=~MPU_CTRL_ENABLE_Msk;
    MPU->CTRL=RME_CMX_MPU_PRIVDEF|MPU_CTRL_ENABLE_Msk;
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
    
    /* Enable all fault handlers */
    SCB->SHCSR|=RME_CMX_SHCSR_USGFAULTENA|RME_CMX_SHCSR_BUSFAULTENA|RME_CMX_SHCSR_MEMFAULTENA;
    
    /* Set the priority of timer, svc and faults to the lowest */
    NVIC_SetPriorityGrouping(RME_CMX_NVIC_GROUPING);
    NVIC_SetPriority(SVCall_IRQn, 0xFF);
    NVIC_SetPriority(PendSV_IRQn, 0xFF);
    NVIC_SetPriority(SysTick_IRQn, 0xFF);
    NVIC_SetPriority(BusFault_IRQn, 0xFF);
    NVIC_SetPriority(UsageFault_IRQn, 0xFF);
    NVIC_SetPriority(DebugMonitor_IRQn, 0xFF);
    
    /* Configure systick */
    SysTick_Config(RME_CMX_SYSTICK_VAL);
    return 0;
}
/* End Function:__RME_Low_Level_Init *****************************************/

/* Begin Function:main ********************************************************
Description : The entrance of the operating system. This function is for compatibility
              with the ARM toolchain.
Input       : None.
Output      : None.
Return      : int - This function never returns.
******************************************************************************/
int main(void)
{
    /* The main function of the kernel - we will start our kernel boot here */
    _RME_Kmain(RME_KMEM_STACK_ADDR);
}
/* End Function:main *********************************************************/

/* Begin Function:__RME_Boot **************************************************
Description : Boot the first process in the system.
Input       : None.
Output      : None.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t __RME_Boot(void)
{
    ptr_t Cur_Addr;
    
    Cur_Addr=RME_KMEM_VA_START;
    
    /* Create the capability table for the init process */
    RME_ASSERT(_RME_Captbl_Boot_Crt(RME_BOOT_CAPTBL,Cur_Addr,18)==0);
    Cur_Addr+=RME_KOTBL_ROUND(RME_CAPTBL_SIZE(18));
    
    /* Create the page table for the init process, and map in the page alloted for it */
    /* The top-level page table - covers 4G address range */
    RME_ASSERT(_RME_Pgtbl_Boot_Crt(RME_CMX_CPT, RME_BOOT_CAPTBL, RME_BOOT_PGTBL, 
               Cur_Addr, 0x00000000, RME_PGTBL_TOP, RME_PGTBL_SIZE_512M, RME_PGTBL_NUM_8)==0);
    Cur_Addr+=RME_KOTBL_ROUND(RME_PGTBL_SIZE_TOP(RME_PGTBL_NUM_8));
    /* Other memory regions will be directly added, because we do not protect them in the init process */
    RME_ASSERT(_RME_Pgtbl_Boot_Add(RME_CMX_CPT, RME_BOOT_PGTBL, 0x00000000, 0, RME_PGTBL_ALL_PERM)==0);
    RME_ASSERT(_RME_Pgtbl_Boot_Add(RME_CMX_CPT, RME_BOOT_PGTBL, 0x20000000, 1, RME_PGTBL_ALL_PERM)==0);
    RME_ASSERT(_RME_Pgtbl_Boot_Add(RME_CMX_CPT, RME_BOOT_PGTBL, 0x40000000, 2, RME_PGTBL_ALL_PERM)==0);
    RME_ASSERT(_RME_Pgtbl_Boot_Add(RME_CMX_CPT, RME_BOOT_PGTBL, 0x60000000, 3, RME_PGTBL_ALL_PERM)==0);
    RME_ASSERT(_RME_Pgtbl_Boot_Add(RME_CMX_CPT, RME_BOOT_PGTBL, 0x80000000, 4, RME_PGTBL_ALL_PERM)==0);
    RME_ASSERT(_RME_Pgtbl_Boot_Add(RME_CMX_CPT, RME_BOOT_PGTBL, 0xA0000000, 5, RME_PGTBL_ALL_PERM)==0);
    RME_ASSERT(_RME_Pgtbl_Boot_Add(RME_CMX_CPT, RME_BOOT_PGTBL, 0xC0000000, 6, RME_PGTBL_ALL_PERM)==0);
    RME_ASSERT(_RME_Pgtbl_Boot_Add(RME_CMX_CPT, RME_BOOT_PGTBL, 0xE0000000, 7, RME_PGTBL_ALL_PERM)==0);
    
    /* Activate the first process - This process cannot be deleted */
    RME_ASSERT(_RME_Proc_Boot_Crt(RME_CMX_CPT, RME_BOOT_CAPTBL, RME_BOOT_INIT_PROC, 
                                  RME_BOOT_CAPTBL, RME_BOOT_PGTBL, Cur_Addr)==0);
    Cur_Addr+=RME_KOTBL_ROUND(RME_PROC_SIZE);
    
    /* Create the initial kernel function capability, and kernel memory capability */
    RME_ASSERT(_RME_Kern_Boot_Crt(RME_CMX_CPT, RME_BOOT_CAPTBL, RME_BOOT_INIT_KERN)==0);
    RME_ASSERT(_RME_Kmem_Boot_Crt(RME_CMX_CPT, RME_BOOT_CAPTBL, RME_BOOT_INIT_KMEM)==0);
    
    /* Create the initial kernel endpoint for timer ticks */
    RME_Tick_Sig[0]=(struct RME_Sig_Struct*)Cur_Addr;
    RME_ASSERT(_RME_Sig_Boot_Crt(RME_CMX_CPT, RME_BOOT_CAPTBL, RME_BOOT_INIT_TIMER, Cur_Addr)==0);
    Cur_Addr+=RME_KOTBL_ROUND(RME_SIG_SIZE);
    
    /* Create the initial kernel endpoint for thread faults */
    RME_Fault_Sig[0]=(struct RME_Sig_Struct*)Cur_Addr;
    RME_ASSERT(_RME_Sig_Boot_Crt(RME_CMX_CPT, RME_BOOT_CAPTBL, RME_BOOT_INIT_FAULT, Cur_Addr)==0);
    Cur_Addr+=RME_KOTBL_ROUND(RME_SIG_SIZE);
    
    /* Create the initial kernel endpoint for all other interrupts */
    RME_Int_Sig[0]=(struct RME_Sig_Struct*)Cur_Addr;
    RME_ASSERT(_RME_Sig_Boot_Crt(RME_CMX_CPT, RME_BOOT_CAPTBL, RME_BOOT_INIT_INT, Cur_Addr)==0);
    Cur_Addr+=RME_KOTBL_ROUND(RME_SIG_SIZE);
    
    /* Clean up the region for interrupts */
    _RME_Clear((void*)RME_CMX_INT_FLAG_ADDR,sizeof(struct __RME_CMX_Flags));
    
    /* Activate the first thread, and set its priority */
    RME_ASSERT(_RME_Thd_Boot_Crt(RME_CMX_CPT, RME_BOOT_CAPTBL, RME_BOOT_INIT_THD,
                                 RME_BOOT_INIT_PROC, Cur_Addr, 0)==0);
    Cur_Addr+=RME_KOTBL_ROUND(RME_THD_SIZE);
    
    /* Before we go into user level, make sure that the kernel object allocation is within the limits */
    RME_ASSERT(Cur_Addr<RME_CMX_KMEM_BOOT_FRONTIER);
    /* Enable the MPU & interrupt */
    __RME_Pgtbl_Set(RME_CAP_GETOBJ(RME_Cur_Thd[RME_CPUID()]->Sched.Proc->Pgtbl,ptr_t));
    __RME_Enable_Int();
    /* Boot into the init thread */
    __RME_Enter_User_Mode(RME_CMX_INIT_ENTRY, RME_CMX_INIT_STACK);
    return 0;
}
/* End Function:__RME_Boot ***************************************************/

/* Begin Function:__RME_Reboot ************************************************
Description : Reboot the machine, abandon all operating system states.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void __RME_Reboot(void)
{
    /* While(1) loop */
    RME_ASSERT(RME_WORD_BITS!=RME_POW2(RME_WORD_ORDER));
}
/* End Function:__RME_Reboot *************************************************/

/* Begin Function:__RME_Shutdown **********************************************
Description : Shutdown the machine, abandon all operating system states.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void __RME_Shutdown(void)
{
    /* While(1)loop */
    RME_ASSERT(RME_WORD_BITS!=RME_POW2(RME_WORD_ORDER));
}
/* End Function:__RME_Shutdown ***********************************************/

/* Begin Function:__RME_CPUID_Get *********************************************
Description : Get the CPUID. This is to identify where we are executing.
Input       : None.
Output      : None.
Return      : ptr_t - The CPUID. On Cortex-M, this is certainly always 0.
******************************************************************************/
ptr_t __RME_CPUID_Get(void)
{
    return 0;
}
/* End Function:__RME_CPUID_Get **********************************************/

/* Begin Function:__RME_Get_Syscall_Param *************************************
Description : Get the system call parameters from the stack frame.
Input       : struct RME_Reg_Struct* Reg - The register set.
Output      : ptr_t* Svc - The system service number.
              ptr_t* Capid - The capability ID number.
              ptr_t* Param - The parameters.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t __RME_Get_Syscall_Param(struct RME_Reg_Struct* Reg, ptr_t* Svc, ptr_t* Capid, ptr_t* Param)
{
    *Svc=(Reg->R4)>>16;
    *Capid=(Reg->R4)&0xFFFF;
    Param[0]=Reg->R5;
    Param[1]=Reg->R6;
    Param[2]=Reg->R7;
    return 0;
}
/* End Function:__RME_Get_Syscall_Param **************************************/

/* Begin Function:__RME_Set_Syscall_Retval ************************************
Description : Set the system call return value to the stack frame. This function 
              may carry up to 4 return values. If the last 3 is not needed, just set
              them to zero.
Input       : ret_t Retval - The return value.
Output      : struct RME_Reg_Struct* Reg - The register set.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t __RME_Set_Syscall_Retval(struct RME_Reg_Struct* Reg, ret_t Retval)
{
    Reg->R4=(ptr_t)Retval;
    return 0;
}
/* End Function:__RME_Set_Syscall_Retval *************************************/

/* Begin Function:__RME_Get_Inv_Retval ****************************************
Description : Get the invocation return value from the stack frame.
Input       : struct RME_Reg_Struct* Reg - The register set.
Output      : None.
Return      : ptr_t - The return value.
******************************************************************************/
ptr_t __RME_Get_Inv_Retval(struct RME_Reg_Struct* Reg)
{
    return Reg->R5;
}
/* End Function:__RME_Get_Inv_Retval *****************************************/

/* Begin Function:__RME_Set_Inv_Retval ****************************************
Description : Set the invocation return value to the stack frame.
Input       : ret_t Retval - The return value.
Output      : struct RME_Reg_Struct* Reg - The register set.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t __RME_Set_Inv_Retval(struct RME_Reg_Struct* Reg, ret_t Retval)
{
    Reg->R5=(ptr_t)Retval;
    return 0;
}
/* End Function:__RME_Set_Inv_Retval *****************************************/

/* Begin Function:__RME_Thd_Reg_Init ******************************************
Description : Initialize the register set for the thread.
Input       : ptr_t Entry - The thread entry address.
              ptr_t Stack - The thread stack address.
Output      : struct RME_Reg_Struct* Reg - The register set content generated.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t __RME_Thd_Reg_Init(ptr_t Entry, ptr_t Stack, struct RME_Reg_Struct* Reg)
{
    /* Set the LR to a value indicating that we have never used FPU in this new task */
    Reg->LR=0xFFFFFFFD;
    /* Here the process address is not passed because we only have one entry point anyway */
    Reg->R4=Entry;
    /* Put something in the SP later */
    Reg->SP=Stack;
    return 0;
}
/* End Function:__RME_Thd_Reg_Init *******************************************/

/* Begin Function:__RME_Thd_Reg_Read ******************************************
Description : Read the SP, PC and LR for the thread. On Cortex-M, R4 is read instead of
              PC because we will use R4 to load the PC in some circumstances.
Input       : struct RME_Reg_Struct* Reg - The current register set content.
Output      : ptr_t* Entry - The current PC address.
              ptr_t* Stack - The current SP address.
              ptr_t* Status - The current status word.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t __RME_Thd_Reg_Read(ptr_t* Entry, ptr_t* Stack, ptr_t* Status, struct RME_Reg_Struct* Reg)
{
    *Entry=Reg->R4;
    *Stack=Reg->SP;
    *Status=Reg->LR;
    return 0;
}
/* End Function:__RME_Thd_Reg_Read *******************************************/

/* Begin Function:__RME_Thd_Reg_Copy ******************************************
Description : Copy one set of registers into another.
Input       : struct RME_Reg_Struct* Src - The source register set.
Output      : struct RME_Reg_Struct* Dst - The destination register set.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t __RME_Thd_Reg_Copy(struct RME_Reg_Struct* Dst, struct RME_Reg_Struct* Src)
{
    /* Make sure that the ordering is the same so the compiler can optimize */
    Dst->SP=Src->SP;
    Dst->R4=Src->R4;
    Dst->R5=Src->R5;
    Dst->R6=Src->R6;
    Dst->R7=Src->R7;
    Dst->R8=Src->R8;
    Dst->R9=Src->R9;
    Dst->R10=Src->R10;
    Dst->R11=Src->R11;
    Dst->LR=Src->LR;
    return 0;
}
/* End Function:__RME_Thd_Reg_Copy *******************************************/

/* Begin Function:__RME_Thd_Cop_Init ******************************************
Description : Initialize the coprocessor register set for the thread.
Input       : ptr_t Entry - The thread entry address.
              ptr_t Stack - The thread stack address.
Output      : struct RME_Reg_Cop_Struct* Cop_Reg - The register set content generated.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t __RME_Thd_Cop_Init(ptr_t Entry, ptr_t Stack, struct RME_Cop_Struct* Cop_Reg)
{
    /* Empty function, return immediately. The FPU contents is not predictable */
    return 0;
}
/* End Function:__RME_Thd_Cop_Reg_Init ***************************************/

/* Begin Function:__RME_Thd_Cop_Save ******************************************
Description : Save the co-op register sets. This operation is flexible - If the
              program does not use the FPU, we do not save its context.
Input       : struct RME_Reg_Struct* Reg - The context, used to decide whether
                                           to save the context of the coprocessor.
Output      : s struct RME_Cop_Struct* Cop_Reg - The pointer to the coprocessor contents.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t __RME_Thd_Cop_Save(struct RME_Reg_Struct* Reg, struct RME_Cop_Struct* Cop_Reg)
{
    /* If we do not have a FPU, return 0 directly */
#ifdef RME_CMX_FPU_TYPE
#if(RME_CMX_FPU_TYPE!=RME_CMX_FPU_NONE)
    /* If this is a standard frame which does not contain FPU usage&context */
    if(((Reg->LR)&RME_CMX_EXC_RET_STD_FRAME)!=0)
        return 0;
    /* Not. We save the context of FPU */
    ___RME_CMX_Thd_Cop_Save(Cop_Reg);
#endif
#endif
    return 0;
}
/* End Function:__RME_Thd_Cop_Save *******************************************/

/* Begin Function:__RME_Thd_Cop_Restore ***************************************
Description : Restore the co-op register sets. This operation is flexible - If the
              FPU is not used, we do not restore its context.
Input       : struct RME_Reg_Struct* Reg - The context, used to decide whether
                                           to save the context of the coprocessor.
Output      : s struct RME_Cop_Struct* Cop_Reg - The pointer to the coprocessor contents.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t __RME_Thd_Cop_Restore(struct RME_Reg_Struct* Reg, struct RME_Cop_Struct* Cop_Reg)
{    
/* If we do not have a FPU, return 0 directly */
#ifdef RME_CMX_FPU_TYPE
#if(RME_CMX_FPU_TYPE!=RME_CMX_FPU_NONE)
    /* If this is a standard frame which does not contain FPU usage&context */
    if(((Reg->LR)&RME_CMX_EXC_RET_STD_FRAME)!=0)
        return 0;
    /* Not. We restore the context of FPU */
    ___RME_CMX_Thd_Cop_Restore(Cop_Reg);
#endif
#endif
    return 0;
}
/* End Function:__RME_Thd_Cop_Restore ****************************************/

/* Begin Function:__RME_Inv_Reg_Init ******************************************
Description : Initialize the register set for the invocation.
Input       : ptr_t Param - The parameter.
Output      : struct RME_Reg_Struct* Reg - The register set content generated.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t __RME_Inv_Reg_Init(ptr_t Param, struct RME_Reg_Struct* Reg)
{
    Reg->R6=Param;
    return 0;
}
/* End Function:__RME_Inv_Reg_Init *******************************************/

/* Begin Function:__RME_Inv_Cop_Init ******************************************
Description : Initialize the coprocessor register set for the invocation.
Input       : ptr_t Param - The parameter.
Output      : struct RME_Reg_Struct* Reg - The register set content generated.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t __RME_Inv_Cop_Init(ptr_t Param, struct RME_Cop_Struct* Cop_Reg)
{
    /* Empty function */
    return 0;
}
/* End Function:__RME_Inv_Cop_Init *******************************************/

/* Begin Function:__RME_Kern_Func_Handler *************************************
Description : Initialize the coprocessor register set for the invocation.
Input       : struct RME_Reg_Struct* Reg - The current register set.
              ptr_t Func_ID - The function ID.
              ptr_t Param1 - The first parameter.
              ptr_t Param2 - The second parameter.
Output      : None.
Return      : ptr_t - The value that the function returned.
******************************************************************************/
ptr_t __RME_Kern_Func_Handler(struct RME_Reg_Struct* Reg, ptr_t Func_ID,
                              ptr_t Param1, ptr_t Param2)
{
    /* It must be interrupt-related operations */
    if(Func_ID<240)
    {
        if(Param1==RME_CMX_INT_OP)
        {
            if(Param2==RME_CMX_INT_ENABLE)
            {
                NVIC_DisableIRQ((IRQn_Type)Func_ID);
                /* When the IRQ is newly enabled, we set its priority to as low as the rest as always */
                NVIC_SetPriority((IRQn_Type)Func_ID, 0xFF);
                NVIC_EnableIRQ((IRQn_Type)Func_ID);
            }
            else
                NVIC_DisableIRQ((IRQn_Type)Func_ID);
            
            __RME_Set_Syscall_Retval(Reg,0);
            return 0;
        }
        else if(Param1==RME_CMX_INT_PRIO)
        {
            /* Only changing the subpriority is allowed. main priority is as low as the rest */
            NVIC_SetPriority((IRQn_Type)Func_ID,((0xFF<<(RME_CMX_NVIC_GROUPING+1))|Param2)&0xFF);
            __RME_Set_Syscall_Retval(Reg,0);
            return 0;
        }
    }
    else if(Func_ID==240)
    {
        /* Wait for interrupt to happen */
        __RME_CMX_WFI();
        __RME_Set_Syscall_Retval(Reg,0);
        return 0;
    }
    
    /* If it gets here, we must have failed */
    return RME_ERR_PGT_OPFAIL;
}
/* End Function:__RME_Kern_Func_Handler **************************************/

/* Begin Function:___RME_Pgtbl_MPU_Gen_RASR ***********************************
Description : Generate the RASR metadata for this level of page table.
Input       : ptr_t* Table - The table to generate data for. This is directly the
                             raw page table itself, without accounting for metadata.
              ptr_t Flags - The flags for each entry.
Output      : struct __RME_CMX_MPU_Entry* Entry - The data generated.
Return      : ptr_t - The RASR value returned.
******************************************************************************/
ptr_t ___RME_Pgtbl_MPU_Gen_RASR(ptr_t* Table, ptr_t Flags, ptr_t Entry_Size_Order)
{
    ptr_t RASR;
    ptr_t Count;
    
    /* Get the SRD part first */
    RASR=0;
    for(Count=0;Count<8;Count++)
    {
        if(((Table[Count]&RME_CMX_PGTBL_PRESENT)!=0)&&((Table[Count]&RME_CMX_PGTBL_TERMINAL)!=0))
            RASR|=(((ptr_t)1)<<(Count+8));
    }
    if(RASR==0)
        return 0;
    
    RASR=RME_CMX_MPU_SRDCLR&(~RASR);
    RASR|=RME_CMX_MPU_SZENABLE;
    /* Is it read-only? - we do not care if the read bit is set, because it is always readable anyway */
    if((Flags&RME_PGTBL_WRITE)!=0)
        RASR|=RME_CMX_MPU_RW;
    else
        RASR|=RME_CMX_MPU_RO;
    /* Can we fetch instructions from there? */
    if((Flags&RME_PGTBL_EXECUTE)==0)
        RASR|=RME_CMX_MPU_XN;
    /* Is the area cacheable? */
    if((Flags&RME_PGTBL_CACHEABLE)!=0)
        RASR|=RME_CMX_MPU_CACHEABLE;
    /* Is the area bufferable? */
    if((Flags&RME_PGTBL_BUFFERABLE)!=0)
        RASR|=RME_CMX_MPU_BUFFERABLE;
    /* What is the region size? */
    RASR|=RME_CMX_MPU_REGIONSIZE(Entry_Size_Order);
    
    return RASR;
}
/* End Function:___RME_Pgtbl_MPU_Gen_RASR ************************************/

/* Begin Function:___RME_Pgtbl_MPU_Clear **************************************
Description : Clear the MPU setting of this directory. If it exists, clear it;
              If it does not exist, don't do anything.
Input       : struct __RME_CMX_MPU_Data* Top_MPU - The top-level MPU metadata
              ptr_t Start_Addr - The start mapping address of the directory.
              ptr_t Size_Order - The size order of each entry in the directory.
Output      : None.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t ___RME_Pgtbl_MPU_Clear(struct __RME_CMX_MPU_Data* Top_MPU, 
                             ptr_t Start_Addr, ptr_t Size_Order)
{
    ptr_t Count;
    
    for(Count=0;Count<RME_CMX_MPU_REGIONS;Count++)
    {
        if((Top_MPU->State&(((ptr_t)1)<<Count))!=0)
        {
            /* We got one MPU region valid here */
            if((RME_CMX_MPU_ADDR(Top_MPU->Data[Count].MPU_RBAR)==Start_Addr)&&
               (RME_CMX_MPU_SZORD(Top_MPU->Data[Count].MPU_RASR)==Size_Order))
            {
                /* Clean it up and return */
                Top_MPU->Data[Count].MPU_RBAR=RME_CMX_MPU_VALID|Count;
                Top_MPU->Data[Count].MPU_RASR=0;
                Top_MPU->State&=~(((ptr_t)1)<<Count);
                /* Maybe no need to clean the static flag */
                Top_MPU->State&=~(((ptr_t)1)<<(Count+16));
                return 0;
            }
        }
    }
    
    return 0;
}
/* End Function:___RME_Pgtbl_MPU_Clear ***************************************/

/* Begin Function:___RME_Pgtbl_MPU_Add ****************************************
Description : Add or update the MPU entry in the top-level MPU table. MPU region
              0 is always reserved for dynamic pages.
Input       : struct __RME_CMX_MPU_Data* Top_MPU - The top-level MPU metadata
              ptr_t Start_Addr - The start mapping address of the directory.
              ptr_t Size_Order - The size order of each entry in the directory.
              ptr_t MPU_RASR - The RASR register content, if set.
              ptr_t Static_Flag - The flag denoting if this entry is static.
Output      : None.
Return      : ptr_t - Always 0.
******************************************************************************/
ptr_t ___RME_Pgtbl_MPU_Add(struct __RME_CMX_MPU_Data* Top_MPU, 
                           ptr_t Start_Addr, ptr_t Size_Order,
                           ptr_t MPU_RASR, ptr_t Static_Flag)
{
    ptr_t Count;
    /* The last empty slot */
    ptr_t Last_Empty;
    /* The last dynamic slot */
    ptr_t Last_Dynamic;
    
    /* Set these values to some overrange value */
    Last_Empty=RME_CMX_MPU_REGIONS;
    Last_Dynamic=RME_CMX_MPU_REGIONS;
    for(Count=0;Count<RME_CMX_MPU_REGIONS;Count++)
    {
        if((Top_MPU->State&(((ptr_t)1)<<Count))!=0)
        {
            if((Top_MPU->State&(((ptr_t)1)<<(Count+16)))==0)
                Last_Dynamic=Count;
            /* We got one MPU region valid here */
            if((RME_CMX_MPU_ADDR(Top_MPU->Data[Count].MPU_RBAR)==Start_Addr)&&
               (RME_CMX_MPU_SZORD(Top_MPU->Data[Count].MPU_RASR)==Size_Order))
            {
                /* Update the RASR - all flag changes except static are reflected here */
                Top_MPU->Data[Count].MPU_RASR=MPU_RASR;
                /* Static or not is reflected in the state */
                if(Static_Flag!=0)
                    Top_MPU->State|=((ptr_t)1)<<(Count+16);
                else
                    Top_MPU->State&=~(((ptr_t)1)<<(Count+16));
                return 0;
            }
        }
        else
            Last_Empty=Count;
    }
    
    /* Update unsuccessful, we didn't find any match. We will need a new slot. 
     * See if there are any new empty slots that we can use. 
     * See if the last empty slot is 0. If yes, we can only map dynamic pages */
    if((Last_Empty!=RME_CMX_MPU_REGIONS)&&((Last_Empty!=0)||(Static_Flag==0)))
    {
        /* Put the data to this slot */
        Top_MPU->State|=((ptr_t)1)<<(Last_Empty);
        Top_MPU->Data[Last_Empty].MPU_RBAR=RME_CMX_MPU_ADDR(Start_Addr)|RME_CMX_MPU_VALID|Last_Empty;
        Top_MPU->Data[Last_Empty].MPU_RASR=MPU_RASR;
        /* Static or not is reflected in the state */
        if(Static_Flag!=0)
            Top_MPU->State|=((ptr_t)1)<<(Last_Empty+16);
        else
            Top_MPU->State&=~(((ptr_t)1)<<(Last_Empty+16));
        
        return 0;
    }
    
    /* No empty slots exist. We must replace a dynamic region */
    if((Last_Dynamic!=RME_CMX_MPU_REGIONS)&&((Last_Dynamic!=0)||(Static_Flag==0)))
    {
        /* Put the data to this slot */
        Top_MPU->State|=((ptr_t)1)<<(Last_Empty);
        Top_MPU->Data[Last_Empty].MPU_RBAR=RME_CMX_MPU_ADDR(Start_Addr)|RME_CMX_MPU_VALID|Last_Empty;
        Top_MPU->Data[Last_Empty].MPU_RASR=MPU_RASR;
        /* Static or not is reflected in the state */
        if(Static_Flag!=0)
            Top_MPU->State|=((ptr_t)1)<<(Last_Empty+16);
        else
            Top_MPU->State&=~(((ptr_t)1)<<(Last_Empty+16));
        
        return 0;
    }
    
    /* All effort is futile. we report failure */
    return RME_ERR_PGT_OPFAIL;
}
/* End Function:___RME_Pgtbl_MPU_Add *****************************************/

/* Begin Function:___RME_Pgtbl_MPU_Update *************************************
Description : Update the top-level MPU metadata for this level of page table.
Input       : struct __RME_CMX_Pgtbl_Meta* Meta - This page table.
              ptr_t Op_Flag - The operation flag. 1 for add, 0 for clean.
Output      : None.
Return      : ptr_t - If successful, 0; else RME_ERR_PGT_OPFAIL.
******************************************************************************/
ptr_t ___RME_Pgtbl_MPU_Update(struct __RME_CMX_Pgtbl_Meta* Meta, ptr_t Op_Flag)
{
    ptr_t* Table;
    ptr_t MPU_RASR;
    struct __RME_CMX_MPU_Data* Top_MPU;
    
    /* Is it possible for MPU to represent this? */
    if(RME_CMX_PGTBL_NUMORD(Meta->Size_Num_Order)!=3)
        return RME_ERR_PGT_OPFAIL;
    
    /* Get the tables */
    if(Meta->Toplevel!=0)
    {
        /* We have a top-level */
        Top_MPU=(struct __RME_CMX_MPU_Data*)(Meta->Toplevel+sizeof(struct __RME_CMX_Pgtbl_Meta));
        Table=RME_CMX_PGTBL_TBL_NOM((ptr_t*)Meta);
    }
    else if(((Meta->Start_Addr)&RME_PGTBL_TOP)!=0)
    {
        /* We don't have a top-level, but we are the top-level */
        Top_MPU=(struct __RME_CMX_MPU_Data*)(((ptr_t)Meta)+sizeof(struct __RME_CMX_Pgtbl_Meta));
        Table=RME_CMX_PGTBL_TBL_TOP((ptr_t*)Meta);
    }
    else
        return RME_ERR_PGT_OPFAIL;
    
    if(Op_Flag==RME_CMX_MPU_CLR)
    {
        /* Clear the metadata - this function will never fail */
        ___RME_Pgtbl_MPU_Clear(Top_MPU,
                               RME_CMX_PGTBL_START(Meta->Start_Addr),
                               RME_CMX_PGTBL_SIZEORD(Meta->Size_Num_Order));
    }
    else
    {
        /* See if the RASR contains anything */
        MPU_RASR=___RME_Pgtbl_MPU_Gen_RASR(Table, Meta->Page_Flags, 
                                           RME_CMX_PGTBL_SIZEORD(Meta->Size_Num_Order));
        if(MPU_RASR==0)
        {
            /* All pages are unmapped. Clear this from the MPU data */
            ___RME_Pgtbl_MPU_Clear(Top_MPU,
                                   RME_CMX_PGTBL_START(Meta->Start_Addr),
                                   RME_CMX_PGTBL_SIZEORD(Meta->Size_Num_Order));
        }
        else
        {
            /* At least one of the pages are there. Map it */
            if(___RME_Pgtbl_MPU_Add(Top_MPU,
                                    RME_CMX_PGTBL_START(Meta->Start_Addr),
                                    RME_CMX_PGTBL_SIZEORD(Meta->Size_Num_Order),
                                    MPU_RASR,Meta->Page_Flags&RME_PGTBL_STATIC)!=0)
                return RME_ERR_PGT_OPFAIL;
        }
    }
    
    return 0;
}
/* End Function:___RME_Pgtbl_MPU_Update **************************************/

/* Begin Function:__RME_Pgtbl_Set *********************************************
Description : Set the processor's page table.
Input       : ptr_t Pgtbl - The virtual address of the page table.
Output      : None.
Return      : None.
******************************************************************************/
void __RME_Pgtbl_Set(ptr_t Pgtbl)
{
    struct __RME_CMX_MPU_Data* MPU_Data;
    
    MPU_Data=(struct __RME_CMX_MPU_Data*)(Pgtbl+sizeof(struct __RME_CMX_Pgtbl_Meta));
    /* Get the physical address of the page table - here we do not need any conversion,
     * because VA = PA as always. We just need to extract the MPU metadata part
     * and pass it down */
    ___RME_CMX_MPU_Set((ptr_t)(&(MPU_Data->Data[0].MPU_RBAR)));
}
/* End Function:__RME_Pgtbl_Set **********************************************/

/* Begin Function:__RME_CMX_Fault_Handler *************************************
Description : The fault handler of RME. In Cortex-M, this is used to handle multiple
              faults.
Input       : struct RME_Reg_Struct* Reg - The register set when entering the handler.
Output      : struct RME_Reg_Struct* Reg - The register set when exiting the handler.
Return      : None.
******************************************************************************/
void __RME_CMX_Fault_Handler(struct RME_Reg_Struct* Reg)
{
    ptr_t Cur_HFSR;
    ptr_t Cur_CFSR;
    ptr_t Cur_MMFAR;
    ptr_t Flags;
    struct RME_Proc_Struct* Proc;
    struct __RME_CMX_Pgtbl_Meta* Meta;
    
    /* Is it a kernel-level fault? If yes, panic */
    RME_ASSERT((Reg->LR&RME_CMX_EXC_RET_RET_USER)!=0);
    
    /* Get the address of this faulty address, and what caused this fault */
    Cur_HFSR=SCB->HFSR;
    Cur_CFSR=SCB->CFSR;
    Cur_MMFAR=SCB->MMFAR;
    
    /* Are we activating the NMI? If yes, we directly lockup */
    RME_ASSERT((SCB->ICSR&RME_CMX_ICSR_NMIPENDSET)==0);
    /* If this is a hardfault, make sure that it is not the vector table problem */
    RME_ASSERT((Cur_HFSR&RME_CMX_HFSR_VECTTBL)==0);
    /* Is this a escalated hard fault? If yes, we continue processing; If not,
     * see if it can be ignored. If not, we just lockup */
    if((Cur_HFSR&RME_CMX_HFSR_FORCED)!=0)
    {
        /* This must be a debug event */
        RME_ASSERT((Cur_HFSR&RME_CMX_HFSR_DEBUGEVT)!=0);
        return;
    }
    
    /* Can we cover from this? */
    if(((Cur_CFSR&RME_CMX_FAULT_FATAL)!=0)||((Cur_CFSR&RME_CMX_MFSR_MMARVALID)==0))
        __RME_Thd_Fatal(Reg);
    else
    {
        /* See if the fault address can be found in our current page table, and
         * if it is there, we only care about the flags */
        __RME_Thd_Inv_Top_Proc(RME_Cur_Thd[RME_CPUID()], &Proc);
        if(__RME_Pgtbl_Walk(Proc->Pgtbl, Cur_MMFAR, (ptr_t*)(&Meta), 0, 0, 0, 0, &Flags)!=0)
            __RME_Thd_Fatal(Reg);
        else
        {
            /* This fault involves instruction fetch, and that page would not allow this */
            if(((Cur_CFSR&RME_CMX_MFSR_IACCVIOL)!=0)&&((Flags&RME_PGTBL_EXECUTE)==0))
                __RME_Thd_Fatal(Reg);
            else
            {
                /* This must be a dynamic page. Or there must be something wrong in the kernel */
                RME_ASSERT((Flags&RME_PGTBL_STATIC)==0);
                /* Try to update the dynamic page */
                if(___RME_Pgtbl_MPU_Update(Meta, 1)!=0)
                    __RME_Thd_Fatal(Reg);
            }
        }
    }
    /* Clear all bits in these status registers */
    SCB->HFSR=(ptr_t)(-1);
    SCB->CFSR=(ptr_t)(-1);
}
/* End Function:__RME_CMX_Fault_Handler **************************************/

/* Begin Function:__RME_CMX_Generic_Handler ***********************************
Description : The generic interrupt handler of RME for Cortex-M.
Input       : struct RME_Reg_Struct* Reg - The register set when entering the handler.
              ptr_t Int_Num - The interrupt number.
Output      : struct RME_Reg_Struct* Reg - The register set when exiting the handler.
Return      : None.
******************************************************************************/
void __RME_CMX_Generic_Handler(struct RME_Reg_Struct* Reg, ptr_t Int_Num)
{
    struct __RME_CMX_Flag_Set* Flags;

#ifdef RME_CMX_VECT_HOOK
    /* Do in-kernel processing first */
    RME_CMX_VECT_HOOK(Int_Num);
#endif
    
    /* Choose a data structure that is not locked at the moment */
    if(((struct __RME_CMX_Flags*)RME_CMX_INT_FLAG_ADDR)->Set0.Lock==0)
        Flags=&(((struct __RME_CMX_Flags*)RME_CMX_INT_FLAG_ADDR)->Set0);
    else
        Flags=&(((struct __RME_CMX_Flags*)RME_CMX_INT_FLAG_ADDR)->Set1);
    
    /* Set the flags for this interrupt source */
    Flags->Group|=(((ptr_t)1)<<(Int_Num>>RME_WORD_ORDER));
    Flags->Flags[Int_Num>>RME_WORD_ORDER]|=(((ptr_t)1)<<(Int_Num&RME_MASK_END(RME_WORD_ORDER-1)));
    _RME_Kern_Snd(Reg, RME_Int_Sig[RME_CPUID()]);
}
/* End Function:__RME_CMX_Generic_Handler ************************************/

/* Begin Function:__RME_Pgtbl_Kmem_Init ***************************************
Description : Initialize the kernel mapping tables, so it can be added to all the
              top-level page tables. In STM32, we do not need to add such pages.
Input       : None.
Output      : None.
Return      : ptr_t - If successful, 0; else RME_ERR_PGT_OPFAIL.
******************************************************************************/
ptr_t __RME_Pgtbl_Kmem_Init(void)
{
    /* Empty function, always immediately successful */
    return 0;
}
/* End Function:__RME_Pgtbl_Kmem_Init ****************************************/

/* Begin Function:__RME_Pgtbl_Check *******************************************
Description : Check if the page table parameters are feasible, according to the
              parameters. This is only used in page table creation.
Input       : ptr_t Start_Addr - The start mapping address.
              ptr_t Top_Flag - The top-level flag,
              ptr_t Size_Order - The size order of the page directory.
              ptr_t Num_Order - The number order of the page directory.
Output      : None.
Return      : ptr_t - If successful, 0; else RME_ERR_PGT_OPFAIL.
******************************************************************************/
ptr_t __RME_Pgtbl_Check(ptr_t Start_Addr, ptr_t Top_Flag, ptr_t Size_Order, ptr_t Num_Order)
{
    if(Num_Order<RME_PGTBL_NUM_2)
        return RME_ERR_PGT_OPFAIL;
    if(Num_Order>RME_PGTBL_NUM_256)
        return RME_ERR_PGT_OPFAIL;
    if(Size_Order<RME_PGTBL_SIZE_32B)
        return RME_ERR_PGT_OPFAIL;
    if(Size_Order>RME_PGTBL_SIZE_2G)
        return RME_ERR_PGT_OPFAIL;
    
    return 0;
}
/* End Function:__RME_Pgtbl_Check ********************************************/

/* Begin Function:__RME_Pgtbl_Init ********************************************
Description : Initialize the page table data structure, according to the capability.
Input       : struct RME_Cap_Pgtbl* - The capability to the page table to operate on.
Output      : None.
Return      : ptr_t - If successful, 0; else RME_ERR_PGT_OPFAIL.
******************************************************************************/
ptr_t __RME_Pgtbl_Init(struct RME_Cap_Pgtbl* Pgtbl_Op)
{
    cnt_t Count;
    ptr_t* Ptr;
    
    /* Get the actual table */
    Ptr=RME_CAP_GETOBJ(Pgtbl_Op,ptr_t*);
    
    /* Initialize the causal metadata */
    ((struct __RME_CMX_Pgtbl_Meta*)Ptr)->Start_Addr=Pgtbl_Op->Start_Addr;
    ((struct __RME_CMX_Pgtbl_Meta*)Ptr)->Toplevel=0;
    ((struct __RME_CMX_Pgtbl_Meta*)Ptr)->Size_Num_Order=Pgtbl_Op->Size_Num_Order;
    ((struct __RME_CMX_Pgtbl_Meta*)Ptr)->Dir_Page_Count=0;
    Ptr+=sizeof(struct __RME_CMX_Pgtbl_Meta)/sizeof(ptr_t);
    
    /* Is this a top-level? If it is, we need to clean up the MPU data. In MMU
     * environments, if it is top-level, we need to add kernel pages as well */
    if(((Pgtbl_Op->Start_Addr)&RME_PGTBL_TOP)!=0)
    {
        ((struct __RME_CMX_MPU_Data*)Ptr)->State=0;
        
        for(Count=0;Count<8;Count++)
        {
            ((struct __RME_CMX_MPU_Data*)Ptr)->Data[Count].MPU_RBAR=RME_CMX_MPU_VALID|Count;
            ((struct __RME_CMX_MPU_Data*)Ptr)->Data[Count].MPU_RASR=0;
        }
        
        Ptr+=sizeof(struct __RME_CMX_MPU_Data)/sizeof(ptr_t);
    }
    
    /* Clean up the table itself - This is could be virtually unbounded if the user
     * pass in some very large length value */
    for(Count=0;Count<RME_POW2(RME_PGTBL_NUMORD(Pgtbl_Op->Size_Num_Order));Count++)
        Ptr[Count]=0;
    
    return 0;
}
/* End Function:__RME_Pgtbl_Init *********************************************/

/* Begin Function:__RME_Pgtbl_Del_Check ***************************************
Description : Check if the page table can be deleted.
Input       : struct RME_Cap_Pgtbl Pgtbl_Op* - The capability to the page table to operate on.
Output      : None.
Return      : ptr_t - If can be deleted, 0; else RME_ERR_PGT_OPFAIL.
******************************************************************************/
ptr_t __RME_Pgtbl_Del_Check(struct RME_Cap_Pgtbl* Pgtbl_Op)
{
    /* Check if we are standalone */
    if(((RME_CAP_GETOBJ(Pgtbl_Op,struct __RME_CMX_Pgtbl_Meta*)->Dir_Page_Count)>>16)!=0)
        return RME_ERR_PGT_OPFAIL;
    
    if(RME_CAP_GETOBJ(Pgtbl_Op,struct __RME_CMX_Pgtbl_Meta*)->Toplevel!=0)
        return RME_ERR_PGT_OPFAIL;
    
    return 0;
}
/* End Function:__RME_Pgtbl_Del_Check ****************************************/

/* Begin Function:__RME_Pgtbl_Page_Map ****************************************
Description : Map a page into the page table. If a page is mapped into the slot, the
              flags is actually placed on the metadata place because all pages are
              required to have the same flags. We take advantage of this to increase
              the page granularity.
Input       : struct RME_Cap_Pgtbl* - The cap ability to the page table to operate on.
              ptr_t Paddr - The physical address to map to. If we are unmapping, this have no effect.
              ptr_t Pos - The position in the page table.
              ptr_t Flags - The RME standard page attributes. Need to translate them into 
                            architecture specific page table's settings.
Output      : None.
Return      : ptr_t - If successful, 0; else RME_ERR_PGT_OPFAIL.
******************************************************************************/
ptr_t __RME_Pgtbl_Page_Map(struct RME_Cap_Pgtbl* Pgtbl_Op, ptr_t Paddr, ptr_t Pos, ptr_t Flags)
{
    ptr_t* Table;
    struct __RME_CMX_Pgtbl_Meta* Meta;
        
    /* We are doing page-based operations on this, so the page directory should
     * be MPU-representable. Only page sizes of 8 are representable for Cortex-M */
    if(RME_PGTBL_NUMORD(Pgtbl_Op->Size_Num_Order)!=3)
        return RME_ERR_PGT_OPFAIL;
    
    /* Get the metadata */
    Meta=RME_CAP_GETOBJ(Pgtbl_Op,struct __RME_CMX_Pgtbl_Meta*);
    
    /* Where is the entry slot */
    if(((Pgtbl_Op->Start_Addr)&RME_PGTBL_TOP)!=0)
        Table=RME_CMX_PGTBL_TBL_TOP((ptr_t*)Meta);
    else
        Table=RME_CMX_PGTBL_TBL_NOM((ptr_t*)Meta);
    
    /* Check if we are trying to make duplicate mappings into the same location */
    if((Table[Pos]&RME_CMX_PGTBL_PRESENT)!=0)
        return RME_ERR_PGT_OPFAIL;

    /* Trying to map something. Check if the pages flags are consistent. MPU
     * subregions shall share the same flags in Cortex-M */
    if(RME_CMX_PGTBL_PAGENUM(Meta->Dir_Page_Count)==0)
        Meta->Page_Flags=Flags;
    else
    {
        if(Meta->Page_Flags!=Flags)
            return RME_ERR_PGT_OPFAIL;
    }

    /* Register into the page table */
    Table[Pos]=RME_CMX_PGTBL_PRESENT|RME_CMX_PGTBL_TERMINAL|
               RME_ROUND_DOWN(Paddr,RME_PGTBL_SIZEORD(Pgtbl_Op->Size_Num_Order));
   
    /* If we are the top level or we have a top level, and we have static pages mapped in, do MPU updates */
    if((Meta->Toplevel!=0)||(((Pgtbl_Op->Start_Addr)&RME_PGTBL_TOP)!=0))
    {
        if((Flags&RME_PGTBL_STATIC)!=0)
        {
            /* Mapping static pages, update the MPU representation */
            if(___RME_Pgtbl_MPU_Update(Meta, RME_CMX_MPU_UPD)==RME_ERR_PGT_OPFAIL)
            {
                /* MPU update failed. Revert operations */
                Table[Pos]=0;
                return RME_ERR_PGT_OPFAIL;
            }
        }
    }
    /* Modify count */
    RME_CMX_PGTBL_INC_PAGENUM(Meta->Dir_Page_Count);
    
    return 0;
}
/* End Function:__RME_Pgtbl_Page_Map *****************************************/

/* Begin Function:__RME_Pgtbl_Page_Unmap **************************************
Description : Unmap a page from the page table.
Input       : struct RME_Cap_Pgtbl* - The capability to the page table to operate on.
              ptr_t Pos - The position in the page table.
Output      : None.
Return      : ptr_t - If successful, 0; else RME_ERR_PGT_OPFAIL.
******************************************************************************/
ptr_t __RME_Pgtbl_Page_Unmap(struct RME_Cap_Pgtbl* Pgtbl_Op, ptr_t Pos)
{
    ptr_t* Table;
    ptr_t Temp;
    struct __RME_CMX_Pgtbl_Meta* Meta;
        
    /* We are doing page-based operations on this, so the page directory should
     * be MPU-representable. Only page sizes of 8 are representable for Cortex-M */
    if(RME_PGTBL_NUMORD(Pgtbl_Op->Size_Num_Order)!=3)
        return RME_ERR_PGT_OPFAIL;
    
    /* Get the metadata */
    Meta=RME_CAP_GETOBJ(Pgtbl_Op,struct __RME_CMX_Pgtbl_Meta*);
    
    /* Where is the entry slot */
    if(((Pgtbl_Op->Start_Addr)&RME_PGTBL_TOP)!=0)
        Table=RME_CMX_PGTBL_TBL_TOP((ptr_t*)Meta);
    else
        Table=RME_CMX_PGTBL_TBL_NOM((ptr_t*)Meta);

    /* Check if we are trying to remove something that does not exist, or trying to
     * remove a page directory */
    if(((Table[Pos]&RME_CMX_PGTBL_PRESENT)==0)||((Table[Pos]&RME_CMX_PGTBL_TERMINAL)==0))
        return RME_ERR_PGT_OPFAIL;

    Temp=Table[Pos];
    Table[Pos]=0;
    /* If we are top-level or we have a top-level, do MPU updates */
    if((Meta->Toplevel!=0)||(((Pgtbl_Op->Start_Addr)&RME_PGTBL_TOP)!=0))
    {
        /* Now we are unmapping the pages - Immediately update MPU representations */
        if(___RME_Pgtbl_MPU_Update(Meta, RME_CMX_MPU_UPD)==RME_ERR_PGT_OPFAIL)
        {
            /* Revert operations */
            Table[Pos]=Temp;
            return RME_ERR_PGT_OPFAIL;
        }
    }
    /* Modify count */
    RME_CMX_PGTBL_DEC_PAGENUM(Meta->Dir_Page_Count);
    
    return 0;
}
/* End Function:__RME_Pgtbl_Page_Unmap ***************************************/

/* Begin Function:__RME_Pgtbl_Pgdir_Map ***************************************
Description : Map a page directory into the page table.
Input       : struct RME_Cap_Pgtbl* Pgtbl_Parent - The parent page table.
              struct RME_Cap_Pgtbl* Pgtbl_Child - The child page table.
              ptr_t Pos - The position in the destination page table.
Output      : None.
Return      : ptr_t - If successful, 0; else RME_ERR_PGT_OPFAIL.
******************************************************************************/
ptr_t __RME_Pgtbl_Pgdir_Map(struct RME_Cap_Pgtbl* Pgtbl_Parent, ptr_t Pos, 
                            struct RME_Cap_Pgtbl* Pgtbl_Child)
{
    ptr_t* Parent_Table;
    struct __RME_CMX_Pgtbl_Meta* Parent_Meta;
    struct __RME_CMX_Pgtbl_Meta* Child_Meta;
    
    /* Is the child a designated top level directory? If it is, we do not allow 
     * constructions. In Cortex-M, we only allow the designated top-level to be
     * the actual top-level. */
    if(((Pgtbl_Child->Start_Addr)&RME_PGTBL_TOP)!=0)
        return RME_ERR_PGT_OPFAIL;
    
    /* Get the metadata */
    Parent_Meta=RME_CAP_GETOBJ(Pgtbl_Parent,struct __RME_CMX_Pgtbl_Meta*);
    Child_Meta=RME_CAP_GETOBJ(Pgtbl_Child,struct __RME_CMX_Pgtbl_Meta*);
    
    /* The parent table must have or be a top-directory */
    if((Parent_Meta->Toplevel==0)&&(((Parent_Meta->Start_Addr)&RME_PGTBL_TOP)==0))
        return RME_ERR_PGT_OPFAIL;
    
    /* Check if the child already mapped somewhere, or have grandchild directories */
    if(((Child_Meta->Toplevel)!=0)||(RME_CMX_PGTBL_DIRNUM(Child_Meta->Dir_Page_Count)!=0))
        return RME_ERR_PGT_OPFAIL;
    
    /* Where is the entry slot? */
    if(((Parent_Meta->Start_Addr)&RME_PGTBL_TOP)!=0)
        Parent_Table=RME_CMX_PGTBL_TBL_TOP((ptr_t*)Parent_Meta);
    else
        Parent_Table=RME_CMX_PGTBL_TBL_NOM((ptr_t*)Parent_Meta);
    
    /* Check if anything already mapped in */
    if((Parent_Table[Pos]&RME_CMX_PGTBL_PRESENT)!=0)
        return RME_ERR_PGT_OPFAIL;
    
    /* The address must be aligned to a word */
    Parent_Table[Pos]=RME_CMX_PGTBL_PRESENT|RME_CMX_PGTBL_PGD_ADDR((ptr_t)Child_Meta);
    
    /* Log the entry into the destination */
    Child_Meta->Toplevel=(ptr_t)Parent_Meta;
    RME_CMX_PGTBL_INC_DIRNUM(Parent_Meta->Dir_Page_Count);
    
    /* Update MPU settings if there are static pages mapped into the source. If there
     * are any, update the MPU settings */
    if((RME_CMX_PGTBL_PAGENUM(Child_Meta->Dir_Page_Count)!=0)&&
       (((Child_Meta->Page_Flags)&RME_CMX_PGTBL_STATIC)!=0))
    {
        if(___RME_Pgtbl_MPU_Update(Child_Meta, RME_CMX_MPU_UPD)==RME_ERR_PGT_OPFAIL)
        {
            /* Mapping failed. Revert operations */
            Parent_Table[Pos]=0;
            Child_Meta->Toplevel=0;
            RME_CMX_PGTBL_DEC_DIRNUM(Parent_Meta->Dir_Page_Count);
            return RME_ERR_PGT_OPFAIL;
        }
    }

    return 0;
}
/* End Function:__RME_Pgtbl_Pgdir_Map ****************************************/

/* Begin Function:__RME_Pgtbl_Pgdir_Unmap *************************************
Description : Unmap a page directory from the page table.
Input       : struct RME_Cap_Pgtbl* Pgtbl_Op - The page table to operate on.
              ptr_t Pos - The position in the page table.
Output      : None.
Return      : ptr_t - If successful, 0; else RME_ERR_PGT_OPFAIL.
******************************************************************************/
ptr_t __RME_Pgtbl_Pgdir_Unmap(struct RME_Cap_Pgtbl* Pgtbl_Op, ptr_t Pos)
{
    ptr_t* Table;
    struct __RME_CMX_Pgtbl_Meta* Dst_Meta;
    struct __RME_CMX_Pgtbl_Meta* Src_Meta;
    
    /* Get the metadata */
    Dst_Meta=RME_CAP_GETOBJ(Pgtbl_Op,struct __RME_CMX_Pgtbl_Meta*);
    
    /* Where is the entry slot */
    if(((Pgtbl_Op->Start_Addr)&RME_PGTBL_TOP)!=0)
        Table=RME_CMX_PGTBL_TBL_TOP((ptr_t*)Dst_Meta);
    else
        Table=RME_CMX_PGTBL_TBL_NOM((ptr_t*)Dst_Meta);

    /* Check if we try to remove something nonexistent, or a page */
    if(((Table[Pos]&RME_CMX_PGTBL_PRESENT)==0)||((Table[Pos]&RME_CMX_PGTBL_TERMINAL)!=0))
        return RME_ERR_PGT_OPFAIL;
    
    Src_Meta=(struct __RME_CMX_Pgtbl_Meta*)RME_CMX_PGTBL_PGD_ADDR(Table[Pos]);

    /* Check if the directory still have child directories */
    if(RME_CMX_PGTBL_DIRNUM(Src_Meta->Dir_Page_Count)!=0)
        return RME_ERR_PGT_OPFAIL;
    
    /* We are removing a page directory. Do MPU updates if any page mapped in */
    if(RME_CMX_PGTBL_PAGENUM(Src_Meta->Dir_Page_Count)!=0)
    {
        if(___RME_Pgtbl_MPU_Update(Src_Meta, RME_CMX_MPU_CLR)==RME_ERR_PGT_OPFAIL)
            return RME_ERR_PGT_OPFAIL;
    }
        
    Table[Pos]=0;
    Src_Meta->Toplevel=0;
    RME_CMX_PGTBL_DEC_DIRNUM(Dst_Meta->Dir_Page_Count);

    return 0;
}
/* End Function:__RME_Pgtbl_Pgdir_Unmap **************************************/

/* Begin Function:__RME_Pgtbl_Lookup ********************************************
Description : Lookup a page entry in a page directory.
Input       : struct RME_Cap_Pgtbl* Pgtbl_Op - The page directory to lookup.
              ptr_t Pos - The position to look up.
Output      : ptr_t* Paddr - The physical address of the page.
              ptr_t* Flags - The RME standard flags of the page.
Return      : ptr_t - If successful, 0; else RME_ERR_PGT_OPFAIL.
******************************************************************************/
ptr_t __RME_Pgtbl_Lookup(struct RME_Cap_Pgtbl* Pgtbl_Op, ptr_t Pos, ptr_t* Paddr, ptr_t* Flags)
{
    ptr_t* Table;
    
    /* Check if the position is within the range of this page table */
    if((Pos>>RME_PGTBL_NUMORD(Pgtbl_Op->Size_Num_Order))!=0)
        return RME_ERR_PGT_OPFAIL;
    
    /* Check if this is the top-level page table. Get the table */
    if(((Pgtbl_Op->Start_Addr)&RME_PGTBL_TOP)!=0)
        Table=RME_CMX_PGTBL_TBL_TOP(RME_CAP_GETOBJ(Pgtbl_Op,ptr_t*));
    else
        Table=RME_CMX_PGTBL_TBL_NOM(RME_CAP_GETOBJ(Pgtbl_Op,ptr_t*));
    
    /* Start lookup */
    if(((Table[Pos]&RME_CMX_PGTBL_PRESENT)==0)||
       ((Table[Pos]&RME_CMX_PGTBL_TERMINAL)==0))
        return RME_ERR_PGT_OPFAIL;
    
    /* This is a page. Return the physical address and flags */
    if(Paddr!=0)
        *Paddr=RME_CMX_PGTBL_PTE_ADDR(Table[Pos]);
    
    if(Flags!=0)
        *Flags=RME_CAP_GETOBJ(Pgtbl_Op,struct __RME_CMX_Pgtbl_Meta*)->Page_Flags;

    return 0;
}
/* End Function:__RME_Pgtbl_Lookup *******************************************/

/* Begin Function:__RME_Pgtbl_Walk ********************************************
Description : Walking function for the page table. This function just does page
              table lookups. The page table that is being walked must be the top-
              level page table. The output values are optional; only pass in pointers
              when you need that value.
Input       : struct RME_Cap_Pgtbl* Pgtbl_Op - The page table to walk.
              ptr_t Vaddr - The virtual address to look up.
Output      : ptr_t* Pgtbl - The pointer to the page table level.
              ptr_t* Map_Vaddr - The virtual address that starts mapping.
              ptr_t* Paddr - The physical address of the page.
              ptr_t* Size_Order - The size order of the page.
              ptr_t* Num_Order - The entry order of the page.
              ptr_t* Flags - The RME standard flags of the page.
Return      : ptr_t - If successful, 0; else RME_ERR_PGT_OPFAIL.
******************************************************************************/
ptr_t __RME_Pgtbl_Walk(struct RME_Cap_Pgtbl* Pgtbl_Op, ptr_t Vaddr, ptr_t* Pgtbl,
                       ptr_t* Map_Vaddr, ptr_t* Paddr, ptr_t* Size_Order, ptr_t* Num_Order, ptr_t* Flags)
{
    struct __RME_CMX_Pgtbl_Meta* Meta;
    ptr_t* Table;
    ptr_t Pos;
    
    /* Check if this is the top-level page table */
    if(((Pgtbl_Op->Start_Addr)&RME_PGTBL_TOP)==0)
        return RME_ERR_PGT_OPFAIL;
    
    /* Get the table and start lookup */
    Meta=RME_CAP_GETOBJ(Pgtbl_Op, struct __RME_CMX_Pgtbl_Meta*);
    Table=RME_CMX_PGTBL_TBL_TOP((ptr_t*)Meta);
    
    /* Do lookup recursively */
    while(1)
    {
        /* Check if the virtual address is in our range */
        if(Vaddr<RME_CMX_PGTBL_START(Meta->Start_Addr))
            return RME_ERR_PGT_OPFAIL;
        /* Calculate where is the entry */
        Pos=(Vaddr-RME_CMX_PGTBL_START(Meta->Start_Addr))>>RME_CMX_PGTBL_SIZEORD(Meta->Size_Num_Order);
        /* See if the entry is overrange */
        if((Pos>>RME_CMX_PGTBL_NUMORD(Meta->Size_Num_Order))!=0)
            return RME_ERR_PGT_OPFAIL;
        /* Find the position of the entry - Is there a page, a directory, or nothing? */
        if((Table[Pos]&RME_CMX_PGTBL_PRESENT)==0)
            return RME_ERR_PGT_OPFAIL;
        if((Table[Pos]&RME_CMX_PGTBL_TERMINAL)!=0)
        {
            /* This is a page - we found it */
            if(Pgtbl!=0)
                *Pgtbl=(ptr_t)Meta;
            if(Map_Vaddr!=0)
                *Map_Vaddr=RME_CMX_PGTBL_START(Meta->Start_Addr)+(Pos<<RME_CMX_PGTBL_SIZEORD(Meta->Size_Num_Order));
            if(Paddr!=0)
                *Paddr=RME_CMX_PGTBL_START(Meta->Start_Addr)+(Pos<<RME_CMX_PGTBL_SIZEORD(Meta->Size_Num_Order));
            if(Size_Order!=0)
                *Size_Order=RME_CMX_PGTBL_SIZEORD(Meta->Size_Num_Order);
            if(Num_Order!=0)
                *Num_Order=RME_CMX_PGTBL_NUMORD(Meta->Size_Num_Order);
            if(Flags!=0)
                *Flags=Meta->Page_Flags;
            
            break;
        }
        else
        {
            /* This is a directory, we goto that directory to continue walking */
            Meta=(struct __RME_CMX_Pgtbl_Meta*)RME_CMX_PGTBL_PGD_ADDR(Table[Pos]);
            Table=RME_CMX_PGTBL_TBL_NOM((ptr_t*)Meta);
        }
    }
    return 0;
}
/* End Function:__RME_Pgtbl_Walk *********************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

/*-----------------------------------------------------------------*-

	Copyright(c) 2014 Techeart Technology Corp. All rights reserved.
    Date: 14/Nov/2014
    E-Mail: DruculaC@gmail.com	

-*-----------------------------------------------------------------*/

#ifndef _SCHEDULAR_H
#define _SCHEDULAR_H

#include "Main.H"

// ------ Public data type declarations ----------------

// Store in DATA area, if possible, for rapid access  
// Total memory per task is 7 bytes
typedef data struct 
   {
   // Pointer to the task (must be a 'void (void)' function)
   void (code * pTask)(void);  

   // Delay (ticks) until the function will (next) be run
   // - see SCH_Add_Task() for further details
   tWord Delay;       

   // Interval (ticks) between subsequent runs.
   // - see SCH_Add_Task() for further details
   tWord Period;       

   // Incremented (by scheduler) when task is due to execute
   tByte RunMe;       
   } sTask; 

// ------ Public function prototypes -------------------------------

// Core scheduler functions
void  SCH_Dispatch_Tasks(void);
tByte SCH_Add_Task(void (code*) (void), const tWord, const tWord);  
bit   SCH_Delete_Task(const tByte);
void  SCH_Report_Status(void);

// ------ Public constants -----------------------------------------

// The maximum number of tasks required at any one time
// during the execution of the program
//
// MUST BE ADJUSTED FOR EACH NEW PROJECT
#define SCH_MAX_TASKS   (10)   
 
#endif
                              
/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/

			﻿+-----------------+
                         |      CS 410     |
                         |      PINTOS    |
                         | DESIGN DOCUMENT |
                         +-----------------+


---- GROUP ----


Chloe Dorward <dorwardc@lafayette.edu>
Seth Markarian <markaris@lafayette.edu>
Trey Miller <milledav@lafayette.edu>
Matthew Sabatino <sabatinm@lafayette.edu>




TIMER_SLEEP
============


---- DATA STRUCTURES ----
In timer.h:
struct list_elem sleepelem;
	We added a list_elem to the thread struct so that the thread can be held in the sleep_list. 
unsigned wakeup;
	We added a field wakeup to the thread struct to keep track of the thread’s wakeup time.


In thread.c:
static struct list sleep_list;
	We created a list called sleep_list that holds threads that are currently in the state of “sleep”. It’s ordered by the number of ticks each thread has.
     static long long total_ticks
	We added a field called total_ticks to represent the total number of clock ticks, to be compared to a thread’s wakeup time.


---- ALGORITHMS ----
To eliminate busy-wait for the timer_sleep() function, we decided to add the sleeping thread to a priority queue of sleeping threads ordered by wakeup time. At every tick, the scheduler pops any threads to be woken up, and adds them to the ready list.
The overall flow for one thread calling timer_sleep():
1. timer_sleep() is called by the current thread. This function checks if the sleep time is greater than 0, and if so, calls the thread_sleep() function.
2. In thread_sleep(), interrupts are turned off.
3. The current thread’s wakeup time is calculated and assigned to the thread’s wakeup field.
4. The current thread gets added to the sleep_list, ordered based on wakeup time.
5. thread_block() is called, then interrupts are enabled again.
6. In thread_tick(), total_ticks is incremented. Then, any thread on sleep_list that can be woken up gets popped off of the list, and thread_unblock() is called. Interrupts are disabled before editing sleep_list(), and re-enabled after.


---- SYNCHRONIZATION ----
Race conditions related to sleep_list are avoided by disabling interrupts before sleep_list is edited, then restating the prior interrupt level after. The timer_tick function is responsible for keeping track of which threads need to be woken up. Since no other threads interfere with this, synchronization issues are minimized.


---- RATIONALE ----
With our design, we wanted to eliminate the time required for each thread to check whether or not it should be woken up and continue executing. To do this, we first decided to consolidate the workload to be done by one thread, the main thread that calls thread_tick. During each tick, the thread determines which sleeping threads can be woken up. The sleeping threads are stored in a singular list, sleep_list, in thread.c. In order to check which threads need to be woken up efficiently, we decided that a priority queue ordered based on wakeup time would be the best way to store the sleeping threads. Using the included list data structure, it takes O(n) time to insert into sleep_list, and O(1) time to peek at the top of the list, and O(1) time to pop from the top of the list. A potential disadvantage of our approach is that there is added execution time with every thread tick.




SCHEDULING
============


---- DATA STRUCTURES ---
In thread.h:
struct lock* waiting;
Included in the thread struct, represents the lock that the thread is waiting on. 


struct list acquired;
Included in the thread struct, the list contains the locks that a thread have acquired.


     int base_priority;
	Included in the thread struct, assigns a priority as soon as the thread is created.


In synch.h:
     struct list_elem acquired_elem; 
	Included in the lock struct, stored in thread that holds lock list element


     int priority;
	Included in the lock struct, holds priority of the highest-priority thread


---- ALGORITHMS ----
To ensure a priority scheduler, we sorted the ready_list by priority. Every time a thread is inserted into the read_list, it is sorted in order. Also, if the priority is updated, then the thread is yielded, which reinserts it into the sorted ready_list. Since the next thread to run is chosen from the beginning of the ready list, then the next thread will always have the highest priority.


To track the priority that is donated through a lock, we stored the priority of a lock. This priority is equal to the maximum priority of all threads that are waiting for that lock. If no thread is waiting for a lock, then that lock's priority is minimal (0).


When a thread attempts to acquire the lock, the priority of the lock can change. The priority could increase if the lock is already acquired (causing the thread to be added to the waiting list) and the new thread has a higher priority than all currently waiting threads. A lock’s priority could also change when it is released when other threads are waiting for it. If the next thread has the highest priority of the waiters, then the lock’s priority will decrease. Finally, if a thread waiting for the lock changes priority, then the lock could change priority. For this reason, there is a method lock_update_priority that recalculates and stores the priority. In any case, if the lock's priority does change, then the method thread_update_priority is called for the thread that has acquired that lock.


A thread’s priority is now the maximum of the thread’s base priority and the priority of the lock that the thread is waiting on. If the thread is not waiting for any lock, then its priority is equal to its base priority.


When a thread’s base priority is changed (through thread_set_priority) the priority of the thread could change. Also, when the priority of a lock that it acquired changes, the thread’s priority could change. There’s a method thread_update_priority that recalculates and stores the priority of the thread. In any case, if the priority of the thread is changed, then the method lock_update_priority is called for the lock that this thread is waiting for if such lock exists. This will update the priority of the thread holding that lock, which will update the priority of the lock that the holding thread is waiting on, if one such exists, and so on.


---- SYNCHRONIZATION ----
Race conditions related to the waiting (within a semaphore) or acquired (within a thread) lists are avoided by disabling interrupts entirely while editing or accessing data from the lists, then restating the prior interrupt level after. The waiting list is only edited in synch.c while the acquired list is edited in both synch.c and thread.c.


---- RATIONALE ----
With our design, we wanted to reduce the amount of additional data in global variables and that individual threads have to store to allow for priority scheduling and donation. By storing the ready threads in a list based on priority, then popping the top of the list, it only takes O(n) time to insert a thread into the ready list, and O(1) time to pop. This design choice allowed us to use the existing ready_list structure. For each individual thread, the only additional data stored is the list of acquired locks ordered based on priority, the pointer to the lock the thread is waiting on, and the integer representing the baseline priority. Inserting an acquired lock takes O(n) time, and popping the front takes O(1) time. A distinct disadvantage of our design is that every time a lock’s priority gets updated due to priority donation, the lock must be removed and then reinserted into the list of its holder.


Additionally, our design does not pass the priority-donate-nested test.

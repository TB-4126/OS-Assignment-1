//producer.cpp
//Thomas Bond
//March 6th 2025
#include <iostream>
#include <cstdlib>
#include <ctime>


//Test which of these are actually necessary
/*
https://www.softprayog.in/programming/interprocess-communication-using-posix-shared-memory-in-linux
https://www.softprayog.in/programming/semaphore-basics
*/

#include <string>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>


#include "producer.hpp"


int processNUM = 0; //Defines which process this is

int main() {

   //
   /*Pretend this is the table*/
   //
   int dataSize = 2;
   int totalProcesses = 2;

   nodeTable<int> tableData[dataSize]; //Stores the data itself

   //Process Activity Semaphores
   int turn = 0; //Tracks which processes turn it is
   bool prodInCrit = false;
   bool conInCrit = false;
   bool flag[totalProcesses]; //Tracks processes that are ready
   //
   //
   //

   int TABLESIZE = 4096;

   int shm_fd = shm_open("table", O_CREAT | O_RDWR, 0666);
   ftruncate(shm_fd, TABLESIZE);
   void* shmPTR = mmap(0, TABLESIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

   if (shmPTR == MAP_FAILED) { //Checks if the shared memory space is created
      std::cerr << "Failed to create shared memory space." << std::endl;
      exit(1);
   }


   


   /*
   How do we add variables to the shared memory space?

   Threading requires pthread library
   
   are the semaphores separate from the shared memory space?
   */

   int* dataGen = nullptr; //Initialize pointer to generated data
   nodeTable<int>* dataIN = nullptr; //Initialize the dataIN pointer to nullptr
   nodeTable<int> prodSlotTrack[dataSize]; //Local version of table data

   while(true) {
      flag[processNUM] = true; //shows producer is ready and waiting

      //Thread 1: Generates the data
      if (dataGen == nullptr) {
         srand(time(nullptr));
         dataGen = new int(rand()); //Generates random data
      }

      //
      //Critical section
      //
      while (conInCrit == true && turn != 0) ; //Do nothing while consumer is in critical section
      prodInCrit = true; //Updates semaphore to show process 0 is ins critical section

      //Thread 2: Updates the local version of the table
      for (int i = 0; i < dataSize; ++i) { prodSlotTrack[i] = tableData[i]; }

      //Thread 2: Looks for an empty slot in local table
      for (int i = 0; i < dataSize && dataIN == nullptr; ++i) {
         //Sets dataIN pointer to the empty slot, breaking the loop
         if (prodSlotTrack[i].full == false) { dataIN = &(prodSlotTrack[i]); }
      }

      //Thread 3: Pushes the data onto local table
      while (dataIN == nullptr || dataGen == nullptr) ; //wait for a slot to open and data to be generated

      dataIN->data = *dataGen; //Writes generated data to local table
      dataIN->full = true; //Updates slot boolean
      dataIN = nullptr; //Sets dataIN to nullptr
      delete dataGen; //Deallocates dataGen
      dataGen = nullptr; //Sets dataGen to null

      //Thread 2: Updates the shared table
      while (dataIN != nullptr) ; //Wait until the empty slot is filled

      for (int i = 0; i < dataSize; ++i) { tableData[i] = prodSlotTrack[i]; }

      turn = 1; //Updates the semaphore turn
      prodInCrit = false; //Updates the critical section lock
      //
      //End of critical section
      //

   }
}

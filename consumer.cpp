//consumer.cpp
//Thomas Bond
//March 6th 2025
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <sys/mman.h>
#include "consumer.hpp"

int processNUM = 1; //Defines which process this is

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

   //POSIX API stuff, but what does it do?
   int shm_fd = shm_open("table", O_RDONLY, 0666);
   void* shmPTR = mmap(0, 6, PROT_READ, MAP_SHARED, shm_fd, 0);

   if (shmPTR == MAP_FAILED) { //Checks if the shared memory space is created
      std::cerr << "Failed to create shared memory space." << std::endl;
      exit(1);
   }

   
   int* dataConsumption = nullptr;
   nodeTable<int>* dataOUT = nullptr;
   nodeTable<int> conSlotTrack[dataSize];

   while(true) {
      flag[processNUM] = true; //shows consumer is ready and waiting

      //
      //Critical section
      //
      while (prodInCrit == true && turn != 1) ; //Do nothing while producer is in critical section
      conInCrit = true; //Updates semaphore to show process 1 in critical section

      //Thread 2: Updates local table
      for (int i = 0; i < dataSize; ++i) { conSlotTrack[i] = tableData[i]; }

      //Thread 2: Finds a piece of data in the local table
      for (int i = 0; i < dataSize; ++i) {
         //Points dataOUT to the full slot in the local table
         if (conSlotTrack[i].full == true) { dataOUT = &(conSlotTrack[i]); }
      }
      

      //Thread 1: Uses the data for operations
      while (dataOUT == nullptr) ; //Wait for a piece of data to be produced

      dataConsumption = new int(dataOUT->data); //Copies the data to a local variable
      std::cout << "Data generated in slot X is: " << dataConsumption << std::endl;


      //Thread 1: Removes the data from local table

      //How do we stop it from removing data while it's updating?

      dataOUT->data = 0;
      dataOUT->full = false;
      dataOUT = nullptr;
      delete dataConsumption;
      dataConsumption = nullptr;


      //Thread 2: Updates shared table
      while (dataOUT != nullptr) ; //Wait until after data is written or if no data is found

      for (int i = 0; i < dataSize; ++i) { tableData[i] = conSlotTrack[i]; }
      
      //
      //End of critical section
      //
      turn = 0; //Updates the semaphore turn
      prodInCrit = false;
   }

   
}

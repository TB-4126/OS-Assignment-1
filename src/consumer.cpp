//consumer.cpp
//Thomas Bond
//March 6th 2025
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "table.hpp"

int processNUM = 1; //Defines which process this is

int main() {

   int TABLESIZE = sizeof(table<int>);

   int sharedMemory = shm_open("table", O_RDWR, 0666);
   table<int>* sharedMemTable;
   sharedMemTable = (table<int>*)mmap(0, TABLESIZE, PROT_WRITE | PROT_READ, MAP_SHARED, sharedMemory, 0);

   if (sharedMemTable == MAP_FAILED) { //Checks if the shared memory space is created
      std::cerr << "Failed to create shared memory space(consumer)." << std::endl;
      exit(1);
   }
   
   int* dataConsumption = nullptr;
   nodeTable<int>* dataOUT = nullptr;
   nodeTable<int> conSlotTrack[dataSize];

   while(true) {
      sharedMemTable->flag[processNUM] = true; //shows consumer is ready and waiting

      //
      //Critical section
      //
      while (sharedMemTable->prodInCrit == true && sharedMemTable->turn != 1) ; //Do nothing while producer is in critical section
      sharedMemTable->conInCrit = true; //Updates semaphore to show process 1 in critical section

      //Thread 2: Updates local table
      for (int i = 0; i < dataSize; ++i) { conSlotTrack[i] = sharedMemTable->tableData[i]; }

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

      for (int i = 0; i < dataSize; ++i) { sharedMemTable->tableData[i] = conSlotTrack[i]; }
      
      sharedMemTable->turn = 0; //Updates the semaphore turn
      sharedMemTable->prodInCrit = false;
      //
      //End of critical section
      //
   }

   close(sharedMemory); //Closes the shared memory space
}

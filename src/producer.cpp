//producer.cpp
//Thomas Bond
//March 6th 2025
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "table.hpp"

const int processNUM = 0; //Defines which process this is

int main() {
   int TABLESIZE = sizeof(table<int>); //Creates an integer set to the size of sharedTable class in memory

   int sharedMemory = shm_open("table", O_CREAT | O_RDWR, 0666);
   ftruncate(sharedMemory, TABLESIZE);

   table<int>* sharedMemTable; //Creates a mapping to the shared table
   sharedMemTable = (table<int>*)mmap(0, TABLESIZE, PROT_WRITE | PROT_READ, MAP_SHARED, sharedMemory, 0);

   if (sharedMemTable == MAP_FAILED) { //Checks if the shared memory space is created
      std::cerr << "Failed to create shared memory space(producer)." << std::endl;
      exit(1);
   }

   int* dataGen = nullptr; //Initialize pointer to generated data
   nodeTable<int>* dataIN = nullptr; //Initialize the dataIN pointer to nullptr
   nodeTable<int> prodSlotTrack[dataSize]; //Local version of table data

   while(true) {
      sharedMemTable->flag[processNUM] = true; //shows producer is ready and waiting

      //Thread 1: Generates the data
      if (dataGen == nullptr) {
         srand(time(nullptr));
         dataGen = new int(rand()); //Generates random data
      }

      //
      //Critical section
      //
      while (sharedMemTable->conInCrit == true && sharedMemTable->turn != 0) /*std::cout << "waiting(producer)" << std::endl*/; //Do nothing while consumer is in critical section
      sharedMemTable->prodInCrit = true; //Updates semaphore to show process 0 is ins critical section

      //Thread 2: Updates the local version of the table
      for (int i = 0; i < dataSize; ++i) { prodSlotTrack[i] = sharedMemTable->tableData[i]; }

      //Thread 2: Looks for an empty slot in local table
      for (int i = 0; i < dataSize && dataIN == nullptr; ++i) {
         //Sets dataIN pointer to the empty slot, breaking the loop
         if (prodSlotTrack[i].full == false) { dataIN = &(prodSlotTrack[i]); std::cout << "Empty slot found" << std::endl; }
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

      for (int i = 0; i < dataSize; ++i) { sharedMemTable->tableData[i] = prodSlotTrack[i]; }

      sharedMemTable->turn = 1; //Updates the semaphore turn
      sharedMemTable->prodInCrit = false; //Updates the critical section lock
      //
      //End of critical section
      //

   }

   close(sharedMemory); //Closes the shared memory space
}

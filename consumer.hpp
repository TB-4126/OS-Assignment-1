//producer.cpp
//Thomas Bond
//March 6th 2025

#ifndef CONSUMER_HPP_
#define CONSUMER_HPP_

template <typename T>
class nodeTable {
public:
   nodeTable() : full(false), data(0) {}; //Default constructor
   nodeTable(const T& value) : data(value), full(true) {}; //T Constructor
   nodeTable(const nodeTable& copy) : full(copy.full), data(copy.data) {}; //Copy Constructor
   //Destructor
   void swap(nodeTable<T>& rhs); //Void Swap Function
   nodeTable& operator=(nodeTable<T> rhs) { swap(rhs); return *this; };//Equals Operator

   bool full; //Tracks if the node has had data pushed or removed from it
   T data; //Stores the actual data
};

//nodeTable Void Swap Function
template <typename T>
void nodeTable<T>::swap(nodeTable<T>& rhs) {
   bool tempFull = full;
   full = rhs.full;
   rhs.full = tempFull;

   T tempData = data;
   data = rhs.data;
   rhs.data = tempData;
}


#endif

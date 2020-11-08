//============================================================================
// Name        : hashskeleton.cpp
// Author      : 
// Version     :
// Copyright   : Code adapted From https://www.tutorialspoint.com/
// Description : Hashing using open addressing
//============================================================================

#include "readfile.h"
#include <iostream>
using namespace std;

void insert(int key,int data);
int deleteItem(int key);
void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
struct DataItem * search(int key);

int Alg;
int filehandle;   //handler for the database file

/* DBMS (DataBase Management System) needs to store its data in something non-volatile
 * so it stores its data into files (manteqy :)).

 * Some DBMS or even file-systems constraints the size of that file. 

 * for the efficiency of storing and retrieval, DBMS uses hashing 
 * as hashing make it very easy to store and retrieve, it introduces 
 * another type of problem which is handling conflicts when two items
 * have the same hash index

 * In this exercise we will store our database into a file and experience
 * how to retrieve, store, update and delete the data into this file 

 * This file has a constant capacity and uses external-hashing to store records,
 * however, it suffers from hashing conflicts.
 * 
 * You are required to help us resolve the hashing conflicts 

 * For simplification, consider the database has only one table 
 * which has two columns key and data (both are int)

 * Functions in this file are just wrapper functions, the actual functions are in openAddressing.cpp

*/

int main(){

//here we create a sample test to read and write to our database file

  //1. Create Database file or Open it if it already exists, check readfile.cpp
  // In case of applying chaining algorithm we will need overflow part
   //filehandle = createFile(FILESIZE + OVERFLOW_PART,"openaddressing");
   filehandle = createFile(FILESIZE,"openaddressing");
   
   //=========================CHOOSE ALGORITHM AND TESTCASE==========================
   int testcase;

   cout << "Enter Algorithm number"<<endl<<"0: Open Addressing" <<endl<<
   "1: Chaining"<<endl<<"2: Multiple Hashing"<< endl;
   cin >> Alg; 

   cout << "Enter test case number from "<<Alg*2+1 <<" to "<< Alg*2 + 2 << endl;
   cin >> testcase;
   //=================================================================================

   //2. Display the database file, check openAddressing.cpp
   DisplayFile(filehandle);

   if (testcase == 1)
      test1();
   if (testcase == 2)
      test2();
   if (testcase == 3)
      test3();
   if (testcase == 4)
      test4();
   if (testcase == 5)
      test5();
   if (testcase == 6)
      test6();   

   //7. Display the final data base
   DisplayFile(filehandle);

   //In case of using chaining algorithm display overflow part
   //DisplayOverflowPart(filehandle);

   // And Finally don't forget to close the file.
   close(filehandle);
   int x = remove("openaddressing");
   return 0;
}

/* functionality: insert the (key,data) pair into the database table
                  and print the number of comparisons it needed to find
    Input: key, data
    Output: print statement with the no. of comparisons
*/
void insert(int key,int data){
     struct DataItem item ;
     item.data = data;
     item.key = key;
     item.valid = 1;
     int result;
     if(Alg == 0)
         result= insertItemOA(filehandle,item);  //TODO: implement this function in openAddressing.cpp
     else if(Alg == 1)
         result= insertItemChainingAlgorithm(filehandle,item);
      else
         result= insertItemMH(filehandle,item);

     printf("Insert: No. of searched records:%d\n",abs(result));
}

/* Functionality: search for a data in the table using the key

   Input:  key
   Output: the return data Item
*/
struct DataItem * search(int key)
{
   struct DataItem* item = (struct DataItem *) malloc(sizeof(struct DataItem));
      item->key = key;
      int diff = 0;
      int Offset;
      if(Alg == 0)
         Offset= searchItemOA(filehandle,item,&diff); //this function is implemented for you in openAddressing.cpp
      else if(Alg == 1)
         Offset= searchItemChainingAlgorithm(filehandle,item,&diff);
      else
         Offset= searchItemMH(filehandle,item,&diff);

      printf("Search: No of records searched is %d\n",diff);
      if(Offset <0)  //If offset is negative then the key doesn't exists in the table
            printf("Item not found\n");
      else
            printf("Item found at Offset: %d,  Data: %d and key: %d\n",Offset,item->data,item->key);
   return item;
}

/* Functionality: delete a record with a certain key

   Input:  key
   Output: return 1 on success and -1 on failure
*/
int deleteItem(int key){
   struct DataItem* item = (struct DataItem *) malloc(sizeof(struct DataItem));
   item->key = key;
   int diff = 0;
   int Offset;
   if(Alg == 0)
      Offset= searchItemOA(filehandle,item,&diff);
   else if (Alg == 1)
      Offset= searchItemChainingAlgorithm(filehandle,item,&diff);
   else
      Offset= searchItemMH(filehandle,item,&diff);


   printf("Delete: No of records searched is %d\n",diff);
   printf("Delete: Offset %d\n",Offset);
   if(Offset >=0 )
   {
      return deleteOffset(filehandle,Offset);
   }
   return -1;
}

void test1(){
   //3. Add some data in the table
   insert(1, 20);
   insert(2, 70);
   insert(42, 80);
   insert(4, 25);
   insert(15, 44);
   insert(14, 32);
   insert(17, 11);
   insert(13, 78);
   insert(37, 97);
   insert(11, 34);
   insert(22, 730);
   insert(46, 840);
   insert(9, 83);
   insert(21, 424);
   insert(41, 115);
   insert(71, 47);
   insert(31, 92);
   insert(73, 45);

   //4. Display the database file again
   DisplayFile(filehandle);

   //5. Search the database
   search(13);
   search(9);
   search(31);
   search(15);

   //6. delete an item from the database
   deleteItem(31);
   deleteItem(1);
   deleteItem(4);

   insert(41, 92);
   insert(44, 920);

}
void test2(){
   //3. Add some data in the table
   insert(1, 20);
   insert(11, 70);
   insert(41, 80);
   insert(21, 25);
   insert(31, 44);
   insert(12, 32);
   insert(22, 11);
   insert(32, 78);
   insert(33, 97);
   insert(13, 34);
   insert(23, 730);
   insert(43, 840);
   insert(95, 83);
   insert(25, 424);
   insert(47, 115);
   insert(71, 47);
   insert(61, 92);
   insert(73, 45);

   //4. Display the database file again
   DisplayFile(filehandle);

   //5. Search the database
   search(11);
   search(33);
   search(71);

   //6. delete an item from the database
   deleteItem(31);
   deleteItem(61);
   deleteItem(25);

   insert(3, 45);
   insert(2, 405);
   insert(5, 450);

}
void test3(){
   //3. Add some data in the table
   insert(1, 20);
   insert(2, 70);
   insert(42, 80);
   insert(4, 25);
   insert(12, 44);
   insert(14, 32);
   insert(17, 11);
   insert(13, 78);
   insert(37, 97);
   insert(11, 34);
   insert(22, 730);
   insert(46, 840);
   insert(9, 83);
   insert(21, 424);
   insert(41, 115);
   insert(71, 47);
   insert(31, 92);
   insert(73, 45);

   //4. Display the database file again
   DisplayFile(filehandle);

   //In case of using chaining algorithm display overflow part
   DisplayOverflowPart(filehandle);

   //5. Search the database
   search(13);

   //6. delete an item from the database
   deleteItem(31);
}
void test4(){
   //3. Add some data in the table
   insert(1, 20);
   insert(2, 70);
   insert(42, 80);
   insert(4, 25);
   insert(12, 44);
   insert(14, 32);
   insert(17, 11);
   insert(13, 78);
   insert(37, 97);
   insert(11, 34);
   insert(22, 730);
   insert(46, 840);
   insert(9, 83);
   insert(21, 424);
   insert(41, 115);
   insert(71, 47);
   insert(31, 92);
   insert(73, 45);

   //4. Display the database file again
   DisplayFile(filehandle);

   //In case of using chaining algorithm display overflow part
   DisplayOverflowPart(filehandle);

   //5. Search the database
   search(13);

   //6. delete an item from the database
   deleteItem(31);
}
void test5(){
   //3. Add some data in the table
   insert(1, 20);
   insert(2, 70);
   insert(42, 80);
   insert(4, 25);
   insert(15, 44);
   insert(14, 32);
   insert(17, 11);
   insert(13, 78);
   insert(37, 97);
   insert(11, 34);
   insert(22, 730);
   insert(46, 840);
   insert(9, 83);
   insert(21, 424);
   insert(41, 115);
   insert(71, 47);
   insert(31, 92);
   insert(73, 45);

   //4. Display the database file again
   DisplayFile(filehandle);

   //5. Search the database
   search(13);
   search(9);
   search(31);
   search(15);

   //6. delete an item from the database
   deleteItem(31);
   deleteItem(1);
   deleteItem(4);

   insert(41, 92);
   insert(44, 920);
}
void test6(){
   //3. Add some data in the table
   insert(1, 20);
   insert(11, 70);
   insert(41, 80);
   insert(21, 25);
   insert(31, 44);
   insert(12, 32);
   insert(22, 11);
   insert(32, 78);
   insert(33, 97);
   insert(13, 34);
   insert(23, 730);
   insert(43, 840);
   insert(95, 83);
   insert(25, 424);
   insert(47, 115);
   insert(71, 47);
   insert(61, 92);
   insert(73, 45);

   //4. Display the database file again
   DisplayFile(filehandle);

   //5. Search the database
   search(11);
   search(33);
   search(71);

   //6. delete an item from the database
   deleteItem(31);
   deleteItem(61);
   deleteItem(25);

   insert(3, 45);
   insert(2, 405);
   insert(5, 450);

}
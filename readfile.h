/*
 * readfile.h
 *
 *  Created on: Sep 20, 2016
 *      Author: dina
 */

#ifndef READFILE_H_
#define READFILE_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#define MBUCKETS  10					                        //Number of BUCKETS
#define RECORDSPERBUCKET 2				                     //No. of records inside each Bucket
#define BUCKETSIZE sizeof(Bucket)		                  //Size of the bucket (in bytes)
#define FILESIZE BUCKETSIZE*MBUCKETS                     //Size of the file 

#define RECORD_SIZE sizeof(DataItem)                     //Size of one record
#define MOVERFLOW_RECORDS 10                             //Number of overflow records
#define STARTING_ADDRESS_OF_OVERFLOW_RECORDS FILESIZE    //Starting address for the overflow records within file
#define OVERFLOW_PART MOVERFLOW_RECORDS*RECORD_SIZE


//Data Record inside the file
struct DataItem {
   int valid;    //) means invalid record, 1 = valid record
   int data;     
   int key;
   int nextOffset;            // Needed for chaining algorithm
};


//Each bucket contains number of records
struct Bucket {
	struct DataItem  dataItem[RECORDSPERBUCKET];

};

//Check the create File
int createFile(int size, char *);

//check the openAddressing File
int deleteItem(int key);
int insertItemOA(int fd,DataItem item);
int DisplayFile(int fd);
int deleteOffset(int filehandle, int Offset);
int searchItemOA(int filehandle,struct DataItem* item,int *count);

//check chaining file
int insertItemChainingAlgorithm(int fd,DataItem item);
int searchItemChainingAlgorithm(int fd,struct DataItem* item,int *count);
int DisplayOverflowPart(int fd);

//check multipleHAshing file
int searchItemMH(int filehandle,struct DataItem* item,int *count);
int insertItemMH(int fd,DataItem item);




#endif /* READFILE_H_ */

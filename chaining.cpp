#include "readfile.h"

/* Hash function to choose bucket
 * Input: key used to calculate the hash
 * Output: HashValue;
 */
int hashCodeChaining(int key){
   return key % MBUCKETS;
}


/* Functionality insert the data item into the correct position
 *          1. use the hash function to determine which bucket to insert into
 *          2. search for the first empty space within the bucket
 *          	2.1. if it has empty space
 *          	           insert the item
 *          	     else
 *          	          use Chaining to insert the record
 *          3. return the number of records accessed (searched)
 *
 * Input:  fd: filehandler which contains the db
 *         item: the dataitem which should be inserted into the database
 *
 * Output: No. of record searched
 */
int insertItemChainingAlgorithm(int fd,DataItem item){

    item.nextOffset = -1;
    int offsetOfLastRecordInChain;
    int count = 0;				                        //No of accessed records
	int hashIndex = hashCodeChaining(item.key);  				//calculate the Bucket index
	int Offset = hashIndex*sizeof(Bucket);				//Offset variable which we will use to iterate on the db
    struct DataItem data1,data2;
    

    // Loop on all records within same bucket until you find a free space.
    for(int i = 0; i < RECORDSPERBUCKET; i++) {
	    ssize_t result = pread(fd,&data1,sizeof(DataItem), Offset);
        offsetOfLastRecordInChain = Offset;
        //one record accessed
        count++;
        //check whether it is a valid record or not
        if(result <= 0) //either an error happened in the pread or it hit an unallocated space
        { 	 // perror("some error occurred in pread");
            return -1;
        }
        // No data
        else if (data1.valid == 0) {
            if((data1.nextOffset > 0) && (i == RECORDSPERBUCKET-1)) {
                item.nextOffset = data1.nextOffset;
            }
            //No record available means empty space we can insert in it
            ssize_t wresult = pwrite(fd, &item, sizeof(DataItem), Offset);
            if(wresult < 0) {
                perror("Insertion Failed");
                return count;
            }
            return count;
        }   
        else { //look for the next free record
            Offset +=sizeof(DataItem);  //move the offset to next record
        }
    }

    // If we reached here it means that we have gone through all records in the bucket 
    // so we are going to see if there is any chaining occured
    int offsetOfNextRecord = data1.nextOffset;

    while(offsetOfNextRecord != -1)
    {
        offsetOfLastRecordInChain = offsetOfNextRecord;
        // Get the result of chaining
        ssize_t result = pread(fd,&data1,sizeof(DataItem), offsetOfNextRecord);
        //one record accessed
        count++;
        offsetOfNextRecord = data1.nextOffset;
    }

    //printf("HERE : %d,%d\n", offsetOfLastRecordInChain, data1.key);


    for(Offset = STARTING_ADDRESS_OF_OVERFLOW_RECORDS; Offset< (FILESIZE + OVERFLOW_PART); Offset+=sizeof(DataItem)) {
	    ssize_t result = pread(fd,&data2,sizeof(DataItem), Offset);
       
        //one record accessed
        count++;
        //check whether it is a valid record or not
        if(result <= 0) //either an error happened in the pread or it hit an unallocated space
        { 	 // perror("some error occurred in pread");
            return -1;
        }
        // No data
        else if (data2.valid == 0) {
            //No record available means empty space we can insert in it
            ssize_t wresult = pwrite(fd, &item, sizeof(DataItem), Offset);
            if(wresult < 0) {
                perror("Insertion Failed");
                return count;
            }
            // put address to the next record in the chain
            
            data1.nextOffset = Offset;
            wresult = pwrite(fd, &data1, sizeof(DataItem), offsetOfLastRecordInChain);
            if(wresult < 0) {
                perror("Insertion Failed");
                return count;
            }
            return count;
        }   
    }
    return count;
}


/* Functionality: using a key, it searches for the data item
 *          1. use the hash function to determine which bucket to search into
 *          2. search for the element in the hashed bucket index.
 *          3. return the number of records accessed (searched)
 *
 * Input:  fd: filehandler which contains the db
 *         item: the dataitem which contains the key you will search for
 *               the dataitem is modified with the data when found
 *         count: No. of record searched
 *
 * Output: the number of read operations in the file until we found the item
 */

int searchItemChainingAlgorithm(int fd,struct DataItem* item,int *count)
{
	//Definitions
	struct DataItem data;                               //a variable to read in it the records from the db
	*count = 0;				                            //No of accessed records
	int hashIndex = hashCodeChaining(item->key);  		//calculate the Bucket index
	int Offset = hashIndex*sizeof(Bucket);				//Offset variable which we will use to iterate on the db

    // Loop on all records within same bucket.
    for(int i = 0; i < RECORDSPERBUCKET; i++) {
        //on the linux terminal use man pread to check the function manual
	    ssize_t result = pread(fd,&data,sizeof(DataItem), Offset);
        //one record accessed
        (*count)++;
        //check whether it is a valid record or not
        if(result <= 0) //either an error happened in the pread or it hit an unallocated space
        { 	 
            perror("some error occurred in pread");
            return -1;
        }
        else if (data.valid == 1 && data.key == item->key) {
            //I found the needed record
            item->data = data.data ;
            return Offset;
        }   
        else { //not the record I am looking for
            Offset +=sizeof(DataItem);  //move the offset to next record
        }
    }

    // If we reached here it means that we have gone through all records in the bucket 
    // so we are going to see if there is any chaining occured. 
    int offsetOfNextRecord = data.nextOffset;

    // If there is no chaining return -1.
    while(offsetOfNextRecord != -1)
    {
        // Get the result of chaining
        ssize_t result = pread(fd,&data,sizeof(DataItem), offsetOfNextRecord);
        //one record accessed
        (*count)++;
        //check whether it is a valid record or not
        if(result <= 0) //either an error happened in the pread or it hit an unallocated space
        { 	 // perror("some error occurred in pread");
            return -1;
        }
        else if (data.valid == 1 && data.key == item->key) {
            //I found the needed record
            item->data = data.data;
            return offsetOfNextRecord;
        }  
        offsetOfNextRecord = data.nextOffset;
    }
     
    // No item found
    return -1;
}


int DisplayOverflowPart(int fd){
	struct DataItem data;
	int count = 0;
	int Offset = 0;

	for(Offset = STARTING_ADDRESS_OF_OVERFLOW_RECORDS; Offset< (FILESIZE + OVERFLOW_PART); Offset+=sizeof(DataItem)) 
	{
		ssize_t result = pread(fd,&data,sizeof(DataItem), Offset);
		if(result < 0)
		{ 	  perror("some error occurred in pread");
			  return -1;
		} else if (result == 0 || data.valid == 0 ) { //empty space found or end of file
			printf("Overflow Record: %d, Offset %d:~\n",(Offset-STARTING_ADDRESS_OF_OVERFLOW_RECORDS)/sizeof(DataItem), Offset);
		} else {
			pread(fd,&data,sizeof(DataItem), Offset);
			printf("Overflow Record: %d, Offset: %d, Data: %d, key: %d , Next Offset: %d\n", 
                (Offset-STARTING_ADDRESS_OF_OVERFLOW_RECORDS)/sizeof(DataItem),Offset,data.data,data.key, data.nextOffset);
			count++;
		}
	}
	return count;
}

int deleteItemChainingAlgorithm(int fd, struct DataItem* item, int *count){

    //Case 1: Item that will be deleted in the bucket
    	//Definitions
	struct DataItem data, prevData;                     //Variables to read in it the records from the db
    int prevOffset, nextOffset;                                     //Store previous offset.
	*count = 0;				                            //No of accessed records
	int hashIndex = hashCodeChaining(item->key);  		//calculate the Bucket index
	int Offset = hashIndex*sizeof(Bucket);				//Offset variable which we will use to iterate on the db

    // Loop on all records within same bucket.
    for(int i = 0; i < RECORDSPERBUCKET; i++) {
        //on the linux terminal use man pread to check the function manual
	    ssize_t result = pread(fd,&data,sizeof(DataItem), Offset);
        //one record accessed
        (*count)++;
        //check whether it is a valid record or not
        if(result <= 0) //either an error happened in the pread or it hit an unallocated space
        { 	 
            perror("some error occurred in pread");
            return -1;
        }
        else if (data.valid == 1 && data.key == item->key) {
            //I found the needed record
            printf("Delete: Offset %d\n",Offset);
            return deleteItemChaining(fd, Offset, data.nextOffset);
        }   
        else { //not the record I am looking for
            Offset +=sizeof(DataItem);  //move the offset to next record
        }
    }

    // Case 2: deleting item in overflow part
    prevData = data;
    prevOffset = Offset - sizeof(DataItem);
    nextOffset = data.nextOffset;
    while(nextOffset != -1) {
        Offset = nextOffset;
        // Get the result of chaining
        ssize_t result = pread(fd,&data,sizeof(DataItem), Offset);
        nextOffset = data.nextOffset;
        //one record accessed
        (*count)++;
        //check whether it is a valid record or not
        if(result <= 0) //either an error happened in the pread or it hit an unallocated space
        { 	 // perror("some error occurred in pread");
            return -1;
        }
        else if (data.valid == 1 && data.key == item->key) {
            //I found the needed record
            prevData.nextOffset = nextOffset;
            ssize_t wresult = pwrite(fd, &prevData, sizeof(DataItem), prevOffset);
            if(wresult < 0) {
                perror("Deletion Failed");
                return -1;                      //Failed
            }
            printf("Delete: Offset %d\n",Offset);
            return deleteItemChaining(fd, Offset, 0);
        }  
        prevData = data;
        prevOffset = Offset;
    }

    // No item found
    printf("No Item found to delete");
    return -1;
}

int deleteItemChaining(int fd, int Offset, int nextOffset)
{
	struct DataItem dummyItem;
	dummyItem.valid = 0;
	dummyItem.key = -1;
	dummyItem.data = 0;
    dummyItem.nextOffset = nextOffset;
	int result = pwrite(fd,&dummyItem,sizeof(DataItem), Offset);
	return result;
}

int displayChainingAlgorithm(int fd) {
    struct DataItem data;
	int count = 0;
	int Offset = 0;
	for(Offset =0; Offset< FILESIZE;Offset += sizeof(DataItem))
	{
		ssize_t result = pread(fd,&data,sizeof(DataItem), Offset);
		if(result < 0)
		{ 	  perror("some error occurred in pread");
			  return -1;
		} else if (result == 0 || data.valid == 0 ) { //empty space found or end of file
			printf("Bucket: %d, Offset %d:~, Next Offset: %d\n",Offset/BUCKETSIZE, Offset, data.nextOffset);
		} else {
			pread(fd,&data,sizeof(DataItem), Offset);
			printf("Bucket: %d, Offset: %d, Data: %d, key: %d, Next Offset: %d\n",Offset/BUCKETSIZE,Offset,data.data,data.key, data.nextOffset);
					 count++;
		}
	}
    count += DisplayOverflowPart(fd);
	return count;
}
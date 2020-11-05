#include "readfile.h"

/* Hash function to choose bucket
 * Input: key used to calculate the hash
 * Output: HashValue;
 */
int hashCode(int key){
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
   //TODO
   return 0;
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
	int hashIndex = hashCode(item->key);  				//calculate the Bucket index
	int Offset = hashIndex*sizeof(Bucket);				//Offset variable which we will use to iterate on the db

    // Loop on all records within same bucket.
    for(int i = 0; i < RECORDSPERBUCKET; i++) {
        //on the linux terminal use man pread to check the function manual
	    ssize_t result = pread(fd,&data,sizeof(DataItem), Offset);
        //one record accessed
        (*count)++;
        //check whether it is a valid record or not
        if(result <= 0) //either an error happened in the pread or it hit an unallocated space
        { 	 // perror("some error occurred in pread");
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
    int offsetOfNextRecord = item->nextOffset;

    // If there is no chaining return -1.
    while(offsetOfNextRecord != -1)
    {
        // Get the result of chaining
        ssize_t result = pread(fd,&data,sizeof(DataItem), Offset);
        //one record accessed
        (*count)++;
        //check whether it is a valid record or not
        if(result <= 0) //either an error happened in the pread or it hit an unallocated space
        { 	 // perror("some error occurred in pread");
            return -1;
        }
        else if (data.valid == 1 && data.key == item->key) {
            //I found the needed record
            item->data = data.data ;
            return Offset;
        }  
        offsetOfNextRecord = item->nextOffset;
    }
     
    // No item found
    return -1;

}
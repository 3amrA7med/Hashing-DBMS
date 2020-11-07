#include "readfile.h"

#define PRIME 7     //I CHOSED SEVEN CAUSE IT NEEDS TO BE LESS THAN THE TABEL LIST (WHICH IS 10) AND PRIME

int hash1(int key){
    return key % MBUCKETS;
}

//this hash funtion squares the key and take the middle digit
// note that here we took one digit cause the array we have is just 10 elements
// int hash2(int key) {
//     int square = key*key;
//     if(square>10) {
//         square = square/10;
//         return square % 10;
//     } else {
//         return square;      //in this case square = square % 10
//     }
// }

int primeHash(int key) {
    return PRIME - (key % PRIME);
}

int hash2(int key) {
    return (hash1(key) + primeHash(key)) % MBUCKETS;
}

int insertItem(int fd,DataItem item) 
{

    struct DataItem data;
    int count = 0;
    //first hash function
    int hashIndex = hash1(item.key);
    int Offset = hashIndex*sizeof(Bucket);

    ssize_t result = pread(fd,&data,sizeof(DataItem), Offset);
    count++;
    if(result <= 0) 
    {
        return -1;  //error
    }
    else if (data.valid == 0)
    {   //found
        pwrite(fd,&item,sizeof(DataItem),Offset);
        return count; //stored
    }
    
    //checking second row in the same bucket
    Offset += sizeof(DataItem);
    result = pread(fd,&data,sizeof(DataItem), Offset);
    count++;

    if(result <= 0) {
        return -1;  //error
    }
    else if (data.valid == 0) {
        pwrite(fd,&item,sizeof(DataItem),Offset);
        return count; //stored
    }

    //second hash function
    hashIndex = hash2(item.key);
    Offset = hashIndex*sizeof(Bucket);
    result = pread(fd,&data,sizeof(DataItem), Offset);
    count++;

    if(result <= 0) 
    {
        return -1;  //error
    } 
    else if (data.valid == 0) //found
    {   
        pwrite(fd,&item,sizeof(DataItem),Offset);
        return count;//stored
    }

    //starting openAddressing
    int start = Offset;
    Offset += sizeof(DataItem);
    while(Offset != start) 
    {
        result = pread(fd,&data,sizeof(DataItem), Offset);
        count++;
        if (result <= 0) 
        {
            return -1;
        } 
        else if (data.valid == 0)
        {
            pwrite(fd,&item,sizeof(DataItem),Offset);
            return count; //stored
        } 

        Offset += sizeof(DataItem);

        //for starting from the begining of the file if we reached the end
        if(Offset == FILESIZE) 
            Offset = 0;
    }
    return -2; //array is full
}



int searchItem(int fd,struct DataItem* item,int *count)
{
	struct DataItem data;
    //first hash function
    int hashIndex = hash1(item->key);
    int Offset = hashIndex*sizeof(Bucket);

    ssize_t result = pread(fd,&data,sizeof(DataItem), Offset);
    (*count)++;

    if(result <= 0) 
    {
        return -1;  //error
    }
    else if (data.valid == 1 && data.key == item->key)
    {   //found
        item->data = data.data;
        return Offset;
    } 

    //checking second row in the same bucket
    Offset += sizeof(DataItem);
    result = pread(fd,&data,sizeof(DataItem), Offset);
    (*count)++;

    if(result <= 0) {
        return -1;  //error
    }
    else if (data.valid == 1 && data.key == item->key) //found
    {
        item->data = data.data;
        return Offset;
    }

    //second hash function
    hashIndex = hash2(item->key);
    Offset = hashIndex*sizeof(Bucket);
    result = pread(fd,&data,sizeof(DataItem), Offset);
    (*count)++;

    if(result <= 0) 
    {
        return -1;  //error
    } 
    else if (data.valid == 1 && data.key == item->key) //found
    {   
        item->data = data.data;
        return Offset;
    } 
    //starting openAddressing
    int start = Offset;
    Offset += sizeof(DataItem);
    while(Offset != start) 
    {
        result = pread(fd,&data,sizeof(DataItem), Offset);
        (*count)++;

        if (result <= 0) 
        {
            return -1;
        } 
        else if (data.valid == 1 && data.key == item->key) 
        {
            item->data = data.data;
            return Offset;
        } 

        Offset += sizeof(DataItem);

        //for starting from the begining of the file if we reached the end
        if(Offset == FILESIZE)
            Offset = 0;
    }
    return -2; //item not found
}

int DisplayFile(int fd){

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
			printf("Bucket: %d, Offset %d:~\n",Offset/BUCKETSIZE,Offset);
		} else {
			pread(fd,&data,sizeof(DataItem), Offset);
			printf("Bucket: %d, Offset: %d, Data: %d, key: %d\n",Offset/BUCKETSIZE,Offset,data.data,data.key);
					 count++;
		}
	}
	return count;
}


/* Functionality: Delete item at certain offset
 *
 * Input:  fd: filehandler which contains the db
 *         Offset: place where it should delete
 *
 * Hint: you could only set the valid key and write just and integer instead of the whole record
 */
int deleteOffset(int fd, int Offset)
{
	struct DataItem dummyItem;
	dummyItem.valid = 0;
	dummyItem.key = -1;
	dummyItem.data = 0;
	int result = pwrite(fd,&dummyItem,sizeof(DataItem), Offset);
	return result;
}


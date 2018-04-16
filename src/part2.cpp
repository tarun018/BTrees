#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <math.h> 

using namespace std;

vector < vector < vector < int > > > data;
vector < int > outputBuffer;

long int totalbuckets, bucketSize, currentPtr, ptr = 2, totalRecords = 0;;

long int M, B;

void printResult()
{
	int j;
	int size=outputBuffer.size();
	//cout<<"--------------------------------------------------------\n";
	for(j=0;j<size; j++)
	{
		cout << outputBuffer[j] << endl;
	}
}

long int flipMsb(long int bucket)
{
	long int tempBucket, finalBucket;
	unsigned long int res;

	while(bucket >= totalbuckets)
	{
		tempBucket=bucket;
		res=1;		

		while(tempBucket >>= 1 && tempBucket)
		{
		    res <<= 1;
		}
		finalBucket = bucket-res;
		bucket = finalBucket;
	}

	return bucket;
}

int isRecordPresent(long int bucket, long int record, long int cmod)
{
	
	int flag,j,k,size1,size2,po;
	flag=0;
	size1=data[bucket].size();
	long int tempBucket;
	for(po=cmod;po>1;po=po/2)
	{
		if(record>0){
			tempBucket = record % po;
		}
		else{
			tempBucket = po-((-record) % po);
			if(tempBucket==po){
				tempBucket=0;	
			}
		}

		bucket = flipMsb(tempBucket);
		size1=data[bucket].size();
		for(j=0;j<size1;j++)	
		{
			size2=data[bucket][j].size();
			for(k=0;k<size2;k++)
			{
				if(data[bucket][j][k] == record)
				{
					flag=1;
					break;
				}
			}
			if(flag==1){
				break;
			}
		}

		
	}
	return flag;
}

void checkBufferSize(){
	if(outputBuffer.size() == (B/4))
	{
		printResult();
		outputBuffer.clear();
	}

}

void insert(vector< int > input)
{
	long int bucketNum = 0, tempBucket=0, overflowBucketNo = 1, isOverflow = 0,overflowBucketNumber = 1;
	long int newBucketNum = 0, tempNewBucketNum = 0, record = 0;
	float occupancy = 0.0;
	int logVal,modulus,size1,size2,size3,ip;
	unsigned int j,k;
	vector < vector < int > > outerBucket;
	vector < vector < int > > newOuterBucket;
	vector < int > primaryBucket;
	vector < int > overflowBucket;
	vector < int > newBucket;

	size1=input.size();

	for(ip=0;ip<size1;ip++)
	{
		isOverflow=0;
		logVal = ceil(log2(totalbuckets));
		modulus = pow(2,logVal);
		if(input[ip]>0){
			tempBucket = input[ip] % modulus;
		}
		else{
			tempBucket = modulus-((-input[ip]) % modulus);
			if(tempBucket==modulus){
				tempBucket=0;	
			}
		}
		if(tempBucket >=totalbuckets){
			bucketNum = flipMsb(tempBucket);	
		}
		else{
			bucketNum=tempBucket;
		}

		if(data[bucketNum].size() == 0)
		{
			outerBucket.clear();
			primaryBucket.clear();
			totalRecords=totalRecords+1;
			primaryBucket.push_back(input[ip]);
			outerBucket.push_back(primaryBucket);
			data[bucketNum] = outerBucket;
			
			checkBufferSize();
			outputBuffer.push_back(input[ip]);
		}
		else if(isRecordPresent(bucketNum, input[ip],modulus)==1)
		{
			continue;
		}
		else if(data[bucketNum][0].size() != bucketSize)
		{
			totalRecords=totalRecords+1;
			data[bucketNum][0].push_back(input[ip]);
	
			checkBufferSize();
			outputBuffer.push_back(input[ip]);
		}
		else
		{
			overflowBucketNumber = 1;
			size2=data[bucketNum].size();
			overflowBucketNo = size2;
			while(overflowBucketNumber < overflowBucketNo)
			{
				size3=data[bucketNum][overflowBucketNumber].size();
				if(size3 != bucketSize)
				{
					isOverflow = 1;
					totalRecords=totalRecords+1;
					data[bucketNum][overflowBucketNumber].push_back(input[ip]);
					checkBufferSize();
					outputBuffer.push_back(input[ip]);
					break;
				}
				else
				{
					overflowBucketNumber++;
				}
			}
			if(isOverflow == 0)
			{
				overflowBucket.clear();
				overflowBucket.push_back(input[ip]);
				totalRecords=totalRecords+1;
				data[bucketNum].push_back(overflowBucket);
				checkBufferSize();
				outputBuffer.push_back(input[ip]);
			}
			
		}

		float total=(bucketSize * totalbuckets);
		occupancy = totalRecords / total; 
		if(occupancy >= 0.75)
		{
			// introduce new bucket
			totalbuckets=totalbuckets+1;
			newOuterBucket.clear();
			newBucket.clear();
			newOuterBucket.push_back(newBucket);
			data.push_back(newOuterBucket);

			newBucketNum = 0, tempNewBucketNum = 0, record = 0;

			for(j = 0; j < data[currentPtr].size(); j++)
			{
				for(k = 0; k < data[currentPtr][j].size(); k++)
				{
					record=0;
					record = data[currentPtr][j][k];
					logVal = ceil(log2(totalbuckets));
					modulus = pow(2,logVal);
					if(record>0){
						tempNewBucketNum = record % modulus;
					}
					else{
						tempNewBucketNum = modulus-((-record) % modulus);
						if(tempBucket==modulus){
						tempBucket=0;	
			}
					}
					if(tempNewBucketNum >=totalbuckets){
						newBucketNum = flipMsb(tempNewBucketNum);	
					}
					else{
						newBucketNum=tempNewBucketNum;
					}
					
					if(newBucketNum == currentPtr)
					{
						continue;
					}
					else
					{	
						if(data[newBucketNum][0].size() == bucketSize)
						{
							overflowBucketNo = data[newBucketNum].size();
							overflowBucketNumber = 1;
							while(overflowBucketNumber < overflowBucketNo)
							{
								if(data[newBucketNum][overflowBucketNumber].size() == bucketSize)
								{
									overflowBucketNumber=overflowBucketNumber+1;
								}
								else
								{
									data[newBucketNum][overflowBucketNumber].push_back(record);
									data[currentPtr][j].erase (data[currentPtr][j].begin()+k);
									k=k-1;
									isOverflow = 1;
									break;
								}
							}
							if(isOverflow == 0)
							{
								overflowBucket.clear();
								overflowBucket.push_back(record);
								data[newBucketNum].push_back(overflowBucket);
								data[currentPtr][j].erase (data[currentPtr][j].begin()+k);
								k=k-1;
							}
						}
						else
						{
							k=k+1;
							data[newBucketNum][0].push_back(record);
							k=k-1;
							data[currentPtr][j].erase (data[currentPtr][j].begin()+k);
							k=k-1;		
						}
					}
				}
			}
			currentPtr = currentPtr + 1;
			if(currentPtr == ptr)
			{
				currentPtr = 0;
				ptr = ptr * 2;
				
			}
		}
	}
}


int main(int argc, char *argv[])
{
	long int inputBufferCapacity = 0;
	string filename;
	char line[100];
	vector < int > inputBuffer;

	data.resize(2);
	totalbuckets = 2;

	if(argc != 4)
	{
		cout << "Give command line arguments :Filename, number of buffers(M), buffer size(B)\n";
		exit(1);
	}	

	filename = argv[1];
	M = atoi(argv[2]);
	B = atoi(argv[3]);
	bucketSize = B/4;	

	if(bucketSize == 0)
	{
		cout << "Insufficient buffer capacity\n";
		exit(1);
	}

	FILE *fd;
	fd = fopen(filename.c_str(), "r");
	inputBufferCapacity = bucketSize*(M - 1);
	if(fd == NULL)
	{
		cout << "Input file does not exist\n";
		exit(1);
	}

	while(fgets(line, sizeof(line), fd) != NULL)
	{
		if(inputBuffer.size() == inputBufferCapacity)
		{
			insert(inputBuffer);
			inputBuffer.clear();
		}
		inputBuffer.push_back(atoi(line));
		
	}
	if(inputBuffer.size() > 0)
	{
		insert(inputBuffer);
		inputBuffer.clear();
	}

	
	if(outputBuffer.size() > 0)
	{
		printResult();
	}
	return 0;
}

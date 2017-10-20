#ifndef FUNCTIONS
#define FUNCTIONS

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define BLOCK_SIZE 512
#define ENTRY_SIZE 10

struct BlockIndex {
	char fileName[50];
	int size;
	char userName[50];
	char* contents;
	int startBit;
	struct BlockIndex* next;

};

typedef struct BlockIndex* DataIndex;


struct FileNameTable {
	char name[50];
	uint32_t size;
	uint32_t sizeFilled;
	uint32_t sizeLeft;
	int numberOfFiles;
	DataIndex* blockTable; //array of pointers to block indexes

};

typedef struct FileNameTable* Disk;

int diskIsNull(Disk disk){
	if(disk == NULL) return 1;
	else return 0;
}

DataIndex allocateBlock(Disk disk){
	// int* index = &disk->numberOfFiles;
	// disk->blockTable = (DataIndex*)realloc(disk->blockTable, (size_t)(index+1)*sizeof(DataIndex));
	DataIndex block = (DataIndex)calloc(1, sizeof(struct BlockIndex));
	block->size = ENTRY_SIZE;
	// block->fileName = (char*)calloc(50, sizeof(char));
	block->contents = (char*)calloc(ENTRY_SIZE, sizeof(char));
	block->startBit = 1;
	block->next = NULL;
	(disk->sizeFilled)+=ENTRY_SIZE;
	(disk->sizeLeft)-=ENTRY_SIZE;
	return block;
}

int findNULLPointer(Disk disk){
	for(int i = 0; i*ENTRY_SIZE < disk->size; i++){
		if(disk->blockTable[i] == NULL){
			printf("%d\n", i);
			return i;
		} 
	}

	return -1;
}

DataIndex appendBlock(Disk disk,DataIndex index){
	(disk->sizeFilled)+= ENTRY_SIZE;
	(disk->sizeLeft)-= ENTRY_SIZE;
	int tableIndex = findNULLPointer(disk);
	DataIndex block = (DataIndex)calloc(1, sizeof(struct BlockIndex));

	index->next = block;
	disk->blockTable[tableIndex] = block;

	block->size = BLOCK_SIZE/4;
	strcpy(block->fileName, index->fileName);
	strcpy(block->userName, index->userName);
	block->contents = calloc(ENTRY_SIZE, sizeof(char));
	block->startBit = 0;
	block->next = NULL;
	return block;
}

int fileExists(Disk disk, char* fileName, char* userName){
	for(int i = 0; i < disk->numberOfFiles; i++){
		if(disk->blockTable[i] == NULL) continue;
		if((strcmp(fileName, disk->blockTable[i]->fileName) == 0) && (strcmp(userName, disk->blockTable[i]->userName) == 0)){
			return 1;
		}
	}
	return 0;
}



void fileToBlocks(Disk disk, DataIndex block, FILE* fp){
	fseek(fp, 0, SEEK_SET);
	int index = 0;
	int ch;
	int tableIndex = findNULLPointer(disk);
	disk->blockTable[tableIndex] = block;
	while((ch = fgetc(fp)) != EOF){
		if(index == ENTRY_SIZE){
			block = appendBlock(disk, block);
			index = 0;
		}
		block->contents[index++] = ch;
	}
}

void freeBlockList(DataIndex index){
	while(index != NULL){
		DataIndex next = index->next;
		free(index->contents);
		free(index);
		index = next;
	}
}

void printBlockList(DataIndex index){
	while(index != NULL){
		DataIndex next = index->next;
		printf("%s\n", index->fileName );
		index = next;
	}
}

void setNullPointers(Disk disk, char* fileName){
	for(int i = 0; i*ENTRY_SIZE < disk->size; i++){
		if(disk->blockTable[i] == NULL) continue;
		if(strcmp(disk->blockTable[i]->fileName, fileName) == 0){
			disk->blockTable[i] = NULL;
		}
	}
}


void clearBlockTable(Disk disk){
	int size = disk->numberOfFiles;
	for(int i = 0; i*ENTRY_SIZE < disk->size; i++){
		if(disk->blockTable[i] == NULL) continue;
		DataIndex block = disk->blockTable[i];
		setNullPointers(disk, disk->blockTable[i]->fileName);

		freeBlockList(block);
	}
}

void freeDisk(Disk disk){
	clearBlockTable(disk);
	free(disk->blockTable);
	free(disk);
}

void removeFile(Disk disk){
	char fileName[50];
	char userName[50];
	printf("Enter a file name to remove:\n");
	scanf("%s", fileName);
	printf("Enter the owner of the file:\n");
	scanf("%s", userName);
	DataIndex block = NULL;
	int flag = 1;
	for(int i = 0; i*ENTRY_SIZE < disk->size; i++){
		if(disk->blockTable[i] == NULL) continue;
		if((strcmp(fileName, disk->blockTable[i]->fileName) == 0) && (strcmp(userName, disk->blockTable[i]->userName) == 0)){
			if(flag){
				block = disk->blockTable[i];
				flag = 0;
			}
			disk->blockTable[i] = NULL;

		}
	}

	if(block!= NULL)
		freeBlockList(block);

}

void renameFile(Disk disk){
	char oldName[50];
	char newName[50];
	char userName[50];
	printf("Enter a file name to replace:\n");
	scanf("%s", oldName);
	printf("Enter the owner of the file:\n");
	scanf("%s", userName);
	printf("Enter the new file name:\n");
	scanf("%s", newName);

	for(int i = 0; i*ENTRY_SIZE < disk->size; i++){
		if(disk->blockTable[i] == NULL) continue;

		if((strcmp(oldName, disk->blockTable[i]->fileName) == 0) && (strcmp(userName, disk->blockTable[i]->userName) == 0)) {
			strcpy(disk->blockTable[i]->fileName, newName);
		}
	}

	
}

void loadDisk(){ //needs implementation

}

void printDiskToFile(Disk disk){ //needs implementation
	char buffer[50];
	FILE* fp;
	printf("Enter a file name to save your disk:\n");
	scanf("%s", buffer);
	fp = fopen(buffer, "w");
	if(fp == NULL){
		printf("Cannot write to file\n");
		return;
	}

	fprintf(fp, "%s %u %u %d ", disk->name, disk->size, disk->sizeFilled, disk->numberOfFiles);
	fclose(fp);

}

void printFileNames(Disk disk){
	int size = disk->numberOfFiles;
	for(int i = 0; i*ENTRY_SIZE < disk->size; i++){
		if(disk->blockTable[i] == NULL) continue;
		printf("File Name-%s\n", disk->blockTable[i]->fileName);
		printf("User Name-%s\n\n", disk->blockTable[i]->userName);
	}
}

void printDiskInformation(Disk disk){
	printf("Disk name-%s\n", disk->name );
	printf("Disk size-%d\n", disk->size);
	printf("Disk size filled-%d\n", disk->sizeFilled );
	printf("Disk size left-%d\n\n", disk->sizeLeft);
	printf("Files on disk:\n");
	printFileNames(disk);
}

void extractFile(Disk disk){

}



void addFile(Disk disk){
	FILE* fp;
	int size;
	// int* index = &disk->numberOfFiles;
	char buffer[50];
	char buffer2[50];
	printf("Enter a file name:\n");
	scanf("%s", buffer);
	fp = fopen(buffer, "r");
	if(fp == NULL){
		printf("File not found\n");
		return;
	}

	printf("Enter a user name:\n");
	scanf("%s", buffer2);
	if(fileExists(disk, buffer, buffer2)){
		printf("File already exits\n");
		return;
	}

	fseek(fp, 0, SEEK_END);//sets file pointer to a given offset
    size = ftell(fp);//finds size of a file
    if(size > disk->sizeLeft){
		printf("Not enough space on disk\n");
		return;
	}
	
	DataIndex block = allocateBlock(disk);
	strcpy(block->fileName, buffer);
	
	strcpy(block->userName, buffer2);
    fileToBlocks(disk, block, fp);

	strcpy(block->fileName, buffer);
	(disk->numberOfFiles)++;
    fclose(fp);
}




Disk allocateDisk(){
	char buffer[50];
	int numberOfBlock = 0;
	printf("Name your disk: \n");
	scanf("%s", buffer);
	printf("Enter number of blocks to allocate for your disk (size of 512bytes per block): \n");
	scanf("%d", &numberOfBlock);

	Disk disk = (Disk)calloc(1,sizeof(struct FileNameTable));
	strcpy(disk->name, buffer);
	disk->size = numberOfBlock*BLOCK_SIZE;
	disk->sizeLeft = numberOfBlock*BLOCK_SIZE;
	disk->numberOfFiles = 0;
	disk->sizeFilled = 0;
	disk->blockTable = (DataIndex*)calloc(disk->size/ENTRY_SIZE, sizeof(DataIndex)); //allocating array of BlockTables
	for(int i = 0; i*ENTRY_SIZE < disk->size; i++){
		disk->blockTable[i] = NULL;
		// printf("%d\n", i);
	}

	return disk;
}

#endif //FUNCTIONS
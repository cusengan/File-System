#ifndef FUNCTIONS
#define FUNCTIONS

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define BLOCK_SIZE 512
#define ENTRY_SIZE 128

struct BlockIndex {
	char fileName[50];
	int size;
	char userName[50];
	char* contents;
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

void allocateBlock(Disk disk){
	int* index = &disk->numberOfFiles;
	disk->blockTable[*index] = (DataIndex)calloc(1, sizeof(struct BlockIndex));
	disk->blockTable[*index]->size = BLOCK_SIZE/4;
	disk->blockTable[*index]->contents = calloc(ENTRY_SIZE, sizeof(char));
	disk->blockTable[*index]->next = NULL;
	(disk->sizeFilled)+=BLOCK_SIZE;
}

DataIndex appendBlock(DataIndex index){
	index->next = (DataIndex)calloc(1, sizeof(struct BlockIndex));
	index->next->size = BLOCK_SIZE/4;
	index->next->contents = calloc(ENTRY_SIZE, sizeof(char));
	index->next->next = NULL;
	return index->next;
}

void fileToBlocks(DataIndex block, FILE* fp){
	fseek(fp, 0, SEEK_SET);
	int index = 0;
	int ch;
	while((ch = fgetc(fp)) != EOF){
		if(index == ENTRY_SIZE){
			block = appendBlock(block);
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

void clearBlockTable(Disk disk){
	int size = disk->numberOfFiles;
	for(int i = 0; i < size; i++){
		freeBlockList(disk->blockTable[i]);
	}
	free(disk->blockTable);
}

void renameFile(Disk disk){
	char oldName[50];
	char newName[50];
	printf("Enter a file name to replace:\n");
	scanf("%s", oldName);
	printf("Enter the new file name:\n");
	scanf("%s", newName);

	for(int i = 0; i < disk->numberOfFiles; i++){
		if(strcmp(oldName, disk->blockTable[i]->fileName) == 0){
			strcpy(disk->blockTable[i]->fileName, newName);
			return;
		}
	}

	printf("File not found\n");
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
	for(int i = 0; i < size; i++){
		printf("File Name-%s\n", disk->blockTable[i]->fileName);
		printf("User Name-%s\n\n", disk->blockTable[i]->userName);
	}
}

void printDiskInformation(Disk disk){
	printf("Disk name-%s\n", disk->name );
	printf("Disk size-%d\n", disk->size);
	printf("Disk size filled-%d\n\n", disk->sizeFilled );
	printf("Files on disk:\n");
	printFileNames(disk);
}

void freeDisk(Disk disk){
	clearBlockTable(disk);
	free(disk);
}


void addFile(Disk disk){
	FILE* fp;
	int size;
	if(disk->sizeFilled == disk->size){
		printf("Disk is full\n");
		return;
	}
	int* index = &disk->numberOfFiles;
	char buffer[50];
	allocateBlock(disk);
	printf("Enter a file name:\n");
	scanf("%s", buffer);
	fp = fopen(buffer, "r");
	if(fp == NULL){
		printf("File not found\n");
		return;
	}
	strcpy(disk->blockTable[*index]->fileName, buffer);
	fseek(fp, 0, SEEK_END);//sets file pointer to a given offset
    size = ftell(fp);//finds size of a file
    if(size > disk->size - disk->sizeFilled){
    	printf("Not enough space on disk\n");
    	return;
    }
    fileToBlocks(disk->blockTable[*index], fp);

	strcpy(disk->blockTable[*index]->fileName, buffer);
	printf("Enter a user name:\n");
	scanf("%s", disk->blockTable[*index]->userName);
	(*index)++;
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
	disk->numberOfFiles = 0;
	disk->sizeFilled = 0;
	disk->blockTable = (DataIndex*)calloc(1, sizeof(DataIndex)); //allocating array of BlockTables

	return disk;
}

#endif //FUNCTIONS
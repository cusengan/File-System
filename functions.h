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

void allocateBlock(Disk disk){
	// int* index = &disk->numberOfFiles;
	// disk->blockTable = (DataIndex*)realloc(disk->blockTable, (size_t)(index+1)*sizeof(DataIndex));
	disk->blockTable[*index] = (DataIndex)calloc(1, sizeof(struct BlockIndex));
	disk->blockTable[*index]->size = ENTRY_SIZE;
	disk->blockTable[*index]->contents = calloc(ENTRY_SIZE, sizeof(char));
	disk->blockTable[*index]->startBit = 1;
	disk->blockTable[*index]->next = NULL;
	(disk->sizeFilled)+=ENTRY_SIZE;
	(disk->sizeLeft)-=ENTRY_SIZE;
}

DataIndex appendBlock(Disk disk,DataIndex index){
	(disk->sizeFilled)+= ENTRY_SIZE;
	(disk->sizeLeft)-= ENTRY_SIZE;
	index->next = (DataIndex)calloc(1, sizeof(struct BlockIndex));
	index->next->size = BLOCK_SIZE/4;
	strcpy(index->next->fileName, index->fileName);
	strcpy(index->next->userName, index->userName);
	index->next->contents = calloc(ENTRY_SIZE, sizeof(char));
	index->next->startBit = 0;
	index->next->next = NULL;
	return index->next;
}

void fileToBlocks(Disk disk, DataIndex block, FILE* fp){
	fseek(fp, 0, SEEK_SET);
	int index = 0;
	int ch;
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


void clearBlockTable(Disk disk){
	int size = disk->numberOfFiles;
	for(int i = 0; i < size; i++){
		freeBlockList(disk->blockTable[i]);
	}
}

void freeDisk(Disk disk){
	clearBlockTable(disk);
	free(disk->blockTable);
	free(disk);
}

void changeBlockPointers(Disk disk, int index){
	if(index == disk->numberOfFiles - 1){
		freeBlockList(disk->blockTable[index]);
		disk->blockTable = (DataIndex*)realloc(disk->blockTable, (size_t)(disk->numberOfFiles - 1)*sizeof(DataIndex));
		(disk->numberOfFiles)-- ;
	}else{
		freeBlockList(disk->blockTable[index]);
		while(index < disk->numberOfFiles-1){
			disk->blockTable[index] = disk->blockTable[index+1];
			index++;
		}

		disk->blockTable = (DataIndex*)realloc(disk->blockTable, (size_t)(disk->numberOfFiles - 1)*sizeof(DataIndex));
		(disk->numberOfFiles)-- ;
	}
}

void removeFile(Disk disk){
	char fileName[50];
	printf("Enter a file name to remove:\n");
	scanf("%s", fileName);
	for(int i = 0; i < disk->numberOfFiles; i++){
		if(strcmp(fileName, disk->blockTable[i]->fileName) == 0){
			changeBlockPointers(disk, i);
			return;
		}
	}

	printf("File not found\n");

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
	printf("Disk size filled-%d\n", disk->sizeFilled );
	printf("Disk size left-%d\n\n", disk->sizeLeft);
	printf("Files on disk:\n");
	printFileNames(disk);
}



void addFile(Disk disk){
	FILE* fp;
	int size;
	if(disk->sizeFilled == disk->size){
		printf("Not enough space on disk\n");
		return;
	}
	int* index = &disk->numberOfFiles;
	char buffer[50];
	printf("Enter a file name:\n");
	scanf("%s", buffer);
	fp = fopen(buffer, "r");
	if(fp == NULL){
		printf("File not found\n");
		return;
	}
	allocateBlock(disk);
	strcpy(disk->blockTable[*index]->fileName, buffer);
	fseek(fp, 0, SEEK_END);//sets file pointer to a given offset
    size = ftell(fp);//finds size of a file
    fileToBlocks(disk, disk->blockTable[*index], fp);

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
	disk->sizeLeft = numberOfBlock*BLOCK_SIZE;
	disk->numberOfFiles = 0;
	disk->sizeFilled = 0;
	disk->blockTable = (DataIndex*)calloc(size/ENTRY_SIZE, sizeof(DataIndex)); //allocating array of BlockTables

	return disk;
}

#endif //FUNCTIONS
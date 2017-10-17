#include <stdio.h>
#include "functions.h"

int main(){
	int choice;
	Disk disk = NULL;

	while(1){
		printf("1: Allocate a disk\n");
		printf("2. Add a file\n");
		printf("3. Print contents\n");
		printf("4. Print disk to file\n");
		printf("5. Rename a file\n");
		printf("6. Remove a file\n");
		printf("7. Exit\n");
		scanf("%d", &choice);

		switch(choice){
			case 1:
				if(diskIsNull(disk)){
				disk = allocateDisk();
				}else{
					freeDisk(disk);
					disk = allocateDisk();
				} 
				break;
			case 2:
				if(diskIsNull(disk)){
					printf("Please allocate a disk\n");
				}else{
					addFile(disk);
				} 
				break;
			case 3:
				if(diskIsNull(disk)){
					printf("Please allocate a disk\n");
				}else{
					printDiskInformation(disk);
				} 
				break;
			case 4:
				if(diskIsNull(disk)){
					printf("Please allocate a disk\n");
				}else{
					printDiskToFile(disk);
				} 
				break;
			case 5:
				if(diskIsNull(disk)){
					printf("Please allocate a disk\n");
				}else{
					renameFile(disk);
				} 
				break;
			case 6:
				if(diskIsNull(disk)){
					printf("Please allocate a disk\n");
				}else{
					removeFile(disk);
				} 
				break;
			case 7:
				if(!diskIsNull(disk))
					freeDisk(disk);
				exit(0);

			default:
				printf("Not a valid choice\n");

		}
	}
}
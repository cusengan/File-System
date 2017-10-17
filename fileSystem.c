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
		printf("6. Exit\n");
		scanf("%d", &choice);

		switch(choice){
			case 1: 
				disk = allocateDisk();
				break;
			case 2:
				addFile(disk);
				break;
			case 3:
				printDiskInformation(disk);
				break;
			case 4:
				printDiskToFile(disk);
				break;
			case 5:
				renameFile(disk);
				break;
			case 6:
				freeDisk(disk);
				exit(0);

			default:
				printf("Not a valid choice\n");

		}
	}
}
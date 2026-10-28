/*
 * Operating Systems Laboratory - Assignment III
 * Question 6: Simulation of Linked File Allocation Technique
 *
 * Description:
 * In Linked File Allocation, each file is represented as a linked list of disk blocks.
 * The directory entry contains the file name, the starting block, and the ending block.
 * Each block points to the next block in the file, with the last block pointing to -1 (NULL).
 *
 * Features:
 * 1. Allocate a file using Linked Allocation (chain of blocks).
 * 2. Display the File Directory and the block chain (Start -> ... -> End -> NULL).
 * 3. Search and trace an allocated file's block traversal.
 * 4. Delete an existing file and free all linked blocks.
 * 5. Display the complete disk block allocation status.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_BLOCKS 50
#define MAX_FILES 20

// Representation of each disk block
typedef struct {
    int is_allocated; // 0 = Free, 1 = Allocated
    int next;         // Index of next block, or -1 if last block
    char file_name[30];
} DiskBlock;

// Representation of file entry in directory
typedef struct {
    char name[30];
    int start_block;
    int end_block;
    int num_blocks;
} FileEntry;

DiskBlock disk[TOTAL_BLOCKS];
FileEntry directory[MAX_FILES];
int file_count = 0;

// Initialize disk
void init_disk() {
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        disk[i].is_allocated = 0;
        disk[i].next = -1;
        strcpy(disk[i].file_name, "");
    }
    file_count = 0;
}

// Display disk block allocation map
void display_disk_status() {
    printf("\n--- Disk Block Map (Total %d Blocks: 0 to %d) ---\n", TOTAL_BLOCKS, TOTAL_BLOCKS - 1);
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        if (i % 10 == 0) printf("\n[%2d-%2d]: ", i, (i + 9 < TOTAL_BLOCKS) ? i + 9 : TOTAL_BLOCKS - 1);
        printf("%c ", disk[i].is_allocated ? 'X' : '.'); // 'X' = Allocated, '.' = Free
    }
    printf("\n(Legend: '.' = Free Block, 'X' = Allocated Block)\n");
}

// Count free blocks on disk
int count_free_blocks() {
    int free_count = 0;
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        if (!disk[i].is_allocated) free_count++;
    }
    return free_count;
}

// Create a file with linked allocation
void create_file() {
    if (file_count >= MAX_FILES) {
        printf("Error: Directory is full (Max %d files).\n", MAX_FILES);
        return;
    }

    char fname[30];
    int num_blocks;

    printf("\nEnter file name: ");
    scanf("%s", fname);

    // Check duplicate file name
    for (int i = 0; i < file_count; i++) {
        if (strcmp(directory[i].name, fname) == 0) {
            printf("Error: A file named '%s' already exists!\n", fname);
            return;
        }
    }

    printf("Enter number of blocks required: ");
    scanf("%d", &num_blocks);

    if (num_blocks <= 0) {
        printf("Error: Block count must be greater than 0!\n");
        return;
    }

    if (count_free_blocks() < num_blocks) {
        printf("Error: Not enough free blocks available! (Needed: %d, Available: %d)\n",
               num_blocks, count_free_blocks());
        return;
    }

    int mode;
    printf("Block allocation mode:\n");
    printf("1. Enter block chain manually\n");
    printf("2. Allocate free blocks automatically\n");
    printf("Enter choice (1-2): ");
    scanf("%d", &mode);

    int allocated[TOTAL_BLOCKS];

    if (mode == 1) {
        printf("Enter %d distinct block numbers in sequence:\n", num_blocks);
        for (int i = 0; i < num_blocks; i++) {
            int blk;
            printf("Block %d: ", i + 1);
            scanf("%d", &blk);

            if (blk < 0 || blk >= TOTAL_BLOCKS) {
                printf("Error: Block %d is out of disk range!\n", blk);
                return;
            }
            if (disk[blk].is_allocated) {
                printf("Error: Block %d is already allocated to file '%s'!\n", blk, disk[blk].file_name);
                return;
            }
            for (int j = 0; j < i; j++) {
                if (allocated[j] == blk) {
                    printf("Error: Block %d entered more than once!\n", blk);
                    return;
                }
            }
            allocated[i] = blk;
        }
    } else {
        // Automatic allocation of first available free blocks
        int found = 0;
        for (int i = 0; i < TOTAL_BLOCKS && found < num_blocks; i++) {
            if (!disk[i].is_allocated) {
                allocated[found++] = i;
            }
        }
    }

    // Link the allocated blocks together
    for (int i = 0; i < num_blocks; i++) {
        int curr = allocated[i];
        disk[curr].is_allocated = 1;
        strcpy(disk[curr].file_name, fname);

        if (i < num_blocks - 1) {
            disk[curr].next = allocated[i + 1]; // points to next block
        } else {
            disk[curr].next = -1; // last block points to -1 (NULL)
        }
    }

    // Add entry to file directory
    strcpy(directory[file_count].name, fname);
    directory[file_count].start_block = allocated[0];
    directory[file_count].end_block = allocated[num_blocks - 1];
    directory[file_count].num_blocks = num_blocks;
    file_count++;

    printf("\nFile '%s' successfully allocated!\n", fname);
    printf("Start Block: %d, End Block: %d\n", allocated[0], allocated[num_blocks - 1]);
    printf("Linked Chain: ");
    for (int i = 0; i < num_blocks; i++) {
        printf("%d -> ", allocated[i]);
    }
    printf("NULL\n");
}

// Display all files and their linked chains
void display_directory() {
    if (file_count == 0) {
        printf("\nDirectory is currently empty. No files allocated yet.\n");
        return;
    }

    printf("\n======================================================================\n");
    printf("                 LINKED FILE ALLOCATION DIRECTORY                     \n");
    printf("======================================================================\n");
    printf("+-----------------+-------------+-----------+------------+---------------------------+\n");
    printf("| File Name       | Start Block | End Block | Block Count| Linked Chain              |\n");
    printf("+-----------------+-------------+-----------+------------+---------------------------+\n");

    for (int i = 0; i < file_count; i++) {
        printf("| %-15s | %-11d | %-9d | %-10d | ",
               directory[i].name, directory[i].start_block, directory[i].end_block, directory[i].num_blocks);

        // Traverse chain
        int curr = directory[i].start_block;
        int printed = 0;
        while (curr != -1) {
            char buf[20];
            sprintf(buf, "%d -> ", curr);
            printf("%s", buf);
            printed += strlen(buf);
            curr = disk[curr].next;
        }
        printf("NULL");
        printed += 4;

        for (int p = printed; p < 25; p++) printf(" ");
        printf(" |\n");
    }
    printf("+-----------------+-------------+-----------+------------+---------------------------+\n");
}

// Search and trace a file
void search_file() {
    if (file_count == 0) {
        printf("\nDirectory is empty.\n");
        return;
    }

    char fname[30];
    printf("\nEnter file name to search: ");
    scanf("%s", fname);

    int idx = -1;
    for (int i = 0; i < file_count; i++) {
        if (strcmp(directory[i].name, fname) == 0) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("File '%s' not found!\n", fname);
        return;
    }

    printf("\n--- File Details for '%s' ---\n", directory[idx].name);
    printf("Start Block : %d\n", directory[idx].start_block);
    printf("End Block   : %d\n", directory[idx].end_block);
    printf("Total Blocks: %d\n", directory[idx].num_blocks);
    printf("Traversal Order of Blocks:\n");

    int curr = directory[idx].start_block;
    int step = 1;
    while (curr != -1) {
        printf("  Block #%d: [Disk Block %d] -> Next Pointer: %d\n",
               step++, curr, disk[curr].next);
        curr = disk[curr].next;
    }
}

// Delete a file and free its linked blocks
void delete_file() {
    if (file_count == 0) {
        printf("\nDirectory is empty.\n");
        return;
    }

    char fname[30];
    printf("\nEnter file name to delete: ");
    scanf("%s", fname);

    int idx = -1;
    for (int i = 0; i < file_count; i++) {
        if (strcmp(directory[i].name, fname) == 0) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("File '%s' not found!\n", fname);
        return;
    }

    // Traverse the linked chain and free each block
    int curr = directory[idx].start_block;
    int freed_count = 0;
    while (curr != -1) {
        int next_block = disk[curr].next;
        disk[curr].is_allocated = 0;
        disk[curr].next = -1;
        strcpy(disk[curr].file_name, "");
        freed_count++;
        curr = next_block;
    }

    printf("File '%s' deleted successfully. Freed %d linked blocks.\n", fname, freed_count);

    // Remove from directory list
    for (int i = idx; i < file_count - 1; i++) {
        directory[i] = directory[i + 1];
    }
    file_count--;
}

int main() {
    init_disk();

    int choice;
    while (1) {
        printf("\n==================================================\n");
        printf("     LINKED FILE ALLOCATION SIMULATION (Q6)       \n");
        printf("==================================================\n");
        printf("1. Create / Allocate File\n");
        printf("2. Display File Directory and Links\n");
        printf("3. Search and Trace File Chain\n");
        printf("4. Delete File\n");
        printf("5. Display Disk Allocation Map\n");
        printf("6. Exit\n");
        printf("Enter your choice (1-6): ");

        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1: create_file(); break;
            case 2: display_directory(); break;
            case 3: search_file(); break;
            case 4: delete_file(); break;
            case 5: display_disk_status(); break;
            case 6:
                printf("Exiting Linked File Allocation simulation.\n");
                return 0;
            default:
                printf("Invalid choice! Please choose between 1 and 6.\n");
        }
    }

    return 0;
}

/*
 * Operating Systems Laboratory - Assignment III
 * Question 5: Simulation of Indexed File Allocation Technique
 *
 * Description:
 * In Indexed File Allocation, each file has an index block that stores the addresses
 * of all the data blocks allocated to that file. The directory entry contains only the
 * file name and the index block pointer.
 *
 * Features:
 * 1. Allocate a file using Indexed Allocation (Index block + Data blocks).
 * 2. Display the File Directory and Index Block contents.
 * 3. Search and view details of an allocated file.
 * 4. Delete an existing file and free its blocks.
 * 5. Display the complete disk block allocation status.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_BLOCKS 50
#define MAX_FILES 20
#define MAX_BLOCKS_PER_FILE 20

// Disk block status: 0 = Free, 1 = Allocated
int disk[TOTAL_BLOCKS];

// Structure to store file information in the directory
typedef struct {
    char name[30];
    int index_block;
    int num_blocks;
    int data_blocks[MAX_BLOCKS_PER_FILE];
} File;

File directory[MAX_FILES];
int file_count = 0;

// Initialize disk
void init_disk() {
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        disk[i] = 0; // all blocks free initially
    }
    file_count = 0;
}

// Display disk block allocation map
void display_disk_status() {
    printf("\n--- Disk Block Map (Total %d Blocks: 0 to %d) ---\n", TOTAL_BLOCKS, TOTAL_BLOCKS - 1);
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        if (i % 10 == 0) printf("\n[%2d-%2d]: ", i, (i + 9 < TOTAL_BLOCKS) ? i + 9 : TOTAL_BLOCKS - 1);
        printf("%c ", disk[i] ? 'X' : '.'); // 'X' = Allocated, '.' = Free
    }
    printf("\n(Legend: '.' = Free Block, 'X' = Allocated Block)\n");
}

// Find free blocks count
int count_free_blocks() {
    int free_count = 0;
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        if (!disk[i]) free_count++;
    }
    return free_count;
}

// Create a file with indexed allocation
void create_file() {
    if (file_count >= MAX_FILES) {
        printf("Error: Directory is full (Max %d files).\n", MAX_FILES);
        return;
    }

    char fname[30];
    int num_blocks;

    printf("\nEnter file name: ");
    scanf("%s", fname);

    // Check if file already exists
    for (int i = 0; i < file_count; i++) {
        if (strcmp(directory[i].name, fname) == 0) {
            printf("Error: A file with name '%s' already exists!\n", fname);
            return;
        }
    }

    printf("Enter number of data blocks required: ");
    scanf("%d", &num_blocks);

    if (num_blocks <= 0 || num_blocks > MAX_BLOCKS_PER_FILE) {
        printf("Error: Invalid number of blocks (1 to %d allowed).\n", MAX_BLOCKS_PER_FILE);
        return;
    }

    // Need 1 index block + num_blocks data blocks
    if (count_free_blocks() < num_blocks + 1) {
        printf("Error: Not enough free blocks available! (Needed: %d, Available: %d)\n",
               num_blocks + 1, count_free_blocks());
        return;
    }

    int index_blk;
    printf("Enter Index Block number (0 to %d): ", TOTAL_BLOCKS - 1);
    scanf("%d", &index_blk);

    if (index_blk < 0 || index_blk >= TOTAL_BLOCKS) {
        printf("Error: Invalid block number!\n");
        return;
    }

    if (disk[index_blk] == 1) {
        printf("Error: Block %d is already allocated! Cannot use as index block.\n", index_blk);
        return;
    }

    int mode;
    printf("Block allocation mode:\n");
    printf("1. Enter block numbers manually\n");
    printf("2. Allocate free blocks automatically\n");
    printf("Enter choice (1-2): ");
    scanf("%d", &mode);

    int allocated_blocks[MAX_BLOCKS_PER_FILE];

    if (mode == 1) {
        printf("Enter %d distinct block numbers:\n", num_blocks);
        for (int i = 0; i < num_blocks; i++) {
            int blk;
            printf("Block %d: ", i + 1);
            scanf("%d", &blk);

            if (blk < 0 || blk >= TOTAL_BLOCKS) {
                printf("Error: Block %d is out of range!\n", blk);
                return;
            }
            if (blk == index_blk) {
                printf("Error: Block %d is already chosen as the Index Block!\n", blk);
                return;
            }
            if (disk[blk] == 1) {
                printf("Error: Block %d is already allocated on disk!\n", blk);
                return;
            }
            // Check duplicate in current allocation
            for (int j = 0; j < i; j++) {
                if (allocated_blocks[j] == blk) {
                    printf("Error: Block %d already specified for this file!\n", blk);
                    return;
                }
            }
            allocated_blocks[i] = blk;
        }
    } else {
        // Automatic allocation
        int found = 0;
        for (int i = 0; i < TOTAL_BLOCKS && found < num_blocks; i++) {
            if (i != index_blk && disk[i] == 0) {
                allocated_blocks[found++] = i;
            }
        }
    }

    // Commit allocation
    disk[index_blk] = 1;
    for (int i = 0; i < num_blocks; i++) {
        disk[allocated_blocks[i]] = 1;
    }

    // Save to directory
    strcpy(directory[file_count].name, fname);
    directory[file_count].index_block = index_blk;
    directory[file_count].num_blocks = num_blocks;
    for (int i = 0; i < num_blocks; i++) {
        directory[file_count].data_blocks[i] = allocated_blocks[i];
    }
    file_count++;

    printf("\nFile '%s' successfully allocated!\n", fname);
    printf("Index Block: %d\n", index_blk);
    printf("Allocated Data Blocks: ");
    for (int i = 0; i < num_blocks; i++) {
        printf("%d ", allocated_blocks[i]);
    }
    printf("\n");
}

// Display File Directory
void display_directory() {
    if (file_count == 0) {
        printf("\nDirectory is currently empty. No files allocated yet.\n");
        return;
    }

    printf("\n======================================================================\n");
    printf("                INDEXED FILE ALLOCATION DIRECTORY                     \n");
    printf("======================================================================\n");
    printf("+-----------------+-------------+------------+------------------------+\n");
    printf("| File Name       | Index Block | Block Count| Allocated Data Blocks  |\n");
    printf("+-----------------+-------------+------------+------------------------+\n");

    for (int i = 0; i < file_count; i++) {
        printf("| %-15s | %-11d | %-10d | ", directory[i].name, directory[i].index_block, directory[i].num_blocks);
        for (int j = 0; j < directory[i].num_blocks; j++) {
            printf("%d ", directory[i].data_blocks[j]);
        }
        // Pad for neat alignment
        int printed_chars = 0;
        for (int j = 0; j < directory[i].num_blocks; j++) {
            if (directory[i].data_blocks[j] >= 10) printed_chars += 3;
            else printed_chars += 2;
        }
        for (int p = printed_chars; p < 22; p++) printf(" ");
        printf(" |\n");
    }
    printf("+-----------------+-------------+------------+------------------------+\n");
}

// Search and inspect a file's index block
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
    printf("Index Block : %d\n", directory[idx].index_block);
    printf("Number of Data Blocks: %d\n", directory[idx].num_blocks);
    printf("Index Table (Pointers in Block %d):\n", directory[idx].index_block);
    for (int i = 0; i < directory[idx].num_blocks; i++) {
        printf("  Entry %d -> Data Block %d\n", i + 1, directory[idx].data_blocks[i]);
    }
}

// Delete a file and free its blocks
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

    // Free the index block and all data blocks
    disk[directory[idx].index_block] = 0;
    for (int i = 0; i < directory[idx].num_blocks; i++) {
        disk[directory[idx].data_blocks[i]] = 0;
    }

    printf("File '%s' deleted. Freed Index Block %d and %d Data Blocks.\n",
           fname, directory[idx].index_block, directory[idx].num_blocks);

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
        printf("    INDEXED FILE ALLOCATION SIMULATION (Q5)       \n");
        printf("==================================================\n");
        printf("1. Create / Allocate File\n");
        printf("2. Display File Directory\n");
        printf("3. Search and Inspect File (Index Block)\n");
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
                printf("Exiting Indexed File Allocation simulation.\n");
                return 0;
            default:
                printf("Invalid choice! Please choose between 1 and 6.\n");
        }
    }

    return 0;
}

#include "mem_sim.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>

#define FRAME_NO_EXISTS_IN_RAM -1
#define OUT_LIMIT -2

int indexRam = 0;
int size = 0;
int numberOfUnusedPages = NUM_OF_PAGES;

simulationInfo* init_system(){
    int i;
    struct simulationInfo * simInfo = (struct simulationInfo *) malloc( sizeof(struct simulationInfo ) ) ;

    for (i = 0; i < MEMORY_SIZE; i++)
        simInfo->main_memory[i] = '0';

    for (i = 0; i < NUM_OF_PAGES; i++) {
        simInfo->page_table[i].D = 0;
        simInfo->page_table[i].frame = -1;
        simInfo->page_table[i].V = 0;
    }
    return simInfo ;
}

void store(struct simulationInfo *mem_sim, int address, char value) { // main memorye store islemi
    int physicAdd, physicBlc;
    int pageNumber = address / PAGE_SIZE;
    int offset = address % PAGE_SIZE;

    int pagePlace = findPage(mem_sim, address, pageNumber);

    if (pagePlace == OUT_LIMIT) return;

    mem_sim->page_table[pageNumber].D = 1;
    mem_sim->page_table[pageNumber].V = 1;
    mem_sim->page_table[pageNumber].frame = indexRam / PAGE_SIZE;

    physicBlc = get_frame_in_ram(mem_sim, address);
    if(physicBlc != -1) physicBlc = pageNumber;

    physicAdd = physicBlc * PAGE_SIZE + offset;
    indexRam = physicAdd;

    mem_sim->main_memory[physicAdd] = value;
    size++;

}

int checkIfItFits(char file[]) { // dosya sigiyor mu kontrolu
    FILE* fp = fopen(file, "r");

    if (fp == NULL) {
        printf("File Not Found!\n");
        return -1;
    }

    fseek(fp, 0L, SEEK_END);
    long int sizeOfFile = ftell(fp);
    fclose(fp);

    if((MEMORY_SIZE - size) < sizeOfFile) return -1;
    int numberOfPages = sizeOfFile/PAGE_SIZE;
    if(sizeOfFile % PAGE_SIZE != 0) numberOfPages++;
    if(numberOfUnusedPages < numberOfPages) return -1;

    else return sizeOfFile;
}

int readFromFile(struct simulationInfo *mem_sim, char file[]) { // task dosyasinda verilen dosyalari okumak icin
    char *code = NULL;
    int sizeOfFile = checkIfItFits(file);
    if(sizeOfFile == -1){
        printf("HELLL NO.\n");
        return -1;
    }

    int numberOfPages = sizeOfFile/PAGE_SIZE;
    if(sizeOfFile % PAGE_SIZE != 0) numberOfPages++;
    code = malloc(sizeOfFile * sizeof(char));
    FILE *filePointer = fopen(file, "r");
    int count = 0;
    int c = 0;
    int oldRandomPage = 0, randomPage = 0;

    do{
        c = fgetc(filePointer);
        *code = (char) c;
        if(count == PAGE_SIZE || count == 0){
            while(1) {
                randomPage = (rand() % (NUM_OF_PAGES));
                if (mem_sim->page_table[randomPage].frame == -1) break;
            }
            count = 0;
            numberOfUnusedPages--;
        }
        if(c != EOF)
            store(mem_sim, randomPage*PAGE_SIZE + count, *code);

        *code++;   count++;
    }while(c != EOF);
    fclose(filePointer);
    return 1;
}

int get_frame_in_ram(struct simulationInfo *mem_sim, int address){ //calc the place of the block
    int numPage = address / PAGE_SIZE;
    if(mem_sim->page_table[numPage].V == 1){ //the page exists in ram
        return mem_sim->page_table[numPage].frame;
    }
    return FRAME_NO_EXISTS_IN_RAM;
}

void clear_system(struct simulationInfo * mem_sim){
    free(mem_sim);
}

void print_memory(simulationInfo* mem_sim) {
    int i;
    printf("\n Physical memory\n");
    for(i = 0; i < MEMORY_SIZE; i++) {
        if(mem_sim->main_memory[i] == '\n')
            printf("%i [\\n]\n", i);
        else
            printf("%i [%c]\n",i, mem_sim->main_memory[i]);
    }

}

void print_page_table(simulationInfo* mem_sim) {
    int i;
    printf("\n page table \n");
    printf("Valid\t\tDirty\t\tFrame\n");

    for (i = 0; i < NUM_OF_PAGES; i++)
        printf("[%d]\t\t[%d]\t\t[%d]\n", mem_sim->page_table[i].V,
               mem_sim->page_table[i].D,
               mem_sim->page_table[i].frame);
}

int findPage(struct simulationInfo *mem_sim, int address, int LogicalPage){
    int frame = get_frame_in_ram(mem_sim, address);

    if (address < 0 || address >= MEMORY_SIZE) {
        perror("ERROR: Outside the limit");
        return OUT_LIMIT;
    }

    if (frame != FRAME_NO_EXISTS_IN_RAM) return 0;

    /**
     * MISS page not in ram
     */
    return -1;
}


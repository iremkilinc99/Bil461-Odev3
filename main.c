#include<stdio.h>
#include <string.h>
#include "stdlib.h"
#include "mem_sim.h"


int main(){
    int numberOfLines = 0, numberOfMemoryAccess = 0;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    struct simulationInfo *mem_sim = init_system() ;

    FILE* fp = fopen("./tasks.txt", "r");
    if (fp == NULL) {
        printf("File Not Found!\n");
        return -1;
    }

    while ((read = getline(&line, &len, fp)) != -1) numberOfLines++; // to calculate number of processes
    fclose(fp);

    int *accessAdress[numberOfLines], processes[numberOfLines], burst_time[numberOfLines];

    fp = fopen("./tasks.txt", "r");
    if (fp == NULL) {
        printf("File Not Found!\n");
        return -1;
    }

    numberOfLines = 0;
    while ((read = getline(&line, &len, fp)) != -1){
        char *fileName = strtok(line," ");
        while(strtok(NULL," ") != NULL) {
            numberOfMemoryAccess++;
        }
        accessAdress[numberOfLines] = malloc(sizeof(int) * numberOfMemoryAccess); // her process icin jagged array olusturdum
        accessAdress[numberOfLines++][0] = numberOfMemoryAccess;
        numberOfMemoryAccess = 0;
    }

    fclose(fp);

    int i, currentProcess = 0;
    for(i = 0; i < numberOfLines; i++){
        processes[i] = i;
        burst_time[i] = 5; // kendim sabit bir burst time verdim ama random da olabilir denemek isterseniz
    }

    fp = fopen("./tasks.txt", "r");
    if (fp == NULL) {
        printf("File Not Found!\n");
        return -1;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        char *fileName = strtok(line," ");
        int read = readFromFile(mem_sim, fileName);
        int size = accessAdress[currentProcess][0];

        if(read != -1){ // it fits in memory
            int indexOfAdress = 0;
            while(indexOfAdress != size) {
                char *adress = strtok(NULL," ");
                accessAdress[currentProcess][indexOfAdress] = atoi(adress);
                indexOfAdress++;
            }
         //   print_memory(mem_sim);
            print_page_table(mem_sim);
            printf("\n");
        }else
            accessAdress[currentProcess][0]  = -1;
        currentProcess ++;
    }

    fclose(fp);
    if (line)
        free(line);

    roundRobinScheduler(processes, numberOfLines, burst_time, (int **) &accessAdress, 3, mem_sim);
    clear_system(mem_sim);
}

void roundRobinScheduler(int processes[], int n, int bt[], int **accessAddresses, int quantum, struct simulationInfo *mem_sim) {
    // Make a copy of burst times bt[] to store remaining burst times.
    int rem_bt[n], queue[n];
    int i;

    for (i = 0 ; i < n ; i++){
        rem_bt[i] =  bt[i];
        queue[i] = processes[i];
    }

    // Keep traversing processes in round robin manner
    // until all of them are not done.
    while(1){
        int done = 1;
        // Traverse all processes one by one repeatedly
        int i;
        for (i = 0 ; i < n; i++){
            // If burst time of a process is greater than 0 then only need to process further
            if (rem_bt[i] > 0){
                done = 0; // There is a pending process

                if (rem_bt[i] > quantum && accessAddresses[queue[i]][0] != -1){
                    int j ;
                    for (j = bt[i] - rem_bt[i] ; j < quantum; j++)
                        if(mem_sim->main_memory[accessAddresses[queue[i]][j]] == '\n')
                            printf("%i [\\n]\n", accessAddresses[queue[i]][j]);
                        else if (mem_sim->main_memory[accessAddresses[queue[i]][j]] != NULL)
                            printf("%i [%c]\n", accessAddresses[queue[i]][j], mem_sim->main_memory[accessAddresses[queue[i]][j]]);
                        else
                            printf("%i is empty\n", accessAddresses[queue[i]][j]);

                    rem_bt[i] -= quantum;

                }else if(accessAddresses[queue[i]][0] != -1){// If burst time is smaller than or equal to quantum. Last cycle for this process

                    int j;
                    for (j = bt[i] - rem_bt[i]; j < bt[i]; j++){
                        if(mem_sim->main_memory[accessAddresses[queue[i]][j]] == '\n')
                            printf("%i [\\n]\n", accessAddresses[queue[i]][j]);
                        else if (mem_sim->main_memory[accessAddresses[queue[i]][j]] != NULL)
                            printf("%i [%c]\n", accessAddresses[queue[i]][j], mem_sim->main_memory[accessAddresses[queue[i]][j]]);
                        else
                            printf("%i is empty\n", accessAddresses[queue[i]][j]);
                    }

                    int k ;
                    // make its remaining burst time = 0
                    rem_bt[i] = 0;
                }else{
                    rem_bt[i] = 0;
                }
            }
        }

        // If all processes are done
        if (done == 1)
            break;
    }
}

#ifndef MEM_SIM_H_INCLUDED
#define MEM_SIM_H_INCLUDED

#define PAGE_SIZE 2048 //10 kucuk sayi ile denemek isterseniz
#define MEMORY_SIZE 1048576 // 50 kucuk sayi ile denemek isterseniz
#define NUM_OF_PAGES 512  //5 kucuk sayi ile denemek isterseniz benim kendi gönderdigim dosyalar icin kullanabilirsiniz

typedef struct page_descriptor{
    int V ;
    int D;
    int frame;
}page_descriptor;

typedef struct simulationInfo{
    page_descriptor page_table[NUM_OF_PAGES];
    char main_memory[MEMORY_SIZE];
}simulationInfo;

void roundRobinScheduler(int processes[], int n, int bt[], int**, int quantum, struct simulationInfo *mem_sim);
void store(struct simulationInfo * mem_sim , int address, char value);
int indexRam;
simulationInfo* init_system();
int findPage(struct simulationInfo *mem_sim, int address, int LogicalPage);
int readFromFile(struct simulationInfo *mem_sim, char *buffer);
int get_frame_in_ram(struct simulationInfo *mem_sim, int address);
void writeToMemory(struct simulationInfo * mem_sim ,int LogicPage, int from);
void print_memory(simulationInfo* mem_sim);
void clear_system(struct simulationInfo * mem_sim);
void print_page_table(simulationInfo* mem_sim);
int checkIfItFits(char taskFile[]);
#endif // MEM_SIM_H_INCLUDED

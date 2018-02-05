#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sysexits.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
//pipe
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h> //mmap
#include "Job.h"

#define FILE_LENGTH 0x100

const char* program_name;
int master(int quantum, char input[], char preempt_output[], char no_preempt_output[]);
int childSpeak();
void calculateLenght(Job* job);
void print_help(FILE* stream, int exit_code);

void print3();
void print2();
void print1();

char *name = "mem.txt";
int FILESIZE = 1024;

void print_help(FILE* stream, int exit_code)
{
    fprintf(stream, "Usage: %s options [inputfile ...]\n", program_name);
    fprintf(stream,
            " -op	--output-preemption         	Output of preemption scheduler\n"
            " -on	--output-no-preemption          Output of no-preemption scheduler\n"
            " -i 	--input                         Input file with jobs list\n"
            " -q 	--quantum                       Quantum lenght(preempt. scheduler)\n");

    exit(exit_code);
}


int main(int argc, char* argv[]){  
	
	int quantum;
	int ch;
	char input[]= "01.dat";
	char preempt_output[50];
	char no_preempt_output[50];
   /*static struct option long_options[] =
{
    {"output_preemption", required_argument, NULL, 'p'},
    {"output_no_preemption", required_argument, NULL, 'n'},
    {"input", required_argument, NULL, 'i'},
    {"quantum", required_argument, NULL, 'q'},
    {NULL, 0, NULL, 0}
};
	
	int k=2;
	while ((ch = getopt_long(argc, argv, "o:,i:,q:", long_options, NULL)) != -1){
    // check to see if a single character or long option came through
    
    switch (ch)
    {            
		 case 'i':
			strcpy(input, optarg);
			
			break;
		 
		 case 'q':
			quantum = atoi(optarg);		
			
			break;
		
		case '?':
			print_help(stderr, EX_USAGE);
			break;
			
		case 'o':
			if(optarg[0] == 'n'){
				
				k=2;
				while(optarg[k] != '\0'){
					no_preempt_output[k-2] = optarg[k];
					
					k++;				
				}
				
				no_preempt_output[k-2] = '\0';
				
			}else if(optarg[0] == 'p'){
				
				k=2;
				while(optarg[k] != '\0'){
					preempt_output[k-2] = optarg[k];
					k++;
				}
				
				preempt_output[k-2] = '\0';
				
			}
			
			//printf("option O %s e il char %c\n",optarg, optarg[0]);
			break;
			
    }
    
}	*/
	
    /****Creazione processi*****/

    /*int child_status, child_pid;

    child_pid = master(quantum, input, preempt_output, no_preempt_output);

    waitpid(child_pid, &child_status, 0); 

    if(WIFEXITED(child_status)) {
        //printf("The child process exited normally with exit code %d.\n", WEXITSTATUS(child_status));
    }
    else {
        //printf("The child process exited abnormally with code %d.\n", WEXITSTATUS(child_status));
    }*/

    //printf("Done with the main program.\n");
    /**fine creazione processi**/
    

	pid_t firstChild, secondChild;
	firstChild = fork();
	if(firstChild > 0)
	{
	  // In parent
	  secondChild = fork();
	  if(secondChild > 0)
	  {
			//print1();
			//printf("P");
			master(10, "01.dat", "preempt", "no_preempt");
		 
		// In parent
	  }
	  else if(secondChild < 0)
	  {
		// Error
	  }
	  else
	  {
		  print2();
		// In secondChild
	  }
	}
	else if(firstChild < 0 )
	{
	  // Error
	} 
	else
	{	
		//scheduler_not_preemptive( 5, "nonserve");
		//print3();
	  // In firstChild
	}
	
	int status1, status2;
	/*firstChild = wait(&status1);
	secondChild = wait(&status2);*/
	waitpid(firstChild,&status1,0);
	waitpid(secondChild,&status2,0);
    return 0;
}
/*Ora quese funziona sono a caso, ma posso lancaire tre funzioni diverse*/
void print1(){
	
	int fd;
    void* file_memory;
	
	int num =11;
	/* Prepare a file large enough to hold an unsigned integer. */
    fd = open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    lseek(fd, FILE_LENGTH+1, SEEK_SET);
    write(fd, "", 1);
    lseek(fd, 0, SEEK_SET);

	/* Create the memory mapping. */
    file_memory = mmap(0, FILE_LENGTH, PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

	/* Write a random integer to memory-mapped area. */
    sprintf((char*)file_memory, "%d\n", num);

	/* Release the memory (unnecessary because the program exits). */
    munmap(file_memory, FILE_LENGTH);

   
    
	//master(10, "01.dat", "preempt", "no_preempt");
	pid_t p = getpid();
	printf("P: %d\n",p);

}
void print2(){
	/**Nuova versione**/
	sleep(3);
	int i;
    int fd;
    int *map;  /* mmapped array of int's */

    fd = open("jobMem.txt", O_RDONLY);
    if (fd == -1) {
	perror("Error opening file for reading");
	exit(EXIT_FAILURE);
    }

    map = mmap(0, FILESIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
	close(fd);
	perror("Error mmapping the file (reading)");
	exit(EXIT_FAILURE);
    }
    
    /* Read the file int-by-int from the mmap
     */
    for (i = 1; i <=100; ++i) {
	printf("%d: %d\n", i, map[i]);
    }

    if (munmap(map, FILESIZE) == -1) {
	perror("Error un-mmapping the file");
    }
    close(fd);


	pid_t p = getpid();
	printf("S: %d\n",p);
	exit(0);
}


void print3(){
	
	int fd;
    void* file_memory;
    int integer;

    fd = open(name, O_RDWR, S_IRUSR | S_IWUSR);

    file_memory = mmap(0, FILE_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    close(fd);

    sscanf(file_memory, "%d", &integer);

    printf("value:%d\n", integer);

    sprintf((char*)file_memory, "%d\n", 2*integer);

    munmap(file_memory, FILE_LENGTH);
    
	pid_t p = getpid();
	
	printf("F: %d\n",p);

}

int childSpeak(){
        pid_t child_pid;

        child_pid = fork();
        if(child_pid != 0) {
            return child_pid; // if I am the parent, return the child pid

        }else{
            printf("This is the child\n");
            exit(EX_OSERR);
        }

}
int master(int quantum, char input[], char preempt_output[], char no_preempt_output[]){
		
        pid_t child_pid;
		
        child_pid = fork();
        if(child_pid != 0) {
            return child_pid; // if I am the parent, return the child pid

        }else{

            FILE * infile;
            char line[121];
            char ** info = NULL; //per ora è dove salvo le istruzioni
            int llen;
            long counter = 0;
            char *isJ; //per verifica j
            int jobCount = 0;
            int *instrCount = NULL;
            const char s[2] = ",";
            char *token;

            infile = fopen(input,"r");
            if(infile == NULL){
              printf("Error!");
              exit(1);
            }

             while (fgets(line,120,infile)) {

                // Allocate memory for pointer to line just added
                info = realloc(info,(counter+1) * sizeof(char *));
                // And allocate memory for that line itself!
                llen = strlen(line);
                info[counter] = calloc(sizeof(char),llen+1);
                // Copy the line just read into that memory
                strcpy(info[counter],line);
                isJ = strchr(info[counter],'j');
                if( isJ != NULL){
                    jobCount++;

                }
                counter++;
            }
            fclose(infile);
            /**inzio instrCount**/

            instrCount = malloc(jobCount*sizeof(int));
            for(int i=0;i<jobCount; i++){
                instrCount[i]=0; //inizializzo a zero
            }
            //conto le istruzione per ogni job
            int temp=-1;
            for (int i = 0; i<counter; i++) {
                isJ = strchr(info[i],'j');
                if( isJ != NULL){
                    /**NUOVO JOB**/
                    temp++;
                }else{
                    instrCount[temp]++;
                    /**NUOVA ISTRUZIONE**/
                }
            }
            //alloco lo spazio per le istruzioni
            Job jobs[jobCount];
             for (int i=0;i<jobCount; i++){
                //printf("%d\n",instrCount[i]);
                jobs[i].instr = malloc(instrCount[i]*sizeof(*jobs[i].instr));
            }

            int k = -1; //per il numero dei jobs
            int h = -1; //per il sottonumero delle istruzioni per ogni job

            for (int i = 0; i<counter; i++) {
                isJ = strchr(info[i],'j');


                if( isJ != NULL){

                    if(k != -1){
                        //printf("Job NUMERO: %d\n",k);
                        jobs[k].numbOfInstr = h ;
                        calculateLenght(&jobs[k]);
                         //printf( "Job number %d  has IstrNumb %d\n", k, jobs[k].numbOfInstr);
                    }
                    /**NUOVO JOB**/
                    h = -1; //riazzero il numero delle istruzione per un nuovo j
                    k++;

                   /* get the first token */
                    token = strtok(info[i], s);
                    //ID
                    token = strtok(NULL, s);
                    jobs[k].id = atoi(token);

                    //Arrival time
                    token = strtok(NULL, s);
                    jobs[k].arrival_time = atoi(token);
                    //all'inizio setto instrDone a 0
                    jobs[k].instrDone = 0;
                    //printf( "Job number %d with id %d and arrival time %d\n", k, jobs[k].id, jobs[k].arrival_time);

                }else{

                    /**NUOVA ISTRUZIONE**/
                    token = strtok(info[i], s);

                    //TYPE FLAG
                    token = strtok(NULL, s);
                    jobs[k].instr[h].type_flag = atoi(token);

                    //LENGHT
                    token = strtok(NULL, s);
                    jobs[k].instr[h].lenght = atoi(token);

                    //IO_MAX
                    token = strtok(NULL, s);
                    jobs[k].instr[h].io_max = atoi(token);
                    //printf( "Istruction number %d with type flag %d,lenght %d and I/O max %d \n", h, jobs[k].instr[h].type_flag, jobs[k].instr[h].lenght, jobs[k].instr[h].io_max );
                }
                h++;

                //controllo per l'ultimo job che non ha una j dopo
                if(i == (counter -1)){
                    jobs[k].numbOfInstr = h;
                    //printf( "Job number %d  has IstrNumb %d\n", k, jobs[k].numbOfInstr);
                    calculateLenght(&jobs[k]);
                }

            }

            /**ora si passano i job agli scheduler**/
            printf("ora sto scrivendo\n");
			int i;
			int fd;
			int result;
			int *map;  /* mmapped array of int's */

			/* Open a file for writing.
			 *  - Creating the file if it doesn't exist.
			 *  - Truncating it to 0 size if it already exists. (not really needed)
			 *
			 * Note: "O_WRONLY" mode is not sufficient when mmaping.
			 */
			fd = open("jobMem.txt", O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
			if (fd == -1) {
			perror("Error opening file for writing");
			exit(EXIT_FAILURE);
			}

			/* Stretch the file size to the size of the (mmapped) array of ints
			 */
			int FILESIZE = 100* sizeof(int);
			result = lseek(fd, FILESIZE-1, SEEK_SET);
			if (result == -1) {
			close(fd);
			perror("Error calling lseek() to 'stretch' the file");
			exit(EXIT_FAILURE);
			}
			
			/* Something needs to be written at the end of the file to
			 * have the file actually have the new size.
			 * Just writing an empty string at the current file position will do.
			 *
			 * Note:
			 *  - The current position in the file is at the end of the stretched 
			 *    file due to the call to lseek().
			 *  - An empty string is actually a single '\0' character, so a zero-byte
			 *    will be written at the last byte of the file.
			 */
			result = write(fd, "", 1);
			if (result != 1) {
			close(fd);
			perror("Error writing last byte of the file");
			exit(EXIT_FAILURE);
			}

			/* Now the file is ready to be mmapped.
			 */
			map = mmap(0, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
			if (map == MAP_FAILED) {
			close(fd);
			perror("Error mmapping the file (writing)");
			exit(EXIT_FAILURE);
			}
			
			/* Now write int's to the file as if it were memory (an array of ints).
			 */
			for (i = 1; i <=100; ++i) {
			map[i] = 2 * i; 
			}

			/* Don't forget to free the mmapped memory
			 */
			if (munmap(map, FILESIZE) == -1) {
			perror("Error un-mmapping the file");
			/* Decide here whether to close(fd) and exit() or not. Depends... */
			}

			/* Un-mmaping doesn't close the file, so we still need to do that.
			 */
			close(fd);
			 printf("ora HO FINITO di scrivere\n");
			/****************************************/
			
            for(int c = 0; c<jobCount; c++){
                jobs[c].pState = NEW;
            }
			/*Job newJobs[jobCount];
			for(int ii =0; ii< jobCount; ii++){
				newJobs[ii] = jobs[ii];
			}*/
			//scheduler_preemptive(jobs, quantum, jobCount, preempt_output);
            //fprintf(stderr,"Finito preemptive\n");
            
            //scheduler_not_preemptive(newJobs, jobCount, no_preempt_output);
            //printf("Finito Not preemptive\n");	
            
            //exit(EX_OSERR);
            
            for (int i=0;i<jobCount; i++){
                free(jobs[i].instr);
            }
            free(instrCount);

        }
	//exit(0);
	return 0;
 }

 void calculateLenght(Job* job){
    int max =  (*job).numbOfInstr;
    int count;
    for (int i = 0; i < max; i++){
        count += (*job).instr[i].lenght;
    }
    (*job).totalLeght = count;
    //printf("Lunghezza istruzioni del job %d è: %d\n",(*job).id,(*job).totalLeght);

 }

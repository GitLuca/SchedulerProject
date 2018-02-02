#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sysexits.h>
#include <unistd.h>
#include <string.h>
//pipe
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Job.h"

#define MAX_BUF 1024 //per la named pipe

const char* program_name;
int master(int quantum, char input[], char preempt_output[], char no_preempt_output[]);
int childSpeak();
void calculateLenght(Job* job);
void print_help(FILE* stream, int exit_code);

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
			print1();
			//printf("P");
	
		 
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
		print3();
	  // In firstChild
	}

	int status1, status2;
	firstChild = wait(&status1);
	secondChild = wait(&status2);

    return 0;
}
/*Ora quese funziona sono a caso, ma posso lancaire tre funzioni diverse*/
void print1(){
	 /*int fd;
    char * myfifo = "/tmp/myfifo";

    // create the FIFO (named pipe) 
    mkfifo(myfifo, 0666);
    int temp[20];
    for(int i=0; i<20; i++){
		temp[i] = i;
	}	
	//write(fd, my_struct_pointer, sizeof(*my_struct_pointer)); //questo??????
    // write "Hi" to the FIFO 
    fd = open(myfifo, O_WRONLY);
    //write(fd, "Hi", sizeof("Hi"));
    write(fd, &temp, sizeof(temp));
    close(fd);

    // remove the FIFO 
    unlink(myfifo);*/
	
	pid_t p = getpid();
	printf("P: %d\n",p);
	
}
void print2(){
	master(10, "01.dat", "preempt", "no_preempt");
	/*int fd;
    char * myfifo = "/tmp/myfifo";
    //char buf[MAX_BUF];
	int tempPoint[20];
    // open, read, and display the message from the FIFO 
    fd = open(myfifo, O_RDONLY);
    //read(fd, buf, MAX_BUF);
    
    for(int i=0; i<20; i++){
		read(fd, &tempPoint[i], sizeof(tempPoint));
		 printf("Received: %d\n", tempPoint[i]);
	}	
   
    close(fd);*/

	pid_t p = getpid();
	printf("S: %d\n",p);
	
}
void print3(){
	pid_t p = getpid();
	int fd;
    char * jobfifo = "/tmp/jobfifo";
    //char buf[MAX_BUF];
	Job tempPoint[MAX_BUF];
    /* open, read, and display the message from the FIFO */
    fd = open(jobfifo, O_RDONLY);
    //read(fd, buf, MAX_BUF);
    
    for(int i=0; i<20; i++){
		read(fd, &tempPoint[i], sizeof(tempPoint));
		 printf("Received: %d\n", tempPoint[i].id);
	}	
   
    close(fd);
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
                    printf( "Job number %d with id %d and arrival time %d\n", k, jobs[k].id, jobs[k].arrival_time);

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
            
            printf( "PIPE\n");
             int fd;
			char * jobfifo = "/tmp/jobfifo";

			/* create the FIFO (named pipe) */
			mkfifo(jobfifo, 0666);

			fd = open(jobfifo, O_WRONLY);
			write(fd, &jobs, sizeof(jobs));
			close(fd);

			/* remove the FIFO */
			unlink(jobfifo);
			/****************************************/



            for(int c = 0; c<jobCount; c++){
                jobs[c].pState = NEW;
            }
			Job newJobs[jobCount];
			for(int ii =0; ii< jobCount; ii++){
				newJobs[ii] = jobs[ii];
			}
			//scheduler_preemptive(jobs, quantum, jobCount, preempt_output);
            //fprintf(stderr,"Finito preemptive\n");
            
            //scheduler_not_preemptive(newJobs, jobCount, no_preempt_output);
            //printf("Finito Not preemptive\n");	
            
            exit(EX_OSERR);
            fclose(infile);
            for (int i=0;i<jobCount; i++){
                free(jobs[i].instr);
            }
            free(instrCount);

        }

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

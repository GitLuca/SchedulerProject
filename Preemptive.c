#include "Job.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
/**round robin**/

void* corePreem (void* parameters);
int rand_lim1(int limit);
int core_status1[2];
int jobDone1 = 0;
int Q;

int mut1 = 4;

pthread_mutex_t lock1[4];

void scheduler_preemptive(Job* jobs, int quantum, int jobCount, char preempt_output[]){
    Q = quantum;
	
	FILE *fp;
	fp = freopen(preempt_output, "w+", stdout);
    /**inizializza il MUTEX **/
    for(int iii=0; iii<mut1; iii++){
        if(pthread_mutex_init(&lock1[iii], NULL) != 0){
            fprintf(stderr,"\n mutex init failed\n");
        }
    }

    for(int c = 0; c<jobCount; c++){
        jobs[c].pState = READY;
    }
    
    long long clock1 = 0;
    long long clock2 = 0;
    pthread_t thread[2];
    args thread_args[2];

    while(1){
        pthread_mutex_lock(&lock1[1]);
        if(jobDone1 == jobCount){
            break;
        }
        pthread_mutex_unlock(&lock1[1]);
        //printf("JOBDONE %d\n", clock1 );

        for (int i = 0; i < jobCount; i++){
            pthread_mutex_lock(&lock1[0]);
            if(core_status1[0] == 0  && jobs[i].pState == READY  && clock1 > jobs[i].arrival_time){

                /**metto argomenti**/

                core_status1[0] = 1;

                thread_args[0].job = jobs[i];
                thread_args[0].clock = clock1;
                thread_args[0].core = 0;
                jobs[i].pState = RUNNING;
                
				printf("core%d,%llu,%d,RUNNING\n", thread_args[0].core,clock1,thread_args[0].job.id);
                //fprintf(stderr, "Numero %d VA verso il core %d (%d e numbOfInstr %d)\n", thread_args[0].job.id, thread_args[0].core, thread_args[0].job.pState, thread_args[0].job.numbOfInstr);
                //printf("Job %d ha pState %d\n", i ,jobs[i].pState);
                pthread_create (&thread[0], NULL, &corePreem, &thread_args[0]);

            }else if(core_status1[1] == 0  && jobs[i].pState == READY && clock2 > jobs[i].arrival_time){


                /**metto argomenti**/
                core_status1[1] = 1;
                thread_args[1].job = jobs[i];
                thread_args[1].clock = clock2;
                thread_args[1].core = 1;
                jobs[i].pState = RUNNING;

				printf("core%d,%llu,%d,RUNNING\n", thread_args[1].core,clock2,thread_args[1].job.id);
                //printf("Numero %d VA verso il core %d (pState %d e numbOfInstr %d)\n", thread_args[1].job.id, thread_args[1].core, thread_args[0].job.pState, thread_args[0].job.numbOfInstr);
                
                pthread_create (&thread[1], NULL, &corePreem, &thread_args[1]);
            }else{
                //printf("Finito qua\n");
                
                //printf("JOBDONE %d\n", jobs[5].pState );
            }
            pthread_mutex_unlock(&lock1[0]);




        }
        pthread_mutex_lock(&lock1[0]);
        if(core_status1[0] == 2){


            pthread_join (thread[0], NULL);
            core_status1[0] = 0;
            clock1 = thread_args[0].clock;
            jobs[thread_args[0].job.id] = thread_args[0].job;
            //(void*)&thread_args[0]
            //printf("CORE 0 STATUS: %d\n",core_status1[0]);

            //printf("CORE 0 ha lasciato il JOB %d ha pState %d e istrDone %d\n", thread_args[0].job.id, jobs[thread_args[0].job.id].pState, jobs[thread_args[0].job.id].instrDone);
            if(thread_args[0].job.pState == BLOCKED ){
                //printf("job %d messo in READY \n", thread_args[0].job.id);
                jobs[thread_args[0].job.id].pState = READY;
                 printf("core%d,%llu,%d,READY\n", thread_args[0].core, clock1, thread_args[0].job.id);
                //thread_args[0].job.pState = READY;

            }
            ///thread_args[0].job.id = NULL;



        }
         if(core_status1[1] == 2){


            pthread_join (thread[1], NULL);
            core_status1[1] = 0;
            clock2 = thread_args[1].clock;
            jobs[thread_args[1].job.id] = thread_args[1].job;
            //printf("CORE 1 STATUS: %d\n",core_status1[1]);

            //printf("CORE 1 ha lasciato il JOB %d ha pState %d e istrDone %d\n", jobs[thread_args[1].job.id].id, jobs[thread_args[1].job.id].pState, jobs[thread_args[1].job.id].instrDone);
             if(thread_args[1].job.pState == BLOCKED ){
                //printf("job %d messo in READY\n", thread_args[1].job.id);
                printf("core%d,%llu,%d,READY\n", thread_args[1].core, clock2, thread_args[1].job.id);
                //thread_args[1].job.pState = READY;
                jobs[thread_args[1].job.id].pState = READY;
            }


        }
        pthread_mutex_unlock(&lock1[0]);
		if(core_status1[0] == 0){
			clock1++;
		}
        if(core_status1[1] == 0){
			clock2++;
		}


    }
	
	fclose(fp);
}

void* corePreem (void* parameters){


    args* th_args = (args*) parameters;
    long long clock = (*th_args).clock;
    int coreNumb = (*th_args).core;
    int localQ = Q;
    int count = 0; //per poter riprendere un job se è stato interrotto
    //printf("job numero:%d cxon numero istruzioni %d, pState %d e lunghezza %d IStrDone %d\n", (*th_args).job.id, (*th_args).job.numbOfInstr,(*th_args).job.pState, (*th_args).job.totalLeght,(*th_args).job.instrDone);

    if((*th_args).job.instrDone != 0){
        count = (*th_args).job.instrDone;
    }
    //printf("COUNT 1 del Job %d: %d\n",(*th_args).job.id, count);
    for (; count < (*th_args).job.numbOfInstr; count++){

        if (localQ < 0){
             //printf("Nel core %d è finito il QUANTO all'istruzione %d del job %d\n",coreNumb, count, (*th_args).job.id);
              printf("core%d,%llu,%d,BLOCKED\n", coreNumb, clock , (*th_args).job.id);
            (*th_args).job.pState = BLOCKED;
            break;
        }
        if((*th_args).job.instr[count].type_flag == 0){ //se l'itruzione è non bloccante

            int lenght = (*th_args).job.instr[count].lenght;
            //fprintf(stderr,"core %d ha eseguito un istruzione n. %d del job: %d\n",coreNumb, count, (*th_args).job.id);
            (*th_args).job.instrDone++;
            for (int j=0; j<lenght; j++){
                clock++;
                localQ--;
            }

        }else if((*th_args).job.instr[count].type_flag == 1){

            clock++;
            int max = (*th_args).job.instr[count].io_max;
            (*th_args).job.instrDone = (count+1);
            /*espediente per fare da 1 a io_max*/
            int rnd = rand_lim1(max);

            /*se è bloccato metto lo stato BLOCKED e l'arrival_time lo setto al clock attuale
            aggiungendo il numero di cicli per cui deve restare bloccato (rnd) ANCHE SE  POTREBBE ESSERE SBAGLIATO
            ESSENDOCI DUE CICLI DI CLOCK DIVERSI*/
			printf("core%d,%llu,%d,BLOCKED\n", coreNumb, clock , (*th_args).job.id);
            //printf("core %d ha eseguito l' istruzione n. %d del job: %d ma è stato bloccato\n",coreNumb, count, (*th_args).job.id);

            (*th_args).job.pState = BLOCKED;

            //printf("Job %d ha pState %d\n", (*th_args).job.id, (*th_args).job.pState);
            (*th_args).job.arrival_time = (*th_args).clock + rnd;
            break;

        }else{
            fprintf(stderr, "Errore nella lettura del type_flag");
            (*th_args).job.pState = BLOCKED;
        }

    }

    if((*th_args).job.numbOfInstr == count){
        /*completa procedura con job concluso*/
        pthread_mutex_lock(&lock1[1]);
        jobDone1++;
        pthread_mutex_unlock(&lock1[1]);
        //fprintf(stderr,"CORE %d ha FINITO il job: %d\n",coreNumb, (*th_args).job.id);
		 printf("core%d,%llu,%d,EXIT\n", coreNumb, clock , (*th_args).job.id);
        (*th_args).job.pState = EXIT;

    }

    (*th_args).clock = clock;

    //printf("job numero:%d con numero istruzioni %d, pState %d e lunghezza %d IStrDone %d\n", (*th_args).job.id, (*th_args).job.numbOfInstr,(*th_args).job.pState, (*th_args).job.totalLeght,(*th_args).job.instrDone);

    /**Completa job con mutex e status del core**/
    pthread_mutex_lock(&lock1[0]);
    core_status1[coreNumb] = 2;
    pthread_mutex_unlock(&lock1[0]);

    return NULL;

}

int rand_lim1(int limit) {

    limit -=1;
    int divisor = RAND_MAX/(limit+1);
    int retval;

    do {
        retval = rand() / divisor;
    } while (retval > limit);
    retval++;
    return retval;
}

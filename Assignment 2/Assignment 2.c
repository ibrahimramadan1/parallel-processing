#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

int isSub(char s1[],char s2[])
{
    int counter=0;
    int M = strlen(s1);
    int N = strlen(s2);
    int i,j ;
    for (i= 0; i <= N - M; i++)
    {
        for (j = 0; j < M; j++)
        {
            if (s2[i + j] != s1[j])
            {
                break;
            }
        }

        if (j == M)
        {
            counter++;
        }
    }

    return counter;

}
int main(int argc, char * argv[])
{
    int my_rank;		/* rank of process	*/
    int p;			/* number of process	*/
    int source;		/* rank of sender	*/
    int dest;		/* rank of receiver	*/
    int tag = 0;    /* tag for messages	*/
    MPI_Status status;	/* return status for 	*/
    MPI_Init( &argc, &argv );
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    int i,j,k,temp;
    int avg,extra,taken,finalResult;
    char query[200];
    char query2[10][20];
    char query3[20];
    char b[6];
    char a[100];
    char line[500];
    char Foundline[50000];
    int *found;
    int sum;
    int start[100];
    int end[100];
    int rank,noq,nof;
    int istart,iend;
    avg=50/p;
    extra=50%p;
    double startTime,endTime;

    found=malloc(sizeof(int)*p);
    if (my_rank==0)
    {

        printf("Query: ");
        scanf("%[^\n]%*c",query);
        sum=0;

        temp=0;


        for(i=0; i<p; i++)
        {
            taken=(extra--<=0)? avg: avg+1;
            start[i]=temp;
            temp+=taken;
            end[i]=temp;
        }
        noq=0;
        strcpy(query2[0],"");
        j=0;
        for(i=0; i<100; i++)
        {
            if (query[i]==' ')
            {

                query2[noq][j]='\0';
                j=0;
                noq++;
                strcpy(query2[noq],"");
                continue;

            }
            query2[noq][j++]=query[i];

        }
        noq++;
        startTime = MPI_Wtime();

    }
    MPI_Bcast(query, 200, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(query2, 200, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(&(sum), 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&noq, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(&start, 1, MPI_INT, &istart, 1, MPI_INT, 0,MPI_COMM_WORLD);
    MPI_Scatter(&end, 1, MPI_INT, &iend, 1, MPI_INT, 0,MPI_COMM_WORLD);
    int counter=0;
    strcpy(Foundline,"");
    for (i=istart; i<iend; i++)
    {
        strcpy(b,"");
        strcpy(a,"dic/Aristo-Mini-Corpus P-");
        sprintf(b,"%d",i+1);
        strcat(b,".txt");
        strcat(a,b);

        FILE *read=fopen(a,"r");
        if(read == NULL)
        {
            perror("Unable to open file!");
            printf("%s \n",a);
            exit(1);
        }
        else
        {
            counter=0;
            nof=0;
            while(fgets(line, sizeof(line), read) != NULL)
            {
                nof=0;
                for (j=0; j<noq; j++)
                {
                    strcpy(query3,query2[j]);
                    if (isSub(query3,line)==0)
                    {
                        nof=0;
                        break;
                    }
                    nof+=isSub(query3,line);


                }
                if (nof>0)
                {
                    strcat(Foundline,line);
                    counter++;
                }
            }
            sum+=counter;
            fclose(read);


        }


    }
    if (my_rank!=0)
    {
        MPI_Send(Foundline, 50000, MPI_CHAR, 0,tag, MPI_COMM_WORLD);
    }
    MPI_Gather(&sum,1,MPI_INT,found,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Reduce(&sum, &finalResult, 1, MPI_INT, MPI_SUM, 0,MPI_COMM_WORLD);
    if (my_rank==0)
    {
        endTime= MPI_Wtime();
        strcpy(a,"sol/");
        strcat(a,query);
        strcat(a,".txt");
        FILE *write = fopen(a,"w");
        fprintf(write,"Query: ");
        fprintf(write,query);
        fprintf(write,"\n");
        fprintf(write,"\n");
        fprintf(write,"\n");
        fprintf(write,"%d lines contain ",finalResult);
        fprintf(write,query);
        fprintf(write,"\nsearchTime : %f sec \n",endTime-startTime);

        fprintf(write,"\n");
        fprintf(write,"\n");
        fprintf(write,"\n");
        fprintf(write,"Master found %d lines \n",found[0]);
        fprintf(write,"\n");
        fprintf(write,"\n");
        fprintf(write,"\n");

        fprintf(write,Foundline);

        fprintf(write,"\n");
        fprintf(write,"\n");
        fprintf(write,"\n");
        for (i=1; i<p; i++)
        {
            fprintf(write,"process %d found %d lines \n",i,found[i]);
            MPI_Recv(&Foundline, 50000, MPI_CHAR, i, tag, MPI_COMM_WORLD, &status );
            fprintf(write,"\n");
            fprintf(write,"\n");
            fprintf(write,"\n");
            fprintf(write,Foundline);
            fprintf(write,"\n");
            fprintf(write,"\n");
            fprintf(write,"\n");
        }
        fclose(write);
        free(found);

    }

    /* shutdown MPI */
    MPI_Finalize();
    return 0;
}

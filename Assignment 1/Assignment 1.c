#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
int **alloc_2d_int(int rows, int cols)
{
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    int i;
    for (i=0; i<rows; i++)
        array[i] = &(data[cols*i]);

    return array;
}
void free2D(int **mat)
{
    free(mat[0]);
    free(mat);

}
void printMat(int **mat,int row, int col)
{
    int i,j;
    for (i=0; i<row; i++)
    {
        for (j=0; j<col; j++)
        {
            printf("%d   ",mat[i][j]);
        }
        printf("\n");
    }

}
int main(int argc, char * argv[])
{
    int my_rank;		/* rank of process	*/
    int p;			/* number of process	*/
    int source;		/* rank of sender	*/
    int dest;		/* rank of reciever	*/
    int tag = 0;		/* tag for messages	*/
    MPI_Status status;	/* return status for 	*/
    MPI_Init( &argc, &argv );
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (my_rank==0)
    {
        int row1,col1,row2,col2,i,j,val;
        int **mat1;
        int **mat2;
        int **mat3;
        printf("Enter no.rows of 1st matrix: ");
        scanf("%d",&row1);
        printf("Enter no.cols of 1st matrix: ");
        scanf("%d",&col1);
        printf("Enter no.rows of 2nd matrix: ");
        scanf("%d",&row2);
        printf("Enter no.cols of 2nd matrix: ");
        scanf("%d",&col2);
        if (col1!=row2)
        {
            printf("these matrices cant be multiplied !!");
            /* shutdown MPI */
            MPI_Finalize();
            return 0;
        }
        else
        {
            mat1=alloc_2d_int(row1, col1);
            mat2=alloc_2d_int(row2, col2);
            mat3=alloc_2d_int(row1, col2);
            printf("Enter first matrix : ");
            for (i=0; i<row1; i=i+1)
            {
                for (j=0; j<col1; j=j+1)
                {
                    scanf("%d",&val);
                    mat1[i][j]=val;
                }
            }
            printf("Enter second matrix : ");
            for (i=0; i<row2; i=i+1)
            {
                for (j=0; j<col2; j=j+1)
                {
                    scanf("%d",&val);
                    mat2[i][j]=val;
                }
            }

            for (source=1; source<p; source++)
            {


                MPI_Send(&row1,1,MPI_INT,source,tag,MPI_COMM_WORLD);
                MPI_Send(&col1,1,MPI_INT,source,tag,MPI_COMM_WORLD);
                MPI_Send(&row2,1,MPI_INT,source,tag,MPI_COMM_WORLD);
                MPI_Send(&col2,1,MPI_INT,source,tag,MPI_COMM_WORLD);

                for (i=0; i<row1; i++)
                {
                    for(j=0; j<col1; j++)
                    {
                        MPI_Send(&mat1[i][j], 1, MPI_INT, source,tag, MPI_COMM_WORLD);
                    }
                }

                for (i=0; i<row2; i++)
                {
                    for(j=0; j<col2; j++)
                    {
                        MPI_Send(&mat2[i][j], 1, MPI_INT, source,tag, MPI_COMM_WORLD);
                    }
                }
            }
            for (i=0; i<row1; i++)
            {
                for (j=0; j<col2; j++)
                {
                    MPI_Recv(&(mat3[i][j]), 1, MPI_INT, i%(p-1)+1, tag, MPI_COMM_WORLD, &status );

                }
            }

            printf("Mat1*Mat2 = \n");
            printMat(mat3,row1,col2);
            free2D(mat1);
            free2D(mat2);
            free2D(mat3);


            /*
                        printMat(mat1,row1,col1);
                        printMat(mat2,row2,col2);

            */

        }

    }

    else
    {
        int row1,col1,row2,col2,i,j,k,sum;
        int **mat1;
        int **mat2;

        dest=0;
        MPI_Recv(&row1,1,MPI_INT,dest,tag, MPI_COMM_WORLD,&status);
        MPI_Recv(&col1,1,MPI_INT,dest,tag, MPI_COMM_WORLD,&status);
        MPI_Recv(&row2,1,MPI_INT,dest,tag, MPI_COMM_WORLD,&status);
        MPI_Recv(&col2,1,MPI_INT,dest,tag, MPI_COMM_WORLD,&status);
        mat1=alloc_2d_int(row1, col1);
        mat2=alloc_2d_int(row2, col2);

        for (i=0; i<row1; i++)
        {
            for (j=0; j<col1; j++)
            {
                MPI_Recv(&mat1[i][j],1,MPI_INT,dest,tag, MPI_COMM_WORLD,&status);
            }
        }

        for (i=0; i<row2; i++)
        {
            for (j=0; j<col2; j++)
            {
                MPI_Recv(&mat2[i][j],1,MPI_INT,dest,tag, MPI_COMM_WORLD,&status);
            }
        }

        sum=0;
        for (i=0; i<row1; i++)
        {
            if (my_rank != (i%(p-1)+1))
                continue;
            for (j=0; j<col2; j++)
            {
                for(k=0; k<col1; k++)
                {
                    sum += mat1[i][k]*mat2[k][j];
                }
                MPI_Send(&(sum), 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
                sum = 0;
            }
        }

        free2D(mat1);
        free2D(mat2);
    }
    /* shutdown MPI */
    MPI_Finalize();
    return 0;
}

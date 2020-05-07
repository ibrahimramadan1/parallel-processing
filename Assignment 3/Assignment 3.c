#include <omp.h>
#include <stdio.h>
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
int main()
{
    int row1,col1,row2,col2,val;
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

    if ((row1!=row2)||(col1!=col2)||(row1<=0||row2<=0||col1<=0||col2<=0))
    {
        printf("Invalid Dimensions \n");
        return 0;
    }
    else
    {

            mat1=alloc_2d_int(row1, col1);
            mat2=alloc_2d_int(row2, col2);
            mat3=alloc_2d_int(row1, col2);
            int i,j;
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
            #pragma omp parallel shared(mat1,mat2,mat3,row1,col1) private(i,j)
            {
                int my_rank=omp_get_thread_num();
                int p= omp_get_num_threads();
                for (i=0;i<row1;i++)
                {
                    if (my_rank != (i%p))
                        continue;
                    for (j=0;j<col1;j++)
                    {
                        mat3[i][j]=mat1[i][j]+mat2[i][j];
                    }
                }
            }
            printf("Mat1+Mat2 = \n");
            printMat(mat3,row1,col2);
            free2D(mat1);
        free2D(mat2);
        free2D(mat3);
    }
}

//============================================================================
// Name        : ACO.cpp
// Author      : Papikas Ioannis
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 200

int distances[N][N];
double H[N][N];
double T[N][N];

#define MIN_T 0.01
#define MAX_T 200

#define MAX_NO_OPT 300

#define bestK 1
#define bestSoFar 2

#define K 5

int mode=bestSoFar;

#define M 20
#define	p 0.95

int paths[M][N];
double scores[M];

int best_path[N];
double best_score=-1;


void readfile(char *filename)
{
	int i,j;
	FILE *f;
	f=fopen(filename,"r");

	for(i=0;i<N;i++) {
		for(j=i+1;j<N;j++)
		{
			fscanf(f,"%d",&distances[i][j]);
			distances[j][i]=distances[i][j];
		}
		distances[i][i]=0;
	}
}

void print_distances(){
	int i,j;
	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
			printf("%d ",distances[i][j]);
		printf("\n");
	}
}

void initialize()
{
	int i,j;
	for(i=0;i<N;i++)
		for(j=0;j<N;j++)
		{
			if (i!=j)
				H[i][j]=1000.0/distances[i][j];
			else
				H[i][j]=0;

			T[i][j]=H[i][j];
		}

//	for(int i=0;i<N;i++)
//	{
//		for(int j=0;j<N;j++)
//			printf("%f ",T[i][j]);
//		printf("\n");
//	}

	for(i=0;i<N;i++)
		for(j=0;j<N;j++)
		if (T[i][j]>MAX_T)
			T[i][j]=MAX_T;
		else if (T[i][j]<MIN_T)
			T[i][j]=MIN_T;
}


double nextrand()
{
	return (double)rand() / (double) RAND_MAX;
}

void create_path(int i)
{
	int j;
	double scores[N];
	paths[i][0]=(int) (nextrand()*N);
	if (paths[i][0]>=N)
		paths[i][0]=N-1;

	for (j=1;j<N;j++)
	{
		double sum=0;
		for (int k=0;k<N;k++)
		{
			bool found=false;
			for(int l=0;l<j;l++)
				if (paths[i][l]==k)
					found=true;

			if (found)
				scores[k]=0;
			else
				scores[k]=H[paths[i][j-1]][k] * T[paths[i][j-1]][k];

			sum+=scores[k];
		}

		double r=nextrand()*sum;

		double temp=scores[0];
		int k=0;
		while (temp<r){
			k++;
			temp+=scores[k];
		};

		paths[i][j]=k;
	}

	for (int m=0;m<N;m++)
		if (paths[i][m]<0 || paths[i][m]>=N)
			printf("****");
}

void print_path(int i)
{
	for(int j=0;j<N;j++)
		printf("%d ",paths[i][j]);
	printf("\n");
}

void evaluate_path(int i)
{
	int j;
	double score=0;
	for(j=0;j<N-1;j++)
		score+=distances[paths[i][j]][paths[i][j+1]];

	score+=distances[paths[i][N-1]][paths[i][0]];

	scores[i]=1000*N/score;

//	if (mode==bestK)
//		scores[i] = scores[i]/ K;

	if (scores[i]>best_score)
	{
		best_score=scores[i];
		for(j=0;j<N;j++)
			best_path[j]=paths[i][j];

		printf("Best path found so far: %f   -     %f\n",1000.0*N/best_score, best_score);
		print_path(i);
	}
}

void create_generation()
{
	for(int i=0;i<M;i++)
	{
		create_path(i);
//		print_path(i);
		evaluate_path(i);
//		printf("%f  -   %f\n",1000.0/scores[i], scores[i]);
	}
}

void update_T()
{
	int i,j;
	int ordering[M];

	for(i=0;i<N;i++)
		for(j=0;j<N;j++)
			T[i][j]*=p;

	if (mode==bestK)
	{
		for(j=0;j<M;j++)
			ordering[j]=j;

		// bubble sort
		for(int k=0;k<M-1;k++)
			for(int l=M-1;l>k;l--)
				if (scores[ordering[l]]>scores[ordering[l-1]])
				{
					int temp=ordering[l-1];
					ordering[l-1]=ordering[l];
					ordering[l]=temp;
				}

		for(int i=0;i<K;i++)
		{
			for(j=0;j<N-1;j++)
			{
				T[paths[ordering[i]][j]][paths[ordering[i]][j+1]]+=(1-p)*scores[ordering[i]];
				T[paths[ordering[i]][j+1]][paths[ordering[i]][j]]+=(1-p)*scores[ordering[i]];
			}

			T[paths[ordering[i]][N-1]][paths[ordering[i]][0]]+=(1-p)*scores[ordering[i]];
			T[paths[ordering[i]][0]][paths[ordering[i]][N-1]]+=(1-p)*scores[ordering[i]];
		}
	}
	else if (mode==bestSoFar)
	{
		for(j=0;j<N-1;j++)
		{
			T[best_path[j]][best_path[j+1]] += (1-p)*best_score;
			T[best_path[j+1]][best_path[j]] += (1-p)*best_score;
		}

		T[best_path[N-1]][best_path[0]] += (1-p)*best_score;
		T[best_path[0]][best_path[N-1]] += (1-p)*best_score;
	}

	for(i=0;i<N;i++)
		for(j=0;j<N;j++)
		if (T[i][j]>MAX_T)
			T[i][j]=MAX_T;
		else if (T[i][j]<MIN_T)
			T[i][j]=MIN_T;
}

int main(){
	srand( (unsigned)time( NULL ) );
	readfile("cities200.txt");
//	print_distances();
	initialize();

	int total_count=0;
	int count_no_opt=0;

	double temp_best_score=best_score;

	while (count_no_opt < MAX_NO_OPT)
	{
		total_count++;
//		printf("%d (%d)\n",total_count, count_no_opt);

		create_generation();
		update_T();

		if (best_score>temp_best_score)
		{
			count_no_opt=0;
			temp_best_score=best_score;
		}
		else
		{
			count_no_opt++;
			printf("No Opt: %d\n", count_no_opt);
		}
	}

//	for(int i=0;i<N;i++)
//	{
//		for(int j=0;j<N;j++)
//			printf("%f ",T[i][j]);
//		printf("\n");
//	}

	return 1;
}

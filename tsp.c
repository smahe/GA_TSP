#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<cuda.h>
#include "cuda_runtime_api.h"

#define citySize 100
#define datasize 100
#define size 100
float data[100][2];

struct graph{
	float adj[citySize][citySize];
};
struct graph *g;
	
void readData(){
	FILE *fptr;
	int i = 0 , j = 0;
	char c;
	fptr = fopen("euclideancoordinatesdataset.txt","r");
	for(i = 0;i<citySize;i++){
		for(j = 0;j<2;j++){ 
			fscanf(fptr,"%f %c",&data[i][j],&c);
		}
	}
	fclose(fptr);
}


float euclideanDistance(float x1,float y1,float x2,float y2){
	return abs(sqrt(pow(x2-x1,2)+pow(y2-y1,2)));
}

bool checkPathValidity(int val,int i,int j,int population[][citySize]){
	bool flag = true;
	int k = 0;
	for(;k<j; k++){
		if(val == population[i][k]){
			flag = false;
			break;
		}
	}
	if(flag){
		return true;
	}else {
		return false;
	}
}

unsigned int rand_interval(unsigned int min, unsigned int max)
{
    int r;
    const unsigned int range = 1 + max - min;
    const unsigned int buckets = RAND_MAX / range;
    const unsigned int limit = buckets * range;
	do
    {
     	r = rand();
    } while (r >= limit);

    return min + (r / buckets);
}

void initializePopulation(int (*lpopulation)[citySize],int maxPopSize){
	int i = 0,j = 0,max = maxPopSize,min = 1;
	unsigned int genValue = 0;
	for(i = 0; i<maxPopSize; i++){
		j = 0;
		lpopulation[i][j] = 1;
		for(j=1;j<citySize;j++){
			while(1){					
				genValue = rand_interval(1,citySize);
				bool resultant = checkPathValidity(genValue,i,j,lpopulation);
				if(resultant){
					lpopulation[i][j] = genValue;
					break;
				}
			}
		}
	}
}

int fitness(int population[][citySize],int i){
	 int fitnessValue = 0,origin=0,end=0,k=0;
	for(k = 0;k<citySize-2;k++){
		origin = population[i][k] - 1;
		origin = abs(origin);
		end = population[i][k+1] - 1;
		end = abs(end);
		fitnessValue+=g->adj[origin][end];
	}
	return fitnessValue;
}


void crossover(int i,int j,int population[][citySize],int highest,int secondhighest){
	bool local[citySize];
	int child[2][citySize],index = 0,k = 0,nextIndex = 1,val = 0,currIndex = 1;
	int p1 = 0,p2 = 0,c1 = 0,c2 = 0;
	while(k<citySize){
		local[k] = false;
		k++;
	}	
	k = 0;
	while(!local[currIndex]){	
		local[currIndex] = true;
		child[0][currIndex] = population[highest][currIndex];
		val = population[secondhighest][currIndex];
		for(index = 1;index<citySize;index++){
			if(population[highest][index] == val && index!=currIndex){
				nextIndex = index;
				break;
			}
		}
		currIndex = nextIndex;
	}

	for(k = 0;k<citySize; k++){
		if(!local[k]){
			child[0][k] = population[secondhighest][k];
			child[1][k] = population[highest][k];
		}else{
			child[1][k] = population[secondhighest][k];
		}
	}
		for(k = 0;k<citySize; k++){
			population[i][k] = child[0][k];
		}
		for(k = 0;k<citySize; k++){
			population[j][k] = child[1][k];
		}
	
}

void mutation(int i,int population[][citySize]){
	unsigned int startIndex = 0,temp = 0,endIndex = 0;
	int k = 0, j = 0;
	while(1){
		startIndex = rand_interval(1,citySize-1);
		endIndex   = rand_interval(1,citySize-1);
		startIndex -=1;
		endIndex-=1;
		if(startIndex == endIndex){
			continue;
		}else{
			break;
		}
	}
	j = endIndex;
	k = startIndex;
	while(j >= k){
		temp = population[i][k];
		population[i][k] = population[i][j];
		population[i][j] = temp;
		j--;
		k++;
	}
}

void print(int pop[][citySize],int i){
	int j = 0;
		for(j = 0;j<citySize;j++){
			printf("%d\t",pop[i][j]);
		}
}

void print(int pop[][citySize]){
	int j = 0,i=0;
	for(i=0;i<size;i++){	
		for(j = 0;j<citySize;j++){
			printf("%d\t",pop[i][j]);
		}
	}
}

void calculate(){
	int i = 0,j = 0,k = 0;
	float x1 = 0,y1 = 0;
	for(k = 0;k<citySize;k++){
		x1 = data[k][0];
		y1 = data[k][1];
		for(i = 0;i<citySize;i++){
			g->adj[k][i] = euclideanDistance(x1,y1,data[i][0],data[i][1]);		
		}
	}
}

void sort(float *score){
	int i = 1 , j = 0,k=0;
	float key = 0;
	for(j = 1;j<size;j++){
		key = score[j];
		i = j - 1;
		while(i>=0 && score[i] > key){
			score[i+1]= score[i];
			i--;
		}
		score[i+1] = key; 
	}
}

void printz(float *a){
	int j = 0;
	for(j = 0;j<size;j++){
		printf(" %f \t",a[j]);
	}
}

void printstruct(){
	int i = 0,j = 0;
	for(i = 0;i<citySize;i++){
		for(j = 0;j<citySize;j++){
			printf("%f ",g->adj[i][j]);
		}
		printf("\n");
	}
}

int binarySearch(float *a,int val){
	int high = size-1,low = 0,mid = 0;
	while(low<=high){
		mid = (int)(high+low)/2;
		if((int)a[mid] == val){
			return mid;
		}else if((int)a[mid]<val){
			low = mid+1;
		}else{
			high = mid-1;
		}
	}
}

int search(float *a,int val){
	int j = 0;
	for(j = 0;j<size;j++){
		if((int)a[j] == val){
			return j;
		}
	}
}

int main(){
	float *score;
	FILE *f = fopen("output.txt", "a");
	float *sortedscore;
	int population[size][citySize];
	int ch = 0 , i = 0,j=0;
	int wp1Index = 0,wp2Index=0,sp1Index,sp2Index;
	score = (float*)malloc(sizeof(float)*size);
	sortedscore = (float*)malloc(sizeof(float)*size);
	readData();
	g = (struct graph*)malloc(sizeof(struct graph));
	calculate();
	
	initializePopulation(population,size);
	for(i = 0;i<size;i++){
		score[i] = fitness(population,i);
	}
	memcpy(sortedscore,score,sizeof(float)*size);
	sort(sortedscore);
	for(j = 0;j<500;j++){
		sp1Index = search(score,sortedscore[0]);
		sp2Index = search(score,sortedscore[1]);
		wp1Index = search(score,sortedscore[size-1]);
		wp2Index = search(score,sortedscore[size-2]);
		crossover(wp1Index,wp2Index,population,sp1Index,sp2Index);
		if (f == NULL)
		{
			printf("Error opening file!\n");
			exit(1);
		}
	
		for(i = 0;i<size;i++){
			score[i] = fitness(population,i);
			sortedscore[i] = score[i];
		}
		sort(sortedscore);		
		mutation(sp1Index,population);
		sort(sortedscore);		
		printf("best score = %d",(int)sortedscore[0]);
		fprintf(f, "%d\n",(int)sortedscore[0]);
	}
	fclose(f);
	sp1Index = binarySearch(score,sortedscore[0]);
	printf("best score = %d \n",(int)sortedscore[0]);
	for(i = 0;i<citySize;i++){
		printf("%d\t",population[sp1Index][i]);
	}
	scanf("%d",&sp1Index);
	free(score);
	free(sortedscore);
	free(g);
}

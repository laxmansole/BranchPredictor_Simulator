#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>


#define Taken 1
#define NotTaken 0
enum
{
	bimodal,
	gshare,
	hybrid
};
	FILE *traceFile;
	char *predictor, *tracefile;
	int simType,K,M1,N,M2;
	char *ptr_taken,*ptr_add;
	char *bmTable,*gsTable,*chooserTable;
	char line[128];
	int i, num,pcAddr,actual,predict;
	unsigned long long n,mp;
void updateIndex(int actual, char *bmTable, int index, int predict)
{
	if(actual == Taken)
	{	
		if(bmTable[index]<3)
		bmTable[index]++;
	}
	else
	{
		if(bmTable[index]>0)
		bmTable[index]--;
	}
	//printf(" \t value= %d\n\n",bmTable[index]);
	if(predict != actual)
		mp++;
}
int doPrediction(char *bmTable,int index)
{
	return (bmTable[index]>=2)?Taken:NotTaken;
}
int doHybrid(int K, int pcAddr)
{
	return (pcAddr & (int)(pow(2,K)-1));
}
int doGshare(int M1, int pcAddr,int N)
{
	int m,x,y,z,index;
	x = M1-N;
	m = (pcAddr & (int)(pow(2,M1)-1));		
	y = (num & (int)(pow(2,N)-1));
	z = (m & (int)(pow(2,x)-1));	
	index = ((((m>>x)^y) << x)|z);
	//updateIndex(actual,bmTable,index,predict);
	num = (actual << (N-1))|(num >>1);
	return index;
}

int doBimodal(int M2, int pcAddr)
{
	return (pcAddr & (int)(pow(2,M2)-1));
	
	//printf("n= %llu\t index =%d\t value=%d\t predict =%d\t actual = %d",n-1,index,bmTable[index],predict,actual);
	//updateIndex(actual,bmTable,index);
}	
int main(int argc, char *argv[])
{
	int index1,predict1,index2,predict2,index3;
	if((argc == 4) && (!strcmp(argv[1],"bimodal")))
	{
		simType = bimodal;
		M2 = atoi(argv[2]);
		tracefile = argv[3];
		printf("COMMAND\n%s %s %d %s\n",argv[0],argv[1],M2,tracefile);
		bmTable =malloc((int)pow(2,M2)*sizeof(char));
		for(i=0;i<(int)pow(2,M2);i++)
		{
			bmTable[i] = 2; 
		}	
	}
	else if((argc == 5) && (!strcmp(argv[1],"gshare")))
	{
		simType = gshare;
		M1 = atoi(argv[2]);
		N  = atoi(argv[3]);
		tracefile = argv[4];
		printf("COMMAND\n%s %s %d %d %s\n",argv[0],argv[1],M1,N,tracefile);
		gsTable =malloc((int)pow(2,M1)*sizeof(char));
		for(i=0;i<(int)pow(2,M1);i++)
		{
			gsTable[i] = 2; 
		}
		num = 0;
	}
	else if( (argc == 7) && (!strcmp(argv[1],"hybrid")))
	{
		simType = hybrid;
		K  = atoi(argv[2]);
		M1 = atoi(argv[3]);
		N  = atoi(argv[4]);
		M2 = atoi(argv[5]);
		tracefile = argv[6];
		printf("COMMAND\n%s %s %d %d %d %d %s\n",argv[0],argv[1],K,M1,N,M2,tracefile);

		bmTable =malloc((int)pow(2,M2)*sizeof(char));
		for(i=0;i<(int)pow(2,M2);i++)
		{
			bmTable[i] = 2; 
		}	
		
		gsTable =malloc((int)pow(2,M1)*sizeof(char));
		for(i=0;i<(int)pow(2,M1);i++)
		{
			gsTable[i] = 2; 
		}

		chooserTable =malloc((int)pow(2,K)*sizeof(char));		
		for(i=0;i<(int)pow(2,K);i++)
		{
			chooserTable[i] = 1; 
		}
		num = 0;
	}
	else
	{
		printf("Invalid parameters\n");
		return 0;
	}
	predictor = argv[1]; 
	traceFile = fopen(tracefile,"r");
	if(traceFile==NULL)
	{
		printf("Unable to open file %s\n",tracefile);
		return 1;
	}
	/*for(i=0;i<(int)pow(2,M2);i++)
	{
		printf("%d\t%d\n",i,bmTable[i]);
	}*/
	while(fgets(line,sizeof(line),traceFile))
	{
		ptr_add = strtok(line," ");
		if(ptr_add!=NULL)
		{
			sscanf(ptr_add,"%x",&pcAddr);
			pcAddr = pcAddr >> 2 ;
			//printf("%s\t%d",ptr_add,pcAddr);
		}
		ptr_taken = strtok(NULL,"\n");
		if(ptr_taken!=NULL)
		{
		//	printf("--%s--",ptr_taken);
		}
		if(!strcmp(ptr_taken,"n"))
			actual = NotTaken;
		else 
			actual = Taken;
		n++;
		switch(simType)
		{
			case bimodal:
				index1 = doBimodal(M2,pcAddr);
				predict1 = doPrediction(bmTable,index1);
				updateIndex(actual,bmTable,index1,predict1);
			break;
			case gshare:
				index2 = doGshare(M1, pcAddr,N);
				predict2 = doPrediction(gsTable,index2);
				updateIndex(actual,gsTable,index2,predict2);
			break;
			case hybrid:
				index1 = doBimodal(M2,pcAddr);
				predict1 = doPrediction(bmTable,index1);
				index2 = doGshare(M1, pcAddr,N);
				predict2 = doPrediction(gsTable,index2);
				index3 = doHybrid(K,pcAddr);
				//printf("m here");
				if(chooserTable[index3]>=2)
				{
					updateIndex(actual,gsTable,index2,predict2);
					
				}
				else
				{
					updateIndex(actual,bmTable,index1,predict1);
				}
				//if((predict1!=actual)&&(predict2!=actual))||((predict1==actual)&&(predict2==actual))
				if((predict2 == actual)&&(predict1!=actual))
				{
					if(chooserTable[index3]<3)
						chooserTable[index3]++;
				}
				else if((predict2 != actual)&&(predict1==actual))
				{
					if(chooserTable[index3]>0)
						chooserTable[index3]--;
				}
			break;
		}
	}
	printf("OUTPUT\n");
	printf("number of predictions :%llu\n",n);
	printf("number of mispredictions :%llu\n",mp);
	printf("misprediction rate :%.2f%%\n",(double)100*mp/n);
	
	switch(simType)
	{
		case bimodal:
			printf("FINAL BIMODAL CONTENTS\n");
			for(i=0;i<(int)pow(2,M2);i++)
			{
				printf("  %d\t\t%d\n",i,bmTable[i]);
			}
		break;
		case gshare:
			printf("FINAL GSHARE CONTENTS\n");
			for(i=0;i<(int)pow(2,M1);i++)
			{
				printf("  %d\t\t%d\n",i,gsTable[i]);
			}
		break;
		case hybrid:
			printf("FINAL CHOOSER CONTENTS\n");
			for(i=0;i<(int)pow(2,K);i++)
			{
				printf("  %d\t\t%d\n",i,chooserTable[i]);
			}
			printf("FINAL GSHARE CONTENTS\n");
			for(i=0;i<(int)pow(2,M1);i++)
			{
				printf("  %d\t\t%d\n",i,gsTable[i]);
			}
			printf("FINAL BIMODAL CONTENTS\n");
			for(i=0;i<(int)pow(2,M2);i++)
			{
				printf("  %d\t\t%d\n",i,bmTable[i]);
			}
		break;
	}
	
		fclose(traceFile);
	return 0;
	
}

#define _GNU_SOURCE
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<getopt.h>
#include<stdio.h>

typedef struct {
	char** map;
	int columns;
	int rows;
	FILE * i;
	FILE * o;
	int p;
	int d;
	int s;
}Maze;


int readline(Maze *maze, size_t size){
    char *buf=NULL;
    int count = 0;
    getline(&buf,&size,maze->i);
    if(!feof(maze->i)){
	//if((buf[0]!=48)||(buf[0]!=49)||buf[0]!=' '){
	  //  return 0;
	//}
	//printf("%d\n",maze->rows);
        maze->map = realloc(maze->map,(maze->rows+1)*8); 
        maze->map[maze->rows]=malloc(maze->columns);
        for(int i = 0;i<(signed)size;i++){
	    if(buf[i]==48||buf[i]==49){
	        maze->map[maze->rows][count]=buf[i];
	        count++;
	    }
        }
	return 1;
    }
    return 0;
}


void createMap(Maze *maze){
    FILE *fp = maze->i;
    char* buf=NULL;
    size_t len=0;
    int a=0;
    int count = 0;
    getline(&buf,&len,fp);
    for(int i=0; i<(signed)len; i++){
        if(buf[i]==48||buf[i]==49){
	    count++;
	}
    }
    maze->columns=count;
    maze->map=malloc(8);
    maze->map[0]=malloc(maze->columns);
    //maze->map[0][0]='1';
    //printf("1\n");
    for(int i=0;i<(signed)len; i++){
        if((buf[i]==48)||(buf[i]==49)){
	    maze->map[0][a]=buf[i];
	    a++;
	}
    }
    //maze->map[a]='\0';
    maze->rows = 1;
    while(readline(maze,maze->columns)==1){
	maze->rows++;
    }
}


void printMap(Maze *maze){
    //printf("%d",maze->rows);
    for(int i=0;i<maze->rows;i++){
	for(int j=0; j<maze->columns;j++){
	    printf("%c ",maze->map[i][j]);
	}
	printf("\n");
    }
}


int main(int argc, char** argv){
    //printf("Before getopt:\n");
    //for (int i = 0;
    Maze *maze = malloc(sizeof(Maze));
    int opt;
    FILE *i=stdin;
    FILE *o=stdout;
    while((opt=getopt(argc, argv, "hdspi:o:"))!=-1){
        switch(opt){
	    case 'i':
		printf("Input file: %s\n",optarg);
		i=fopen(optarg,"r");
		break;
	    case 'o':
		printf("Output file: %s\n",optarg);
		o=fopen(optarg,"w");
		break;
	    case 'h':
		printf("Print this helpful message to stdout and exit\n");
		exit(0);
		break;
	    case 'd':
		printf("Pretty print the maze\n");
		maze->d=1;
		break;
	    case 's':
		printf("Print shortest solution steps\n");
		maze->s=1;
		break;
	    case 'p':
		printf("print the optimal path\n");
		maze->p=1;
		break;
	    default:
		printf("unrecognized command: '%c'.\n",opt);
	}
    }
    maze->i=i;
    maze->o=o;
    createMap(maze);
    printMap(maze);
    return 1;
}

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
	int solvable;
}Maze;

//-s command
void checkSolvable(Maze *maze,int crumbs[][maze->columns],int row,int col){
    //uses depth first search to check if the maze is solvable.
    crumbs[row][col]=1;
    if((row+1==maze->rows)&&(col+1==maze->columns)){
        //printf("solvable\n");
	maze->solvable=1;
    }
    if(row+1!=maze->rows){
	if(maze->map[row+1][col]=='0'&&crumbs[row+1][col]==0){
	    //printf("(%d,%d) down\n",row+1,col);
	    checkSolvable(maze,crumbs,row+1,col);
	}
    }
    if(col+1!=maze->columns){
	if(maze->map[row][col+1]=='0'&&crumbs[row][col+1]==0){
	    //printf("(%d,%d) right\n",row,col+1);
	    checkSolvable(maze,crumbs,row,col+1);
	}
    }
    if(row-1!=-1){
	if(maze->map[row-1][col]=='0'&&crumbs[row-1][col]==0){
	    //printf("(%d,%d) up\n",row-1,col);
	    checkSolvable(maze,crumbs,row-1,col);
	}
    }
    if(col-1!=-1){
	if(maze->map[row][col-1]=='0'&&crumbs[row][col-1]==0){
	    //printf("(%d,%d) left\n",row,col-1);
	    checkSolvable(maze,crumbs,row,col-1);
	}
    }
}


int readline(Maze *maze, size_t size){
    char *buf=NULL;
    int count = 0;
    getline(&buf,&size,maze->i);
    if(!feof(maze->i)){
        maze->map = realloc(maze->map,(maze->rows+1)*8); 
        maze->map[maze->rows]=malloc(maze->columns);
        for(int i = 0;i<(signed)size;i++){
	    if(buf[i]==48||buf[i]==49){
	        maze->map[maze->rows][count]=buf[i];
	        count++;
	    }
        }
	if(buf!=NULL){
            free(buf);
	}
	return 1;
    }
    if(buf!=NULL){
	free(buf);
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
    for(int i=0;i<(signed)len; i++){
	if(buf!=NULL){
            if((buf[i]==48)||(buf[i]==49)){
	        maze->map[0][a]=buf[i];
	        a++;
	    }
        }   
    }
    if(buf!=NULL){
	free(buf);
    }
    maze->rows = 1;
    while(readline(maze,maze->columns)==1){
	maze->rows++;
    }
}


void printMap(Maze *maze){
    fprintf(maze->o,"|");
    for(int i=0;i<maze->columns*2+1;i++){
        fprintf(maze->o,"-");
    }
    fprintf(maze->o,"|\n");
    for(int i=0;i<maze->rows;i++){
	if(i!=0){
	    fprintf(maze->o,"| ");
	}
	else{
	    fprintf(maze->o,"  ");
	}
	for(int j=0; j<maze->columns;j++){
	    if(maze->map[i][j]=='0'){
		fprintf(maze->o,". ");
	    }
	    else{
	        fprintf(maze->o,"# ");
	    }
	}
	if(i!=maze->rows-1){
	    fprintf(maze->o,"|\n");
	}
	else{
	    fprintf(maze->o,"\n");
	}
    }
    fprintf(maze->o,"|");
    for(int i=0;i<maze->columns*2+1;i++){
        fprintf(maze->o,"-");
    }
    fprintf(maze->o,"|\n");
}


int main(int argc, char** argv){
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
    if(maze->d==1){
        printMap(maze);
    }
    int crumbs[maze->rows][maze->columns]; 
	   // malloc(maze->rows*sizeof(int*));
    for(int i=0; i<maze->rows; i++){
        //crumbs[i]=malloc(maze->columns);
	for(int j=0; j<maze->columns; j++){
	    crumbs[i][j]=0;
	}
    }
    maze->solvable=0;
    checkSolvable(maze,crumbs,0,0);
    if(maze->solvable==1){
	printf("solvable\n");
    }
    else{
	printf("not solvable\n");
    }
    return 1;
}

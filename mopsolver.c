#define _GNU_SOURCE
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<getopt.h>
#include<stdio.h>
//@author Matthew Carey


//this structure is to keep track of coordinates
typedef struct {
    int row;
    int col;
}Cell;

//this stack is used for breadth first search to find the fastest route
typedef struct {
    Cell* data;
    Cell* parents;
    int size;
    int maxsize;//maxsize is used for the free function
}Stack;

//the maze is used to keep track of the maze, dimentios, files and command bools
typedef struct {
    char** map;//the maze
    int columns;
    int rows;
    FILE * i;//input file
    FILE * o;//output file
    int p;
    int d;
    int s;
    int solvable;//true of false whether its solvable
    int moves;//fastest solution number of moves
}Maze;

//this function takes a maze struct and prints the map in the pretty print format and can also print the solved mazed
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
	    fprintf(maze->o,"  ");//for the first row doesn't print barrier
	}
	for(int j=0; j<maze->columns;j++){
	    if(maze->map[i][j]=='0'){
		fprintf(maze->o,". ");//. if passable
	    }
	    else if(maze->map[i][j]=='1'){
	        fprintf(maze->o,"# ");//# if blocked
	    }
	    else{
		fprintf(maze->o,"+ ");//+ if part of the solution path
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

//this function takes a row and col and creates one cell function
Cell createCell(int row, int col){
    Cell cell;
    cell.row=row;
    cell.col=col;
    return cell;
}

//this function creates a stack function
Stack* createStack(){
    Stack *stack = malloc(sizeof(Stack));
    stack->size = 0;
    return stack;
}

//this function takes a stack and frees the memory
void freeStack(Stack* stack){
    free(stack->data);
    free(stack->parents);
    free(stack);
}

//takes a stack and returns 1 if stack is empty 0 if not empty
int isEmpty(Stack *stack){
    if(stack->size==0){
	return 1;
    }
    return 0;
}

//takes a stack and 2 cells, the first cell is the new current cell and the other is the parent to that cell, the first cell is added to stack->data and the parent is added to stack->parents
void enqueue(Stack *stack, Cell item, Cell parent){
    if(isEmpty(stack)){//if stack is empty we malloc instead of realloc
	stack->size++;
	stack->maxsize++;
	stack->data = malloc(sizeof(Cell));
	stack->parents = malloc(sizeof(Cell));
	stack->data[0]=item;
	stack->parents[0] = parent;
	return;
    }
    stack->size++;
    stack->data = realloc(stack->data,stack->size*sizeof(Cell));
    stack->parents = realloc(stack->parents,stack->size*sizeof(Cell));
    stack->data[stack->size-1]=item;
    stack->parents[stack->size-1]=parent;
}

//takes a stack and returns the top node of the parent array
Cell getParent(Stack *stack){
    return stack->parents[stack->size-1];
}

//rakes a stack and returns the top node of the data array
Cell gettop(Stack *stack){
    return stack->data[stack->size-1];
}

//this function takes a maze and uses breadth first search to create a stack and check if theres a solution. Then if there is a solution it interpretes the stack to find the number of moves and changes the locations the solution goes through to 3. If the command p was used it reprints the maze and prints +'s where the 3's are. 
void bestSolution(Maze *maze){ 
    int **visited = malloc(8*maze->rows+1);
    for(int i=0; i<maze->rows; i++){
	visited[i]=malloc(maze->columns*4);
	for(int y=0; y<maze->columns;y++){
	    visited[i][y]=0;
	    //printf("%d\n",visited[i][y]);
	}
    }
    //int visited[maze->rows][maze->columns];//array to show which locations have been visited
    //for(int i=0; i<maze->rows; i++){//initialises visited array to 0's
//	for(int j=0; j<maze->columns; j++){
//	    visited[i][j]=0;
//	}
//    }
    visited[0][0] = 1;//sets initial lcation to 1;
    Stack *stack = createStack();//creates stack
    enqueue(stack,createCell(0,0),createCell(-1,-1));//enques start location with parent (-1,-1)
    int i=0;
    int row;
    int col;
    while(i!=stack->size){//loops until all items of stack have been processed
	row = stack->data[i].row;
	col = stack->data[i].col;
        if(row+1<maze->rows){
	    if(maze->map[row+1][col]=='0'&&visited[row+1][col]==0){//if passable and not visited it enqueues 
	        enqueue(stack,createCell(row+1,col),createCell(row,col));
		visited[row+1][col] = 1;
		if((row+2==maze->rows)&&(col+1==maze->columns)){//breaks if its at the accepting node
		    maze->solvable=1; //location becomes visited
		    break;
		}
	    }
        }
        if(col+1<maze->columns){
	    if(maze->map[row][col+1]=='0'&&visited[row][col+1]==0){//if passable and not visited it enqueueus
	        enqueue(stack,createCell(row,col+1),createCell(row,col));
		visited[row][col+1] = 1; //location becomes visited
		if((row+1==maze->rows)&&(col+2==maze->columns)){//breaks if its at the accepting node
		    maze->solvable=1; 
		    break;
		}
	    }
        }
        if(row-1>-1){
	    if(maze->map[row-1][col]=='0'&&visited[row-1][col]==0){//if passable and not visited it enqueues
	        enqueue(stack,createCell(row-1,col),createCell(row,col));
		visited[row-1][col] = 1; //location becomes visited
	    }
        }
        if(col-1>-1){
	    if(maze->map[row][col-1]=='0'&&visited[row][col-1]==0){//if passable and not visited it enqueues
	        enqueue(stack,createCell(row,col-1),createCell(row,col));
		visited[row][col-1] = 1;//location becomes visited
	    }
        }
	i++;
    }
    if(maze->map[0][0]=='1'){//if the first node is 1 the maze cannot be solvable
        maze->solvable=0;
    }
    if(maze->solvable==1){//if solvable be can interperet the stack to find the solution
        while(stack->size>0){
            row = getParent(stack).row;//saves parent value
            col = getParent(stack).col;
	    while(stack->size>0){
	        if((gettop(stack).row==row)&&(gettop(stack).col==col)){//loops until data value is equal to the parent saved previously
                    maze->map[gettop(stack).row][gettop(stack).col]='3';//if its on the solution path changes value in maze to 3
		    maze->moves++;
		    break;
	        }
	        stack->size--;//moves down the stack
	    }
        }
	maze->map[maze->rows-1][maze->columns-1]='3';//sets end of maze to 0
    }
    if(maze->s==1){//if s command was used 
	if(maze->solvable==1){//if solvable prints number of moves
	    printf("Solution in %d steps.\n",maze->moves+1);
	}
	else{//if not solvable prints no solution
	    printf("No solution.\n");
	}
    }
    if(maze->p==1){//if p command was used
        printMap(maze);//reprints the map with the values on the solution path set to 3 
    }
    for(int i = 0;i<maze->rows;i++){//iterates through the map and frees every row
        free(visited[i]);
    }
    free(visited);
    freeStack(stack);//frees the stack
}

//reads through one line and reallocs to map to include another row and mallocs the new row then reads the line and adds all 1's and 0's to the new row. 
int readline(Maze *maze, size_t size){
    char *buf=NULL;
    int count = 0;
    getline(&buf,&size,maze->i);
    if(!feof(maze->i)){
        maze->map = realloc(maze->map,(maze->rows+1)*8);//reallocs maze
        maze->map[maze->rows]=malloc(maze->columns);//mallocs new row
        for(int i = 0;i<(signed)strlen(buf);i++){//iterates through the line. 
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

//reads the file to create the map part of maze. First it reads the first line to get the number of columns and saves it to the maze struct, then it adds all 1's and 0's to the map. Then it calls readline over and over until its at the end of the fle.
void createMap(Maze *maze){
    FILE *fp = maze->i;
    char* buf=NULL;//initializes buf to NULL
    size_t len=0;//initializes len to 0
    int a=0;
    int count = 0;
    getline(&buf,&len,fp);
    for(int i=0; i<(signed)strlen(buf); i++){
        if(buf[i]==48||buf[i]==49){
	    count++;//count is used to track the number of columns
	}
    }
    maze->columns=count;//sets columns to count
    maze->map=malloc(8);//mallocs the map to include 1 row 
    maze->map[0]=malloc(maze->columns);//mallocs the first row
    for(int i=0;i<(signed)strlen(buf); i++){
	if(buf!=NULL){
            if((buf[i]==48)||(buf[i]==49)){//adds all 1's and 0's to the map
	        maze->map[0][a]=buf[i];
	        a++;
	    }
        }   
    }
    if(buf!=NULL){
	free(buf);
    }
    maze->rows = 1;
    while(readline(maze,maze->columns)==1){//calls readline over and over again until the whole file is read
	maze->rows++;
    }
}

//this function takes a maze and frees it
void freeMaze(Maze* maze){
    for(int i = 0;i<maze->rows;i++){//iterates through the map and frees every row
	free(maze->map[i]);
    }
    free(maze->map);//frees the map
    free(maze);//frees the maze stuct
}

//main checks the commands and creates a maze. Then initialised the maze with the command values and calls create map to create the array part of the maze. Then it calls bestSolution to do the rest. 
int main(int argc, char** argv){
    Maze *maze = malloc(sizeof(Maze));
    int opt;
    maze->p=0;//defaults to false
    maze->d=0;//
    maze->s=0;//
    FILE *i=stdin;//defaults to stdin
    FILE *o=stdout;//defaults to stdout
    while((opt=getopt(argc, argv, "hdspi:o:"))!=-1){//checks all the commands
        switch(opt){
	    case 'i':
		i=fopen(optarg,"r");
		break;
	    case 'o':
		o=fopen(optarg,"w");
		break;
	    case 'h':
		printf("Print this helpful message to stdout and exit\n");
		exit(0);
		break;
	    case 'd':
		maze->d=1;
		break;
	    case 's':
		maze->s=1;
		break;
	    case 'p':
		maze->p=1;
		break;
	    default:
		printf("unrecognized command: '%c'.\n",opt);
	}
    }
    maze->i=i;//sets the input file to i
    maze->o=o;//sets the output file to o
    maze->moves=0;//initialised moves to 0
    createMap(maze);//creates map
    if(maze->d==1){//if command d was used
        printMap(maze);//print the map
    }
    maze->solvable=0;//initializes solvable to 0
    bestSolution(maze);
    fclose(maze->i);//closes the files
    fclose(maze->o);
    freeMaze(maze);//frees the maze
    return 0;
}

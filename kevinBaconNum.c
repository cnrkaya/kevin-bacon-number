#include <stdio.h>
#include <stdlib.h>
#define MAX_WORD_CHAR 100 // it is calculated 98 in given text
#define MAX_STEP_SIZE 100 // used in backtrace array size
#define N_ACTOR 170518
#define N_MOVIE 14129
#define FACTOR 1.37
#define R 2
#define QUEUE_SIZE 190000


/*represents the edges of the graph */
typedef struct listNode{
    unsigned int hashNum ;
    struct listNode * next;
}LINKED_LIST;

/*represents the nodes of the graph */
typedef struct hashNode{
    char name[MAX_WORD_CHAR];  //name of the node
    LINKED_LIST * head;        //edges of the node
    short  visited;            // visited mark
    int type;                  //movie or actor
    struct hashNode * parent;  //to make back tracking
}HASH_NODE;

/* Queue structure*/
typedef struct queue {
    HASH_NODE * items[QUEUE_SIZE];
    int front;
    int rear;
}Q;

/* Function Prototypes */

Q* createQueue();
void enqueue(Q* q, HASH_NODE *);
int dequeue(Q* q);
int isEmpty(Q* q);

LINKED_LIST * createConnection(int hashNO);
HASH_NODE * createHashNode(char * name,int type);
HASH_NODE ** createTable(M_TABLE);
void addVertexToSet(HASH_NODE ** sett, int M_SET ,char * word, int* value,int type);
void addEdgeToSet(HASH_NODE ** sett ,int vertex,int edge);
void readAndPlace(FILE * ,HASH_NODE **  , HASH_NODE ** ,int ,int );
void getActorName(char * word,HASH_NODE **actors,int M_ACTOR_TABLE,Q * q,int isTarget);

unsigned long long int word_key(char * word);
int doubleHash(unsigned long long int key,int i,int M);
int findValue(HASH_NODE ** sett, int M_SET ,char * word);

HASH_NODE * bfs(Q * q,char * word,char *,HASH_NODE ** movies, HASH_NODE ** actors);
int backTrace(HASH_NODE * lastStep);

void clearTraversals(HASH_NODE ** sett,int M_SIZE);
void printMenu();


/* Creates edges for nodes */
LINKED_LIST * createConnection(int hashNO){
    LINKED_LIST * newCon = (LINKED_LIST * ) malloc(sizeof(LINKED_LIST));
    if (newCon == NULL){
	    printf("Allocation Error\n");
        exit (-1);
    }
    newCon->hashNum = hashNO;
    newCon->next = NULL;
    return newCon;
}

/* Creates a hash node for the graph */
HASH_NODE * createHashNode(char * name,int type){
    HASH_NODE * newNode = (HASH_NODE * ) malloc(sizeof(HASH_NODE));
    if (newNode == NULL){
	    printf("Allocation Error\n");
        exit (-1);
    }
    strcpy(newNode->name,name);
    newNode->head = NULL;
    newNode->visited = 0;
    newNode->type = type;
    return newNode;
}

/*Creates a hash table which represents graph */
HASH_NODE ** createTable(M_TABLE){
    int i;
    HASH_NODE** table = (HASH_NODE  **) malloc(sizeof(HASH_NODE *)*M_TABLE);
    if (table == NULL){
	    printf("Allocation Error\n");
        return -1;
    }

    for(i = 0 ; i < M_TABLE ; i++)
        table[i] = NULL;
    return table;
}
/*Generates a hash value corresponding to the given key*/
int doubleHash(unsigned long long int key,int i,int M){

    int hashValue;
    int h1;
    int h2;
    h1 = key % M;
    h2 = 1 + (key % M-1);
    hashValue = (h1 + h2 * i)% M;
            //reads the actor name from the user
return hashValue;
}

/*Calculates key of the word by Horner's rule*/
unsigned long long int word_key(char * word){

    int i = 0;
    unsigned long long int key = 0;

    //goes to the end of the word
    while(word[i] != NULL) i++;

    unsigned long long int multiplier = 1;
    for(i=i-1; i>=0;i--){
        key += multiplier * word[i];
        multiplier *=R;

    }
    return key;
}

/*returns the value of the hash node to which the given name belongs */
int findValue(HASH_NODE ** sett, int M_SET ,char * word){
    unsigned long long int key = word_key(word);
    int value;
    int l = 0;
    value = doubleHash(key,l,M_SET);
    while( (sett[value] != NULL)
          && (strcmp(sett[value]->name,word) != 0)){
        l++;
        value = doubleHash(key,l,M_SET);
    }
    if( sett[value] != NULL )
        return value;
    else
        return -1; //if no node with given name
}

/* Creates and adds a new vertex with the given attribute to the set*/
void addVertexToSet(HASH_NODE ** sett, int M_SET ,char * word, int* value,int type){

    unsigned long long int key = word_key(word);
    int l = 0;
    *value = doubleHash(key,l,M_SET);

    while( (sett[*value] != NULL)
          && (strcmp(sett[*value]->name,word) != 0)){
        l++;
        *value = doubleHash(key,l,M_SET);
    }

    if(sett[*value] == NULL){
        //If the actor has not been added before
        HASH_NODE * newNode = createHashNode(word,type);
        sett[*value] = newNode;
    }
}

/* Creates and adds a new edge with the given attribute to the vertex*/
void addEdgeToSet(HASH_NODE ** sett ,int vertex,int edge){

    LINKED_LIST * newConn = createConnection(edge);
    LINKED_LIST * tmp = sett[vertex]->head ;
    if( tmp == NULL){
        sett[vertex]->head = newConn;
    }
    else{
        while(tmp->next != NULL){
            tmp = tmp->next;
        }
        tmp->next = newConn;
    }

}
/* reads the given dataset and creates the nodes and places them in the graph*/
void readAndPlace(FILE * fp ,HASH_NODE ** movies , HASH_NODE ** actors,
                  int M_MOVIE_TABLE,int M_ACTOR_TABLE){
    int movie_value;
    int actor_value;
    int i,k;
    char tmp_word[MAX_WORD_CHAR];
    char * fileName[MAX_WORD_CHAR] ; // = "input-mpaa.txt";
    printf("Please enter the file name : ");
    scanf("%s",fileName);
    fp  =fopen(fileName, "r");

    if (fp == NULL){
        printf("%s could not be opened\n",fileName);
        exit(EXIT_FAILURE);
    }
    int nextWordMovie = 1;

    //read character by character
    char c =getc(fp);
    while( c!=EOF ) {
        i = 0;
        while(c!= '/' && c!= '\n'){
            /*After '/' new actor name,
            After '\n' new movie name */
            tmp_word[i] = c;
            i++;
            c = getc(fp);
        }
        tmp_word[i] = NULL;
        if(nextWordMovie == 1){
            //tmp_word : Movie name
            addVertexToSet(movies,M_MOVIE_TABLE,tmp_word,&movie_value,1);
            nextWordMovie =0;
            k++;
        }
        else{
            //tmp_word : Actor name
            addVertexToSet(actors,M_ACTOR_TABLE,tmp_word,&actor_value,0);
            //add movie connection to actors set
            addEdgeToSet(movies,movie_value,actor_value);
            //add actor connection to movies set
            addEdgeToSet(actors,actor_value,movie_value);

        }
        if(c == '\n')
            nextWordMovie = 1;
        c =getc(fp);
    }
}
/* breadth first search algorithm */
HASH_NODE * bfs(Q * q,char * word,char * target,
                HASH_NODE ** movies, HASH_NODE ** actors){

    int isActor; //determines whether the vertex popped from the queue is an actor
    int m = 0;  //distance counter
    HASH_NODE * vertex;
    LINKED_LIST * edge;
    do{
        vertex = (dequeue(q));
        if(vertex->type == 1){
            //vertex is movie type
            m += isActor ^ 1;//m increments if previous type different
            isActor = 1;
        }
        else{
            //vertex is actor type
            isActor =0;
        }

        if( (strcmp(vertex->name,target)) ==0 )
        //found target
            return vertex;

       edge = vertex->head;
       if(isActor == 0){
           while(edge != NULL){
                if(movies[edge->hashNum]->visited != 1){
                    movies[edge->hashNum]->visited = 1;
                    movies[edge->hashNum]->parent = vertex;
                    enqueue(q,movies[edge->hashNum]);
                }
                edge = edge->next;
           }
       }
       else{
           while(edge != NULL){
                if(actors[edge->hashNum]->visited != 1){
                    actors[edge->hashNum]->visited = 1;
                    actors[edge->hashNum]->parent = vertex;
                    enqueue(q,actors[edge->hashNum]);
                }
                edge = edge->next;
            }
       }
    }while(isEmpty(q) == 0 && m < 6 );
    // if no connection in 6 steps returns -1
    return -1;
}
/*clears traversals on the graph */
void clearTraversals(HASH_NODE ** sett,int M_SIZE){
    int i ;
    for (i = 0 ; i< M_SIZE; i++){
        if(sett[i] != NULL){
            sett[i]->parent = 0;
            sett[i]->visited = 0;
        }
    }
}
/*returns distance between two actors and prints steps */
int backTrace(HASH_NODE * lastStep){
    char * steps[MAX_STEP_SIZE];
    steps[0] = lastStep->name;
    int k = 1,i;
    while(lastStep->parent != -1){
        steps[k] = lastStep->parent->name;
        k++;
        lastStep = lastStep->parent;
    }
    printf("----------------------------\n");
    for (i = k-1; i>= 2 ; i = i -2){
        printf("* %s - %s : %s\n",steps[i],steps[i-2],steps[i-1]);
    }
    return (k-1)/2;
}
/*Prints the user menu*/
void printMenu(){

    printf("\nPlease make a choice\n"
       "----------------------------\n");
    printf(" 1 Find Kevin Bacon number \n"
       " 2 Find distance between two actor\n"
       "-1 FOR EXIT\n choice :");
}
/*gets the actor's name from user*/
void getActorName(char * word,HASH_NODE **actors,int M_ACTOR_TABLE,Q * q,int isTarget){
    int confirm; //indicates whether the actor's name is in the table
    int value;
    do{
        gets(word);
        value = findValue(actors,M_ACTOR_TABLE,word);
        if(value == -1){
            printf("No actor with the entered name.\n"
                   "Please make sure enter the name correctly :" );
            confirm = 0;
        }
        else
            confirm = 1;
    }while(confirm == 0);

    if(!isTarget){
        //start node is placed in the queue
        actors[value]->visited = 1;
        actors[value]->parent = -1;
        enqueue(q,actors[value]);
    }
}
int main(){

    //Calculate the number of elements for the hash tables
    int M_ACTOR_TABLE = N_ACTOR * FACTOR;
    int M_MOVIE_TABLE = N_MOVIE * FACTOR;
    HASH_NODE ** actors, **movies;

    HASH_NODE * terminal; // Return value of bfs function

    //actor names indicating the begining and target
    char begining[MAX_WORD_CHAR];
    char target [MAX_WORD_CHAR];

    Q* q;
    int option = 0;  //option variable for user menu

    FILE * fp;      // file pointer

    int k =0;
    int i = 0;  // counter for general loops

    //Memory allocation for hash table
    actors = createTable(M_ACTOR_TABLE);
    movies = createTable(M_MOVIE_TABLE);

    /* reads the given dataset and creates the nodes
        places them in the graph*/
    readAndPlace(fp,movies,actors,M_MOVIE_TABLE,M_ACTOR_TABLE);

    while(option != -1){

        printMenu();
        scanf("%d",&option);
        getchar();  //dummy read
        system("@cls||clear");
        if(option == 1){

            printf("Please enter the actor's name : ");

            q = createQueue();
            //reads the actor name from the user
            getActorName(begining,actors,M_ACTOR_TABLE,q,0);

            if(strcmp(begining,"Bacon, Kevin") == 0)
                printf("Bacon, Kevin : Kevin Bacon number is 0");
            else{
                strcpy(target,"Bacon, Kevin");

               terminal = bfs(q,begining,target,movies,actors);
                if(terminal == -1)
                    printf("There is no connection"
                           "between %s and %s is %d\n",begining,target);
                else{
                    int step = backTrace(terminal);
                    printf("The actor %s 's Kevin Bacon number is %d\n",begining,step);

                    clearTraversals(movies,M_MOVIE_TABLE);
                    clearTraversals(actors,M_ACTOR_TABLE);
                    free(q);
                }
            }

        }else if(option == 2){

            q = createQueue();
            printf("Please enter the first actor's name : ");
            //reads the first actor name from the user
            getActorName(begining,actors,M_ACTOR_TABLE,q,0);
            printf("Please enter the second actor's name : ");
            //reads the second actor name from the user
            getActorName(target,actors,M_ACTOR_TABLE,q,1);

            if(!strcmp(begining,target)){
                printf("The distance to the actor itself is 0 \n");

            }else{

                terminal = bfs(q,begining,target,movies,actors);

                if(terminal == -1)
                    printf("There is no connection between %s and %s is %d\n",begining,target);
                else{
                    int step = backTrace(terminal);
                    printf("Distance between %s and %s is %d\n",begining,target,step);

                    clearTraversals(movies,M_MOVIE_TABLE);
                    clearTraversals(actors,M_ACTOR_TABLE);
                    free(q);

                }
            }
        }
        printf("__________________________\n");

    }


    for(i = 0; i < M_ACTOR_TABLE ; i++)
        free(actors[i]);
    free(actors);

    for(i = 0; i< M_MOVIE_TABLE ; i++)
        free(movies[i]);
    free(movies);
    printf("Dynamic Memory Allocations Released\n"
           "Program terminated succesfully\n");
    return 0;
}

Q * createQueue(){
    Q* q =(Q *) malloc(sizeof(Q));
    q->front = -1;
    q->rear = -1;
    return q;
}
int isEmpty(Q* q) {
    if(q->rear == -1)
        return 1;
    else
        return 0;
}
void enqueue(Q* q, HASH_NODE * value){
    if(q->rear == QUEUE_SIZE-1){
        printf("\nQueue is Full please increase the QUEUE_SIZE\n");
        exit(-1);
    }
    else {
        if(q->front == -1)
            q->front = 0;
        q->rear++;
        q->items[q->rear] = value;
    }
}
int dequeue(Q* q){
    int item;
    if(isEmpty(q)){
        printf("Queue is empty");
        item = -1;
    }
    else{
        item = q->items[q->front];
        q->front++;
        if(q->rear <= q->front  ){
            q->front = -1;
            q->rear = -1;
        }
    }
    return item;
}


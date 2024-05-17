#include "graph.h"

// Helper function to convert character to integer
int ctoi(char a);

// Adds an edge between a node inside DepGraph's AdjList array and a new node. 
void addEdge(struct DepGraph* graph, int src, int dest);

// Helper function to compute x^y. x and y must be non-negative integers
int intPow(int x, int y);

struct AdjListNode* newAdjListNode(int dest){ 
    struct AdjListNode* newNode = (struct AdjListNode*) malloc(sizeof(struct AdjListNode)); 
    newNode->dest = dest; 
    newNode->next = NULL; 
    return newNode; 
} 

// Given an input file which contains a list of commands for each node and a list of node dependencies and a 2D character array, the function will
// create a dependency graph. The character array will be modified to insert the given input commands for each node. For example, at char[0][j], the
// command to be executed for node 0 will be stored. The last character of each row of the character array is terminated with the null character '\0'
// to indicate that the end of the row has been reached. The dependency graph uses an adjacency list data structure, where each element of the 
// adjacency list array indicates the dependencies, or nodes which need to be executed, before such node.
struct DepGraph* createDepGraph(FILE *input, char cmds[][550]){
    ssize_t read;
    size_t len = 0;
    char *line = NULL; // Character array to store input a given line for getline() calls
    int V = 0; // number of nodes


    // Obtaining the number of nodes:

    int retVal = getline(&line, &len, input); // number of nodes (first line of .txt)

    if(retVal == -1) {
        perror("Error reading file with getline()");
    }
    
    // Note: Value of V could be any arbitrary number, need to make it into an integer. Uses base 10 representation of an integer to combine
    // characters into an integer.

    int numChars = 0;

    while(line[numChars] != '\n') { // Get number of characters
        numChars++;
    }

    int s = 0;
    for(int k = numChars-1; k >= 0; k--) { // Get value of V from characters
        V = V + ctoi(line[s]) * intPow(10, k);
        s++;
    }

    retVal = getline(&line, &len, input); // Skip blank line
    if(retVal == -1) {
        perror("Error reading file with getline()");
    }

    // Read all commands from the file and load them onto the cmds array

    retVal = getline(&line, &len, input);
    if(retVal == -1) {
        perror("Error reading file with getline()");
    }
    int i = 0;

    while (line[0] != '\n') { // if first character read in line is \n, this is an empty line, end while loop & move to source-destination section
        int j = 0;
        while(line[j] != '\n') {
            cmds[i][j] = line[j]; // Command is stored onto cmds array
            j++;
        }
        cmds[i][j+1] = '\0';
        i++;
        retVal = getline(&line, &len, input); // Get next line
        if(retVal == -1) {
            perror("Error reading file with getline()");
        }
    }

    // Allocate memory space for DepGraph, Adjlist, and each element in the AdjList array

    struct DepGraph* graph = (struct DepGraph*) malloc(sizeof(struct DepGraph));
    graph->V = V;

    // Initialize AdjList array
    graph->array = (struct AdjList**) malloc(V * sizeof(struct AdjList*));

    // Initialize each element in DepGraph's AdjList array
    for(i = 0; i < V; i++) {
        graph->array[i] = (struct AdjList*) malloc(sizeof(struct AdjList));
        graph->array[i]->head = NULL;
        graph->array[i]->visit = 0;
    }

    // Load source dependencies onto the dependency graph
    // Each iteration of the while loop builds the next node's adjacency list. If the end of the file is reached, the while loop should terminate
    
    while(getline(&line, &len, input) != -1) { // while loop continues reading source-destinations until end of file is reached
        int numCharacters = 0;
        int source = 0;

        // Get value of source. Note that source could be any arbitrary number, which is why we must obtain the number of characters
        // in the source and uses base 10 representation of an integer to combine characters into an integer. Need to obtain number
        // of characters to do this first.

        while(line[numCharacters] != ' ') { // Get number of characters for source
            numCharacters++;
        }

        int j = 0;
        for(int k = numCharacters-1; k >= 0; k--) { // Get the value of source
            source = source + ctoi(line[j]) * intPow(10, k);
            j++;
        }

        int index = numCharacters+1; // tracks next character to read, skip '\n'
        j = index; // store this for later when looping through again

        numCharacters = 0;
        int dest = 0;

        while(line[index] != '\n' && line[index] != '\0') { // Get number of characters for destination
            numCharacters++;
            index++;
        }

        for(int k = numCharacters-1; k >= 0; k--) { // Get the value of source
            dest = dest + ctoi(line[j]) * intPow((double) 10, (double) k);
            j++;
        }

        addEdge(graph, source, dest);
    }
   
    return graph; 
}

// Given a dependent graph, a source (index of array in which to add the destination to), and the destination (dest), the function will add the new 
// node at the index of the source into the adjacency list array.
void addEdge(struct DepGraph* graph, int src, int dest){
    // If the head at the desired source is null,
    // create the new node here, forming the added edge
    if (graph->array[src]->head == NULL) {
        graph->array[src]->head = newAdjListNode(dest);
    }
    // Traverse through the AdjListNodes until a node's next pointer points to null.
    // Create the new node here, forming the added edge.
    else {
        struct AdjListNode* node = graph->array[src]->head;
        while(node->next != NULL) {
            node = node->next;
        }
        node->next = newAdjListNode(dest);
    }
}


// Given a dependency graph, a node in the graph, a list of commands corresponding to the nodes and an execution mode, 
// the function will execute the command of the given node and its dependencies and write the command to an output file either sequentially (0 mode) or in parallel (1 mode).
void DFSVisit(struct DepGraph* graph, int node, char cmds[][550], int mode) {
    FILE *fptr;
    pid_t pid;
    char *cmd;
    char *args[100];
    struct AdjListNode *curr_node = graph->array[node]->head;
	
    // Check mode for sequential (0) or parallel (1)
    if (mode == 0) {
        pid = fork();

        if (pid < 0) { // fork() failure
            perror("fork() failed");
            return;
        } else if (pid == 0) { // Child process code to execute
            // Iterate through dependencies of the current node to traverse recursively
            while (curr_node != NULL) {
                DFSVisit(graph, curr_node->dest, cmds, mode);
                curr_node = curr_node->next;
            }
        } else { // Parent process code to execute
            if(wait(NULL) == -1) { // Wait for child process to finish executing
                perror("wait() failed");
            }
            return;
        }
    } else if (mode == 1) {
        // Get number of nodes
        int count = 0;
        struct AdjListNode *i_node = curr_node;
        while (i_node != NULL) {
            count++;
            i_node = i_node->next;
        }
        // Create array of nodes
        struct AdjListNode *nodes[count];
        i_node = curr_node;
        for (int i = 0; i < count; i++) {
            nodes[i] = i_node;
            i_node = i_node->next;
        }
        // Create child processes
        for (int i = 0; i < count; i++) {
            pid = fork();
            // fork() failure
            if (pid < 0) {
                perror("fork() failed");
                return;
            }
            // Child code
            else if (pid == 0) {
                DFSVisit(graph, nodes[i]->dest, cmds, mode);
            }
        }
        // Have parent wait for all child processes
        int status;
        while (wait(&status) > 0);
        if(status == -1) {
            perror("Parent process had error when waiting for all child processes to terminate with wait()");
        }
    }

    // Isolate the target to be executed
    cmd = strtok(cmds[node], " ");

    // Set cmd as first arg in array of args
    int index = 0;
    args[index] = cmd;

    // Read in the rest of the args
    while (args[index] != NULL) {
        index++;
        args[index] = strtok(NULL, " ");
    }

    fptr = fopen("results.txt", "a");

    // Create a new results file if one does not exist
    if (fptr == NULL) {
        fptr = fopen("results.txt", "w");
        if(fptr == NULL) {
            perror("fopen() failed to create a new \"results.txt\" file");
        }
    }

    // Write process ids to file
    fprintf(fptr, "\n%d %d", getpid(), getppid());

    // Write each arg to the file
    for (int i = 0; i < sizeof(args); i++) {
        if (args[i] == NULL) {
            break;
        }

        fprintf(fptr, " %s", args[i]);
    }

    fclose(fptr);

    // Execute the command and associated arguments
    execvp(cmd, args);
    perror("Failed to execute a command"); // should not reach this line after execvp() is called

    return;
}

// execute the generated graph
void processGraph(struct DepGraph* graph, char cmds[][550], int mode){
    int i;
    int child = fork();
    if (child == 0) {
	   DFSVisit(graph, 0, cmds, mode);
    }
    else if (child < 0) {
        perror("fork() failed");
    }
    else {
        wait(NULL);
    }
}

// Frees the allocated memory of the graph and its internal structures
void freeGraph(struct DepGraph* graph) {
    struct AdjList** graph_arr = graph->array;
    struct AdjListNode* curr_node;
    struct AdjListNode* next_node;

    for (int i = 0; i < graph->V; i++) {
        curr_node = graph_arr[i]->head;

        // Free each element of the linked list at the current array index
        while (curr_node != NULL) {
            next_node = curr_node->next;
            free(curr_node);

            curr_node = next_node;
        }

        // Free the current list of the graph array
        free(graph_arr[i]);
    }
    
    // Free graph array
    free(graph_arr);

    // Free graph structure
    free(graph);
}

// Helper function to convert character to integer
int ctoi(char a) {
    return a - '0';
}

// Helper function to compute x^y. x and y must be non-negative integers
int intPow(int x, int y) {
    int retVal = 1;
    for(int i = 0; i < y; i++) {
        retVal *= x;
    }
    return retVal;
}

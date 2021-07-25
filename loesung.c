#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct NodeList NodeList;
typedef char* ID;

typedef struct {
    ID value;
    unsigned int len;
    unsigned int size;
} NodeID;

typedef struct {
    NodeID *id;
    NodeList *neighbours;
    unsigned int value;
} Node;

struct NodeList{
    Node** nodes;
    unsigned int len;
    unsigned int size;
};

// -------------------------------------------------------------

// TODO: Fix bug when the ID has already been used
NodeList *nodelist; // holds all pointers to nodes in the graph
NodeID *startingNodeID;
unsigned int numOfSteps;

// -------------------------------------------------------------
void freeNode(Node *node);

void freeNodeID(NodeID *id) {
    free(id->value);
    free(id);
}

void freeNeighbourList(NodeList *list) {
    // don't delete nodes in list, cause they are in nodelist too
    free(list->nodes);
    free(list);
}

// frees a node and the neighbourList if necessary
void freeNode(Node *node) {
    freeNodeID(node->id);
    if (node->neighbours != NULL) {
        freeNeighbourList(node->neighbours);
        node->neighbours = NULL;
    }
    free(node);
}

// frees a nodelist
void freeNodeList(NodeList *list) {
    for (unsigned int i = 0; i < list->len; i++) {
        if (list->nodes[i] != NULL) {
            freeNode(list->nodes[i]);
            list->nodes[i] = NULL;
        }
    }

    free(list->nodes);
    free(list);
}

// frees all allocated memory
void freeMemory() {
    freeNodeID(startingNodeID);
    freeNodeList(nodelist);
}

// prints an error message and aborts the program
void throwError(char *msg) {
    fprintf(stderr, "%s\n", msg);
    freeMemory();
    exit(EXIT_FAILURE);
}

// -------------------------------------------------------------

// allocates memory for a new NodeID, returns pointer to NodeID if program runs without errors, else NULL
NodeID* createNewID() {
    unsigned int size = 10;

    NodeID *nodeID = malloc(sizeof(NodeID));
    if (nodeID == NULL) return NULL;

    nodeID->value = calloc(size, sizeof(char));
    if (nodeID->value == NULL) {
        free(nodeID);
        return NULL;
    }

    nodeID->len = 0;
    nodeID->size = size;

    return nodeID;
}

// adds a char to a NodeID and keeps enough space
bool addCharToNodeID(NodeID *id, char c) {
    // expand NodeID space if necessary
    if (id->len == id->size) {
        id->size *= 2;

        ID temp = realloc(id->value, sizeof(char) * id->size);
        if (temp == NULL) return false;
        else id->value = temp;
    }

    // add char to NodeID
    if (c == '\0') {
        id->value[id->len] = c;
        id->size = id->len;
    } else {
        id->value[id->len++] = c;
    }

    return true;
}

// creates a new node and returns the pointer to it
Node* createNewNode() {
    Node *node = malloc(sizeof(Node));
    if (node == NULL) return NULL;

    node->id = NULL;
    node->value = 0;
    node->neighbours = NULL;

    return node;
}

// sets the id of a node to id
void addIDToNode(Node *node, NodeID *id) {
    node->id = id;
}

// creates a new NodeList with specified size, returns a pointer to it
NodeList* createNewNodeList() {
    NodeList *list = malloc(sizeof(NodeList));
    if (list == NULL) return NULL;

    list->size = 5;
    list->len = 0;

    list->nodes = malloc(sizeof(Node*) * list->size);
    if (list->nodes == NULL) {
        free(list);
        return NULL;
    }

    return list;
}

// returns the pointer of element in nodelist if it has the same ID, else returns NULL
Node* getIDInNodelist(NodeList *list, NodeID *nodeId) {
    // use binary search
    int left = 0;
    int right = (int) list->len - 1;

    while (left <= right) {
        int middle = left + (right - left) / 2;
        int cmpVal = strcmp(list->nodes[middle]->id->value, nodeId->value);

        if (cmpVal == 0) {
            return list->nodes[middle];
        } else if (cmpVal > 0) {
            right = middle - 1;
        } else {
            left = middle + 1;
        }
    }

    return NULL;
}

// get the index where to put a node into a NodeList using binary search
int getIndexForInsertion(NodeList *list, Node *node) {
    int left = 0;
    int right = (int) list->len - 1;
    int cmpVal;

    while (left < right) {
        int middle = (left + right) / 2;
        cmpVal = strcmp(node->id->value, list->nodes[middle]->id->value);

        if (cmpVal > 0) {
            left = middle + 1;
        } else if (cmpVal < 0) {
            right = middle - 1;
        } else return -1; // there are no nodes with identical IDs allowed
    }

    // when there is only one space to search for, determine if the node has to be placed left or right
    cmpVal = strcmp(node->id->value, list->nodes[left]->id->value);
    if (cmpVal > 0) return left + 1;
    else if (cmpVal < 0) return left;
    else return -1; // there are no nodes with identical IDs allowed
}

// adds a node into a node list at position index
bool addNodeInsideOfNodeList(NodeList *list, Node *node, unsigned int index) {
    // shift elements behind index one position to the right
    for (int i = (int) list->len - 1; i >= (int) index; i--) {
        list->nodes[i + 1] = list->nodes[i];
    }

    list->nodes[index] = node;
    list->len++;

    return true;
}

// adds a pointer to a node to a nodeList
bool addNodeToNodeList(NodeList *list, Node *node) {
    // resize nodelist if it is full
    if (list->len == list->size) {
        list->size *= 2;

        Node **temp = realloc(list->nodes, sizeof(Node*) * list->size);
        if (temp == NULL) return false;
        else list->nodes = temp;
    }

    // if list is empty, just insert it
    if (list->len == 0) list->nodes[list->len++] = node;
    // else find the correct index and insert
    else {
        int index = getIndexForInsertion(list, node);
        if (index == -1) return false; // identical id already in list
        else return addNodeInsideOfNodeList(list, node, index);
    }

    return true;
}

// -------------------------------------------------------------

// reads a number string from stdin, converts and returns it, if there was an error, it returns -1
int parseValue() {
    char valueStr[11];
    unsigned long int value;

    // parse number as a string
    if(scanf("%11[0-9]", valueStr) != 1) return -1;

    if (getchar() != '\n') return -1;

    // convert it to a long unsigned int
    value = strtol(valueStr, NULL, 10);

    // value has to be in bounds
    if (value > 4294967296 - 1) return -1;

    return (int) value;
}

// gives a node its ID, returns either pointer to Node or NULL if no node will be needed
Node* parseLeftSide() {
    char currChar;

    NodeID *id = createNewID();
    if (id == NULL) throwError("Error allocating memory for new NodeID");

    // check if the list of new nodes is finished
    currChar = (char) getchar();
    if (currChar == 'A') {
        if (getchar() != ':') {
            freeNodeID(id);
            throwError("Error when parsing starting node. Colon after 'A' expected");
        }

        // free the node cause it is not needed
        freeNodeID(id);

        return NULL;
    }
    // parse ID
    else {
        // add char to char to id
        while ((currChar >= 'a' && currChar <= 'z') ||
               (currChar >= '0' && currChar <= '9')) {

            if (!addCharToNodeID(id, currChar)) {
                freeNodeID(id);
                throwError("Couldn't allocate memory for more letters in NodeID");
            }

            currChar = (char) getchar();
        }
        if (!addCharToNodeID(id, '\0')) {
            freeNodeID(id);
            throwError("Couldn't allocate memory for string terminator in NodeID");
        }

        // check if the ID is acceptable and ends with a colon
        if (id->len == 0) {
            freeNodeID(id);
            throwError("Error when parsing left side. Invalid ID");
        } else if (currChar != ':') {
            freeNodeID(id);
            throwError("Error when parsing left side. Colon or alphanumerical value after NodeID expected");
        }

        // create a node if id isn't already in nodelist
        Node *node = getIDInNodelist(nodelist, id);
        if (node == NULL) {
            node = createNewNode();
            if (node == NULL) {
                freeNodeID(id);
                throwError("Couldn't create new node in parseLeftSide");
            }
            addIDToNode(node, id);

            // create neighbour list
            node->neighbours = createNewNodeList();
            if (node->neighbours == NULL) {
                freeNode(node);
                throwError("Couldn't create new neighbour list in parseLeftSide");
            }

            // add the node to nodelist
            if(!addNodeToNodeList(nodelist, node)) {
                freeNode(node);
                throwError("Couldn't add node to nodelist");
            }
        } else {
            freeNodeID(id);
        }

        return node;
    }
}

// parses the list of NodeIDs on the right side of a line and adds them to the leftSideNode's neighbour array
void parseRightSide(Node *leftSideNode) {
    char currChar;
    NodeID *id;

    NodeList *list = createNewNodeList();
    if (list == NULL) throwError("Couldn't create a NodeList while parsing right side");

    // parse every id while there is a comma after it
    do {
        id = createNewID();
        if (id == NULL) {
            freeNodeList(list);
            throwError("Couldn't create a NodeID while parsing right side");
        }

        // add every allowed char to id
        currChar = (char) getchar();
        while ((currChar >= 'a' && currChar <= 'z') ||
               (currChar >= '0' && currChar <= '9')) {

            if (!addCharToNodeID(id, currChar)) {
                freeNodeID(id);
                freeNodeList(list);
                throwError("Couldn't allocate memory for more letters in neighbour NodeID");
            }

            currChar = (char) getchar();
        }
        if (!addCharToNodeID(id, '\0')) {
            freeNodeID(id);
            freeNodeList(list);
            throwError("Couldn't allocate memory for string terminator in neighbour NodeID");
        }

        // check if id is valid
        if (id->len == 0) {
            freeNodeID(id);

            // empty right side with value redefinition is allowed
            if (!(list->len == 0 && currChar == '-')) {
                freeNodeList(list);
                throwError("Error while parsing right side. Invalid ID");
            }
        }
        // loops are not allowed
        else if (strcmp(leftSideNode->id->value, id->value) == 0) {
            freeNodeID(id);
            freeNodeList(list);
            throwError("Loops are not allowed in graph");
        } else {
            Node *node = getIDInNodelist(nodelist, id);

            // create a new node if id isn't in nodelist yet
            if (node == NULL) {
                node = createNewNode();
                if (node == NULL) {
                    freeNodeID(id);
                    freeNodeList(list);
                    throwError("Couldn't create a new node in parseRightSide");
                }
                addIDToNode(node, id);

                // create neighbour list
                node->neighbours = createNewNodeList();
                if (node->neighbours == NULL) {
                    freeNode(node);
                    freeNodeList(list);
                    throwError("Couldn't create a new neighbour list in parseRightSide");
                }

                // add to nodelist
                if (!addNodeToNodeList(nodelist, node)) {
                    freeNode(node);
                    freeNodeList(list);
                    throwError("Couldn't add node to nodelist in parseRightSide");
                }
            } else {
                freeNodeID(id); // id is not necessary anymore
            }

            // add id pointer to list
            if (!addNodeToNodeList(list, node)) {
                freeNode(node);
                freeNodeList(list);
                throwError("Couldn't add node to neighbour nodeList");
            }
        }
    } while (currChar == ',');

    // change value of left hand side node
    if (currChar == '-') {
        int value = parseValue();

        if (value == -1) {
            freeNodeList(list);
            throwError("Error when parsing right side. Invalid value");
        }

        leftSideNode->value = (unsigned int) value;
    }
    else if (currChar != '\n') {
        freeNodeList(list);
        throwError("Error when parsing right side. Comma, dash or linefeed expected");
    }

    // set nodelist as neighbour IDList for leftSideNode
    if (leftSideNode->neighbours != NULL) freeNodeList(leftSideNode->neighbours);
    leftSideNode->neighbours = list;
}

// parses the NodeID of the startingNode
void parseStartingNodeID() {
    char currChar;

    // add every allowed char to id
    currChar = (char) getchar();
    while ((currChar >= 'a' && currChar <= 'z') ||
           (currChar >= '0' && currChar <= '9')) {

        if(!addCharToNodeID(startingNodeID, currChar)) throwError("Couldn't add a letter to startingNodeID");

        currChar = (char) getchar();
    }
    if(!addCharToNodeID(startingNodeID, '\0')) throwError("Couldn't add a letter to startingNodeID");

    // only a newline is allowed after id
    if (currChar != '\n') throwError("Error after parsing starting node id. Alphanumerical char or linefeed expected");

}

// parses number of steps, the ant will have to walk
void parseNumSteps() {
    // handle errors
    if (getchar() != 'I') throwError("Error when parsing number of steps. 'I' expected");
    if (getchar() != ':') throwError("Error when parsing number of steps. ':' after 'I' expected");

    int value = parseValue();
    if (value == -1) throwError("Invalid number of steps");
    else numOfSteps = value;

    if (getchar() != EOF) throwError("Error when parsing number of steps. End of file expected");
}

// -------------------------------------------------------------

// run each line of stdin
void scanContents() {
    Node *currNode;

    // parse all lines till you get to indicator for startingNodeID parsing start
    do {
        currNode = parseLeftSide();

        if (currNode != NULL) {
            parseRightSide(currNode);
        }
    } while (currNode != NULL);

    // parse the ID of the starting node and the number of steps
    parseStartingNodeID();
    parseNumSteps();
}

// adds connection B->A if A->B exists and checks if not both are already present
void completeConnections() {
    // add opposite direction
    for (unsigned int i = 0; i < nodelist->len; i++) {
        Node *currNode = nodelist->nodes[i];

        // check all currNode's neighbours
        for (unsigned int j = 0; j < currNode->neighbours->len; j++) {
            // get pointer to node in nodelist
            Node *destNode = currNode->neighbours->nodes[j];

            // if currNode's ID is already a neighbour of destNode...
            if (getIDInNodelist(destNode->neighbours, currNode->id) != NULL) {
                // ...and connection wasn't previously added (currNode->id > destNode->id), throw error
                if (strcmp(destNode->id->value, currNode->id->value) > 0) {
                    throwError("If there is already a connection A->B, B->A is not allowed");
                }
            }
            // add to the destNode's neighbour list
            else {
                if (!addNodeToNodeList(destNode->neighbours, currNode)) {
                    throwError("Couldn't add backwards vertex");
                }
            }
        }
    }
}

// -----------------------------------------------------------------------

void checkGraph() {
    if (nodelist->len == 0) throwError("Graph doesn't have any nodes");
    if (getIDInNodelist(nodelist, startingNodeID) == NULL) throwError("Starting node ID is not in graph");
}

// prints the output of the program
void printResult(NodeID *endNodeID) {
    // print all NodeIDs with its values
    for (unsigned int i = 0; i < nodelist->len; i++) {
        printf("%s:%u\n", nodelist->nodes[i]->id->value, nodelist->nodes[i]->value);
    }

    // print endNodeID
    printf("E:%s\n", endNodeID->value);
}

// executes the program
void letAntMove() {
    Node *currNode = getIDInNodelist(nodelist, startingNodeID);

    // if the starting node is isolated, the ant will just stay on this node and count the node's value up
    if (currNode->neighbours->len == 0) {
        currNode->value += numOfSteps;
    }
    // the ant walks normally the graph down
    else {
        while (numOfSteps > 0) {
            Node *nextNode = currNode->neighbours->nodes[currNode->value++ % currNode->neighbours->len];
            currNode = nextNode;
            numOfSteps--;
        }
    }
    printResult(currNode->id);
}

// -----------------------------------------------------------------------

void init() {
    startingNodeID = createNewID();
    if (startingNodeID == NULL) {
        fprintf(stderr, "Couldn't allocate memory for startingNodeID\n");
        exit(EXIT_FAILURE);
    }

    nodelist = createNewNodeList();
    if (nodelist == NULL) {
        freeNodeID(startingNodeID);
        fprintf(stderr, "Couldn't allocate memory for nodelist\n");
        exit(EXIT_FAILURE);
    }
}

void printAll() {
    for (unsigned int i = 0; i < nodelist->len; i++) {
        Node *currNode = nodelist->nodes[0];
        printf("%s:%u\n", currNode->id->value, currNode->value);
        if (currNode->neighbours->len > 0) printf("\t");
        for (unsigned int j = 0; j < currNode->neighbours->len; j++) {
            printf("%s, ", currNode->neighbours->nodes[j]->id->value);
        }
    }
}

int main() {
    init();
    scanContents();
    completeConnections();
    checkGraph();
    printAll();
    letAntMove();
    freeMemory();

    return 0;
}
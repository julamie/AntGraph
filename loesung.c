#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <string.h>

typedef char* ID;

typedef struct {
    ID value;
    unsigned int len;
    unsigned int size;
} NodeID;

typedef struct {
    NodeID *id;
    NodeID *neighbours;
    unsigned int value;
    unsigned int numNeighbours;
} Node;

typedef struct {
    Node** nodes;
    unsigned int size;
    unsigned int len;
} NodeList;

// -------------------------------------------------------------

//unsigned int runs = 0;
NodeList *nodelist;
NodeID *startingNodeID;
unsigned int numOfSteps;

// -------------------------------------------------------------

void freeNodeID(NodeID *id) {
    free(id->value);
    free(id);
}

void freeNode(Node *node) {
    freeNodeID(node->id);
    free(node);
}

void freeNodeList(NodeList *list) {
    for (unsigned int i = 0; i < list->len; i++) {
        freeNode(list->nodes[i]);
    }
    free(list->nodes);
    free(list);
}

// frees all allocated memory
void freeEverything() {
    freeNodeID(startingNodeID);
    freeNodeList(nodelist);
}

// prints an error message and aborts the program
void throwError(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

// -------------------------------------------------------------

// allocates memory for a new NodeID, returns if program runs without errors
NodeID* createNewID() {
    unsigned int size = 10;

    NodeID *nodeID = malloc(sizeof(NodeID));
    if (nodeID == NULL) return NULL;

    nodeID->value = malloc(sizeof(char) * size);
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

// creates a new node with a pointer to an ID
Node* createNewNode() {
    Node *node = malloc(sizeof(Node));
    if (node == NULL) return NULL;

    node->id = NULL;
    node->value = 0;
    node->neighbours = NULL;
    node->numNeighbours = 0;

    return node;
}

void addIDToNode(Node *node, NodeID *id) {
    node->id = id;
}

// creates a new NodeList with specified size
NodeList* createNewNodeList() {
    NodeList *list = malloc(sizeof(NodeList));
    if (list == NULL) return NULL;

    list->size = 5;
    list->len = 0;

    list->nodes = malloc(sizeof(Node) * list->size);
    if (list->nodes == NULL) {
        free(list);
        return NULL;
    }

    return list;
}

// adds a pointer to a node to a nodeList
bool addNodeToNodeList(NodeList *list, Node *node) {
    if (list->len == list->size) {
        list->size *= 2;

        Node** temp = realloc(list->nodes, sizeof(**list->nodes) * list->size);
        if (temp == NULL) return false;
        else list->nodes = temp;
    }

    list->nodes[list->len++] = node;

    return true;
}

// -------------------------------------------------------------

// reads a number string from stdin, converts and returns it
unsigned int parseValue() {
    char valueStr[11];
    unsigned long int value;

    // parse number as a string
    if(scanf("%11[0-9]", valueStr) != 1) {
        freeEverything();
        throwError("Error when parsing value. Unsigned int expected");
    }
    if (getchar() != '\n') {
        freeEverything();
        throwError("Error when parsing value. Too long ID (0 <= ID <= 2^32-1) or no Linefeed after ID");
    }

    // convert it to a long unsigned int
    value = strtol(valueStr, NULL, 10);

    // value has to be in bounds
    if (value > 4294967296 - 1) {
        freeEverything();
        throwError("Number of steps has to be between 0 and 2^32-1");
    }

    return (unsigned int) value;
}

// gives a node its ID, returns whether there are more left sides to be parsed
bool parseLeftSide() {
    char currChar;

    NodeID *id = createNewID();
    if (id == NULL) {
        freeEverything();
        throwError("Error allocating memory for new NodeID");
    }

    Node *node = createNewNode();
    if (node == NULL) {
        freeNodeID(id);
        freeEverything();
        throwError("Error when allocating memory for id or node");
        exit(-1); // unnecessary
    }

    // add every allowed char to id
    currChar = (char) getchar();

    // check if the list of new nodes is finished
    if (currChar == 'A') {
        if (getchar() != ':') {
            freeNodeID(id);
            free(node); // cant use freeNode, cause id is not allocated
            throwError("Error when parsing starting node. Colon after 'A' expected");
        }

        // free them cause there are not needed
        freeNodeID(id);
        free(node);

        return false;
    }
    // parse ID
    else {
        while ((currChar >= 'a' && currChar <= 'z') ||
               (currChar >= '0' && currChar <= '9')) {

            if(!addCharToNodeID(id, currChar)) {
                freeNodeID(id);
                free(node);
                freeEverything();
                throwError("Couldn't allocate memory for more letters in NodeID");
                exit(-1);
            }

            currChar = (char) getchar();
        }
        if (!addCharToNodeID(id, '\0')) {
            freeNodeID(id);
            free(node);
            freeEverything();
            throwError("Couldn't allocate memory for more letters in NodeID");
            exit(-1);
        }

        // check if the ID is acceptable and ends with a colon
        if (id->len == 0) {
            freeNodeID(id);
            free(node);
            freeEverything();
            throwError("Error when parsing left side. Invalid ID");
            exit(-1); // unnecessary
        } else if (currChar != ':') {
            freeNodeID(id);
            free(node);
            freeEverything();
            throwError("Error when parsing left side. Colon or alphanumerical value after NodeID expected");
            exit(-1); // unnecessary
        }

        // add the id to node and add that to nodelist
        addIDToNode(node, id);
        if(!addNodeToNodeList(nodelist, node)) {
            free(node);
            freeEverything();
            throwError("Couldn't add node to nodelist");
        }

        return true;
    }
}
/*
void parseRightSide(Node *leftSideNode) {
    NodeList list;
    NodeID id;
    char currChar;
    unsigned int size;
    unsigned int len;

    initNodeList(&list, 5);
    // parse every id while there is a comma after it
    do {
        size = 10;
        len = 0;

        // make space for id
        id = createNewID(size);

        // add every allowed char to id
        currChar = (char) getchar();
        while ((currChar >= 'a' && currChar <= 'z') ||
               (currChar >= '0' && currChar <= '9')) {
            addCharToNodeID(id, currChar, size, len++);
            if (len == size) size = size * 2;

            currChar = (char) getchar();
        }

        // end the string and clip it behind the '\0'
        id[len++] = '\0';
        NodeID temp = realloc(id, sizeof(*id) * len);
        if (temp == NULL) throwError("Error when removing unnecessary space from NodeID");
        else id = temp;

        if (len != 0) {
            printf("Rest: %s\n", id);
            Node newNode = createNewNode(&id);
            addToList(&list, newNode); // add id to result array
        }
    } while (currChar == ',');

    if (currChar == '-') {
        // change value of left hand side node
        unsigned int value = parseValue();
        leftSideNode->value = value;
        printf("Value: %d\n", value);
    } else if (len == 0) {
        throwError("Error while parsing right side. No empty IDs allowed");
    } else if (currChar != '\n') {
        throwError("Error when parsing right side. Comma, dash or linefeed expected");
    }

    free(id);

    return nodelist;
}
*/

void parseStartingNodeID() {
    char currChar;

    // add every allowed char to id
    currChar = (char) getchar();
    while ((currChar >= 'a' && currChar <= 'z') ||
           (currChar >= '0' && currChar <= '9')) {

        if(!addCharToNodeID(startingNodeID, currChar)) {
            freeEverything();
            throwError("Couldn't add a letter to startingNodeID");
        }

        currChar = (char) getchar();
    }
    if(!addCharToNodeID(startingNodeID, '\0')) {
        freeEverything();
        throwError("Couldn't add a letter to startingNodeID");
    }

    // only a newline is allowed after id
    if (currChar != '\n') {
        freeEverything();
        throwError("Error after parsing starting node id. Alphanumerical char or linefeed expected");
    }
}

void parseNumSteps() {
    // handle errors
    if(getchar() != 'I') {
        freeEverything();
        throwError("Error when parsing number of steps. 'I' expected");
    }
    if (getchar() != ':') {
        freeEverything();
        throwError("Error when parsing number of steps. ':' after 'I' expected");
    }

    numOfSteps = parseValue();

    if (getchar() != EOF) {
        freeEverything();
        throwError("Error when parsing number of steps. End of file expected");
    }
}

// -------------------------------------------------------------

// run each line of stdin
void scanContents() {
    int i = 0;
    bool stillNodesToBeParsed = true;
    while (stillNodesToBeParsed) {
        stillNodesToBeParsed = parseLeftSide();
        if (stillNodesToBeParsed) {
            printf("ID: %s\n\n", nodelist->nodes[i++]->id->value);

            // skip till next newline (temporary)
            while (getchar() != '\n') {}
        }
    }

    // parse the ID of the starting node and the number of steps
    parseStartingNodeID();
    printf("StartingNode: %s\n", startingNodeID->value);
    parseNumSteps();
    printf("Number of steps: %u\n", numOfSteps);
    /*bool finished = false;
    while (!finished) {
        Node currNode = parseLeftSide();

        // parse NodeID of starting node
        if (strcmp(*currNode.id, "A") == 0) {
            NodeID startNodeID = parseStartingNodeID();
            printf("Starting NodeID: %s\n", startNodeID);
            unsigned int numSteps = parseNumSteps();
            printf("Number of steps: %u\n", numSteps);

            //free(startNodeID);
            finished = true;
        } else {
            printf("ID: %s\n", *currNode.id);
            parseRightSide(&currNode);
        }

    }

    return 0;*/
}

void init() {
    startingNodeID = createNewID();
    if (startingNodeID == NULL) throwError("Couldn't allocate memory for startingNodeID");

    nodelist = createNewNodeList();
    if (nodelist == NULL) {
        freeNodeID(startingNodeID);
        throwError("Couldn't allocate memory for nodelist");
    }
}

int main() {
    init();
    scanContents();
    freeEverything();

    return 0;
}
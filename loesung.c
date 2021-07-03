#include <stdio.h>
#include <stdlib.h>
//#include <stdbool.h>
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

NodeList nodelist;

// -------------------------------------------------------------

// frees all allocated memory
void freeEverything() {
    // free all NodeID values
    for (unsigned int i = 0; i < nodelist.len; i++) {
        free(nodelist.nodes[i]->id->value);
    }
    // free the list of nodes
    free(nodelist.nodes);
}

// prints an error message and aborts the program
void throwError(char* msg) {
    fprintf(stderr, "%s\n", msg);
    freeEverything();
    exit(EXIT_FAILURE);
}

// -------------------------------------------------------------

// allocates memory for a new NodeID
void createNewID(NodeID* nodeID, unsigned int size) {
    nodeID->value = malloc(sizeof(char) * size);
    if (nodeID->value == NULL) throwError("Error allocating memory for new NodeID");

    nodeID->len = 0;
    nodeID->size = size;
}

// adds a char to a NodeID and keeps enough space
void addCharToNodeID(NodeID* id, char c) {
    if (id->len == id->size) {
        id->size *= 2;

        ID temp = realloc(id->value, sizeof(char) * id->size);
        if (temp == NULL) throwError("Error when allocating more memory for NodeID");
        else id->value = temp;
    }

    // add char to NodeID
    if (c == '\0') {
        id->value[id->len] = c;
        id->size = id->len;
    } else {
        id->value[id->len++] = c;
    }
}

// creates a new node with a pointer to an ID
void createNewNode(Node* node, NodeID *id) {
    node->id = id;
    node->value = 0;
    node->neighbours = NULL;
    node->numNeighbours = 0;
}

// creates a new NodeList with specified size
void createNewNodeList(NodeList *list, unsigned int size) {
    list->size = size;
    list->len = 0;

    list->nodes = malloc(sizeof(**list->nodes) * list->size);
    if (list->nodes == NULL) throwError("Error when allocating memory for new NodeList");
}

// adds a pointer to a node to a nodeList
void addNodeToNodeList(NodeList *list, Node *node) {
    if (list->len == list->size) {
        list->size *= 2;

        Node** temp = realloc(list->nodes, sizeof(**list->nodes) * list->size);
        if (temp == NULL) throwError("Error when increasing size of list");
        else list->nodes = temp;
    }

    list->nodes[list->len++] = node;
}

// -------------------------------------------------------------
/*
// reads an unsigned int from stdin and returns it
unsigned int parseValue() {
    unsigned int value = 5;

    if(scanf("%u", &value) != 1) throwError("Error when parsing value. Unsigned int expected");
    if (getchar() != '\n') throwError("Error when parsing value. Linefeed after value expected");

    return value;
}*/

// gives a node its ID
void parseLeftSide(Node* node, NodeID* id) {
    char currChar;

    createNewID(id, 10);

    // add every allowed char to id
    currChar = (char) getchar();
    while ((currChar >= 'a' && currChar <= 'z') ||
           (currChar >= '0' && currChar <= '9')) {

        addCharToNodeID(id, currChar);

        currChar = (char) getchar();
    }
    addCharToNodeID(id, '\0');

    createNewNode(node, id);
    addNodeToNodeList(&nodelist, node);
    /*
    Node node;
    NodeID id;
    char currChar;
    unsigned int size = 10;
    unsigned int len = 0;

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
    if (temp == NULL) throwError("Error when removing empty space from NodeID");
    else id = temp;

    // check if the list of new nodes is finished
    if (currChar == 'A') {
        if (getchar() != ':') throwError("Error when parsing starting node. Colon after 'A' expected");

        id = "A";
        node = createNewNode(&id);
        return node;
    } else if (len == 0) {
        free(id);
        throwError("Error when parsing left side. No empty IDs allowed");
    }
    else if (currChar != ':') {
        free(id);
        throwError("Error when parsing left side. Colon or alphanumerical value after NodeID expected");
    }

    // create new node and add to nodelist
    node = createNewNode(&id);
    addToList(&nodelist, node);

    return node;*/
}
/*
NodeList parseRightSide(Node* leftSideNode) {
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

NodeID parseStartingNodeID() {
    char* startingNodeID;
    char currChar;
    unsigned int size = 10;
    unsigned int len = 0;

    // make space for startingNodeID
    startingNodeID = createNewID(size);

    // add every allowed char to startingNodeID
    currChar = getchar();
    while ((currChar >= 'a' && currChar <= 'z') ||
           (currChar >= '0' && currChar <= '9')) {

        addCharToNodeID(startingNodeID, currChar, size, len++);
        if (len == size) size = size * 2;

        currChar = getchar();
    }

    // end the string and clip it behind the '\0'
    startingNodeID[len++] = '\0';
    NodeID temp = realloc(startingNodeID, sizeof(*startingNodeID) * len);
    if (temp == NULL) throwError("Error when removing unnecessary space from startingNodeID");
    else startingNodeID = temp;

    // only a newline is allowed after id
    if (currChar != '\n') {
        throwError("Error after parsing starting node id. Alphanumerical char or linefeed expected");
    }

    return startingNodeID;
}

unsigned int parseNumSteps() {
    // handle errors
    if(getchar() != 'I') throwError("Error when parsing number of steps. 'I' expected");
    if (getchar() != ':') throwError("Error when parsing number of steps. ':' after 'I' expected");

    unsigned int numberSteps = parseValue();

    if (getchar() != EOF) {
        throwError("Error when parsing number of steps. End of file expected");
    }

    return numberSteps;
}

// -------------------------------------------------------------

// run each line of stdin
int scanContents() {
    bool finished = false;
    while (!finished) {
        Node currNode = parseLeftSide();

        // parse NodeID of starting node
        if (strcmp(*currNode.id, "A") == 0) {
            NodeID startingNodeID = parseStartingNodeID();
            printf("Starting NodeID: %s\n", startingNodeID);
            unsigned int numSteps = parseNumSteps();
            printf("Number of steps: %u\n", numSteps);

            //free(startingNodeID);
            finished = true;
        } else {
            printf("ID: %s\n", *currNode.id);
            parseRightSide(&currNode);
        }

    }

    return 0;
}

int init() {
    initNodeList(&nodelist, 10);

    return 0;
}*/

int main() {
    /*
    init();
    scanContents();*/
    NodeID testID1;
    Node testNode;

    createNewNodeList(&nodelist, 5);
    parseLeftSide(&testNode, &testID1);

    /*
    NodeID testID1;
    NodeID testID2;
    Node testNode1;
    Node testNode2;
    createNewID(&testID1, 10);
    createNewID(&testID2, 10);

    char c = 0x61;
    for (int i = 0; i < 1000; i++) {
        addCharToNodeID(&testID1, (char) (c + i % 26));
        addCharToNodeID(&testID2, (char) (c + (i*2) % 26));
    }
    addCharToNodeID(&testID1, '\0');
    addCharToNodeID(&testID2, '\0');
    createNewNode(&testNode1, &testID1);
    createNewNode(&testNode2, &testID2);

    createNewNodeList(&nodelist, 10);
    addNodeToNodeList(&nodelist, &testNode1);
    addNodeToNodeList(&nodelist, &testNode2);
*/
    freeEverything();
    return 0;
}
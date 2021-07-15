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

// TODO: In ParseRightSide, search if id is in nodelist, if yes, put pointer there (freeing will be problem prob)
NodeList *nodelist;
NodeID *startingNodeID;
unsigned int numOfSteps;

// -------------------------------------------------------------
void freeNodeList(NodeList *list);

void freeNodeID(NodeID *id) {
    free(id->value);
    free(id);
}

void freeNode(Node *node) {
    freeNodeID(node->id);
    if (node->neighbours != NULL) {
        freeNodeList(node->neighbours);
    }
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
    freeEverything();
    exit(EXIT_FAILURE);
}

// -------------------------------------------------------------

// allocates memory for a new NodeID, returns if program runs without errors
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

// returns the pointer of element in nodelist if it has the same ID, else returns NULL
Node* getPointerToIDInList(NodeList *list, NodeID *nodeId) {
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
unsigned int getIndexForInsertion(NodeList *list, Node *node, int left, int right) {

    // if there is only one space to search for
    if (left >= right) {
        if (strcmp(node->id->value, list->nodes[left]->id->value) > 0) return left + 1;
        else return left;
    }

    unsigned int middle = (left + right) / 2;
    int cmpVal = strcmp(node->id->value, list->nodes[middle]->id->value);

    if (cmpVal == 0) return middle + 1;
    else if (cmpVal > 0) return getIndexForInsertion(list, node, (int) (middle + 1), right);
    else return getIndexForInsertion(list, node, left, (int) (middle - 1));
}

// adds a node into a node list at position index
bool addNodeInsideOfNodeList(NodeList *list, Node *node, unsigned int index) {
    if (list->len == list->size) {
        list->size *= 2;

        Node **temp = realloc(list->nodes, sizeof(Node*) * list->size);
        if (temp == NULL) return false;
        else list->nodes = temp;
    }

    for (int i = (int) list->len - 1; i >= (int) index; i--) {
        list->nodes[i + 1] = list->nodes[i];
    }

    //unsigned int numToShift = list->len - index;
    //memmove(list->nodes[index + 1], list->nodes[index], sizeof(Node*) * numToShift);
    list->nodes[index] = node;
    list->len++;

    return true;
}

// adds a pointer to a node to a nodeList
bool addNodeToNodeList(NodeList *list, Node *node) {
    if (list->len == list->size) {
        list->size *= 2;

        Node **temp = realloc(list->nodes, sizeof(Node*) * list->size);
        if (temp == NULL) return false;
        else list->nodes = temp;
    }

    if (list->len == 0) list->nodes[list->len++] = node;
    else {
        unsigned int index = getIndexForInsertion(list, node, 0, (int) list->len - 1);
        return addNodeInsideOfNodeList(list, node, index);
    }

    return true;
}

void printNodeList(NodeList *list) {
    for (unsigned int i = 0; i < list->len; i++) {
        if (list != nodelist) printf("\tID %u: %s\n", i + 1, list->nodes[i]->id->value);
        else {
            printf("ID %u: %s - %u\n", i + 1, list->nodes[i]->id->value, list->nodes[i]->value);
            printNodeList(list->nodes[i]->neighbours);
        }
    }
}

// -------------------------------------------------------------

// reads a number string from stdin, converts and returns it
unsigned int parseValue() {
    char valueStr[11];
    unsigned long int value;

    // parse number as a string
    if(scanf("%11[0-9]", valueStr) != 1) throwError("Error when parsing value. Unsigned int expected");

    if (getchar() != '\n') throwError("Error when parsing value. Too long ID (0 <= ID <= 2^32-1) or no Linefeed after ID");

    // convert it to a long unsigned int
    value = strtol(valueStr, NULL, 10);

    // value has to be in bounds
    if (value > 4294967296 - 1) throwError("Number of steps has to be between 0 and 2^32-1");

    return (unsigned int) value;
}

// gives a node its ID, returns either pointer to Node or NULL if no node will be needed
Node* parseLeftSide() {
    char currChar;

    NodeID *id = createNewID();
    if (id == NULL) throwError("Error allocating memory for new NodeID");

    Node *node = createNewNode();
    if (node == NULL) {
        freeNodeID(id);
        throwError("Error when allocating memory for id or node");
    }

    // add the id pointer to node
    addIDToNode(node, id);

    // check if the list of new nodes is finished
    currChar = (char) getchar();
    if (currChar == 'A') {
        if (getchar() != ':') {
            freeNodeID(id);
            free(node); // cant use freeNode, cause id is not allocated
            throwError("Error when parsing starting node. Colon after 'A' expected");
        }

        // free the node cause it is not needed
        freeNode(node);

        return NULL;
    }
    // parse ID
    else {
        while ((currChar >= 'a' && currChar <= 'z') ||
               (currChar >= '0' && currChar <= '9')) {

            if(!addCharToNodeID(id, currChar)) {
                freeNode(node);
                throwError("Couldn't allocate memory for more letters in NodeID");
            }

            currChar = (char) getchar();
        }
        if (!addCharToNodeID(id, '\0')) {
            freeNode(node);
            throwError("Couldn't allocate memory for string terminator in NodeID");
        }

        // check if the ID is acceptable and ends with a colon
        if (id->len == 0) {
            freeNode(node);
            throwError("Error when parsing left side. Invalid ID");
        } else if (currChar != ':') {
            freeNode(node);
            throwError("Error when parsing left side. Colon or alphanumerical value after NodeID expected");
        }

        // check if id is already taken
        if (getPointerToIDInList(nodelist, id) != NULL) {
            freeNode(node);
            throwError("ID is already in used");
        }

        // add the node to nodelist
        if(!addNodeToNodeList(nodelist, node)) {
            freeNode(node);
            throwError("Couldn't add node to nodelist");
        }

        return node;
    }
}

void parseRightSide(Node *leftSideNode) {
    char currChar;
    NodeID *id;
    Node *node;

    NodeList *list = createNewNodeList();
    if (list == NULL) throwError("Couldn't create a NodeList while parsing right side");

    // parse every id while there is a comma after it
    do {
        id = createNewID();
        if (id == NULL) {
            freeNodeList(list);
            throwError("Couldn't create a NodeID while parsing right side");
            exit(-1); // unnecessary
        }

        node = createNewNode();
        if (node == NULL) {
            freeNodeID(id);
            freeNodeList(list);
            throwError("Couldn't create a Node while parsing right side");
        }

        // add id to node
        addIDToNode(node, id);

        // add every allowed char to id
        currChar = (char) getchar();
        while ((currChar >= 'a' && currChar <= 'z') ||
               (currChar >= '0' && currChar <= '9')) {

            if (!addCharToNodeID(id, currChar)) {
                freeNode(node);
                freeNodeList(list);
                throwError("Couldn't allocate memory for more letters in neighbour NodeID");
            }

            currChar = (char) getchar();
        }
        if (!addCharToNodeID(id, '\0')) {
            freeNode(node);
            freeNodeList(list);
            throwError("Couldn't allocate memory for string terminator in neighbour NodeID");
        }

        // check if id is valid, empty right side with value redefinition is allowed
        if (id->len == 0) {
            freeNode(node);

            if (!(list->len == 0 && currChar == '-')) {
                freeNodeList(list);
                throwError("Error while parsing right side. Invalid ID");
            }
        } else {
            // add id pointer to list
            if (!addNodeToNodeList(list, node)) {
                freeNode(node);
                freeNodeList(list);
                throwError("Couldn't add node to neighbour nodeList");
            }
        }
    } while (currChar == ',');

    // change value of left hand side node
    if (currChar == '-') leftSideNode->value = parseValue();
    // disallow empty lists without change of value
    else if (list->len == 0) {
        freeNodeList(list);
        throwError("Right side has to have at least one node or a different starting value");
    } else if (currChar != '\n') {
        freeNodeList(list);
        throwError("Error when parsing right side. Comma, dash or linefeed expected");
    }

    // set nodelist as neighbour IDList for leftSideNode
    leftSideNode->neighbours = list;
}

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

void parseNumSteps() {
    // handle errors
    if(getchar() != 'I') throwError("Error when parsing number of steps. 'I' expected");
    if (getchar() != ':') throwError("Error when parsing number of steps. ':' after 'I' expected");

    numOfSteps = parseValue();

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
    printf("StartingNode: %s\n", startingNodeID->value);
    parseNumSteps();
    printf("Number of steps: %u\n", numOfSteps);
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

int main() {
    init();
    scanContents();
    printNodeList(nodelist);
    freeEverything();

    return 0;
}
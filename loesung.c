#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef char* ID;

typedef struct {
    ID value;
    unsigned int len;
    unsigned int size;
} NodeID;

typedef struct {
    NodeID** IDs;
    unsigned int len;
    unsigned int size;
} NodeIDList;

typedef struct {
    NodeID *id;
    NodeIDList *neighbourIDs;
    unsigned int value;
} Node;

typedef struct {
    Node** nodes;
    unsigned int len;
    unsigned int size;
}NodeList;

// -------------------------------------------------------------

NodeList *nodelist;
NodeID *startingNodeID;
unsigned int numOfSteps;

// -------------------------------------------------------------

void freeNodeID(NodeID *id) {
    free(id->value);
    free(id);
}

void freeNodeIDList(NodeIDList *IDList) {
    for (unsigned int i = 0; i < IDList->len; i++) {
        freeNodeID(IDList->IDs[i]);
    }
    free(IDList->IDs);
    free(IDList);
}

void freeNode(Node *node) {
    freeNodeID(node->id);
    if (node->neighbourIDs != NULL) {
        freeNodeIDList(node->neighbourIDs);
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

// helper function for qsort
int compareIDs(const void *lp, const void *rp) {
    Node *leftNode = (Node*) lp;
    Node *rightNode = (Node*) rp;

    return strcmp(leftNode->id->value, rightNode->id->value);
}

NodeIDList* createNewIDList() {
    NodeIDList *list = malloc(sizeof(NodeIDList));
    if (list == NULL) return NULL;

    list->size = 5;
    list->len = 0;

    list->IDs = malloc(sizeof(NodeID) * list->size);
    if (list->IDs == NULL) {
        free(list);
        return NULL;
    }

    return list;
}

bool addIDToIDList(NodeIDList *list, NodeID *id) {
    if (list->len == list->size) {
        list->size *= 2;

        NodeID **temp = realloc(list->IDs, sizeof(NodeID) * list->size);
        if (temp == NULL) return false;
        else list->IDs = temp;
    }

    list->IDs[list->len++] = id;

    return true;
}

// creates a new node with a pointer to an ID
Node* createNewNode() {
    Node *node = malloc(sizeof(Node));
    if (node == NULL) return NULL;

    node->id = NULL;
    node->value = 0;
    node->neighbourIDs = NULL;

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

        Node **temp = realloc(list->nodes, sizeof(Node) * list->size);
        if (temp == NULL) return false;
        else list->nodes = temp;
    }

    list->nodes[list->len++] = node;

    return true;
}

// sorts a nodelist
void sortNodeList(NodeList *list) {
    qsort(list->nodes, list->len, sizeof(Node), compareIDs);
}

void printNodeList(NodeList *list) {
    for (unsigned int i = 0; i < list->len; i++) {
        printf("ID %u: %s\n", i + 1, list->nodes[i]->id->value);
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

// gives a node its ID, returns whether there are more left sides to be parsed
bool parseLeftSide() {
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

        return false;
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

        // add the node to nodelist
        if(!addNodeToNodeList(nodelist, node)) {
            freeNode(node);
            throwError("Couldn't add node to nodelist");
        }

        return true;
    }
}

void parseRightSide(Node *leftSideNode) {
    NodeID *id;
    NodeIDList *IDList = createNewIDList();
    char currChar;

    // parse every id while there is a comma after it
    do {
        id = createNewID();

        // add id pointer to IDList
        if (!addIDToIDList(IDList, id)) {
            freeNodeID(id);
            freeNodeIDList(IDList);
            throwError("Couldn't add node to neighbour nodeList");
        }

        // add every allowed char to id
        currChar = (char) getchar();
        while ((currChar >= 'a' && currChar <= 'z') ||
               (currChar >= '0' && currChar <= '9')) {

            if (!addCharToNodeID(id, currChar)) {
                freeNodeIDList(IDList);
                throwError("Couldn't allocate memory for more letters in neighbour NodeID");
            }

            currChar = (char) getchar();
        }
        if (!addCharToNodeID(id, '\0')) {
            freeNodeIDList(IDList);
            throwError("Couldn't allocate memory for string terminator in neighbour NodeID");
        }

        // check if id is valid, empty right side with value redefinition is allowed
        if (id->len == 0 && !(IDList->len == 0 && currChar == '-')) {
            freeNodeIDList(IDList);
            throwError("Error while parsing right side. Invalid ID");
        }

    } while (currChar == ',');

    // change value of left hand side node
    if (currChar == '-') leftSideNode->value = parseValue();
    // disallow empty lists without change of value
    else if (IDList->len == 0) {
        freeNodeIDList(IDList);
        throwError("Right side has to have at least one node or a different starting value");
    } else if (currChar != '\n') {
        freeNodeIDList(IDList);
        throwError("Error when parsing right side. Comma, dash or linefeed expected");
    }

    // set nodelist as neighbour IDList for leftSideNode
    leftSideNode->neighbourIDs = IDList;
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
    int i = 0;
    bool stillNodesToBeParsed = true;
    while (stillNodesToBeParsed) {
        stillNodesToBeParsed = parseLeftSide();
        if (stillNodesToBeParsed) {
            printf("ID: %s\n", nodelist->nodes[i++]->id->value);

            // add neighbourIDs of recently added Node
            Node *currNode = nodelist->nodes[(nodelist->len)-1];
            parseRightSide(currNode);
            //printf("Neighbours: ");
            for (unsigned int k = 0; k < currNode->neighbourIDs->len; k++) {
                //printf("%s, ", currNode->neighbourIDs->IDs[k]->value);
            }
            //printf("\n");
        }
    }

    // parse the ID of the starting node and the number of steps
    parseStartingNodeID();
    printf("StartingNode: %s\n", startingNodeID->value);
    parseNumSteps();
    printf("Number of steps: %u\n", numOfSteps);
}

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
    //sortNodeList(nodelist);
    //printNodeList(nodelist);
    freeEverything();

    return 0;
}
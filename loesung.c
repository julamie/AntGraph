#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef char* NodeID;
typedef struct Node Node;
typedef struct NodeList NodeList;

// -------------------------------------------------------------

struct NodeList {
    unsigned int size;
    unsigned int len;
    Node* nodes;
};

struct Node {
    NodeID id;
    unsigned int value;
    NodeID *neighbours;
    unsigned int numNeighbours;
};

struct NodeList nodelist;

// -------------------------------------------------------------

void throwError(char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

// -------------------------------------------------------------

// allocates memory to a new NodeID
NodeID createNewID(unsigned int size) {
    NodeID id = NULL;

    NodeID temp = realloc(NULL, sizeof(*id) * size);
    if (temp == NULL) throwError("Error allocating memory for new NodeID");
    else id = temp;

    return id;
}

Node createNewNode(NodeID id) {
    Node node;

    node.id = id;
    node.value = 0;
    node.neighbours = NULL;
    node.numNeighbours = 0;

    return node;
}

NodeList createNewNodeList(unsigned int size) {
    NodeList list;

    list.size = size;
    list.len = 0;

    Node* temp = malloc(sizeof(Node) * nodelist.size);
    if (temp == NULL) throwError("Error when allocating memory for new NodeList");
    else list.nodes = temp;

    return list;
}

// adds a char to an existing NodeID and keeps enough space, returns whether new memory has been allocated
void addCharToNodeID(NodeID id, char c, unsigned int size, unsigned int length) {
    id[length++] = c;

    // increase size of id if necessary
    if (length == size) {
        size = size * 2;

        NodeID temp = realloc(id, sizeof(*id) * size);
        if (temp == NULL) throwError("Error when allocating more memory for NodeID");
        else id = temp;
    }
}

// doubles the size of an array of nodes
void increaseSizeOfList(Node* list, unsigned int size) {
    Node* temp = realloc(list, sizeof(*list) * size * 2);
    if (temp == NULL) throwError("Error when increasing size of list");
    else list = temp;
}

void addToList(NodeList* list, Node node) {
    list->nodes[nodelist.len] = node;
    list->len++;

    if (list->len == list->size) increaseSizeOfList(list->nodes, list->size);
}

// -------------------------------------------------------------

// reads an unsigned int from stdin and returns it
unsigned int parseValue() {
    unsigned int value = 5;

    if(scanf("%u", &value) != 1) throwError("Error when parsing value. Unsigned int expected");
    if (getchar() != '\n') throwError("Error when parsing value. Linefeed after value expected");

    return value;
}

Node parseLeftSide() {
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

        free(id);
        node = createNewNode("A");
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
    node = createNewNode(id);
    addToList(&nodelist, node);

    return node;
}

NodeList parseRightSide(Node* leftSideNode) {
    NodeList list = createNewNodeList(5);
    NodeID id;
    char currChar;
    unsigned int size;
    unsigned int len;

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
            Node newNode = createNewNode(id);
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
        if (strcmp(currNode.id, "A") == 0) {
            NodeID startingNodeID = parseStartingNodeID();
            printf("Starting NodeID: %s\n", startingNodeID);
            unsigned int numSteps = parseNumSteps();
            printf("Number of steps: %u\n", numSteps);

            //free(startingNodeID);
            finished = true;
        } else {
            printf("ID: %s\n", currNode.id);
            parseRightSide(&currNode);
        }
    }

    return 0;
}

int init() {
    nodelist = createNewNodeList(10);

    return 0;
}

int main() {
    init();
    scanContents();

    return 0;
}
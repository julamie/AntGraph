#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef char* NodeID;

struct NodeList {
    unsigned int size;
    unsigned int len;
    struct Node *nodes;
};

struct Node {
    NodeID *id;
    unsigned int value;
    struct Node *neighbours;
    unsigned int numNeighbours;
};

struct NodeList nodelist;

int init() {
    nodelist.size = 10;
    nodelist.len = 0;
    nodelist.nodes = malloc(sizeof(struct Node) * nodelist.size);

    return 0;
}

void throwError(char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

unsigned int parseValue() {
    unsigned int value = 5;

    if(scanf("%u", &value) != 1) throwError("Error when parsing value. Unsigned int expected");
    if (getchar() != '\n') throwError("Error when parsing value. Linefeed after value expected");

    return value;
}

// allocates memory to a new NodeID
NodeID createNewID(unsigned int size) {
    NodeID id;

    id = realloc(NULL, sizeof(*id) * size);
    if (id == NULL) throwError("Error allocating memory for new NodeID");

    return id;
}

bool spaceIsFull(unsigned int len, unsigned int size) {
    return len == size;
}

// adds a char to an existing NodeID and keeps enough space, returns whether new memory has been allocated
NodeID addCharToNodeID(NodeID id, char c, unsigned int size, unsigned int length) {
    id[length++] = c;

    // increase size of id if necessary
    if (length == size) {
        size = size * 2;
        id = realloc(id, sizeof(*id) * size);
        if (id == NULL) throwError("Error when allocating more memory for NodeID");
    }

    return id;
}

NodeID parseLeftSide() {
    NodeID id;
    char currChar;
    unsigned int size = 10;
    unsigned int len = 0;

    // make space for id
    id = createNewID(size);

    // add every allowed char to id
    currChar = getchar();
    while ((currChar >= 'a' && currChar <= 'z') ||
           (currChar >= '0' && currChar <= '9')) {

        id = addCharToNodeID(id, currChar, size, len++);
        if (spaceIsFull(len, size)) size = size * 2;

        currChar = getchar();
    }

    // end the string and clip it behind the '\0'
    id[len++] = '\0';
    id = realloc(id, sizeof(*id) * len);

    // check if the list of new nodes is finished
    if (currChar == 'A') {
        if (getchar() != ':') throwError("Error when parsing starting node. Colon after 'A' expected");

        free(id);
        return "A";
    } else if (len == 0) {
        throwError("Error when parsing left side. No empty IDs allowed");
    }
    else if (currChar != ':') {
        throwError("Error when parsing left side. Colon or alphanumerical value after NodeID expected");
    }

    return id;
}

void parseRightSide() {
    char* id;
    char currChar;
    unsigned int size = 10;
    unsigned int len = 0;

    // make space for id
    id = createNewID(size);

    // add every allowed char to id
    currChar = getchar();
    while ((currChar >= 'a' && currChar <= 'z') ||
           (currChar >= '0' && currChar <= '9')) {

        id = addCharToNodeID(id, currChar, size, len++);
        if (spaceIsFull(len, size)) size = size * 2;

        currChar = getchar();
    }

    // end the string and clip it behind the '\0'
    id[len++] = '\0';
    id = realloc(id, sizeof(*id) * len);

    if (len != 0) {
        printf("Rest: %s\n", id);
    }

    // continue parsing
    if (currChar == ',') {
        parseRightSide();
    } else if (currChar == '-') {
        printf("Value: %d\n", parseValue());
    } else if (len == 0) {
        throwError("Error while parsing right side. No empty IDs allowed");
    } else if (currChar != '\n') {
        throwError("Error when parsing right side. Comma, dash or linefeed expected");
    }

    free(id);
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

        startingNodeID = addCharToNodeID(startingNodeID, currChar, size, len++);
        if (spaceIsFull(len, size)) size = size * 2;

        currChar = getchar();
    }

    // end the string and clip it behind the '\0'
    startingNodeID[len++] = '\0';
    startingNodeID = realloc(startingNodeID, sizeof(*startingNodeID) * len);

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

// run each line of stdin
int scanContents() {
    bool finished = false;
    while (!finished) {
        NodeID currNodeID = parseLeftSide();

        // parse NodeID of starting node
        if (strcmp(currNodeID, "A") == 0) {
            NodeID startingNodeID = parseStartingNodeID();
            printf("Starting NodeID: %s\n", startingNodeID);
            unsigned int numSteps = parseNumSteps();
            printf("Number of steps: %u\n", numSteps);

            free(startingNodeID);
            finished = true;
        } else {
            printf("ID: %s\n", currNodeID);
            free(currNodeID);
            parseRightSide();
        }
    }

    return 0;
}

int main() {
    scanContents();


}
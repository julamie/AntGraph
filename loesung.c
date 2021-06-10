#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct Node;

struct Node {
    char *id;
    uint32_t value;
    struct Node *neighbours;
    uint32_t numNeighbours;
};

int scanContents() {
    struct Node node;
    node.id = malloc(sizeof(*node.id) * 100);
    char* rest = malloc(sizeof(*rest) * 100);

    while (scanf("%[0-9a-z]", node.id) == 1) {
        printf("NodeID: %s\n", node.id);

        if (getchar() != ':') {
            // TODO: ERROR
        }

        int finished = 0;
        while(!finished) {
            scanf("%[0-9a-z]", rest);
            printf("Rest: %s\n", rest);

            char x = getchar();
            if (x == '\n') {
                finished = 1;
            } else if (x == '-') {
                int value;

                if(scanf("%d", &value) == 1) {
                    // TODO: ERROR
                }

                node.value = value;
                printf("NodeValue: %d\n", node.value);
                if (getchar() != '\n') {
                    // TODO: ERROR
                }

                finished = 1;
            } else if (x != ',') {
                // TODO: ERROR
            }

        }
        printf("\n");
    }

    return 0;
}

int main() {
    scanContents();
}

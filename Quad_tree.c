// Quad Tree Implementation
#include <stdio.h>
#include <stdlib.h>

// Define a Point structure to represent coordinates
typedef struct Point {
    int x;
    int y;
} Point;

// Define a Node structure to hold a point and associated data
typedef struct Node {
    Point pos;
    int data;
} Node;

// Define a Quad structure to represent a quadrant in the Quad Tree
typedef struct Quad {
    Point topLeft;
    Point botRight;
    Node* n;                        // Node in the quadrant
    struct Quad* topLeftTree;       // Top-left quadrant
    struct Quad* topRightTree;      // Top-right quadrant
    struct Quad* botLeftTree;       // Bottom-left quadrant
    struct Quad* botRightTree;      // Bottom-right quadrant
} Quad;

// Create and initialize a new quadrant
Quad* newQuad(Point topLeft, Point botRight) {
    Quad* quad = (Quad*)malloc(sizeof(Quad));
    quad->topLeft = topLeft;
    quad->botRight = botRight;
    quad->n = NULL;
    quad->topLeftTree = quad->topRightTree = quad->botLeftTree = quad->botRightTree = NULL;
    return quad;
}

// Check if a point is within the boundaries of a quadrant
int inBoundary(Point topLeft, Point botRight, Point p) {
    return (p.x >= topLeft.x && p.x <= botRight.x &&
            p.y >= topLeft.y && p.y <= botRight.y);
}

// Insert a node into the Quad Tree
void insert(Quad* root, Node* node) {
    if (node == NULL || !inBoundary(root->topLeft, root->botRight, node->pos))
        return;

    // Base case: if the quadrant is a unit square, store the node here
    if (abs(root->topLeft.x - root->botRight.x) <= 1 &&
        abs(root->topLeft.y - root->botRight.y) <= 1) {
        if (root->n == NULL)
            root->n = node;
        return;
    }

    int xMid = (root->topLeft.x + root->botRight.x) / 2;
    int yMid = (root->topLeft.y + root->botRight.y) / 2;

    // Determine quadrant and insert node recursively
    if (node->pos.x <= xMid) {
        if (node->pos.y <= yMid) {
            if (root->topLeftTree == NULL)
                root->topLeftTree = newQuad(root->topLeft, (Point){xMid, yMid});
            insert(root->topLeftTree, node);
        } else {
            if (root->botLeftTree == NULL)
                root->botLeftTree = newQuad((Point){root->topLeft.x, yMid}, (Point){xMid, root->botRight.y});
            insert(root->botLeftTree, node);
        }
    } else {
        if (node->pos.y <= yMid) {
            if (root->topRightTree == NULL)
                root->topRightTree = newQuad((Point){xMid, root->topLeft.y}, (Point){root->botRight.x, yMid});
            insert(root->topRightTree, node);
        } else {
            if (root->botRightTree == NULL)
                root->botRightTree = newQuad((Point){xMid, yMid}, root->botRight);
            insert(root->botRightTree, node);
        }
    }
}

// Search for a point in the Quad Tree
void search(Quad* root, Point p) {
    if (root == NULL) {
        printf("Not found.\n");
        return;
    }

    // Check if the current quadrant has the desired point
    if (root->n != NULL && root->n->pos.x == p.x && root->n->pos.y == p.y) {
        printf("Found node in square with (%d, %d) as topLeft and (%d, %d) as botRight\n",
               root->topLeft.x, root->topLeft.y, root->botRight.x, root->botRight.y);
        return;
    }

    int xMid = (root->topLeft.x + root->botRight.x) / 2;
    int yMid = (root->topLeft.y + root->botRight.y) / 2;

    // Navigate to the appropriate child quadrant
    if (p.x <= xMid) {
        if (p.y <= yMid) {
            printf("Searching in topLeft quadrant with (%d, %d) as topLeft and (%d, %d) as botRight\n",
                   root->topLeft.x, root->topLeft.y, xMid, yMid);
            search(root->topLeftTree, p);
        } else {
            printf("Searching in botLeft quadrant with (%d, %d) as topLeft and (%d, %d) as botRight\n",
                   root->topLeft.x, yMid, xMid, root->botRight.y);
            search(root->botLeftTree, p);
        }
    } else {
        if (p.y <= yMid) {
            printf("Searching in topRight quadrant with (%d, %d) as topLeft and (%d, %d) as botRight\n",
                   xMid, root->topLeft.y, root->botRight.x, yMid);
            search(root->topRightTree, p);
        } else {
            printf("Searching in botRight quadrant with (%d, %d) as topLeft and (%d, %d) as botRight\n",
                   xMid, yMid, root->botRight.x, root->botRight.y);
            search(root->botRightTree, p);
        }
    }
}

int main() {
    Point topLeft = {0, 0};
    Point botRight = {16, 16}; // Define the boundary of the Quad Tree
    Quad* quad = newQuad(topLeft, botRight);

    int n;
    printf("Enter the number of points to add: ");
    scanf("%d", &n);
    printf("Enter the points (x y):\n");
    while (n--) {
        int x, y;
        scanf("%d %d", &x, &y);
        Node* node = (Node*)malloc(sizeof(Node));
        node->pos = (Point){x, y};
        node->data = 1;
        insert(quad, node);
    }

    int x, y;
    printf("Enter the point to search (x y): ");
    scanf("%d %d", &x, &y);
    Point searchPoint = {x, y};
    search(quad, searchPoint);

    return 0;
}

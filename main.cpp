#include <iostream>
#include <string>
using namespace std;

const int MAX_NODES = 1000;
const int MAX_PATH = 1000;

struct EmployeeData {
    string fullName;
    string department;
    int key;
};

struct Node {
    EmployeeData data;
    Node* left;
    Node* right;
};

int allocated = 0;
int freed = 0;

Node* createNode(int key) {
    Node* p = new Node;
    p->data.key = key;
    p->data.fullName = "Employee_" + to_string(key);
    p->data.department = (key % 2 == 0) ? "IT" : "HR";
    p->left = nullptr;
    p->right = nullptr;
    allocated++;
    return p;
}

bool insertNode(Node*& root, int key, int& comparisons, int& visited) {
    Node** current = &root;

    while (*current != nullptr) {
        visited++;
        comparisons++;

        if (key == (*current)->data.key)
            return false;

        comparisons++;

        if (key < (*current)->data.key)
            current = &((*current)->left);
        else
            current = &((*current)->right);
    }

    *current = createNode(key);
    return true;
}

Node* searchIterative(Node* root, int key, int& comparisons,
    int& visited, int path[], int& pathCount) {
    Node* current = root;
    pathCount = 0;

    while (current != nullptr) {
        visited++;
        path[pathCount++] = current->data.key;

        comparisons++;

        if (key == current->data.key)
            return current;

        comparisons++;

        if (key < current->data.key)
            current = current->left;
        else
            current = current->right;
    }

    return nullptr;
}

Node* searchRecursive(Node* root, int key, int& comparisons,
    int& visited, int path[], int& pathCount) {
    if (root == nullptr)
        return nullptr;

    visited++;
    path[pathCount++] = root->data.key;

    comparisons++;

    if (key == root->data.key)
        return root;

    comparisons++;

    if (key < root->data.key)
        return searchRecursive(root->left, key, comparisons,
            visited, path, pathCount);

    return searchRecursive(root->right, key, comparisons,
        visited, path, pathCount);
}

bool deleteNode(Node*& root, int key, int& comparisons, int& visited) {
    Node** link = &root;

    while (*link != nullptr) {
        visited++;
        comparisons++;

        if (key == (*link)->data.key)
            break;

        comparisons++;

        if (key < (*link)->data.key)
            link = &((*link)->left);
        else
            link = &((*link)->right);
    }

    if (*link == nullptr)
        return false;

    Node* target = *link;

    if (target->left == nullptr || target->right == nullptr) {
        Node* child = (target->left != nullptr)
            ? target->left : target->right;

        *link = child;

        delete target;
        freed++;

        return true;
    }

    Node** successorLink = &(target->right);

    while ((*successorLink)->left != nullptr)
        successorLink = &((*successorLink)->left);

    Node* successor = *successorLink;

    target->data = successor->data;
    *successorLink = successor->right;

    delete successor;
    freed++;

    return true;
}

bool checkStructure(Node* root) {
    if (root == nullptr)
        return true;

    Node* stack[MAX_NODES];
    Node* visited[MAX_NODES];

    int top = 0;
    int visitedCount = 0;

    stack[top++] = root;

    while (top > 0) {
        Node* current = stack[--top];

        for (int i = 0; i < visitedCount; i++) {
            if (visited[i] == current)
                return false;
        }

        if (visitedCount >= MAX_NODES)
            return false;

        visited[visitedCount++] = current;

        if (current->left != nullptr) {
            if (top >= MAX_NODES)
                return false;

            stack[top++] = current->left;
        }

        if (current->right != nullptr) {
            if (top >= MAX_NODES)
                return false;

            stack[top++] = current->right;
        }
    }

    return true;
}

bool validateBST(Node* root, long long low, long long high, bool reportError = false) {
    if (root == nullptr)
        return true;

    if (root->data.key <= low) {
        if (reportError) {
            cout << "BST violation at key " << root->data.key
                << ": lower bound (" << low << ") violated\n";
        }
        return false;
    }

    if (root->data.key >= high) {
        if (reportError) {
            cout << "BST violation at key " << root->data.key
                << ": upper bound (" << high << ") violated\n";
        }
        return false;
    }

    return validateBST(root->left, low, root->data.key, reportError) &&
        validateBST(root->right, root->data.key, high, reportError);
}

void inorder(Node* root, int result[], int& count) {
    if (root == nullptr)
        return;

    inorder(root->left, result, count);

    result[count++] = root->data.key;

    inorder(root->right, result, count);
}

int height(Node* root) {
    if (root == nullptr)
        return 0;

    int a = height(root->left);
    int b = height(root->right);

    return 1 + (a > b ? a : b);
}

int countNodes(Node* root) {
    if (root == nullptr)
        return 0;

    return 1 + countNodes(root->left) + countNodes(root->right);
}

void clearTree(Node* root) {
    if (root == nullptr)
        return;

    clearTree(root->left);
    clearTree(root->right);

    delete root;
    freed++;
}

bool sameArray(int a[], int n, int b[], int m) {
    if (n != m)
        return false;

    for (int i = 0; i < n; i++) {
        if (a[i] != b[i])
            return false;
    }

    return true;
}

void printArray(const char* title, int a[], int n) {
    cout << title;

    if (n == 0)
        cout << "empty";

    for (int i = 0; i < n; i++) {
        if (i > 0)
            cout << ", ";

        cout << a[i];
    }

    cout << '\n';
}

void printOperationStatus(Node* root, const char* stageTitle) {
    int result[MAX_NODES];
    int count = 0;
    inorder(root, result, count);

    printArray(stageTitle, result, count);

    bool bstOK = validateBST(root, -2147483649LL, 2147483648LL, true);
    bool cycleFree = checkStructure(root);
    int nodes = countNodes(root);
    bool memoryConsistent = (allocated - freed) == nodes;

    cout << "Invariants -> BST: " << (bstOK ? "OK" : "ERROR")
        << " | Nodes: " << nodes
        << " | Height: " << height(root)
        << " | Cycles: " << (cycleFree ? "NONE" : "FOUND")
        << " | Memory: " << (memoryConsistent ? "OK" : "ERROR") << '\n';
}

bool runTest(int number, const char* name, int initial[],
    int initialCount, char operation, int key,
    int expected[], int expectedCount, bool expectedFound) {

    int beforeAllocated = allocated;
    int beforeFreed = freed;

    Node* root = nullptr;

    int comparisons = 0;
    int visited = 0;

    bool setupOK = true;

    for (int i = 0; i < initialCount; i++) {
        if (!insertNode(root, initial[i], comparisons, visited))
            setupOK = false;
    }

    bool operationResult = false;

    if (operation == 'I') {
        operationResult = insertNode(root, key, comparisons, visited);
    }
    else if (operation == 'D') {
        operationResult = deleteNode(root, key, comparisons, visited);
    }
    else {
        int path[MAX_PATH];
        int pathCount = 0;

        int recursivePath[MAX_PATH];
        int recursiveCount = 0;

        Node* found = searchIterative(root, key, comparisons,
            visited, path, pathCount);

        int recursiveComparisons = 0;
        int recursiveVisited = 0;

        Node* recursiveFound = searchRecursive(
            root, key, recursiveComparisons, recursiveVisited,
            recursivePath, recursiveCount
        );

        operationResult = (found != nullptr);

        if (found != recursiveFound ||
            !sameArray(path, pathCount, recursivePath, recursiveCount))
            setupOK = false;
    }

    int actual[MAX_NODES];
    int actualCount = 0;

    inorder(root, actual, actualCount);

    bool foundCorrect = (operation == 'S')
        ? (operationResult == expectedFound) : true;

    bool operationCorrect = (operation == 'I' || operation == 'D')
        ? (operationResult == expectedFound) : true;

    bool ok = setupOK && foundCorrect && operationCorrect &&
        sameArray(actual, actualCount, expected, expectedCount) &&
        checkStructure(root) &&
        validateBST(root, -2147483649LL, 2147483648LL);

    cout << "Test " << number << ": " << (ok ? "PASS" : "FAIL")
        << " | " << name
        << " | comparisons: " << comparisons
        << " | visited: " << visited << '\n';

    clearTree(root);

    bool memoryOK = (allocated - beforeAllocated) ==
        (freed - beforeFreed);

    return ok && memoryOK;
}

void runVariant() {
    int keys[] = { 70, 45, 85, 25, 55, 75, 95 };

    Node* root = nullptr;

    int comparisons = 0;
    int visited = 0;

    for (int i = 0; i < 7; i++)
        insertNode(root, keys[i], comparisons, visited);

    cout << "\n=== VARIANT 5: 70, 45, 85, 25, 55, 75, 95 ===\n";
    printOperationStatus(root, "Initial inorder: ");

    // 1. Пошук ключа 55
    int path[MAX_PATH];
    int pathCount = 0;
    int recPath[MAX_PATH];
    int recCount = 0;

    comparisons = 0;
    visited = 0;
    int recComparisons = 0;
    int recVisited = 0;

    Node* found = searchIterative(root, 55, comparisons,
        visited, path, pathCount);
    Node* recFound = searchRecursive(root, 55, recComparisons,
        recVisited, recPath, recCount);

    cout << "\n[Search 55]: " << (found ? "FOUND" : "NOT FOUND")
        << " | Iter == Rec: "
        << ((found == recFound && sameArray(path, pathCount, recPath, recCount)) ? "YES" : "NO") << '\n';
    printArray("Path: ", path, pathCount);
    cout << "Comparisons: " << comparisons << " | Visited: " << visited << '\n';
    printOperationStatus(root, "After search inorder: ");

    // 2. Вставлення ключа 80
    comparisons = 0;
    visited = 0;
    bool inserted = insertNode(root, 80, comparisons, visited);

    cout << "\n[Insert 80]: " << (inserted ? "INSERTED" : "NOT INSERTED")
        << " | Comparisons: " << comparisons << " | Visited: " << visited << '\n';
    printOperationStatus(root, "After insert inorder: ");

    // 3. Видалення ключа 85 (два нащадки)
    comparisons = 0;
    visited = 0;
    bool deleted = deleteNode(root, 85, comparisons, visited);

    cout << "\n[Delete 85]: " << (deleted ? "DELETED" : "NOT FOUND")
        << " | Comparisons: " << comparisons << " | Visited: " << visited << '\n';
    printOperationStatus(root, "After delete inorder: ");

    clearTree(root);
}

void runDeletionCases() {
    cout << "\n=== DELETION CASES & ROOT CHECK ===\n";

    // Випадок 1: видалення листка (25)
    Node* t1 = nullptr;
    int c = 0, v = 0;
    int k1[] = { 70, 45, 85, 25, 55, 75, 95 };
    for (int x : k1) insertNode(t1, x, c, v);
    deleteNode(t1, 25, c, v);
    printOperationStatus(t1, "Case 1 (delete leaf 25) inorder: ");
    clearTree(t1);

    // Випадок 2: видалення вузла з одним нащадком (30 -> 20)
    Node* t2 = nullptr;
    int k2[] = { 50, 30, 70, 20 };
    for (int x : k2) insertNode(t2, x, c, v);
    deleteNode(t2, 30, c, v);
    printOperationStatus(t2, "Case 2 (one child, delete 30) inorder: ");
    clearTree(t2);

    // Випадок 3: видалення вузла з двома нащадками (30 -> 20, 40)
    Node* t3 = nullptr;
    int k3[] = { 50, 30, 70, 20, 40 };
    for (int x : k3) insertNode(t3, x, c, v);
    deleteNode(t3, 30, c, v);
    printOperationStatus(t3, "Case 3 (two children, delete 30) inorder: ");
    clearTree(t3);

    // Випадок 4: видалення кореня з двома нащадками (50)
    Node* t4 = nullptr;
    int k4[] = { 50, 30, 70, 20, 40, 60, 80 };
    for (int x : k4) insertNode(t4, x, c, v);
    deleteNode(t4, 50, c, v);
    printOperationStatus(t4, "Case 4 (delete root 50) inorder: ");
    clearTree(t4);
}

void runShapeExperiment() {
    int balanced[] = { 50, 30, 70, 20, 40, 60, 80 };
    int increasing[] = { 20, 30, 40, 50, 60, 70, 80 };
    int mixed[] = { 50, 20, 70, 40, 60, 30, 80 };

    int* orders[] = { balanced, increasing, mixed };

    const char* names[] = {
        "Almost balanced",
        "Increasing",
        "Mixed"
    };

    cout << "\n=== SHAPE EXPERIMENT ===\n";

    for (int t = 0; t < 3; t++) {
        Node* root = nullptr;

        int comparisons = 0;
        int visited = 0;

        for (int i = 0; i < 7; i++)
            insertNode(root, orders[t][i], comparisons, visited);

        int path[MAX_PATH];
        int pathCount = 0;

        comparisons = 0;
        visited = 0;

        searchIterative(root, 80, comparisons, visited,
            path, pathCount);

        cout << names[t] << " | Height: " << height(root)
            << " | Search 80: " << comparisons
            << " comparisons, " << visited << " visited";

        comparisons = 0;
        visited = 0;

        searchIterative(root, 90, comparisons, visited,
            path, pathCount);

        cout << " | Search 90: " << comparisons
            << " comparisons, " << visited << " visited\n";

        clearTree(root);
    }
}

int main() {
    int e0[] = { 0 };
    int a1[] = { 5 };
    int a2[] = { 5, 3, 7 };
    int a3[] = { 5, 3, 7, 6 };
    int a4[] = { 5, 3, 8, 7, 9 };
    int a5[] = { 1, 2, 3, 4 };
    int a6[] = { 4, 2, 6, 1, 3, 5, 7 };
    int a7[] = { 2, 1 };
    int a8[] = { 2, 3 };
    int a9[] = { 10, 5, 15, 3, 7, 12, 18 };
    int a10[] = { 8, 4, 12, 2, 6, 10, 14 };
    int a11[] = { 9, 5, 13, 3, 7, 11, 15 };
    int a12[] = { 6, 3, 9, 2, 4, 8, 10 };

    int x0[] = { 0 };
    int x1[] = { 5 };
    int x2[] = { 3, 4, 5, 7 };
    int x3[] = { 3, 5, 7 };
    int x4[] = { 5, 7 };
    int x5[] = { 3, 5, 6 };
    int x6[] = { 3, 5, 7, 9 };
    int x7[] = { 3, 7, 8, 9 };
    int x8[] = { 1, 2, 3, 4, 5, 6, 7 };
    int x9[] = { 1, 2, 3, 5, 6, 7 };
    int x10[] = { 3 };
    int x17[] = { 1 };
    int x11[] = { 1, 2, 3, 4, 5, 6, 7 };
    int x13[] = { 3, 5, 7, 12, 15, 18 };
    int x14[] = { 2, 4, 6, 10, 12, 14 };
    int x15[] = { 5, 7, 9, 11, 13, 15 };
    int x16[] = { 2, 3, 4, 8, 9, 10 };

    cout << "=== AUTOMATED TESTS ===\n";

    int passed = 0;
    int total = 20;

    if (runTest(1, "Search in empty tree", e0, 0, 'S', 5,
        x0, 0, false)) passed++;

    if (runTest(2, "Insert into empty tree", e0, 0, 'I', 5,
        x1, 1, true)) passed++;

    if (runTest(3, "Delete the only root", a1, 1, 'D', 5,
        x0, 0, true)) passed++;

    if (runTest(4, "Insert between keys", a2, 3, 'I', 4,
        x2, 4, true)) passed++;

    if (runTest(5, "Reject duplicate key", a2, 3, 'I', 5,
        x3, 3, false)) passed++;

    if (runTest(6, "Delete a leaf", a2, 3, 'D', 3,
        x4, 2, true)) passed++;

    if (runTest(7, "Delete node with one child", a3, 4, 'D', 7,
        x5, 3, true)) passed++;

    if (runTest(8, "Delete node with two children", a4, 5, 'D', 8,
        x6, 4, true)) passed++;

    if (runTest(9, "Delete root with two children", a4, 5, 'D', 5,
        x7, 4, true)) passed++;

    if (runTest(10, "Delete missing key", a2, 3, 'D', 10,
        x3, 3, false)) passed++;

    if (runTest(11, "Search existing key in chain", a5, 4, 'S', 4,
        x8, 4, true)) passed++;

    if (runTest(12, "Search existing key in balanced tree", a6, 7, 'S', 7,
        x11, 7, true)) passed++;

    if (runTest(13, "Delete root with two children", a6, 7, 'D', 4,
        x9, 6, true)) passed++;

    if (runTest(14, "Delete root with left child", a7, 2, 'D', 2,
        x17, 1, true)) passed++;

    if (runTest(15, "Delete root with right child", a8, 2, 'D', 2,
        x10, 1, true)) passed++;

    if (runTest(16, "Delete root, successor has child", a9, 7, 'D', 10,
        x13, 6, true)) passed++;

    if (runTest(17, "Delete root with successor", a10, 7, 'D', 8,
        x14, 6, true)) passed++;

    if (runTest(18, "Delete a node with one child", a11, 7, 'D', 3,
        x15, 6, true)) passed++;

    if (runTest(19, "Delete root and retain subtrees", a12, 7, 'D', 6,
        x16, 6, true)) passed++;

    if (runTest(20, "Search absent key", a6, 7, 'S', 99,
        x11, 7, false)) passed++;

    cout << "Passed: " << passed << "/" << total << '\n';

    runVariant();
    runDeletionCases();
    runShapeExperiment();

    cout << "\n=== MEMORY CHECK ===\n";

    cout << "Allocated: " << allocated << '\n';
    cout << "Freed: " << freed << '\n';
    cout << "Active: " << allocated - freed << '\n';

    cout << "Memory balance: "
        << (allocated == freed ? "OK" : "ERROR") << '\n';

    return (passed == total && allocated == freed) ? 0 : 1;
}

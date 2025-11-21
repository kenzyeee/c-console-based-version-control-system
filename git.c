#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CONTENT 1024
#define MAX_LOG 256
#define MAX_VERSIONS 10

// ===== VERSION NODE STRUCTURE =====
typedef struct VersionNode {
    int version_id;
    char *delta; // Stores the differences from previous version
    char *change_log; // Description of changes
    struct VersionNode *next;
    struct VersionNode *prev;
} VersionNode;

// ===== VERSION CONTROL SYSTEM STRUCTURE =====
typedef struct {
    VersionNode *head; // Oldest version
    VersionNode *tail; // Latest version
    char current_content[MAX_CONTENT];
    int version_count;
    int next_version_id;
    int max_versions;
} VCS;

// ===== FUNCTION PROTOTYPES =====
VCS* initializeVCS(int max_versions);
void addVersion(VCS *vcs, char *new_content, char *log);
char* computeDelta(char *old_content, char *new_content);
char* reconstructVersion(VCS *vcs, int version_id);
VersionNode* findVersionNode(VCS *vcs, int version_id);
void displayAllLogs(VCS *vcs);
void freeVCS(VCS *vcs);
void consoleMenu(VCS *vcs);
void displayCurrentVersion(VCS *vcs);
void displayVersion(VCS *vcs, int version_id);

// ===== INITIALIZE VCS =====
VCS* initializeVCS(int max_versions) {
    VCS *vcs = (VCS *)malloc(sizeof(VCS));
    if (vcs == NULL) {
        printf("Memory allocation failed for VCS\n");
        exit(1);
    }
    vcs->head = NULL;
    vcs->tail = NULL;
    vcs->version_count = 0;
    vcs->next_version_id = 1;
    vcs->max_versions = max_versions;
    strcpy(vcs->current_content, "");
    return vcs;
}

// ===== COMPUTE DELTA (Simple line-by-line difference) =====
char* computeDelta(char *old_content, char *new_content) {
    char *delta = (char *)malloc(MAX_CONTENT);
    if (delta == NULL) {
        printf("Memory allocation failed for delta\n");
        exit(1);
    }
    // Simple delta: if content changed, mark with "MODIFIED"
    if (strcmp(old_content, new_content) != 0) {
        sprintf(delta, "MODIFIED: %s -> %s", old_content, new_content);
    } else {
        strcpy(delta, "NO_CHANGE");
    }
    return delta;
}

// ===== CREATE NEW VERSION NODE =====
VersionNode* createVersionNode(int id, char *delta, char *log) {
    VersionNode *new_node = (VersionNode *)malloc(sizeof(VersionNode));
    if (new_node == NULL) {
        printf("Memory allocation failed for version node\n");
        exit(1);
    }
    new_node->version_id = id;
    new_node->delta = (char *)malloc(strlen(delta) + 1);
    if (new_node->delta == NULL) {
        printf("Memory allocation failed for delta\n");
        exit(1);
    }
    strcpy(new_node->delta, delta);

    new_node->change_log = (char *)malloc(strlen(log) + 1);
    if (new_node->change_log == NULL) {
        printf("Memory allocation failed for change log\n");
        exit(1);
    }
    strcpy(new_node->change_log, log);

    new_node->next = NULL;
    new_node->prev = NULL;

    return new_node;
}

// ===== ADD NEW VERSION =====
void addVersion(VCS *vcs, char *new_content, char *log) {
    // Compute delta with current content
    char *delta = computeDelta(vcs->current_content, new_content);
    // Create new version node
    VersionNode *new_node = createVersionNode(vcs->next_version_id, delta, log);

    // Insert at the end (tail)
    if (vcs->tail == NULL) {
        vcs->head = new_node;
        vcs->tail = new_node;
    } else {
        new_node->prev = vcs->tail;
        vcs->tail->next = new_node;
        vcs->tail = new_node;
    }
    vcs->version_count++;
    vcs->next_version_id++;

    // Update current content
    strcpy(vcs->current_content, new_content);

    // Enforce version limit (remove oldest if exceeded)
    if (vcs->version_count > vcs->max_versions) {
        VersionNode *oldest = vcs->head;
        vcs->head = oldest->next;
        if (vcs->head != NULL) {
            vcs->head->prev = NULL;
        }
        free(oldest->delta);
        free(oldest->change_log);
        free(oldest);
        vcs->version_count--;
    }

    free(delta); // Delta was copied to node, so free original
    printf("\n Version %d created successfully!\n", new_node->version_id);
}

// ===== FIND VERSION NODE BY ID =====
VersionNode* findVersionNode(VCS *vcs, int version_id) {
    VersionNode *temp = vcs->head;
    while (temp != NULL) {
        if (temp->version_id == version_id) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

// ===== RECONSTRUCT VERSION CONTENT =====
char* reconstructVersion(VCS *vcs, int version_id) {
    VersionNode *node = findVersionNode(vcs, version_id);
    if (node == NULL) {
        return NULL;
    }
    // Build content by applying deltas from head to target node
    char *reconstructed = (char *)malloc(MAX_CONTENT);
    strcpy(reconstructed, "");
    VersionNode *temp = vcs->head;
    while (temp != NULL && temp->version_id <= version_id) {
        if (strstr(temp->delta, "MODIFIED") != NULL) {
            // Extract the "new" content from delta
            char *start = strchr(temp->delta, '>');
            if (start != NULL) {
                strcpy(reconstructed, start + 2); // Skip "-> "
            }
        }
        if (temp->version_id == version_id) break;
        temp = temp->next;
    }
    return reconstructed;
}

// ===== DISPLAY ALL VERSION LOGS =====
void displayAllLogs(VCS *vcs) {
    if (vcs->version_count == 0) {
        printf("\n! No versions found.\n");
        return;
    }
    printf("\n          VERSION HISTORY\n");
    VersionNode *temp = vcs->head;
    while (temp != NULL) {
        printf("\n[Version %d]\n", temp->version_id);
        printf("  Log: %s\n", temp->change_log);
        printf("  Delta: %s\n", temp->delta);
        temp = temp->next;
    }
    printf("\n\n");
}

// ===== DISPLAY SPECIFIC VERSION =====
void displayVersion(VCS *vcs, int version_id) {
    char *content = reconstructVersion(vcs, version_id);
    if (content == NULL) {
        printf("\n! Version %d not found.\n", version_id);
        return;
    }
    printf("\n           VERSION %d CONTENT\n", version_id);
    printf("%s\n", content);
    printf("\n");
    free(content);
}

// ===== DISPLAY CURRENT VERSION =====
void displayCurrentVersion(VCS *vcs) {
    printf("\n              CURRENT VERSION \n");
    printf("Content: %s\n", vcs->current_content);
    printf("Total Versions: %d\n", vcs->version_count);
    printf("\n");
}

// ===== CONSOLE MENU =====
void consoleMenu(VCS *vcs) {
    int choice;
    char new_content[MAX_CONTENT];
    char log_message[MAX_LOG];
    int version_id;
    while (1) {
        printf("\n            VERSION CONTROL SYSTEM    \n");
        printf("1. Add New Version\n");
        printf("2. View Specific Version\n");
        printf("3. View Current Version\n");
        printf("4. Display All Version Logs\n");
        printf("5. Exit\n");
        printf("=====================================\n");
        printf("Enter your choice (1-5): ");
        scanf("%d", &choice);
        getchar(); // Consume newline
        switch (choice) {
            case 1:
                printf("\nEnter new file content: ");
                fgets(new_content, MAX_CONTENT, stdin);
                new_content[strcspn(new_content, "\n")] = '\0'; // Remove newline
                printf("Enter change log description: ");
                fgets(log_message, MAX_LOG, stdin);
                log_message[strcspn(log_message, "\n")] = '\0'; // Remove newline
                addVersion(vcs, new_content, log_message);
                break;
            case 2:
                printf("\nEnter version ID to retrieve: ");
                scanf("%d", &version_id);
                getchar(); // Consume newline
                displayVersion(vcs, version_id);
                break;
            case 3:
                displayCurrentVersion(vcs);
                break;
            case 4:
                displayAllLogs(vcs);
                break;
            case 5:
                printf("\n Exiting Version Control System.\n");
                freeVCS(vcs);
                exit(0);
            default:
                printf("\n! Invalid choice. Please enter 1-5.\n");
        }
    }
}

// ===== FREE ALL MEMORY =====
void freeVCS(VCS *vcs) {
    VersionNode *current = vcs->head;
    while (current != NULL) {
        VersionNode *temp = current;
        current = current->next;
        free(temp->delta);
        free(temp->change_log);
        free(temp);
    }
    free(vcs);
}

// ===== MAIN FUNCTION =====
int main() {
    printf("========== WELCOME TO VERSION CONTROL SYSTEM ==========\n");
    printf("Simplified Git-like System for File Management\n");
    printf("=======================================================\n\n");
    // Initialize VCS with max 10 versions
    VCS *vcs = initializeVCS(MAX_VERSIONS);
    printf(" Version Control System initialized with max %d versions.\n", MAX_VERSIONS);
    printf(" Enter your first version content below.\n\n");
    // Add initial version
    char initial_content[MAX_CONTENT];
    printf("Enter initial file content: ");
    fgets(initial_content, MAX_CONTENT, stdin);
    initial_content[strcspn(initial_content, "\n")] = '\0';
    strcpy(vcs->current_content, initial_content);
    vcs->version_count = 0; // Don't count initial as version yet
    // Start menu
    consoleMenu(vcs);
    return 0;
}
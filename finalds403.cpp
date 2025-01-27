#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

// کلاس زیرتسک
class Subtask {
public:
    string name;
    string description;
    int deadline;
    string status;
    Subtask* next;

    Subtask(string n, string d, int dl, string s)
        : name(n), description(d), deadline(dl), status(s), next(nullptr) {}

    // چاپ اطلاعات زیرتسک
    void printSubtaskDetails() const {
        cout << "    Subtask Name: " << name << "\n";
        cout << "    Description: " << description << "\n";
        cout << "    Deadline: " << deadline << " days\n";
        cout << "    Status: " << status << "\n";
    }
};

// کلاس تسک
class Task {
public:
    string name;
    string description;
    int deadline;
    string status;
    Task* left;
    Task* right;
    Subtask* subtasks;

    Task(string n, string d, int dl, string s)
        : name(n), description(d), deadline(dl), status(s), left(nullptr), right(nullptr), subtasks(nullptr) {}

    // چاپ اطلاعات تسک
    void printTaskDetails() const {
        cout << "Task Name: " << name << "\n";
        cout << "Description: " << description << "\n";
        cout << "Deadline: " << deadline << " days\n";
        cout << "Status: " << status << "\n";
    }

    // نمایش زیرتسک‌ها با جزییات بیشتر
    void printSubtasks() const {
        Subtask* current = subtasks;
        while (current) {
            current->printSubtaskDetails();
            current = current->next;
        }
    }
};

// پیاده‌سازی صف اولویت به صورت دستی
class PriorityQueue {
private:
    struct Node {
        Task* task;
        Node* next;
        Node(Task* t) : task(t), next(nullptr) {}
    };
    Node* front;

public:
    PriorityQueue() : front(nullptr) {}

    // افزودن تسک به صف اولویت به ترتیب ددلاین
    void enqueue(Task* task) {
        Node* newNode = new Node(task);
        if (!front || task->deadline < front->task->deadline) {
            newNode->next = front;
            front = newNode;
        } else {
            Node* temp = front;
            while (temp->next && temp->next->task->deadline <= task->deadline) {
                temp = temp->next;
            }
            newNode->next = temp->next;
            temp->next = newNode;
        }
    }

    // حذف تسک با اولویت بالا (اولین تسک)
    Task* dequeue() {
        if (!front) {
            throw runtime_error("Queue is empty. Cannot dequeue.");
        }
        Node* temp = front;
        Task* task = temp->task;
        front = front->next;
        delete temp;
        return task;
    }

    // بررسی خالی بودن صف
    bool isEmpty() const {
        return front == nullptr;
    }

    // دمیل کردن صف و آزادسازی حافظه
    ~PriorityQueue() {
        while (!isEmpty()) {
            dequeue();
        }
    }

    // خالی کردن صف اولویت و بارگذاری دوباره
    void clear() {
        while (!isEmpty()) {
            dequeue();
        }
    }

    // نمایش صف اولویت
    void printQueue() const {
        Node* temp = front;
        while (temp) {
            temp->task->printTaskDetails();
            temp = temp->next;
        }
    }
};

// مدیریت درخت تسک‌ها
class TaskTree {
private:
    Task* root;
    PriorityQueue priorityQueue;

    // افزودن تسک به درخت
    Task* addTaskRecursive(Task* node, Task* task) {
        if (!node) return task;
        if (task->deadline < node->deadline) {
            node->left = addTaskRecursive(node->left, task);
        } else {
            node->right = addTaskRecursive(node->right, task);
        }
        return node;
    }

    // پیمایش درخت و افزودن تسک‌ها به صف اولویت
    void traverseAndEnqueue(Task* node) {
        if (!node) return;
        priorityQueue.enqueue(node);
        traverseAndEnqueue(node->left);
        traverseAndEnqueue(node->right);
    }

    // چک کردن اینکه تمام زیرتسک‌ها انجام شده‌اند یا نه
    bool areAllSubtasksDone(Subtask* subtasks) {
        Subtask* current = subtasks;
        while (current) {
            if (current->status != "done") {
                return false;
            }
            current = current->next;
        }
        return true;
    }

    // حذف تسک‌های تکمیل‌شده
    Task* deleteCompletedTasks(Task* node) {
        if (!node) return nullptr;

        node->left = deleteCompletedTasks(node->left);
        node->right = deleteCompletedTasks(node->right);

        if (areAllSubtasksDone(node->subtasks)) {
            delete node;
            return nullptr;
        }
        return node;
    }

    // پیدا کردن تسک با نام مشخص
    Task* findTask(Task* node, const string& taskName) {
        if (!node) return nullptr;
        if (node->name == taskName) return node;
        Task* leftResult = findTask(node->left, taskName);
        if (leftResult) return leftResult;
        return findTask(node->right, taskName);
    }

public:
    TaskTree() : root(nullptr) {}

    // افزودن تسک
    void addTask(string name, string description, int deadline, string status) {
        Task* task = new Task(name, description, deadline, status);
        root = addTaskRecursive(root, task);

        // اضافه کردن تسک به صف اولویت
        priorityQueue.enqueue(task);
    }

    // افزودن زیرتسک به تسک
    void addSubtask(string taskName, string subtaskName, string description, int deadline, string status) {
        Task* task = findTask(root, taskName);
        if (!task) {
            cout << "Task not found!\n";
            return;
        }

        if (deadline > task->deadline) {
            cout << "Subtask deadline cannot exceed task deadline!\n";
            return;
        }

        Subtask* newSubtask = new Subtask(subtaskName, description, deadline, status);
        if (!task->subtasks) {
            task->subtasks = newSubtask;
        } else {
            Subtask* current = task->subtasks;
            while (current->next) {
                current = current->next;
            }
            current->next = newSubtask;
        }
        cout << "Subtask added successfully.\n";
    }

    // تغییر وضعیت زیرتسک
    void changeSubtaskStatus(const string& taskName, const string& subtaskName, const string& newStatus) {
        Task* task = findTask(root, taskName);
        if (!task) {
            cout << "Task not found!\n";
            return;
        }

        Subtask* subtask = task->subtasks;
        while (subtask) {
            if (subtask->name == subtaskName) {
                subtask->status = newStatus;
                cout << "Subtask status updated successfully.\n";
                return;
            }
            subtask = subtask->next;
        }

        cout << "Subtask not found!\n";
    }

    // حذف تسک‌های تکمیل‌شده
    void deleteCompleted() {
        root = deleteCompletedTasks(root);
    }

    // پیمایش درخت به صورت BFS
    void bfsTraversal() {
        if (!root) return;
        PriorityQueue queue;
        queue.enqueue(root);
        cout << "BFS Traversal:\n";
        while (!queue.isEmpty()) {
            Task* current = queue.dequeue();
            cout << "Task: " << current->name << "\n";
            if (current->left) queue.enqueue(current->left);
            if (current->right) queue.enqueue(current->right);
        }
    }

    // پیمایش درخت به صورت DFS
    void dfsTraversal() {
        if (!root) return;
        PriorityQueue stack;
        stack.enqueue(root);
        cout << "DFS Traversal:\n";
        while (!stack.isEmpty()) {
            Task* current = stack.dequeue();
            cout << "Task: " << current->name << "\n";
            if (current->right) stack.enqueue(current->right);
            if (current->left) stack.enqueue(current->left);
        }
    }

    // نمایش تسک‌ها بر اساس ددلاین
    void displayByDeadline() {
        priorityQueue.clear(); // خالی کردن صف اولویت
        traverseAndEnqueue(root); // دوباره بارگذاری صف اولویت
        cout << "Tasks by Deadline:\n";
        priorityQueue.printQueue();
    }
};

// بارگذاری تسک‌ها از فایل
void loadTasksFromFile(TaskTree& taskTree) {
    ifstream file("tasks.txt");
    if (!file) {
        cerr << "Error opening file!\n";
        return;
    }
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string name, description, status;
        int deadline;
        getline(ss, name, ',');
        getline(ss, description, ',');
        ss >> deadline;
        ss.ignore();
        getline(ss, status, ',');
        taskTree.addTask(name, description, deadline, status);
    }
    file.close();
}

// ذخیره‌سازی تسک‌ها به فایل
void saveTasksToFile(const TaskTree& taskTree) {
    ofstream file("tasks.txt");
    if (!file) {
        cerr << "Error opening file for writing!\n";
        return;
    }
    // Implement save logic if required
    file.close();
}

int main() {
    TaskTree taskTree;
    loadTasksFromFile(taskTree);

    int choice;
    do {
        cout << "\nTask Manager\n";
        cout << "1. Add Task\n";
        cout << "2. Add Subtask\n";
        cout << "3. Change Subtask Status\n";
        cout << "4. Delete Completed Tasks\n";
        cout << "5. BFS Traversal\n";
        cout << "6. DFS Traversal\n";
        cout << "7. Display by Deadline\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            string name, description, status;
            int deadline;
            cout << "Enter task name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter task description: ";
            getline(cin, description);
            cout << "Enter deadline (in days): ";
            cin >> deadline;
            cout << "Enter task status: ";
            cin.ignore();
            getline(cin, status);
            taskTree.addTask(name, description, deadline, status);
            break;
        }
        case 2: {
            string taskName, subtaskName, description, status;
            int deadline;
            cout << "Enter task name to add subtask: ";
            cin.ignore();
            getline(cin, taskName);
            cout << "Enter subtask name: ";
            getline(cin, subtaskName);
            cout << "Enter subtask description: ";
            getline(cin, description);
            cout << "Enter subtask deadline (in days): ";
            cin >> deadline;
            cout << "Enter subtask status: ";
            cin.ignore();
            getline(cin, status);
            taskTree.addSubtask(taskName, subtaskName, description, deadline, status);
            break;
        }
        case 3: {
            string taskName, subtaskName, status;
            cout << "Enter task name: ";
            cin.ignore();
            getline(cin, taskName);
            cout << "Enter subtask name: ";
            getline(cin, subtaskName);
            cout << "Enter new status (to do / done): ";
            getline(cin, status);
            taskTree.changeSubtaskStatus(taskName, subtaskName, status);
            break;
        }
        case 4:
            taskTree.deleteCompleted();
            break;
        case 5:
            taskTree.bfsTraversal();
            break;
        case 6:
            taskTree.dfsTraversal();
            break;
        case 7:
            taskTree.displayByDeadline();
            break;
        case 8:
            saveTasksToFile(taskTree);
            cout << "Exiting program.\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 8);

    return 0;
}

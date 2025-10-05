#include "texteditor.h"
#include <map>

// Node structure for doubly linked list
struct CharNode {
    char data;
    CharNode* next;
    CharNode* prev;
    
    CharNode(char c) : data(c), next(nullptr), prev(nullptr) {}
};

// Editor data structure
class EditorData {
public:
    CharNode* head;      // Start of text
    CharNode* cursor;    // Cursor position (between cursor->prev and cursor)
    int cursorPos;       // Cursor position index
    
    EditorData() : head(nullptr), cursor(nullptr), cursorPos(0) {}
    
    ~EditorData() {
        while (head) {
            CharNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

// Global storage for text editor data
static std::map<size_t, EditorData*> editorStorage;
static size_t nextEditorId = 1;

// Get ID stored in object
static size_t& getEditorId(const TextEditor* editor) {
    return *((size_t*)editor);
}

// Get or create editor data
static EditorData* getEditorData(const TextEditor* editor) {
    size_t id = getEditorId(editor);
    if (id == 0 || editorStorage.find(id) == editorStorage.end()) {
        id = nextEditorId++;
        getEditorId(editor) = id;
        editorStorage[id] = new EditorData();
    }
    return editorStorage[id];
}

void TextEditor::insertChar(char c) {
    EditorData* data = getEditorData(this);
    CharNode* newNode = new CharNode(c);
    
    if (!data->head) {
        // Empty editor
        data->head = newNode;
        data->cursor = newNode;
        data->cursorPos = 1;
        return;
    }
    
    if (data->cursorPos == 0) {
        // Insert at beginning
        newNode->next = data->head;
        data->head->prev = newNode;
        data->head = newNode;
        data->cursor = data->head->next;
        data->cursorPos = 1;
        return;
    }
    
    // Find position to insert
    CharNode* current = data->head;
    int pos = 0;
    
    while (current && pos < data->cursorPos - 1) {
        current = current->next;
        pos++;
    }
    
    // Insert after current
    if (current) {
        newNode->next = current->next;
        newNode->prev = current;
        if (current->next) {
            current->next->prev = newNode;
        }
        current->next = newNode;
        data->cursor = newNode->next;
        data->cursorPos++;
    }
}

void TextEditor::deleteChar() {
    EditorData* data = getEditorData(this);
    
    // Can't delete at position 0
    if (data->cursorPos == 0 || !data->head) {
        return;
    }
    
    // Find node to delete (before cursor)
    CharNode* current = data->head;
    int pos = 0;
    
    while (current && pos < data->cursorPos - 1) {
        current = current->next;
        pos++;
    }
    
    if (!current) return;
    
    // Delete the node
    if (current->prev) {
        current->prev->next = current->next;
    } else {
        // Deleting first node
        data->head = current->next;
    }
    
    if (current->next) {
        current->next->prev = current->prev;
    }
    
    // Update cursor
    if (data->cursor == current) {
        data->cursor = current->next;
    }
    
    delete current;
    data->cursorPos--;
}

void TextEditor::moveLeft() {
    EditorData* data = getEditorData(this);
    
    // Can't move left from position 0
    if (data->cursorPos <= 0) {
        return;
    }
    
    data->cursorPos--;
    
    // Update cursor pointer
    if (data->cursor && data->cursor->prev) {
        data->cursor = data->cursor->prev;
    } else if (data->cursorPos == 0) {
        data->cursor = data->head;
    }
}

void TextEditor::moveRight() {
    EditorData* data = getEditorData(this);
    
    // Count total characters
    int totalChars = 0;
    CharNode* temp = data->head;
    while (temp) {
        totalChars++;
        temp = temp->next;
    }
    
    // Can't move right beyond end
    if (data->cursorPos >= totalChars) {
        return;
    }
    
    data->cursorPos++;
    
    // Update cursor pointer
    if (data->cursor) {
        data->cursor = data->cursor->next;
    }
}

std::string TextEditor::getTextWithCursor() const {
    EditorData* data = getEditorData(this);
    std::string result = "";
    
    CharNode* current = data->head;
    int pos = 0;
    
    while (current) {
        if (pos == data->cursorPos) {
            result += '|';
        }
        result += current->data;
        current = current->next;
        pos++;
    }
    
    // Cursor at end
    if (pos == data->cursorPos) {
        result += '|';
    }
    
    return result;
}
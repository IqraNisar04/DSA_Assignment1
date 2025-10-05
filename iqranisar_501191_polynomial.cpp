#include "polynomial.h"
#include <sstream>
#include <map>

// Node structure for linked list
struct TermNode {
    int coefficient;
    int exponent;
    TermNode* next;
    
    TermNode(int coef, int exp) : coefficient(coef), exponent(exp), next(nullptr) {}
};

// Helper class to manage polynomial data
class PolyData {
public:
    TermNode* head;
    
    PolyData() : head(nullptr) {}
    
    ~PolyData() {
        clear();
    }
    
    void clear() {
        while (head) {
            TermNode* temp = head;
            head = head->next;
            delete temp;
        }
        head = nullptr;
    }
    
    PolyData* deepCopy() const {
        PolyData* newData = new PolyData();
        if (!head) return newData;
        
        TermNode* current = head;
        while (current) {
            TermNode* newNode = new TermNode(current->coefficient, current->exponent);
            if (!newData->head) {
                newData->head = newNode;
            } else {
                TermNode* tail = newData->head;
                while (tail->next) tail = tail->next;
                tail->next = newNode;
            }
            current = current->next;
        }
        return newData;
    }
};

// Global storage - maps object addresses to their data
static std::map<size_t, PolyData*> globalStorage;
static size_t nextId = 1;

// Store ID in the object's memory space
static size_t& getId(const Polynomial* poly) {
    return *((size_t*)poly);
}

static PolyData* getData(const Polynomial* poly) {
    size_t id = getId(poly);
    if (id == 0 || globalStorage.find(id) == globalStorage.end()) {
        id = nextId++;
        getId(poly) = id;
        globalStorage[id] = new PolyData();
    }
    return globalStorage[id];
}

void Polynomial::insertTerm(int coefficient, int exponent) {
    if (coefficient == 0) return;
    
    PolyData* data = getData(this);
    TermNode*& head = data->head;
    
    // If list is empty or new term has highest exponent
    if (!head || head->exponent < exponent) {
        TermNode* newNode = new TermNode(coefficient, exponent);
        newNode->next = head;
        head = newNode;
        return;
    }
    
    // Find position to insert (keep sorted by exponent, descending)
    TermNode* current = head;
    TermNode* prev = nullptr;
    
    while (current && current->exponent > exponent) {
        prev = current;
        current = current->next;
    }
    
    // If term with same exponent exists, combine them
    if (current && current->exponent == exponent) {
        current->coefficient += coefficient;
        if (current->coefficient == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                head = current->next;
            }
            delete current;
        }
        return;
    }
    
    // Insert new term
    TermNode* newNode = new TermNode(coefficient, exponent);
    if (prev) {
        prev->next = newNode;
        newNode->next = current;
    } else {
        newNode->next = head;
        head = newNode;
    }
}

std::string Polynomial::toString() const {
    PolyData* data = getData(this);
    TermNode* head = data->head;
    
    if (!head) return "0";
    
    std::ostringstream oss;
    TermNode* current = head;
    bool first = true;
    
    while (current) {
        int coef = current->coefficient;
        int exp = current->exponent;
        
        if (coef == 0) {
            current = current->next;
            continue;
        }
        
        // Add sign
        if (!first) {
            if (coef > 0) {
                oss << " + ";
            } else {
                oss << " - ";
                coef = -coef;
            }
        } else {
            if (coef < 0) {
                oss << "-";
                coef = -coef;
            }
            first = false;
        }
        
        // Add coefficient (only if not 1, or if exponent is 0)
        if (coef != 1 || exp == 0) {
            oss << coef;
        }
        
        // Add variable and exponent
        if (exp > 0) {
            oss << "x";
            if (exp > 1) {
                oss << "^" << exp;
            }
        }
        
        current = current->next;
    }
    
    return oss.str();
}

Polynomial Polynomial::add(const Polynomial& other) const {
    Polynomial result;
    getData(&result);
    
    PolyData* data1 = getData(this);
    PolyData* data2 = getData(&other);
    
    TermNode* p1 = data1->head;
    TermNode* p2 = data2->head;
    
    while (p1 || p2) {
        if (!p2 || (p1 && p1->exponent > p2->exponent)) {
            result.insertTerm(p1->coefficient, p1->exponent);
            p1 = p1->next;
        } else if (!p1 || p2->exponent > p1->exponent) {
            result.insertTerm(p2->coefficient, p2->exponent);
            p2 = p2->next;
        } else {
            int sumCoef = p1->coefficient + p2->coefficient;
            if (sumCoef != 0) {
                result.insertTerm(sumCoef, p1->exponent);
            }
            p1 = p1->next;
            p2 = p2->next;
        }
    }
    
    return result;
}

Polynomial Polynomial::multiply(const Polynomial& other) const {
    Polynomial result;
    getData(&result);
    
    PolyData* data1 = getData(this);
    PolyData* data2 = getData(&other);
    
    TermNode* p1 = data1->head;
    
    while (p1) {
        TermNode* p2 = data2->head;
        while (p2) {
            int newCoef = p1->coefficient * p2->coefficient;
            int newExp = p1->exponent + p2->exponent;
            result.insertTerm(newCoef, newExp);
            p2 = p2->next;
        }
        p1 = p1->next;
    }
    
    return result;
}

Polynomial Polynomial::derivative() const {
    Polynomial result;
    getData(&result);
    
    PolyData* data = getData(this);
    TermNode* current = data->head;
    
    while (current) {
        if (current->exponent > 0) {
            int newCoef = current->coefficient * current->exponent;
            int newExp = current->exponent - 1;
            result.insertTerm(newCoef, newExp);
        }
        current = current->next;
    }
    
    return result;
}
#include "polynomial.h"
#include <iostream>

int main() {
    Polynomial p1;
    p1.insertTerm(3, 4);
    p1.insertTerm(2, 2);
    p1.insertTerm(-1, 1);
    p1.insertTerm(5, 0);
    
    std::cout << "p1: " << p1.toString() << std::endl;
    
    Polynomial p2;
    p2.insertTerm(1, 4);
    p2.insertTerm(1, 0);
    
    std::cout << "p2: " << p2.toString() << std::endl;
    
    Polynomial sum = p1.add(p2);
    std::cout << "sum: " << sum.toString() << std::endl;
    
    Polynomial p3;
    p3.insertTerm(2, 1);
    Polynomial prod = sum.multiply(p3);
    std::cout << "prod: " << prod.toString() << std::endl;
    
    Polynomial deriv = p1.derivative();
    std::cout << "deriv: " << deriv.toString() << std::endl;
    
    return 0;
}
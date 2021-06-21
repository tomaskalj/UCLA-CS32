#include <cassert>
#include <cctype>
#include <stack>
#include <string>

using namespace std;

bool isOperator(char c) {
    return c == '!' || c == '&' || c == '|';
}

int priority(char op) {
    switch (op) {
        case '!': return 3;
        case '&': return 2;
        case '|': return 1;
        default: return 0;
    }
}

bool isValidInfix(string infix) {
    if (infix.empty()) {
//        cout << "Empty" << endl;
        return false;
    }

    if (infix.at(0) == '&' || infix.at(0) == '|' || isOperator(infix.at(infix.length() - 1))) {
//        cout << "Invalid operator at start or end" << endl;
        return false;
    }

    for (int i = 0; i < infix.length() - 1; i++) {
        if (isdigit(infix.at(i))) {
            if (infix.at(i + 1) == '(') {
//                cout << "Missing operator between operand and opening parenthesis" << endl;
                return false;
            } else if (isdigit(infix.at(i + 1))) {
//                cout << "Adjacent operands" << endl;
                return false;
            }
        }

        if (infix.at(i) == '(' && infix.at(i + 1) == ')') {
//            cout << "Empty expression inside parentheses" << endl;
            return false;
        }
    }

    stack<char> s;
    for (int i = 0; i < infix.length(); i++) {
        char c = infix.at(i);
        if (!isdigit(c) && !isOperator(c) && c != '(' && c != ')') {
//            cout << "Invalid character: " << infix.at(i) << endl;
            return false;
        }

        if (c == '(') {
            s.push(c);
        } else if (c == ')') {
            if (!s.empty() && s.top() == '(') {
                s.pop();
            } else {
//                cout << "Extra parentheses" << endl;
                return false;
            }
        }
    }

    return !(!s.empty() && s.top() == '(');
}

int evaluate(string infix, const bool values[], string& postfix, bool& result) {
    // We don't want to deal with spaces in our infix expression
    string copyInfix;
    for (int i = 0; i < infix.length(); i++) {
        if (!isspace(infix.at(i))) {
            copyInfix += infix.at(i);
        }
    }

    if (!isValidInfix(copyInfix)) {
        return 1;
    }

    postfix = "";
    stack<char> operatorStack;

    for (int i = 0; i < copyInfix.length(); i++) {
        char c = copyInfix.at(i);
        if (isdigit(c)) {
            postfix += c;
        } else if (c == '(') {
            operatorStack.push(c);
        } else if (c == ')') {
            while (operatorStack.top() != '(') {
                postfix += operatorStack.top();
                operatorStack.pop();
            }
            operatorStack.pop();
        } else if (isOperator(c)) {
            while (!operatorStack.empty() && operatorStack.top() != '(' && priority(c) <= priority(operatorStack.top())) {
                postfix += operatorStack.top();
                operatorStack.pop();
            }
            operatorStack.push(c);
        }
    }

    while (!operatorStack.empty()) {
        postfix += operatorStack.top();
        operatorStack.pop();
    }

    stack<bool> operandStack;

    for (int i = 0; i < postfix.length(); i++) {
        char c = postfix.at(i);
        if (isdigit(c)) {
            int index = c - '0';
            operandStack.push(values[index]);
        } else if (c == '!') {
            bool op = operandStack.top();
            operandStack.pop();
            operandStack.push(!op);
        } else {
            bool op2 = operandStack.top();
            operandStack.pop();

            bool op1 = operandStack.top();
            operandStack.pop();

            bool operation;
            if (c == '&') {
                operation = op1 && op2;
            } else if (c == '|') {
                operation = op1 || op2;
            }

            operandStack.push(operation);
        }
    }

    result = operandStack.top();
    operandStack.pop();

    return 0;
}
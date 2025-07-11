#include <iostream>
#include <string>
#include <unordered_map>
#include <cctype>
#include <stdexcept>

class Interpreter {
    std::string input;
    size_t pos = 0;
    std::unordered_map<std::string, int> variables;

    char peek() {
        return pos < input.size() ? input[pos] : '\0';
    }

    char get() {
        return pos < input.size() ? input[pos++] : '\0';
    }

    void skipWhitespace() {
        while (isspace(peek())) get();
    }

    std::string parseIdentifier() {
        skipWhitespace();
        std::string id;
        while (isalnum(peek()) || peek() == '_') {
            id += get();
        }
        if (id.empty()) throw std::runtime_error("Expected identifier");
        return id;
    }

    int parseNumber() {
        skipWhitespace();
        int num = 0;
        while (isdigit(peek())) {
            num = num * 10 + (get() - '0');
        }
        return num;
    }

    int parseFactor() {
        skipWhitespace();
        if (peek() == '(') {
            get(); // consume '('
            int result = parseExpression();
            if (get() != ')') throw std::runtime_error("Expected ')'");
            return result;
        } else if (isalpha(peek())) {
            std::string var = parseIdentifier();
            if (variables.find(var) == variables.end())
                throw std::runtime_error("Undefined variable: " + var);
            return variables[var];
        } else {
            return parseNumber();
        }
    }

    int parseTerm() {
        int result = parseFactor();
        while (true) {
            skipWhitespace();
            if (peek() == '*') {
                get();
                result *= parseFactor();
            } else if (peek() == '/') {
                get();
                int divisor = parseFactor();
                if (divisor == 0) throw std::runtime_error("Division by zero");
                result /= divisor;
            } else {
                break;
            }
        }
        return result;
    }

    int parseExpression() {
        int result = parseTerm();
        while (true) {
            skipWhitespace();
            if (peek() == '+') {
                get();
                result += parseTerm();
            } else if (peek() == '-') {
                get();
                result -= parseTerm();
            } else {
                break;
            }
        }
        return result;
    }

public:
    void interpret(const std::string& line) {
        input = line;
        pos = 0;

        skipWhitespace();
        if (isalpha(peek())) {
            std::string var = parseIdentifier();
            skipWhitespace();
            if (get() != '=') throw std::runtime_error("Expected '='");
            int value = parseExpression();
            variables[var] = value;
            std::cout << var << " = " << value << "\n";
        } else {
            int result = parseExpression();
            std::cout << "Result: " << result << "\n";
        }
    }
};

int main() {
    Interpreter interpreter;
    std::string line;

    std::cout << "Enter expressions or assignments (e.g., x = 3 + 4). Type 'exit' to quit:\n";
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        if (line == "exit") break;

        try {
            interpreter.interpret(line);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    return 0;
}

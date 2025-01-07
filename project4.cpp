#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <memory>
#include <stdexcept>
#include <sstream>

// Token types
enum class TokenType {
    Number,
    Plus,
    Minus,
    Multiply,
    Divide,
    LParen,
    RParen,
    End,
    Invalid
};

// Token class
struct Token {
    TokenType type;
    std::string value;
    
    Token(TokenType t, const std::string& v = "") : type(t), value(v) {}
};

// Lexer
class Lexer {
public:
    Lexer(const std::string& input) : input(input), index(0) {}
    
    Token next_token() {
        while (index < input.size() && std::isspace(input[index])) {
            index++;
        }
        
        if (index >= input.size()) {
            return Token(TokenType::End);
        }
        
        char current_char = input[index];
        
        if (std::isdigit(current_char)) {
            return number();
        } else if (current_char == '+') {
            index++;
            return Token(TokenType::Plus, "+");
        } else if (current_char == '-') {
            index++;
            return Token(TokenType::Minus, "-");
        } else if (current_char == '*') {
            index++;
            return Token(TokenType::Multiply, "*");
        } else if (current_char == '/') {
            index++;
            return Token(TokenType::Divide, "/");
        } else if (current_char == '(') {
            index++;
            return Token(TokenType::LParen, "(");
        } else if (current_char == ')') {
            index++;
            return Token(TokenType::RParen, ")");
        } else {
            return Token(TokenType::Invalid);
        }
    }
    
private:
    Token number() {
        size_t start = index;
        while (index < input.size() && std::isdigit(input[index])) {
            index++;
        }
        return Token(TokenType::Number, input.substr(start, index - start));
    }
    
    std::string input;
    size_t index;
};

// AST nodes
struct Expr {
    virtual ~Expr() = default;
};

struct Number : public Expr {
    double value;
    Number(double v) : value(v) {}
};

struct BinOp : public Expr {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    TokenType op;
    
    BinOp(std::unique_ptr<Expr> l, TokenType o, std::unique_ptr<Expr> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
};

// Parser
class Parser {
public:
    Parser(Lexer& lexer) : lexer(lexer), current_token(lexer.next_token()) {}

    std::unique_ptr<Expr> parse() {
        return parse_expression();
    }

private:
    std::unique_ptr<Expr> parse_expression() {
        auto left = parse_term();
        
        while (current_token.type == TokenType::Plus || current_token.type == TokenType::Minus) {
            TokenType op = current_token.type;
            consume_token();
            auto right = parse_term();
            left = std::make_unique<BinOp>(std::move(left), op, std::move(right));
        }
        
        return left;
    }
    
    std::unique_ptr<Expr> parse_term() {
        auto left = parse_factor();
        
        while (current_token.type == TokenType::Multiply || current_token.type == TokenType::Divide) {
            TokenType op = current_token.type;
            consume_token();
            auto right = parse_factor();
            left = std::make_unique<BinOp>(std::move(left), op, std::move(right));
        }
        
        return left;
    }
    
    std::unique_ptr<Expr> parse_factor() {
        if (current_token.type == TokenType::Number) {
            double value = std::stod(current_token.value);
            consume_token();
            return std::make_unique<Number>(value);
        } else if (current_token.type == TokenType::LParen) {
            consume_token();
            auto expr = parse_expression();
            if (current_token.type != TokenType::RParen) {
                throw std::runtime_error("Expected ')'");
            }
            consume_token();
            return expr;
        } else {
            throw std::runtime_error("Unexpected token");
        }
    }
    
    void consume_token() {
        current_token = lexer.next_token();
    }
    
    Lexer& lexer;
    Token current_token;
};

// Evaluator
double evaluate(const Expr& expr) {
    if (const Number* num = dynamic_cast<const Number*>(&expr)) {
        return num->value;
    } else if (const BinOp* binop = dynamic_cast<const BinOp*>(&expr)) {
        double left_val = evaluate(*binop->left);
        double right_val = evaluate(*binop->right);
        
        switch (binop->op) {
            case TokenType::Plus: return left_val + right_val;
            case TokenType::Minus: return left_val - right_val;
            case TokenType::Multiply: return left_val * right_val;
            case TokenType::Divide: return left_val / right_val;
            default: throw std::runtime_error("Unknown operator");
        }
    } else {
        throw std::runtime_error("Unknown expression");
    }
}

int main() {
    std::string input;
    std::cout << "Enter an arithmetic expression: ";
    std::getline(std::cin, input);
    
    try {
        Lexer lexer(input);
        Parser parser(lexer);
        auto ast = parser.parse();
        double result = evaluate(*ast);
        std::cout << "Result: " << result << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}


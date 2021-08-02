#pragma once
#include <cassert>
#include <string>
#include <exception>
#include <map>
#include <vector>

enum class State{
    Empty = 0,
    Numbering = 1,
    Operator = 2,
    Identifier = 3,
};

enum class TokenType{
    Number = 1,
    Operator = 2,
    Identifier = 3,
    Parenthesis = 4,
};

struct Error : std::exception{
    std::string err_message;
    
    Error(const std::string& err_message) : err_message(err_message){}
    
    const char* what() const noexcept override{
        return err_message.c_str();
    }
};

struct Token{
    TokenType type;
    std::string token;
    
    void initialize(TokenType t, char c){
        type = t;
        token.clear();
        token += c;
    }
    
    bool operator==(const Token& other) const{
        return other.type == type && other.token == token;
    }
    
    bool operator!=(const Token& other) const{
        return other.type != type || other.token != token;
    }
};

struct Function{
    int numArguments;
    std::vector<std::string> argumentNames;
    std::vector<Token> funcExpression;
};

int getPrecedence(const Token& op);

bool isDigit(char c);

bool isOperator(char c);

bool isIdentifier(char c);

double factorial(double n);

double defaultFunction_choose(double a, double b);

double defaultFunction_choice(double condition, double a, double b);

//Tokenize an expression into operators, operands, identifiers, and parentheses
std::vector<Token> tokenize(const std::string& text);
/*
struct EvaluationContext
{
    std::map<std::string, Function> customFunctions;
    ...
    
    // needs context information.
    std::vector<Token> convertToPostfix(const std::vector<Token>& tokens);
    
    
};
*/
//Takes a vector of tokens and converts them into postfix notation
std::vector<Token> convertToPostfix(const std::vector<Token>& tokens_, const std::map<std::string, Function>& customFunctions, bool functionCall = false);

//Takes postfix notation expression as a vector of tokens
double evaluateExpression(const std::vector<Token>& tokens, const std::map<std::string, double>& variables, const std::map<std::string, Function>& functions);

//Overload that the user calls
double evaluateExpression(const std::string& expression, std::map<std::string, double>& variables, std::map<std::string, Function>& customFunctions);

void evaluateFile(const std::string& filePath);

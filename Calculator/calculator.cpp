#include <cassert>
#include <cmath>
#include <stack>
#include <fstream>
#include <iostream>

#include "calculator.hpp"

int getPrecedence(const Token& op){
    assert (op.type == TokenType::Operator);
    const static std::map<std::string, int> s_precedence{
        {"?", 0},
        {":", 1},
        {"<", 2}, {">", 2}, {"<=", 2}, {">=", 2}, {"==", 2}, {"&&", 2}, {"||", 2},
        {"+", 3}, {"-", 3},
        {"*", 4}, {"/", 4}, {"%", 4},
        {"^", 5},
    };
    auto it = s_precedence.find(op.token);
    if (it == s_precedence.end())
        throw Error{std::string("[Error]: Unrecognized operator (")+op.token+")"};
    return it->second;
}

bool isDigit(char c){
    return ((c-'0') >= 0 && (c-'0') <= 9) || c == '.';
}

bool isOperator(char c){
    return c == '%' || c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '<' || c == '>' || c == '=' || c == '&' || c == '|' || c == '?' || c == ':' || c == ',';
}

bool isIdentifier(char c){
    return ((c-'a') <= ('z'-'a') && (c-'a') >= 0) || ((c-'A') <= ('Z'-'A') && (c-'A') >= 0) || c == '_';
}

double factorial(double n){
    if (n != std::floor(n))
        throw Error{"[Error]: Cannot calculate factorial of non integer"};
    else if (n < 0)
        throw Error{"[Error]: Cannot calculate factorial of negative number"};
    double ret = n;
    while (n > 1){
        n -= 1;
        ret *= n;
    }
    return ret;
}

typedef double (*FUNCTION_POINTER_ARG1)(double);
const std::map<std::string, FUNCTION_POINTER_ARG1> defaultFunctions_arg1{
    {"sin", std::sin},
    {"cos", std::cos},
    {"tan", std::tan},
    {"abs", std::fabs},
    {"sqrt", std::sqrt},
    {"cbrt", std::cbrt},
    {"floor", std::floor},
    {"ceil", std::ceil},
    {"factorial", factorial},
};

double defaultFunction_choose(double a, double b){
    return factorial(a)/(factorial(b) * factorial(a-b));
}

typedef double (*FUNCTION_POINTER_ARG2)(double, double);
const std::map<std::string, FUNCTION_POINTER_ARG2> defaultFunctions_arg2{
    {"min", std::fmin},
    {"max", std::fmax},
    {"choose", defaultFunction_choose},
};

double defaultFunction_choice(double condition, double a, double b){
    return condition == 0? b : a;
}

typedef double (*FUNCTION_POINTER_ARG3)(double, double, double);
const std::map<std::string, FUNCTION_POINTER_ARG3> defaultFunctions_arg3{
    {"choice", defaultFunction_choice},
};

const std::map<std::string, double> constants{
    {"pi", M_PI},
};

//Tokenize an expression into operators, operands, identifiers, and parentheses
std::vector<Token> tokenize(const std::string& text){
    // the first char of an identifier cannot be a number or operator, a number or identifier name ends when it encounters either an operator or a parentheses
    // the whole string representing a number cannot have more than one .
    std::vector<Token> tokens;
    Token token;
    State state = State::Empty;
    for (char c : text){
        if (c == '(' || c == ')'){
            if (state != State::Empty)
                tokens.push_back(token);
            Token paren;
            paren.initialize(TokenType::Parenthesis, c);
            tokens.push_back(paren);
            state = State::Empty;
            continue;
        }
        switch(state){
            case State::Empty:
            {
                if (isDigit(c)){
                    token.initialize(TokenType::Number, c);
                    state = State::Numbering;
                }
                else if (isOperator(c)){
                    token.initialize(TokenType::Operator, c);
                    state = State::Operator;
                }
                else if (isIdentifier(c)){
                    token.initialize(TokenType::Identifier, c);
                    state = State::Identifier;
                }
                else if (c != ' ')
                    throw Error{std::string("[Error]: Unrecognized symbol: ")+c};
                break;
            }
            case State::Numbering:
            {
                if (isDigit(c)){
                    token.token += c;
                }
                else if (isOperator(c)){
                    tokens.push_back(token);
                    token.initialize(TokenType::Operator, c);
                    state = State::Operator;
                }
                else if (isIdentifier(c)){
                    tokens.push_back(token);
                    token.initialize(TokenType::Identifier, c);
                    state = State::Identifier;
                }
                else if (c == ' '){
                    tokens.push_back(token);
                    state = State::Empty;
                }
                else
                    throw Error{std::string("[Error]: Unrecognized symbol: ")+c};
                break;
            }
            case State::Operator:
            {
                if (isOperator(c)){
                    token.token += c;
                }
                else if (isDigit(c)){
                    if ((token.token == "+" || token.token == "-") && (tokens.empty() || tokens.back().token == "(")){
                        token.token += c;
                        token.type = TokenType::Number;
                        state = State::Numbering;
                    }
                    else{
                        tokens.push_back(token);
                        token.initialize(TokenType::Number, c);
                        state = State::Numbering;
                    }
                }
                else if (isIdentifier(c)){
                    if ((token.token == "+" || token.token == "-") && (tokens.empty() || tokens.back().token == "(")){
                        token.token += c;
                        token.type = TokenType::Identifier;
                        state = State::Identifier;
                    }
                    else{
                        tokens.push_back(token);
                        token.initialize(TokenType::Identifier, c);
                        state = State::Identifier;
                    }
                }
                else if (c == ' '){
                    tokens.push_back(token);
                    state = State::Empty;
                }
                else
                    throw Error{std::string("[Error]: Unrecognized symbol: ")+c};
                break;
            }
            case State::Identifier:
            {
                if (isIdentifier(c)){
                    token.token += c;
                }
                else if (isDigit(c) && c != '.'){
                    token.token += c;
                }
                else if (isOperator(c)){
                    tokens.push_back(token);
                    token.initialize(TokenType::Operator, c);
                    state = State::Operator;
                }
                else if (c == ' '){
                    tokens.push_back(token);
                    state = State::Empty;
                }
                else
                    throw Error{std::string("[Error]: Unrecognized symbol: ")+c};
                break;
            }
        }
    }
    if (state != State::Empty)
        tokens.push_back(token);
    return tokens;
}

//Takes a vector of tokens and converts them into postfix notation
std::vector<Token> convertToPostfix(const std::vector<Token>& tokens_, const std::map<std::string, Function>& customFunctions, bool functionCall){
    std::vector<Token> tokens = tokens_;
    std::stack<Token> stack;
    std::vector<Token> result;
    auto processToken = [&](int index){
        const Token& token = tokens[index];
        switch (token.type){
            case TokenType::Parenthesis:
            {
                if (token.token == "(")
                    stack.push(token);
                else{
                    while (stack.top().token != "("){
                        result.push_back(stack.top());
                        stack.pop();
                    }
                    stack.pop();
                }
                break;
            }
            case TokenType::Number:
            case TokenType::Identifier:
            {
                if ((defaultFunctions_arg1.find(token.token) != defaultFunctions_arg1.end() || defaultFunctions_arg2.find(token.token) != defaultFunctions_arg2.end() || defaultFunctions_arg3.find(token.token) != defaultFunctions_arg3.end() || customFunctions.find(token.token) != customFunctions.end()) && index != tokens.size()-1 && tokens[index+1].token == "("){
                    int parenCount = 1;
                    int closingParenIndex = -1;
                    std::vector<Token> functionArgs;
                    for (int i=index+2; i<tokens.size(); ++i){
                        if (tokens[i].token == "(")
                            ++parenCount;
                        else if (tokens[i].token == ")"){
                            --parenCount;
                            if (parenCount == 0){
                                closingParenIndex = i;
                                break;
                            }
                        }
                        functionArgs.push_back(tokens[i]);
                    }
                    std::vector<Token> recursedResult = convertToPostfix(functionArgs, customFunctions, true);
                    result.insert(result.end(), recursedResult.begin(), recursedResult.end());
                    result.push_back(token);
                    return closingParenIndex;
                }
                else if (index < tokens.size()-2 && tokens[index+1].type != TokenType::Operator && tokens[index+1].token != ")")
                    throw Error{std::string("[Error]: Operator omitted | Token #: ")+std::to_string(index)};
                result.push_back(token);
                break;
            }
            case TokenType::Operator:
            {
                if (token.token == "," && !functionCall)
                    throw Error{"[Error]: Comma cannot be used outside of a function call"};
                else if (token.token != ","){
                    while (stack.top().type == TokenType::Operator){
                        if (getPrecedence(stack.top()) >= getPrecedence(token)){
                            result.push_back(stack.top());
                            stack.pop();
                        }
                        else break;
                    }
                    stack.push(token);
                }
                break;
            }
        }
        return index;
    };
    // make sure parentheses are balanced
    int parenCount = 0;
    for (const Token& token : tokens){
        if (token.token == "(") ++parenCount;
        else if (token.token == ")") --parenCount;
    }
    if (parenCount > 0)
        throw Error{"[Error]: More left parentheses than right"};
    else if (parenCount < 0)
        throw Error{"[Error]: More right parentheses than left"};
    // start processing
    stack.push(Token{TokenType::Parenthesis, "("});
    tokens.push_back(Token{TokenType::Parenthesis, ")"});
    for (int index = 0; index < tokens.size(); ++index){
        index = processToken(index);
    }
    return result;
}

//Takes postfix notation expression as a vector of tokens
double evaluateExpression(const std::vector<Token>& tokens, const std::map<std::string, double>& variables, const std::map<std::string, Function>& customFunctions){
    std::stack<double> operandStack;
    std::stack<double> ternaryOptionStack;
    for (const Token& token : tokens){
        if (token.type == TokenType::Number)
            operandStack.push(std::atof(token.token.c_str()));
        else if (token.type == TokenType::Identifier){
            if (auto it = defaultFunctions_arg1.find(token.token); it != defaultFunctions_arg1.end()){
                if (operandStack.size() < 1)
                    throw Error{std::string("[Error]: Not enough arguments passed to function '")+token.token+"'"};
                double operand = operandStack.top();
                operandStack.pop();
                operandStack.push(it->second(operand));
            }
            else if (auto it = defaultFunctions_arg2.find(token.token); it != defaultFunctions_arg2.end()){
                if (operandStack.size() < 2)
                    throw Error{std::string("[Error]: Not enough arguments passed to function '")+token.token+"'"};
                double operand2 = operandStack.top();
                operandStack.pop();
                double operand1 = operandStack.top();
                operandStack.pop();
                operandStack.push(it->second(operand1, operand2));
            }
            else if (auto it = defaultFunctions_arg3.find(token.token); it != defaultFunctions_arg3.end()){
                if (operandStack.size() < 3)
                    throw Error{std::string("[Error]: Not enough arguments passed to function '")+token.token+"'"};
                double operand3 = operandStack.top();
                operandStack.pop();
                double operand2 = operandStack.top();
                operandStack.pop();
                double operand1 = operandStack.top();
                operandStack.pop();
                operandStack.push(it->second(operand1, operand2, operand3));
            }
            else if (auto it = customFunctions.find(token.token); it != customFunctions.end()){
                double operand;
                std::map<std::string, double> tempVariables;
                for (auto i = it->second.argumentNames.rbegin(); i != it->second.argumentNames.rend(); ++i) {
                    if (!operandStack.empty()){
                        operand = operandStack.top();
                        operandStack.pop();
                    }
                    else
                        throw Error{std::string("[Error]: Not enough arguments passed to function '")+token.token+"'"};
                    tempVariables[*i] = operand;
                }
                operandStack.push(evaluateExpression(it->second.funcExpression, tempVariables, customFunctions));
            }
            else if (auto it = constants.find(token.token); it != constants.end())
                operandStack.push(it->second);
            else if (auto it = constants.find(token.token.substr(1, token.token.length()-1)); it != constants.end()){
                if (token.token[0] == '+')
                    operandStack.push(it->second);
                else if(token.token[0] == '-')
                    operandStack.push(-(it->second));
            }
            else if (auto it = variables.find(token.token); it != variables.end())
                operandStack.push(it->second);
            else if (auto it = variables.find(token.token.substr(1, token.token.length()-1)); it != variables.end()){
                if (token.token[0] == '+')
                    operandStack.push(it->second);
                else if(token.token[0] == '-')
                    operandStack.push(-(it->second));
            }
            else
                throw Error{std::string("[Error]: Unrecognized identifier '")+token.token+"'"};
        }
        else if (token.type == TokenType::Operator){
            double a, b;
            if ((operandStack.size() < 2 && token.token != "?") || (operandStack.size() < 1 && token.token == "?")){
                throw Error{std::string("[Error]: Operator does not have enough operands: ")+token.token};
            }
            else if (token.token == "?"){
                a = operandStack.top();
                operandStack.pop();
                b = -1;
            }
            else{
                b = operandStack.top();
                operandStack.pop();
                a = operandStack.top();
                operandStack.pop();
            }
            if (token.token == "%")
                operandStack.push(fmod(a, b));
            else if (token.token == "+")
                operandStack.push(a+b);
            else if (token.token == "-")
                operandStack.push(a-b);
            else if (token.token == "*")
                operandStack.push(a*b);
            else if (token.token == "/")
                operandStack.push(a/b);
            else if (token.token == "<")
                operandStack.push(a<b);
            else if (token.token == ">")
                operandStack.push(a>b);
            else if (token.token == "<=")
                operandStack.push(a<=b);
            else if (token.token == ">=")
                operandStack.push(a>=b);
            else if (token.token == "==")
                operandStack.push(a==b);
            else if (token.token == "&&")
                operandStack.push(a&&b);
            else if (token.token == "||")
                operandStack.push(a||b);
            else if (token.token == "^"){
                if (a < 0 && b < 1)
                    throw Error{std::string("[Error]: ")+std::to_string(a)+"^"+std::to_string(b)+" is not a number"};
                operandStack.push(pow(a, b));
            }
            else if (token.token == ":"){
                ternaryOptionStack.push(a);
                ternaryOptionStack.push(b);
            }
            else if (token.token == "?"){
                if (ternaryOptionStack.size() < 2)
                    throw Error{"[Error]: Ternary operator ? used without operator :"};
                double op2 = ternaryOptionStack.top();
                ternaryOptionStack.pop();
                double op1 = ternaryOptionStack.top();
                ternaryOptionStack.pop();
                operandStack.push(a?op1:op2);
            }
            else if (token.token != ","){
                throw Error{std::string("[Error]: Invalid operator: ")+token.token};
            }
        }
    }
    if (operandStack.size() > 1)
        throw Error{"[Error]: Unused operand(s)"};
    else if (ternaryOptionStack.size() > 0)
        throw Error{"[Error]: : operator used without ternary operator ?"};
    return operandStack.top();
}

//Overload that the user should call
double evaluateExpression(const std::string& expression, std::map<std::string, double>& variables, std::map<std::string, Function>& customFunctions){
    std::vector<Token> tokenized = tokenize(expression);
    if (auto it = std::find(tokenized.begin(), tokenized.end(), Token{TokenType::Operator, "="}); it != tokenized.end()){
        if (it == tokenized.begin()+1){
            std::vector<Token> rightSide = tokenized;
            rightSide.erase(rightSide.begin(), rightSide.begin()+2);
            double value = evaluateExpression(convertToPostfix(rightSide, customFunctions), variables, customFunctions);
            if (constants.find(tokenized[0].token) == constants.end())
                variables[tokenized[0].token] = value;
        }
        else{
            Function function{0, std::vector<std::string>{}, std::vector<Token>{}};
            if (tokenized[0].type != TokenType::Identifier || tokenized[1].token != "(" || (it-1)->token != ")")
                throw Error{"[Error]: Incorrect function syntax"};
            for (auto i=tokenized.begin()+2; i<it-1; i += 2){
                if (i->type != TokenType::Identifier)
                    throw Error{std::string("[Error]: Function parameter '")+i->token+"' is not a valid identifier"};
                else if (i != tokenized.begin()+2 && (i-1)->token != ",")
                    throw Error{"[Error]: Missing comma in function parameter list"};
                ++function.numArguments;
                function.argumentNames.push_back(i->token);
            }
            std::vector<Token> rightSide = tokenized;
            rightSide.erase(rightSide.begin(), find(rightSide.begin(), rightSide.end(), Token{TokenType::Operator, "="})+1);
            function.funcExpression = convertToPostfix(rightSide, customFunctions);
            if (defaultFunctions_arg1.find(tokenized[0].token) == defaultFunctions_arg1.end() && defaultFunctions_arg2.find(tokenized[0].token) == defaultFunctions_arg2.end() && defaultFunctions_arg3.find(tokenized[0].token) == defaultFunctions_arg3.end())
                customFunctions[tokenized[0].token] = function;
            else
                throw Error{std::string("[Error]: Cannot overwrite default function '")+tokenized[0].token+"'"};
        }
    }
    else
        return evaluateExpression(convertToPostfix(tokenized, customFunctions, true), variables, customFunctions);
    return 0;
}

void evaluateFile(const std::string& filePath){
    std::map<std::string, double> variables;
    std::map<std::string, Function> customFunctions;
    std::ifstream fin(filePath);
    if (!fin)
        throw Error{std::string("[Error]: File '")+filePath+"' does not exist"};
    std::string line;
    while (!fin.eof()) {
        getline(fin, line);
        if (!line.empty()){
            double result = evaluateExpression(line, variables, customFunctions);
            if (std::find(line.begin(), line.end(), '=') == line.end())
                std::cout << result << "\n";
        }
    }
}

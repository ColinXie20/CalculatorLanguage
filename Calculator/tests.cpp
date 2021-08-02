#include <iostream>

#include "tests.hpp"
#include "calculator.hpp"

void test_tokenize(const std::string& str){
    std::vector<Token> tokens = tokenize(str);
    for (const Token& token : tokens)
        std::cout << token.token << " ";
    std::cout << std::endl;
}

void test_convertPostfix(const std::string& str){
    std::vector<Token> postfix = convertToPostfix(tokenize(str), std::map<std::string, Function>{});
    for (const Token& token : postfix)
        std::cout << token.token << " ";
    std::cout << std::endl;
}

void test_evaluate(const std::string& str){
    std::map<std::string, double> variables;
    std::map<std::string, Function> functions;
    std::cout << str << " = " << evaluateExpression(str, variables, functions) << std::endl;
}

std::vector<std::string> tokenize_to_strings(const std::string& str){
    std::vector<Token> tokens = tokenize(str);
    std::vector<std::string> results;
    for (const Token& token : tokens)
        results.push_back(token.token);
    return results;
}

std::vector<std::string> convertPostfix_to_strings(const std::string& str){
    std::vector<Token> postfix = convertToPostfix(tokenize(str), std::map<std::string, Function>{});
    std::vector<std::string> results;
    for (const Token& token : postfix)
        results.push_back(token.token);
    return results;
}

void test_tokenize(){
    expect_eq(tokenize_to_strings("6.328 + (3.1416/  n )"),
              std::vector<std::string>{"6.328", "+", "(", "3.1416", "/", "n", ")"});
    expect_eq(tokenize_to_strings("a1 + a2 + a3"),
              std::vector<std::string>{"a1", "+", "a2", "+", "a3"});
    expect_eq(tokenize_to_strings("1 - (-1) + (-2)"),
              std::vector<std::string>{"1", "-", "(", "-1", ")", "+", "(", "-2", ")"});
    expect_eq(tokenize_to_strings("1 + sin(3.1416) * 2"),
              std::vector<std::string>{"1", "+", "sin", "(", "3.1416", ")", "*", "2"});
}

void test_convertPostfix(){
    expect_eq(convertPostfix_to_strings("1 % 2 % 3"),
              std::vector<std::string>{"1", "2", "%", "3", "%"});
    expect_eq(convertPostfix_to_strings("1 * (3+4)"),
              std::vector<std::string>{"1", "3", "4", "+", "*"});
    expect_eq(convertPostfix_to_strings("(1+2) / (3+4)"),
              std::vector<std::string>{"1", "2", "+", "3", "4", "+", "/"});
    expect_eq(convertPostfix_to_strings("sin(1.57)"),
              std::vector<std::string>{"1.57", "sin"});
    expect_eq(convertPostfix_to_strings("1 + sin(3.1416+1.57) * 2"),
              std::vector<std::string>{"1", "3.1416", "1.57", "+", "sin", "2", "*", "+"});
    expect_eq(convertPostfix_to_strings("1 + sin((3.1416+1)+1.57) * 2"),
              std::vector<std::string>{"1", "3.1416", "1", "+", "1.57", "+", "sin", "2", "*", "+"});
    expect_eq(convertPostfix_to_strings("sin(3.1416+cos(1.57))"),
              std::vector<std::string>{"3.1416", "1.57", "cos", "+", "sin"});
}

void test_evaluate(){
    std::map<std::string, double> variables;
    std::map<std::string, Function> functions;
    expect_near(evaluateExpression("19%3", variables, functions),
                1.0);
    expect_near(evaluateExpression("144^0.5", variables, functions),
                12.0);
    expect_near(evaluateExpression("1+2+3+4+5", variables, functions),
                15.0);
    expect_near(evaluateExpression("sin(pi/2)", variables, functions),
                1.0);
    expect_near(evaluateExpression("sin(3.1416)", variables, functions),
                0.0);
    expect_near(evaluateExpression("sin(abs(-3.1416)-pi/2)", variables, functions),
                1.0);
    expect_near(evaluateExpression("sin(+pi/2)", variables, functions),
                1.0);
    expect_near(evaluateExpression("1 ? 1 : 0", variables, functions),
                1.0);
    expect_near(evaluateExpression("0 ? 1 : sin(pi/2)-cos(0)", variables, functions),
                0.0);
}

void test_varsAndFuncs(){
    std::map<std::string, double> variables;
    std::map<std::string, Function> functions;
    evaluateExpression("a = 1", variables, functions);
    evaluateExpression("b = 2", variables, functions);
    evaluateExpression("multiplyByAHundred(n) = n*100", variables, functions);
    evaluateExpression("addAThousand(n) = n+1000", variables, functions);
    evaluateExpression("sumOfFive(a,b,c,d,e) = a+b+c+d+e", variables, functions);
    evaluateExpression("degreesToRadians(d) = d * ((2*pi)/360)", variables, functions);
    expect_near(evaluateExpression("a+b", variables, functions),
                3.0);
    expect_near(evaluateExpression("a-b", variables, functions),
                -1.0);
    expect_near(evaluateExpression("a*b", variables, functions),
                2.0);
    expect_near(evaluateExpression("a/b", variables, functions),
                0.5);
    expect_near(evaluateExpression("multiplyByAHundred(1)", variables, functions),
                100.0);
    expect_near(evaluateExpression("addAThousand(1)", variables, functions),
                1001.0);
    expect_near(evaluateExpression("sumOfFive(1,2,3,4,5)", variables, functions),
                15.0);
    expect_near(evaluateExpression("choice(1, 2, 3)", variables, functions),
                2.0);
    expect_near(evaluateExpression("choice(0, 2, 3)", variables, functions),
                3.0);
    expect_near(evaluateExpression("0? 1000*1.013 : sin(degreesToRadians(90))", variables, functions),
                1.0);
}

void test_exceptions(){
    
}

void runAllTests(){
    test_tokenize();
    test_convertPostfix();
    test_evaluate();
    test_varsAndFuncs();
    std::cout << "Tests Succeeded\n";
}

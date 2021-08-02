#pragma once
#include <string>
#include <vector>
#include <exception>
#include <cassert>

void test_tokenize(const std::string& str);

void test_convertPostfix(const std::string& str);

void test_evaluate(const std::string& str);

std::vector<std::string> tokenize_to_strings(const std::string& str);

std::vector<std::string> convertPostfix_to_strings(const std::string& str);

template<typename T>
void expect_eq(T a, T b){
    assert(a == b);
}

template<typename T>
void expect_near(T a, T b){
    assert(a-b <= 0.0001 && a-b >= -0.0001);
}

template<typename Callable>
void expect_throw(Callable code, const std::string& errMessage){
    try{
        code();
    }
    catch (const std::exception& err){
        assert(err.what() == errMessage);
        return;
    }
    assert(false);
}

template<typename Callable>
void expect_throw_any(Callable code){
    try{
        code();
    }
    catch (const std::exception& err){
        return;
    }
    assert(false);
}

template<typename Callable>
void expect_no_throw(Callable code){
    try{
        code();
    }
    catch (const std::exception& err){
        assert(false);
    }
}

void test_tokenize();

void test_convertPostfix();

void test_evaluate();

void test_varsAndFuncs();

void test_exceptions();

void runAllTests();

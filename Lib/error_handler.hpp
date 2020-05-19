#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

typedef void (*func_1)(void);
typedef void (*func_2)(int);
typedef void (*func_3)(int, const string &);
typedef void (*func_4)(int, const string &, vector<string> &);

void error_handle(func_1 callback);
void error_handle(func_2 callback, int lineno);
void error_handle(func_3 callback, int lineno, const string &str);
void error_handle(func_4 callback, int lineno, const string &str, vector<string> &argTypes);

#endif /* ERROR_HANDLER_HPP */
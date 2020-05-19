#include "error_handler.hpp"

void error_handle(func_1 callback)
{
  callback();
  exit(1);
}

void error_handle(func_2 callback, int lineno)
{
  callback(lineno);
  exit(1);
}

void error_handle(func_3 callback, int lineno, const string &str)
{
  callback(lineno, str);
  exit(1);
}

void error_handle(func_4 callback, int lineno, const string &str, vector<string> &argTypes)
{
  callback(lineno, str, argTypes);
  exit(1);
}

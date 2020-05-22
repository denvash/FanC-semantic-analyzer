#ifndef PARSER_HPP
#define PARSER_HPP

#include "output.hpp"
#include "semantic_table.hpp"
#include "error_handler.hpp"

using namespace std;

#define YYSTYPE yystype

extern SemanticTable semantic_table;
extern int yylineno;

class Node
{
public:
  virtual int get_value()
  {
    return 0;
  }
  virtual TypeEnum get_type()
  {
    return TYPE_UNDEFINED;
  }
};

struct yystype
{
  int i_value;
  TypeEnum e_type;
  string *str_value;
  Node *node;
};

void init_program();
void close_program();
void return_value(TypeEnum return_type);
void close_scope(bool is_function_scope);
void declare_function(yystype y_identifier, yystype y_arguments);

#endif /* PARSER_HPP */
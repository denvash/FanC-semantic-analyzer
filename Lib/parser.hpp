#ifndef PARSER_HPP
#define PARSER_HPP

#include "output.hpp"
#include "semantic_table.hpp"

using namespace std;
using namespace output;

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
  vector<TypeEnum> arguments_types;
  Node *node;
};

#endif /* PARSER_HPP */
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
void return_value_check(TypeEnum return_type);
void close_scope();
void declare_function(yystype y_identifier, yystype y_arguments);
void declare_formals(yystype yy_formals);

class Exp : public Node
{
public:
  int value;
  string id;
  TypeEnum type;

  Exp(string identifier)
  {
    if (!semantic_table.is_var_exists(identifier))
      error_handle(output::errorUndef, yylineno, identifier);
    auto entry = semantic_table.get_entry(identifier);
    id = identifier;
    type = entry.type_info.type;
  }

  Exp(yystype a, string op, yystype b); /* implemented in cpp */

  virtual int get_value()
  {
    return this->value;
  }
  virtual TypeEnum get_type()
  {
    return this->type;
  }
};

class FormalsList : public Node
{
public:
  vector<yystype> list;
  FormalsList(yystype yy_formal)
  {
    this->list.push_back(yy_formal);
  }

  FormalsList(yystype yy_formals_list, yystype additional)
  {
    FormalsList *formals_list = dynamic_cast<FormalsList *>(yy_formals_list.node);
    this->list.insert(this->list.begin(), formals_list->list.begin(), formals_list->list.end());
    this->list.push_back(additional);
  }
};

#endif /* PARSER_HPP */
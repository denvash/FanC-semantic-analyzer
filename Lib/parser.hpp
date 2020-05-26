#ifndef PARSER_HPP
#define PARSER_HPP

#include "output.hpp"
#include "semantic_table.hpp"
#include "error_handler.hpp"

using namespace std;

#define YYSTYPE yystype

extern SemanticTable semantic_table;
extern int yylineno;
extern int while_scope_count;

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
void func_init(yystype y_identifier, yystype y_arguments);
void declare_formals(yystype yy_formals);
void assign_value(yystype y_identifier, yystype y_expression);
void variable_init(yystype y_identifier, bool isLocal);

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

  FormalsList(yystype yy_formals_list, yystype yy_formal)
  {
    auto formals_list = dynamic_cast<FormalsList *>(yy_formals_list.node);
    this->list.insert(this->list.begin(), formals_list->list.begin(), formals_list->list.end());
    this->list.push_back(yy_formal);
  }
};

class IfExp : public Node
{
public:
  IfExp(yystype yy_exp)
  {
    if (yy_exp.e_type != TYPE_BOOL)
    {
      error_handle(output::errorMismatch, yylineno);
    }
  }
};

class Call : public Node
{
public:
  TypeEnum type;
  int value;

  Call(yystype identifier);

  Call(yystype identifier, yystype yy_exp_list); /* in .cpp */

  virtual TypeEnum get_type()
  {
    return this->type;
  }
};

class ExpList : public Node
{
public:
  vector<yystype> list;
  ExpList(yystype exp)
  {
    list.push_back(exp);
  }
  ExpList(yystype exp_list_yy, yystype yy_exp_added)
  {
    ExpList *exp_list = dynamic_cast<ExpList *>(exp_list_yy.node);
    this->list.insert(this->list.begin(), exp_list->list.begin(), exp_list->list.end());
    this->list.push_back(yy_exp_added);
  }
  virtual int get_value()
  {
    return list.front().i_value;
  }
  virtual TypeEnum get_type()
  {
    return list.front().e_type;
  }
};

#endif /* PARSER_HPP */
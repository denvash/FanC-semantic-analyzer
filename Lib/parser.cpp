#include "parser.hpp"

SemanticTable semantic_table;
int while_scope_count = 0;

/*
  TYPE_UNDEFINED,
  TYPE_VOID,
  TYPE_BOOL,
  TYPE_INT,
  TYPE_BYTE,
  TYPE_STRING,
  TYPE_B,
  HELP_TYPE_NUM_TO_STRING
  */

/* Order matters */
int type_size_map[HELP_TYPE_NUM] = {
    0, // TYPE_NONE
    0, // TYPE_VOID
    1, // TYPE_BOOL
    1, // TYPE_INT
    1, // TYPE_BYTE
    1, // TYPE_STRING
    1  // TYPE_B
};

/* Order matters */
string type_to_string_map[HELP_TYPE_NUM] = {
    "",
    "VOID",
    "BOOL",
    "INT",
    "BYTE",
    "STRING",
    "B",
};

/* Order matters */
bool converstion_map[HELP_TYPE_NUM][HELP_TYPE_NUM] = {
    /* TYPE_UNDEFINED to  */ {
        /* TYPE_UNDEFINED */ true,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ false,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ false,
        /* TYPE_B         */ false,
    },
    /* TYPE_VOID to       */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ true,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ false,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ false,
        /* TYPE_B         */ false,
    },
    /* TYPE_BOOL to       */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ true,
        /* TYPE_INT       */ false,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ false,
        /* TYPE_B         */ false,
    },
    /* TYPE_INT to        */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ true,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ false,
    },
    /* TYPE_BYTE to       */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ true,
        /* TYPE_BYTE      */ true,
        /* TYPE_STRING    */ false,
        /* TYPE_B         */ false,
    },
    /* TYPE_STRING to     */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ false,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ true,
        /* TYPE_B         */ false,
    },
    /* TYPE_B to */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ false,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ true,
        /* TYPE_B         */ true,
    }};

inline void debugParser(const char *text)
{
  debug("parser", text);
}

void init_program()
{
  semantic_table.init_table();
  semantic_table.open_scope();

  /* Insert print and printi to global scope */
  vector<TypeEnum> func_args;
  func_args.push_back(TYPE_STRING);

  /* info type:
    bool is_func;
    int size;
    TypeEnum type;
    vector<TypeEnum> arg_types;
  */
  var_info_t print_func = {true, 0, TYPE_VOID, func_args};

  func_args.pop_back();
  semantic_table.insert("print", &print_func, true);

  func_args.push_back(TYPE_INT);
  var_info_t printi_func = {true, 0, TYPE_VOID, func_args};
  semantic_table.insert("printi", &printi_func, true);
}

void close_scope()
{
  // debugParser("[parser:close-scope] closing scope");
  output::endScope();
  auto entries = semantic_table.get_current_inner_scope();
  for (int i = 0; i < entries.size(); i++)
  {
    auto type = entries[i]->type_info.type;
    auto type_str = type_to_string_map[type];
    auto id = entries[i]->name;
    auto offset = entries[i]->offset;

    // cout << "[parser:entry-id] " << id << endl;

    if (entries[i]->type_info.is_func)
    {
      auto function_entry = entries[i];
      vector<string> func_args;
      for (int i = 0; i < function_entry->type_info.arg_types.size(); i++)
      {
        auto arg_str = type_to_string_map[function_entry->type_info.arg_types[i]];
        arg_str = (arg_str == "VOID") ? "" : arg_str;
        func_args.push_back(arg_str);
      }
      auto function_return_type_str = type_to_string_map[function_entry->type_info.type];
      type_str = output::makeFunctionType(function_return_type_str, func_args);
      /* print special type */
    }

    output::printID(id, offset, type_str);
  }
  semantic_table.close_scope();
  // debugParser("[parser:close-scope] scope closed after printing IDs");
  /* print types */
}

void close_program()
{
  // debugParser("close program");
  if (!semantic_table.is_func_exists("main"))
  {
    // debugParser("No func exists");
    error_handle(output::errorMainMissing);
  }
  if (semantic_table.get_function_args("main").front() != TYPE_VOID)
  {
    // debugParser("Main with args");
    error_handle(output::errorMainMissing);
  }
  if (semantic_table.get_function_type("main") != TYPE_VOID)
  {
    // debugParser("wrong type");
    error_handle(output::errorMainMissing);
  }
  close_scope();
}

void return_value_check(TypeEnum return_type)
{
  // debugParser("return value check");
  bool validConversion = converstion_map[return_type][semantic_table.get_current_function_type()];
  if (!validConversion)
  {
    error_handle(output::errorMismatch, yylineno);
  }
}

void func_init(yystype y_identifier, yystype y_arguments)
{
  // debugParser("declaring function");
  // cout << "[parser:function] " << *y_identifier.str_value << endl;
  if (semantic_table.exists(*y_identifier.str_value))
  {
    error_handle(output::errorDef, yylineno, *y_identifier.str_value);
  }
  var_info_t func;
  func.is_func = true;
  func.type = y_identifier.e_type;

  /* if no arguments to function, args is null */
  if (y_arguments.node == NULL)
  {
    // debugParser("In declaring function, no arguments");
    func.arg_types.push_back(TYPE_VOID);
  }
  else
  {
    auto args = dynamic_cast<FormalsList *>(y_arguments.node);
    for (int i = 0; i < args->list.size(); i++)
    {
      func.arg_types.push_back(args->list[i].e_type);
    }
  }
  func.size = 0;
  semantic_table.insert(*y_identifier.str_value, &func, true);

  // semantic_table.open_scope();
  semantic_table.func_info = func;
}

void variable_init(yystype y_identifier, bool isLocal)
{
  // debugParser("Declaring var");
  // cout << "[parser:var]: " << type_to_string_map[y_identifier.e_type] << endl;
  // cout << "[parser:str_value]: " << *y_identifier.str_value << endl;
  if (semantic_table.exists(*y_identifier.str_value))
  {
    error_handle(output::errorDef, yylineno, *y_identifier.str_value);
  }
  var_info_t var;
  var.is_func = false;
  var.type = y_identifier.e_type;
  var.size = type_size_map[y_identifier.e_type];
  semantic_table.insert(*y_identifier.str_value, &var, isLocal);
}

void declare_formals(yystype yy_formals)
{
  if (yy_formals.node == NULL)
  {
    return;
  }
  auto formals_list = dynamic_cast<FormalsList *>(yy_formals.node);
  for (int i = 0; i < formals_list->list.size(); i++)
  {
    variable_init(formals_list->list[i], false);
  }
  /* reset offsets */
  semantic_table.offsets.top() = 0;
}

Exp::Exp(yystype a, string op, yystype b)
{
  /* check types */
  bool mismatch = false;
  if (op == "and")
  {
    if (a.e_type != TYPE_BOOL || b.e_type != TYPE_BOOL)
      error_handle(output::errorMismatch, yylineno);
    value = a.i_value && b.i_value;
    type = TYPE_BOOL;
  }
  else if (op == "or")
  {
    if (a.e_type != TYPE_BOOL || b.e_type != TYPE_BOOL)
      error_handle(output::errorMismatch, yylineno);
    value = a.i_value || b.i_value;
    type = TYPE_BOOL;
  }
  else if (op == "+")
  {
    if ((a.e_type != TYPE_INT && a.e_type != TYPE_BYTE) ||
        (b.e_type != TYPE_INT && b.e_type != TYPE_BYTE))
      mismatch = true;
    if (a.e_type == TYPE_INT || b.e_type == TYPE_INT)
      type = TYPE_INT;
    else
      type = TYPE_BYTE;
    value = a.i_value + b.i_value;
  }
  else if (op == "-")
  {
    if ((a.e_type != TYPE_INT && a.e_type != TYPE_BYTE) ||
        (b.e_type != TYPE_INT && b.e_type != TYPE_BYTE))
      mismatch = true;
    if (a.e_type == TYPE_INT || b.e_type == TYPE_INT)
      type = TYPE_INT;
    else
      type = TYPE_BYTE;
    value = a.i_value - b.i_value;
  }
  else if (op == "*")
  {
    if ((a.e_type != TYPE_INT && a.e_type != TYPE_BYTE) ||
        (b.e_type != TYPE_INT && b.e_type != TYPE_BYTE))
      mismatch = true;
    if (a.e_type == TYPE_INT || b.e_type == TYPE_INT)
      type = TYPE_INT;
    else
      type = TYPE_BYTE;
    value = a.i_value * b.i_value;
  }
  else if (op == "/")
  {
    if ((a.e_type != TYPE_INT && a.e_type != TYPE_BYTE) ||
        (b.e_type != TYPE_INT && b.e_type != TYPE_BYTE))
      mismatch = true;
    if (a.e_type == TYPE_INT || b.e_type == TYPE_INT)
      type = TYPE_INT;
    else
      type = TYPE_BYTE;
    // value = a.i_value / b.i_value;
  }
  else if (op == "<")
  {
    if ((a.e_type != TYPE_INT && a.e_type != TYPE_BYTE) ||
        (b.e_type != TYPE_INT && b.e_type != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.i_value < b.i_value;
  }
  else if (op == "<=")
  {
    if ((a.e_type != TYPE_INT && a.e_type != TYPE_BYTE) ||
        (b.e_type != TYPE_INT && b.e_type != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.i_value <= b.i_value;
  }
  else if (op == "==")
  {
    if ((a.e_type != TYPE_INT && a.e_type != TYPE_BYTE) ||
        (b.e_type != TYPE_INT && b.e_type != TYPE_BYTE))
    {
      mismatch = true;
    }
    type = TYPE_BOOL;
    value = a.i_value == b.i_value;
  }
  else if (op == ">=")
  {
    if ((a.e_type != TYPE_INT && a.e_type != TYPE_BYTE) ||
        (b.e_type != TYPE_INT && b.e_type != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.i_value >= b.i_value;
  }
  else if (op == ">")
  {
    if ((a.e_type != TYPE_INT && a.e_type != TYPE_BYTE) ||
        (b.e_type != TYPE_INT && b.e_type != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.i_value > b.i_value;
  }
  else if (op == "!=")
  {
    if ((a.e_type != TYPE_INT && a.e_type != TYPE_BYTE) ||
        (b.e_type != TYPE_INT && b.e_type != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.i_value != b.i_value;
  }
  if (mismatch)
  {
    error_handle(output::errorMismatch, yylineno);
  }
}

Call::Call(yystype identifier, yystype yy_exp_list)
{
  if (!semantic_table.is_func_exists(*identifier.str_value))
  {
    error_handle(output::errorUndefFunc, yylineno, *identifier.str_value);
  }

  auto exp_list = dynamic_cast<ExpList *>(yy_exp_list.node);
  auto exp_type_list = exp_list->list;
  auto args_type = semantic_table.get_function_args(*identifier.str_value);

  if (args_type.size() != exp_type_list.size())
  {
    // debugParser("Function Call: Arguments list size mismatch");
    // cout << "[parser:Call:expected list size] " << exp_type_list.size() << endl;
    // cout << "[parser:Call:args list size] " << args_type.size() << endl;
    vector<string> args;
    for (int i = 0; i < args_type.size(); i++)
    {
      args.push_back(type_to_string_map[args_type[i]]);
      auto typeVoid = type_to_string_map[TYPE_VOID];
      auto typeUndefined = type_to_string_map[TYPE_UNDEFINED];
      args.front() = args.front() == typeVoid ? typeUndefined : args.front();
    }
    error_handle(output::errorPrototypeMismatch, yylineno, *identifier.str_value, args);
  }

  for (int i = 0; i < args_type.size(); i++)
  {
    bool is_valid = converstion_map[exp_type_list[i].e_type][args_type[i]];
    if (!is_valid)
    {
      // debugParser("Function Call: Type mismatch");
      vector<string> args;
      for (int j = 0; j < args_type.size(); j++)
      {
        args.push_back(type_to_string_map[args_type[j]]);
      }
      error_handle(output::errorPrototypeMismatch, yylineno, *identifier.str_value, args);
    }
  }

  this->type = semantic_table.get_function_type(*identifier.str_value);
}

Call::Call(yystype identifier)
{
  if (!semantic_table.is_func_exists(*identifier.str_value))
  {
    error_handle(output::errorUndefFunc, yylineno, *identifier.str_value);
  }
  vector<TypeEnum> args_type = semantic_table.get_function_args(*identifier.str_value);
  if (args_type.size() != 1 || args_type.front() != TYPE_VOID)
  {
    vector<string> args;
    for (int j = 0; j < args_type.size(); j++)
    {
      args.push_back(type_to_string_map[args_type[j]]);
      args.front() = args.front() == "VOID" ? "" : args.front();
    }

    error_handle(output::errorPrototypeMismatch, yylineno, *identifier.str_value, args);
  }
  this->type = semantic_table.get_function_type(*identifier.str_value);
}

void assign_value(yystype y_identifier, yystype y_expression)
{
  if (!semantic_table.exists(*y_identifier.str_value))
  {
    error_handle(output::errorUndef, yylineno, *y_identifier.str_value);
  }
  auto entry = semantic_table.get_entry(*y_identifier.str_value);
  if (entry.type_info.is_func)
  {
    error_handle(output::errorUndef, yylineno, *y_identifier.str_value);
  }
  bool is_valid = converstion_map[y_expression.e_type][entry.type_info.type];
  if (!is_valid)
  {
    error_handle(output::errorMismatch, yylineno);
  }
}

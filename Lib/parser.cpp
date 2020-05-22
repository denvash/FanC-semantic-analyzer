#include "parser.hpp"

SemanticTable semantic_table;

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

int type_size_map[HELP_TYPE_NUM] = {
    0, // TYPE_NONE
    0, // TYPE_VOID
    1, // TYPE_BOOL
    1, // TYPE_INT
    1, // TYPE_BYTE
    1, // TYPE_STRING
    1  // TYPE_B
};

string type_to_string_map[HELP_TYPE_NUM] = {
    "",
    "VOID",
    "BOOL",
    "INT",
    "BYTE",
    "STRING",
    "B",
};

bool converstion_map[HELP_TYPE_NUM][HELP_TYPE_NUM] = {
    /* TYPE_UNDEFINED to */ {
        /* TYPE_UNDEFINED */ true,
        /* TYPE_VOID */ false,
        /* TYPE_BOOL */ false,
        /* TYPE_INT */ false,
        /* TYPE_BYTE */ false,
        /* TYPE_STRING */ false,
        /* TYPE_B */ false,

    },
    /* TYPE_VOID to */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID */ true,
        /* TYPE_BOOL */ false,
        /* TYPE_INT */ false,
        /* TYPE_BYTE */ false,
        /* TYPE_STRING */ false,
        /* TYPE_B */ false,
    },
    /* TYPE_BOOL to */ {/* TYPE_UNDEFINED */ false,
                        /* TYPE_VOID */ false,
                        /* TYPE_BOOL */ true,
                        /* TYPE_INT */ false,
                        /* TYPE_BYTE */ false,
                        /* TYPE_STRING */ false,
                        /* TYPE_B */ false},
    /* TYPE_INT to */ {/* TYPE_UNDEFINED */ false,
                       /* TYPE_VOID */ false,
                       /* TYPE_BOOL */ false,
                       /* TYPE_INT */ true,
                       /* TYPE_BYTE */ false,
                       /* TYPE_STRING */ false},
    /* TYPE_BYTE to */ {/* TYPE_UNDEFINED */ false,
                        /* TYPE_VOID */ false,
                        /* TYPE_BOOL */ false,
                        /* TYPE_INT */ true,
                        /* TYPE_BYTE */ true,
                        /* TYPE_STRING */ false,
                        /* TYPE_B */ false},
    /* TYPE_STRING to */ {/* TYPE_UNDEFINED */ false,
                          /* TYPE_VOID */ false,
                          /* TYPE_BOOL */ false,
                          /* TYPE_INT */ false,
                          /* TYPE_BYTE */ false,
                          /* TYPE_STRING */ true,
                          /* TYPE_B */ false},
    /* TYPE_B to */ {/* TYPE_UNDEFINED */ false,
                     /* TYPE_VOID */ false,
                     /* TYPE_BOOL */ false,
                     /* TYPE_INT */ false,
                     /* TYPE_BYTE */ false,
                     /* TYPE_STRING */ true,
                     /* TYPE_B */ true}};

inline void debugParser(const char *text)
{
  debug("parser", text);
}

void init_program()
{
  debugParser("init table");
  semantic_table.init_table();
  semantic_table.open_scope();

  /* Enter print and printi to global scope on program init */

  vector<TypeEnum> func_args;
  func_args.push_back(TYPE_STRING);

  /* info type:
    bool is_func;
    int size;
    TypeEnum type;
    vector<TypeEnum> arg_types;
  */
  type_info_t print_func = {true, 0, TYPE_VOID, func_args};

  semantic_table.insert("print", &print_func, true);

  func_args.push_back(TYPE_INT);
  type_info_t printi_func = {true, 0, TYPE_VOID, func_args};
  semantic_table.insert("printi", &printi_func, true);
}

void close_scope(bool is_function_scope)
{
  debugParser("closing scope");
  output::endScope();
  auto entries = semantic_table.get_current_inner_scope();
  for (int i = 0; i < entries.size(); i++)
  {
    TypeEnum type = entries[i]->type_info.type;
    string type_str = type_to_string_map[type];
    string id = entries[i]->name;
    int offset = entries[i]->offset;

    if (entries[i]->type_info.is_func)
    {
      auto function_entry = entries[i];
      vector<string> func_args;
      for (int i = 0; i < function_entry->type_info.arg_types.size(); i++)
      {
        string arg_str = type_to_string_map[function_entry->type_info.arg_types[i]];
        arg_str = (arg_str == "VOID") ? "" : arg_str;
        func_args.push_back(arg_str);
      }
      string function_return_type_str = type_to_string_map[function_entry->type_info.type];
      type_str = output::makeFunctionType(function_return_type_str, func_args);
      /* print special type */
    }

    output::printID(id, offset, type_str);
  }
  semantic_table.close_scope();
  /* print types */
}

void close_program()
{
  debugParser("close program");
  if (!semantic_table.is_func_exists("main"))
  {
    debugParser("No func exists");
    error_handle(output::errorMainMissing);
  }
  if (semantic_table.get_function_args("main").front() != TYPE_VOID)
  {
    debugParser("Main with args");
    error_handle(output::errorMainMissing);
  }
  if (semantic_table.get_function_type("main") != TYPE_VOID)
  {
    debugParser("wrong type");
    error_handle(output::errorMainMissing);
  }
  close_scope(false);
}

void return_value_check(TypeEnum return_type)
{
  debugParser("return value check");
  bool validConversion = converstion_map[return_type][semantic_table.get_current_function_type()];
  if (!validConversion)
  {
    error_handle(output::errorMismatch, yylineno);
  }
}

void declare_function(yystype y_identifier, yystype y_arguments)
{
  debugParser("declaring function");
  if (semantic_table.exists(*y_identifier.str_value))
  {
    error_handle(output::errorDef, yylineno, *y_identifier.str_value);
  }
  type_info_t func;
  func.is_func = true;
  func.type = y_identifier.e_type;

  /* if no arguments to function, args is null */
  if (y_arguments.node == NULL)
  {
    debugParser("In declaring function, no arguments");
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

  semantic_table.open_scope();
  semantic_table.func_info = func;
}

void declare_var(yystype y_identifier, bool isLocal)
{

  if (semantic_table.exists(*y_identifier.str_value))
  {
    error_handle(output::errorDef, yylineno, *y_identifier.str_value);
  }
  type_info_t var;
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
    declare_var(formals_list->list[i], false);
  }
  /* reset offsets */
  semantic_table.offsets.top() = 0;
}

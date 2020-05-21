#include "parser.hpp"

SemanticTable semantic_table;

/*
  TYPE_B,
  TYPE_BOOL,
  TYPE_BYTE,
  TYPE_INT,
  TYPE_VOID,
  TYPE_STRING,
  TYPE_UNDEFINED,
  HELP_TYPE_NUM_TO_STRING
  */

string type_to_string_map[HELP_TYPE_NUM_TO_STRING] = {
    "B",
    "BOOL",
    "BYTE",
    "INT",
    "VOID",
    "STRING"
    "", // TYPE_UNDEFINED,
};

void init_program()
{
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

  func_args.pop_back();
  semantic_table.insert("print", &print_func, true);

  func_args.push_back(TYPE_INT);
  type_info_t printi_func = {true, 0, TYPE_VOID, func_args};
  semantic_table.insert("printi", &printi_func, true);
}

void close_scope(bool is_function_scope)
{
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
  if (!semantic_table.is_func_exists("main"))
  {
    error_handle(output::errorMainMissing);
  }
  if (semantic_table.get_function_args("main").front() != TYPE_UNDEFINED)
  {
    error_handle(output::errorMainMissing);
  }
  if (semantic_table.get_function_type("main") != TYPE_VOID)
  {
    error_handle(output::errorMainMissing);
  }
  close_scope(false);
}
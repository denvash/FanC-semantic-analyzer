#ifndef SEMANTIC_TABLE_HPP
#define SEMANTIC_TABLE_HPP

#include "types.hpp"
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

#define EMPTY_ENTRY ""

using namespace std;

typedef struct
{
  bool is_func;
  int size;
  TypeEnum type;
  vector<TypeEnum> arg_types;
} type_info_t;

typedef struct
{
  int offset;
  string name;
  type_info_t type_info;
} table_entry_t;

typedef struct table_t
{
  vector<table_entry_t *> table;
  table_t *parent;
  vector<table_entry_t *>::iterator find(string name)
  {
    for (int i = 0; i < table.size(); i++)
    {
      if (table[i]->name == name)
      {
        return table.begin() + i;
      }
    }
    return table.end();
  }
} table_t;

inline void debug(char const *target, char const *text)
{
  cout << "[" << target << "] " << text << endl;
}

inline void debugTable(char const *text)
{
  debug("table", text);
}

class SemanticTable
{
private:
  stack<table_t *> tables;

public:
  stack<int> offsets;
  vector<table_entry_t *> func_scopes;
  type_info_t func_info;

  void init_table()
  {
    auto firstTable = new table_t();
    firstTable->parent = NULL;
    func_scopes = vector<table_entry_t *>();
    tables.push(firstTable);
    offsets.push(0);
  }

  void close_scope()
  {
    table_t *tmp = tables.top();
    tables.pop();
    offsets.pop();
  }

  void open_scope()
  {
    auto table = new table_t();
    table->parent = tables.top();
    tables.push(table);
    offsets.push(offsets.top());
    debugTable("open scope");
  }

  bool exists(string name)
  {
    bool found = false;
    for (table_t *currentTable = tables.top();
         currentTable->parent;
         currentTable = currentTable->parent)
    {
      vector<table_entry_t *>::iterator search = currentTable->find(name);
      if (search != currentTable->table.end())
      {
        found = true;
        break;
      }
    }
    return found;
  }

  vector<table_entry_t *> get_current_inner_scope()
  {
    return tables.top()->table;
  }

  void insert(string name, type_info_t *type, bool is_local)
  {
    // debugTable("insert table entry");
    int offset = offsets.top();
    if (is_local)
    {
      offsets.top() += type->size;
    }
    else
    {
      offset -= type->size;
      offsets.top() -= type->size;
    }
    auto entry = new table_entry_t();
    entry->name = name;
    entry->offset = offset;
    entry->type_info = *type;

    if (entry->type_info.is_func)
    {
      entry->offset = 0;
      func_scopes.push_back(entry);
    }
    tables.top()->table.push_back(entry);
  }

  table_entry_t get_entry(string name)
  {
    auto entry = new table_entry_t();
    entry->name = "";
    entry->offset = 0;
    entry->type_info.type = TYPE_UNDEFINED;

    vector<table_entry_t *>::iterator search;
    for (table_t *currentTable = tables.top();
         currentTable->parent;
         currentTable = currentTable->parent)
    {
      search = currentTable->find(name);
      if (search != currentTable->table.end())
      {
        entry = *search;
      }
    }
    if (entry->name == "")
    {
      for (vector<table_entry_t *>::iterator i = func_scopes.begin(); i != func_scopes.end(); i++)
      {
        auto possibleMain = *i;
        if (possibleMain->name == name)
        {
          entry = possibleMain;
        }
      }
    }
    return *entry;
  }

  bool is_func_exists(string name)
  {
    auto table_entry = get_entry(name);
    if (table_entry.name != EMPTY_ENTRY)
    {
      return table_entry.type_info.is_func;
    }
    return false;
  }

  /* return [TYPE_UNDEFINED] if not exists */
  vector<TypeEnum> get_function_args(const string &identifier)
  {
    debugTable("Getting functions args");
    if (!this->is_func_exists(identifier))
    {
      debugTable("Func not exists");
      return vector<TypeEnum>(1, TYPE_UNDEFINED);
    }
    return this->get_entry(identifier).type_info.arg_types;
  }

  TypeEnum get_function_type(const string &identifier)
  {
    if (!this->is_func_exists(identifier))
    {
      return TYPE_UNDEFINED;
    }
    return this->get_entry(identifier).type_info.type;
  }

  table_entry_t *get_last_function_in_scope()
  {
    return func_scopes.back();
  }

  TypeEnum get_current_function_type()
  {
    return this->func_info.type;
  }

  bool is_var_exists(string name)
  {
    if (exists(name) && !get_entry(name).type_info.is_func)
    {
      return true;
    }
    return false;
  }

  ~SemanticTable(){};
};

#endif
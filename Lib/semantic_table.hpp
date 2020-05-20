#ifndef SEMANTIC_TABLE_HPP
#define SEMANTIC_TABLE_HPP

#include "types.hpp"
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

using namespace std;

typedef struct variable_t
{
  bool is_func;
  int preconditions_count;
  int size;
  TypeEnum type;
  vector<TypeEnum> arguments_types;
} variable_t;

typedef struct
{
  int offset;
  string name;
  variable_t type;
} tableEntry;

typedef struct table_t
{
  vector<tableEntry> table;
  table_t *parent;
  vector<tableEntry>::iterator find(string name)
  {
  }
} table_t;

class SemanticTable
{
private:
  stack<table_t *> tables;

public:
  stack<int> offsets;
  vector<tableEntry *> scopes;
  variable_t func_info;

  void initTable()
  {
    table_t *table = new table_t();
    table->parent = NULL;
    scopes = vector<tableEntry *>();
    tables.push(table);
    offsets.push(0);
  }

  void pushNewScope(){
      table_t *table = new table_t();
      table->parent = tables.top();
      tables.push(table);
      offsets.push(offsets.top());
      offsets.top()++;
  }

    void popScope(){
        tables.pop();
        offsets.pop();
    }

    void insertEntry(string name){
      tableEntry newTableEntry;
      newTableEntry.name=name;
      newTableEntry.offset=(offsets.top());


  ~SemanticTable(){};
};

#endif
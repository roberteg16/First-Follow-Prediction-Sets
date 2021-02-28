#ifndef __COMMON_H__
#define __COMMON_H__

#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#define EPSILON "epsilon"
#define START "Start"
#define ENDL '\n'
#define MAX_ITERATIONS 100

using namespace std;

typedef unordered_map<string, set<string>> Result;
typedef vector<string> Production;
typedef vector<Production> Productions;
typedef unordered_map<string, Productions> Rules;
typedef Rules ProductionSet;
typedef unordered_map<string, vector<pair<string, vector<string>>>>
    Init_data_following;

bool is_nonterminal(const Rules &rules, const string &symbol);
bool is_terminal(const Rules &rules, const string &symbol);

#endif

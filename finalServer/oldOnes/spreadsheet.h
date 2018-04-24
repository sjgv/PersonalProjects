#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include<string>
#include<map>
#include<stack>
#include<mutex>

class spreadsheet
{
 private:
  std::string filename;
  std::map<std::string, std::stack<std::string> > reverts;
  std::map<std::string, std::string> content;
  std::stack<std::string> undos;

 public:
  spreadsheet();
  ~spreadsheet();

  std::string revert(std::string cell);
  std::string undo();
  void addContents(std::string cell, std::string contents);
  void addRevertHistory(std::string cell, std::string contents);
  void addUndoHistory(std::string cell, std::string contents);
};

#endif //SPREADSHEET_H

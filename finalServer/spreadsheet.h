#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include<string>
#include<map>
#include<deque>
#include<mutex>
#include<fstream>
#include<iostream>

class spreadsheet
{
 private:
  std::string filename;
  std::map<std::string, std::deque<std::string> > reverts;
  std::map<std::string, std::string> content;
  std::deque<std::string> undos;


 public:
  spreadsheet();
  ~spreadsheet();

  std::string revert(std::string cell);
  std::string undo();

  void addContents(std::string cell, std::string contents);
  void addRevertHistory(std::string cell, std::string contents);
  void addUndoHistory(std::string cell, std::string contents);

  std::map<std::string, std::string> * getContents();
  std::map<std::string, std::deque<std::string> > * getReverts();
  std::deque<std::string> * getUndos();
  std::string getCellContents(std::string cellName);

  void save(std::string filename);
  void load(std::string filename);
  //FOR TESTING: DELETE THIS METHOD
  void printValues();
};

#endif //SPREADSHEET_H


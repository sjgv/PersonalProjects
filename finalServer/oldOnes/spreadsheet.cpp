#include"spreadsheet.h"

using namespace std;
std::mutex loc;
spreadsheet::~spreadsheet()
{
}
spreadsheet::spreadsheet()
{
}
std::string spreadsheet::revert(std::string cell)
{
  string toReturn = "";
  loc.lock();
  if(reverts[cell].size() > 0)
    {
      toReturn = reverts[cell].top();
      reverts[cell].pop();
    }
  loc.unlock();
  return toReturn;
}
std::string spreadsheet::undo()
{
  string toReturn = "";
  loc.lock();
  if(undos.size() > 0)
    {
      toReturn = undos.top();
      undos.pop();
    }
  loc.unlock();
  return toReturn;
}
void spreadsheet::addContents(std::string cell, std::string contents)
{
  loc.lock();
  content[cell] = contents;
  loc.unlock();
}
void spreadsheet::addRevertHistory(std::string cell, std::string contents)
{
  loc.lock();
  reverts[cell].push(contents);
  loc.unlock();
}
void spreadsheet::addUndoHistory(std::string cell, std::string contents)
{
  loc.lock();
  undos.push(cell+contents);
  loc.unlock();
}

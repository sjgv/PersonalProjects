#include"spreadsheet.h"

using namespace std;
std::mutex loc;
spreadsheet::spreadsheet()
{
}
spreadsheet::~spreadsheet()
{
}
std::string spreadsheet::revert(std::string cell)
{

  /* FOR DEBUGGING
  for (std::map<string,deque<string> >::iterator it=reverts.begin(); it!=reverts.end(); ++it)
    {
      //outgoing << it->first << " ";
      
      for(deque<string>::iterator iter = reverts[it->first].begin(); iter!=reverts[it->first].end(); iter++)
	{
	  cout <<"revert contents = " << it->first << ":" << *iter << "\n";
	}
      
      //outgoing << "\n";
      }
  */
  string toReturn = "";
  loc.lock();
  if(reverts[cell].size() > 0)
    {
      //cout<<"IN MAGNIFICENT IF"<<endl;
      //Check what to return and pop from deque
      toReturn = reverts[cell].front();
      reverts[cell].pop_front();
      
      deque<string>::iterator it;
      for(it = undos.begin(); it != undos.end(); it++)
	{
	  if(it->compare(0, cell.length(), cell) == 0)
	    break;
	  //cout << "cell = " << cell << " matched cell= " << *it << endl; 
	}
	undos.erase(it);
	undos.push_front(cell + ":" + content[cell]);
	content[cell] = toReturn;
     
    }
  loc.unlock();
  return toReturn; //To return returns the contents only 
}
std::string spreadsheet::undo()
{
  string toReturn = "";
  loc.lock();
  if(undos.size() > 0)
    {
      toReturn = undos.front();
      undos.pop_front();
      int i;
      for(i = 0; i < toReturn.length(); i++)
	{
	  if(toReturn[i] == ':')
	    break;
	}
      string cellName = toReturn.substr(0, i);
      string contents = toReturn.substr(i+1, toReturn.length());
      content[cellName] = contents;
      // toReturn = reverts[cellName].front();
      reverts[cellName].pop_front();
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
  reverts[cell].push_front(contents);
  loc.unlock();
}
void spreadsheet::addUndoHistory(std::string cell, std::string contents)
{
  loc.lock();
  undos.push_front(cell+":"+contents);
  loc.unlock();
}
string spreadsheet::getCellContents(string cellName)
{
  return content[cellName];
}

std::map<std::string, std::string> * spreadsheet::getContents()
{
  return &content;
}
std::map<std::string, std::deque<string> > * spreadsheet::getReverts()
{
  return &reverts;
}
std::deque<string> * spreadsheet::getUndos()
{
  return &undos;
}
void spreadsheet::save(string filename)
{
  ofstream outgoing;
  outgoing.open(filename);
  for (std::map<string,string>::iterator it=content.begin(); it!=content.end(); ++it)
    {
      if(it->second.compare("") == 0)
	outgoing << it->first << " " << (char)7 << "\n";
      else
	outgoing << it->first << " " << it->second << "\n";
    }
  outgoing << "REVERTS\n";
   
 for (std::map<string,deque<string> >::iterator it=reverts.begin(); it!=reverts.end(); ++it)
    {
      //outgoing << it->first << " ";
      for(deque<string>::iterator iter = reverts[it->first].begin(); iter!=reverts[it->first].end(); iter++)
	{
	  outgoing << it->first << ":" << *iter << "\n";
	}
      //outgoing << "\n";
    }
  outgoing << "UNDOS\n";
  for(deque<string>::iterator itr = undos.begin(); itr!=undos.end(); itr++)
    {
      outgoing << *itr << "\n";
    }
  
  outgoing.close();
}

void spreadsheet::load(string filename)
{
  ifstream incoming(filename);

  string income;
  string contents;
  //incoming >> income;
  getline(incoming,income);
  while(income != "REVERTS")
    {
      if(incoming.fail())
	break;
      /*incoming >> contents;
      if(contents[0] == (char)7)
	contents = "";
      */   
      int i ;
      for(i = 0; i < income.length(); i++)
	{
	  if(income[i] == ' ')
	    break;
	}
      string cellName = income.substr(0,i);
      contents = income.substr(i+1,income.length()-1);
      content[cellName] = contents;
      //      incoming >> income;
      getline(incoming,income);
    }
  getline(incoming,income);
  while(income != "UNDOS" && income.length() > 0)
    {
      //cout<<"INCOM="<<income<<endl;
      if(incoming.fail())
	break;
      int i;
      for(i = 0; i < income.length(); i++)
	{
	  if(income[i] == ':')
	    break;
	}
      reverts[income.substr(0,i)].push_back(income.substr(i+1,income.length()-1)); 
      getline(incoming,income);
      // incoming >> income;
    }
  //incoming >> income;
  getline(incoming,income);
  while(true)
    {
      if(incoming.fail())
	break;
      undos.push_back(income);// + " " + contents);
      getline(incoming,income);
      // incoming >> income;
    }
  incoming.close();
}

void spreadsheet::printValues()
{
  map<string, string>::iterator it;

  /*FOR DEBUGGING
for ( it = content.begin(); it != content.end(); it++ )
{
    std::cout << it->first  // string (key)
              << ':'
              << it->second   // string's value 
              << std::endl ;
}
  */
}

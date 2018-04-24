/* Server code written by : Sara Chadwick and Sal Gutierrez
 * with help from Mia Ngo
 * Written for CS 3505 Spreadsheet (Project #2)
 * April 2018
 */

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <unistd.h> //For read() and write() funcs
#include <pthread.h> //For POSIX multithreading
#include <signal.h> //For ignoring sig pipes
#include<vector> //For Vectors
#include<algorithm> //For remove and whitespace trim
#include<ctype.h>
#include<map>
#include<stack>
#include<fstream>
#include <boost/algorithm/string.hpp>
#include"commInterface.h"
#include"spreadsheet.h"
#define PORT 2112
using namespace std;
/*-------------------------[GLOBALS]------------------------------*/
static unsigned int clientCount = 0;
static int uid = 0;
bool saveBeforeExit = false;
bool startPinging = false;
/*-------------------------[STRUCTS]------------------------------*/
/*Client structure*/
typedef struct{
  struct sockaddr_in addr;
  int connfd;
  int uid;
  bool readyToPing = false;
  char bufferOut[1024];
  char bufferIn[1024];
}clientStruct;

/* Arguments struct*/
typedef struct{
  int listenfd, connfd, currentUid;
  sockaddr_in * clientAddress;
  vector<clientStruct> * clients;
  vector<commInterface> * interfaces; 
}argsStruct;

/* Arguments for handlingClient */
typedef struct{
  vector<clientStruct> * clients;
  vector<commInterface> * interfaces;
  int uid;
}handlingClientArgs;

/*-------------------------[FUNCTIONS/METHODS]------------------------------*/

/* Adds client clientStruct to vector */
void addClient(clientStruct * client, vector<clientStruct> * clients, vector<commInterface> * interfaces)
{
  commInterface interface;
  clients->push_back(*client);
  interfaces->push_back(interface);
  return;
}

/*Strips a string from whitespace */ 
void stripWhiteSpace(string *  message)
{
  int i=0;
  if(isspace(message->at(0)))
    {
      for(i = 0; i < message->length(); i++)
	{
	  if(isspace(message->at(0)));
	  break;
	}
      message->erase(0,i);
    }
}

/* Used for error printing 
   Abandoned...?? yes*/
void errorHelper(string msg)
{
  cout<<msg<<endl;
  exit(1);
}

/*This method runs on it's own thread
 * it takes a struct of arguments and
 * runs a while loop constantly checking 
 * bufferIn for commands
 */ 
void * handleRead(void * arguments)
{
  handlingClientArgs * argumentz = (handlingClientArgs *) arguments;
  int uid = argumentz->uid;
  clientStruct client = argumentz->clients->at(uid);
  int rlen;
  string strToCopy = ""; //Everything gets concatenated here (like a string builder)
  string offset = ""; //Remainder of message that we don't want but that goes at beginning of Buffer in once a msg has been parsed
  char terminator = (char)3;
  while((rlen = read(client.connfd, client.bufferIn, sizeof(client.bufferIn)-1)) > 0)
    {
      client.bufferIn[rlen] = '\0';
      client.bufferOut[0] = '\0';

      if(strlen(client.bufferIn) == 0)
	continue;
      strToCopy += offset;
      offset = "";
      /*For loop checks until we detect Terminating char*/
      for(int i = 0; client.bufferIn[i] != '\0'; i++)
	{
	  /*This makes sure we leave a white space when edit and load are called so that 
	   we can have strings and file names with spaces*/
	  if(isspace(client.bufferIn[i]) && !(strToCopy.compare(0,4,"edit") == 0) && !(strToCopy.compare(0,4,"load") == 0))
	    {
	    continue;
	    }
	  strToCopy += client.bufferIn[i];
	  if(client.bufferIn[i] == terminator)
	    /*Since ping and ping_response are time sensitive we check for them here
	      rather than in processMessage */
	    {
	      if(strToCopy.compare(0,4,"ping")==0)
		{
		  string pingCheck = "ping_";
		  if(strToCopy.compare(0,5,pingCheck)==0)
		    {
		      argumentz->interfaces->at(uid).setPingResponse(true);
		    }
		  else
		    {
		      argumentz->interfaces->at(uid).setPing(true);
		    }
		}
	      //Add to dequeIn
	      else
		{
		  argumentz->interfaces->at(uid).pushIn(strToCopy);
		}
	      strToCopy = "";
	    }
	}
      sleep(0.2);
    }
}

/*This method runs on it's own thread
  It takes care of writing all the data than needs to be send to clients 
  It takes an argument struct*/
void * handleWrite(void * arguments)
{
  handlingClientArgs * argumentz = (handlingClientArgs *) arguments;
  int uid = argumentz->uid;
  clientStruct client = argumentz->clients->at(uid);  
  string message = "";
  char terminator = (char)3;
  string temp = "ping_response ";
  temp += terminator;
  const char * pingString = temp.c_str();
  string temp2 = "ping ";
  temp2 += terminator;
  const char * sendPing = temp2.c_str();
  string temp3 = "disconnect ";
  temp3 += terminator;
  const char * disconnect = temp3.c_str();
 
  while(1)
    {
      //Check our time sensitive boolean flags here 
      if(argumentz->interfaces->at(uid).gotPing() || argumentz->interfaces->at(uid).getSendPing() || argumentz->interfaces->at(uid).getDisconnect() )
	{
	  if(argumentz->interfaces->at(uid).getDisconnect())
	    {
	      if((write(client.connfd, disconnect, strlen(disconnect)) < 0))
		  printf("disconnect error");
	      close(client.connfd);
	      argumentz->interfaces->at(uid).setDisconnect(false);
	      argumentz->interfaces->at(uid).setDisconnected(true);

	    }
	  else
	    {
	      if(argumentz->interfaces->at(uid).gotPing() && !argumentz->interfaces->at(uid).getDisconnected())
		{
		  argumentz->interfaces->at(uid).setPing(false);
		  //cout << "Writing to client #" << uid << " " << pingString << endl;
		  if((write(client.connfd, pingString, strlen(pingString)) < 0))
		    {
		      printf("Error sending ping response");
		    }	   
		}
	      //If we get the ping_response msg
	      if(argumentz->interfaces->at(uid).getSendPing() && !argumentz->interfaces->at(uid).getDisconnected())
		{
		  argumentz->interfaces->at(uid).setSendPing(false);
		  //cout << "Writing to client #" << uid << " " << sendPing << endl;
		  if((write(client.connfd, sendPing, strlen(sendPing)) < 0))
		    {
		      printf("Error sending our ping");
		    }	   
		}
	    }
	}
      //Now Check outbound message we need to write
      else if(argumentz->interfaces->at(uid).checkOutboundSize() > 0)
	{
	  message  = argumentz->interfaces->at(uid).popOut()+terminator;
	  
	  /*Erasing in cplus means filling with a character of our choice (we're just clearing the buffer here)
	    So we don't get garbage afterwards*/
	  for(int i = 0; i < sizeof(client.bufferOut)-1; i++)
	      client.bufferOut[i] = (char)0;
	  strcpy(client.bufferOut,message.c_str());
	  
	  //cout << "Writing to client #" << uid << " " << message << endl;
	  if((write(client.connfd, client.bufferOut, sizeof(client.bufferOut)-1) < 0))
	      printf("Error sending message general");
	}
      //This is so that our processor doesn't overload
      sleep(0.2);
    }
}



/* Handles all the communication 
 In new threads for every client
 Updates the client count
 Waits to hear ping_response within 60 seconds
 arguments:
    -clients
    -interfaces
    -uid
*/
void * handleClient(void * arguments)
{
  handlingClientArgs * argumentz = (handlingClientArgs *) arguments;  
  clientCount++;
 
  pthread_t tid;
  pthread_create(&tid, NULL, &handleRead,arguments);
  pthread_t tid2;
  pthread_create(&tid2, NULL, &handleWrite,arguments);
    while (1)
    {
      //Check if flag has been turned to true
      if(argumentz->clients->at(argumentz->uid).readyToPing)
	{
	  //Sleep first so we don't immediately disconnect
        sleep(60);
	//Go through all members of interfaces, only ping if we haven't been disconnected
        for(int j = 0; j < argumentz->interfaces->size(); j++)
	{
	  if(argumentz->clients->at(j).readyToPing && !(argumentz->interfaces->at(j).getDisconnected()))
	     {
	       if(!argumentz->interfaces->at(j).getPingResponse())
		 { 
		   //disconnect 
		   argumentz->interfaces->at(j).setDisconnect(true);
		 }
	       else
		 {
		   argumentz->interfaces->at(j).setPingResponse(false);
		 }
	     }
	}
    }
    }
}

/*This function just listens for clients to connect, accepts connection
  Populates clientStruct with values and inputs clientStruct into its data structure
  Then we call handleClient on a different thread*/
void * listenForClients(void * arguments)
{
  argsStruct * argumentz = (argsStruct *) arguments; 
  int listenfd = argumentz->listenfd;
  int connfd = argumentz->connfd;
  sockaddr_in  clientAddress = *argumentz->clientAddress;
  vector<clientStruct> * clients = argumentz->clients;
  vector<commInterface> * interfaces = argumentz->interfaces;
  string acknowledgement = "connect_accepted";

  /* Listen */
  if(listen(listenfd, 5) < 0)
    errorHelper("ERROR: listening failed");

  /* Accept clients */
  while(1)
    {
      socklen_t clientLength = sizeof(clientAddress);
      connfd = accept(listenfd, (struct sockaddr *)&clientAddress, &clientLength);    

      /* Check if file descriptor got mangled */
      if(connfd < 0)                       
	{
	  //printf("ERROR\n");
	  //printf("CANNOT CONNECT");
	  shutdown(connfd,2);
	  close(connfd);
	  continue;
	}
      /*Client settings */
      clientStruct * client = (clientStruct *) malloc(sizeof(clientStruct));
      client->addr = clientAddress; 
      client->connfd = connfd;
      client->uid = uid;
      //Add client to array of structs
      addClient(client, clients,interfaces);
      //cout<<"New client connected"<<clients->at(uid).connfd<<endl;
      sleep(0.2);
      //Create struct for every client
      handlingClientArgs * clientArgs = (handlingClientArgs *)malloc(sizeof(handlingClientArgs));
      clientArgs->clients = clients;
      clientArgs->interfaces = interfaces;
      clientArgs->uid = uid;
      /*Increment the uid */
      uid++;
      pthread_t tid;
      pthread_create(&tid, NULL, &handleClient,clientArgs);
    }
}

/*Sends the cellNumber:contents for every cell */
void sendFullState(map<string, spreadsheet> * spreadsheets, string name, vector<commInterface> * interfaces, int uid, vector<clientStruct> *clients)
{
  map<string,string>::iterator it;
  string toSend = "full_state ";
  for (it=spreadsheets->at(name).getContents()->begin(); it!=spreadsheets->at(name).getContents()->end(); ++it)
    {
      toSend += it->first + ":" + it->second + "\n";
    }
  //We start pinging after full_state
  startPinging = true; 
  clients->at(uid).readyToPing = true;
  interfaces->at(uid).pushOut(toSend);
}
/*Checks to see if spreadsheet exists
  If it does it will load it, if it doesn't
  it will create a new one and add to persistece file*/
void load(map<string, spreadsheet> * spreadsheets, string name, vector<commInterface> * interfaces, int uid, vector<string> * savedFiles, vector<clientStruct> * clients)
{
  //This function only loads NEW spreadsheets; if the spreadsheet
  //is already running on the server, it does nothing.
  try {
    spreadsheets->at(name);
  } catch( out_of_range e1 ) {
    bool exists = false;
    for(int i = 0; i < savedFiles->size(); i++)
      {
	if(savedFiles->at(i).compare(name) == 0)
	  {
	    exists = true;
	  }
      }
    if(exists == false)
      {
	ofstream out;
	//the app flag means append
	out.open("saved_spreadsheets.txt", std::ios::app);
	out<<name<<"\n";
	savedFiles->push_back(name);
	out.close();
      }
    spreadsheet newSpread;
    spreadsheets->insert(pair<string, spreadsheet>(name, newSpread));
    spreadsheets->at(name).load(name);
  }
  //Sends full state either way
  sendFullState(spreadsheets, name, interfaces, uid,clients);
}

/*Puts message into the outQueue of the correct spreadsheet */
void pushChange(string toSend, vector<commInterface> * interfaces, string filename)
{
  vector<commInterface>::iterator it;
  for(it = interfaces->begin(); it != interfaces->end(); it++)
    {
      if(it->getFileName() == filename)
	{
	  it->pushOut(toSend);
	}
    }
}

/*Writes cell name and contents into file name given (it->first)*/
void save(map<string, spreadsheet> * spreadsheets)
{
  //NOTE: THIS FUNCTION HAS A PRECONDITION
  //The given filename MUST exist in spreadsheets or this function
  //will throw an expetion
  for(map<string, spreadsheet>::iterator it = spreadsheets->begin(); it != spreadsheets->end(); it++)
    {
      it->second.save(it->first);
    }
}

/**/
void processMessage(string message, vector<commInterface> * interfaces, int i, map<string,spreadsheet> * spreadsheets, vector<clientStruct> * clients, vector<string> * savedFiles, map<int,string> * foci)
{
  //  char terminator = '\0'; //this works if we use \0
  //if(message.length() > 0)
  //cout<<"Current Message = "<<message<<endl;

  if(message.compare(0,3,"reg") == 0)
    {
      //Itreate through the saved_spreadsheet.txt file and 
      //spit here
      string spreadNames = "connect_accepted ";
      string msg = "";
      for(int i=0; i < savedFiles->size(); i++)
	{
	  msg = savedFiles->at(i);
	  msg.erase(msg.length()-4,4);
	  msg+="\n";
	  spreadNames += msg;
	}
      interfaces->at(i).pushOut(spreadNames);
    }
  else if(message.compare(0,3,"dis") == 0)
    {
      close(clients->at(i).connfd);
    }
  else if(message.compare(0,3,"loa") == 0)
    {
      message.erase(0,5);
      message.erase((message.length()-1),1);
      message = message + ".txt";
      interfaces->at(i).setFileName(message);
      load(spreadsheets, message, interfaces, i, savedFiles, clients);
    }
  else if(message.compare(0,3,"edi") == 0)
    {
      message.erase(0,5);
      message.erase((message.length()-1),1);
      int j;
      for(j = 0; j < message.length(); j++)
	{
	  if(message[j] == ':')
	    break;
	}
      string fname = interfaces->at(i).getFileName();

      //cell name is (0,j)
      string cellName = message.substr(0,j);
      boost::to_upper(cellName);
      string toUndo = spreadsheets->at(fname).getCellContents(cellName);
      spreadsheets->at(fname).addContents(cellName,message.substr(j+1,message.length()));
      spreadsheets->at(fname).addUndoHistory(cellName, toUndo);
      spreadsheets->at(fname).addRevertHistory(cellName, toUndo);
      string toSend = "change " + message;// + (char)3;
      //cout<<"MSG CHANGE="<<toSend<<endl;
      pushChange(toSend, interfaces, fname);
    }
  else if(message.compare(0,3,"und") == 0)
    {
      string fname = interfaces->at(i).getFileName();
      string contents = "change "; 
      string cellName = "";
      int j;
      
      contents += spreadsheets->at(fname).undo();
      for(j = 0; j < contents.length();j++)
	{
	  if(contents.at(j) == ':')
	    break;
	}
      cellName = contents.substr(7,j-7);
      boost::to_upper(cellName);
      pushChange(contents, interfaces, fname);
    }
  else if(message.compare(0,3,"rev") == 0)
    {
      message.erase(0,6);
      message.erase((message.length()-1),1);
      boost::to_upper(message);
      //cout << "revert cell name : " << endl;
      string fname = interfaces->at(i).getFileName();
      string contents = "change " + message + ":" + spreadsheets->at(fname).revert(message);
      pushChange(contents,interfaces,fname);
    }
  else if(message.compare(0,3,"foc") == 0)
    {
      string fname = interfaces->at(i).getFileName();
      string toSend = "focus ";
      message.erase(0,5);
      message.erase(message.length()-1,1);
      boost::to_upper(message);
      foci->insert(std::pair<int,string>(i,message));
      toSend += message + ":" + std::to_string(i);
      pushChange(toSend,interfaces,fname);
    }
  else if(message.compare(0,3,"unf") == 0)
    {
      string fname = interfaces->at(i).getFileName();
      string message = "unfocus ";
      foci->insert(std::pair<int,string>(i,""));
      foci->at(i) = "";
      message += std::to_string(i);
      pushChange(message, interfaces, fname);
    }
}
/*
void * pingResponse(void * arguments)
{
  argsStruct * argumentz = (argsStruct *) arguments;

}
*/
void * pingLoop(void * arguments)
{
  argsStruct * argumentz = (argsStruct *) arguments;
  while(1)
    {
      for(int i=0; i < argumentz->interfaces->size(); i++)
	{
	  //Send ping
	  if(argumentz->interfaces->at(i).getLife() != 0 && argumentz->clients->at(i).readyToPing)
	    argumentz->interfaces->at(i).setSendPing(true); 
	}
      sleep(10);
    }
}

void * persistence(void * arguments)
{
  map<string,spreadsheet> * sp = (map<string,spreadsheet> *) arguments;
  while(1)
    {
      save(sp);
      sleep(60);
    }
}

static void  signalHandler(int sig, siginfo_t * siginfo, void * context)
{
  saveBeforeExit = true;
  //map<string,spreadsheet>  * spreadsheets = (map<string,spreadsheet> *) context;
  //save(spreadsheets);
}

int main(int argc, char * argv[])
{
  vector<clientStruct> clients;
  vector<commInterface> interfaces;
  vector<string> savedFiles;
  int listenfd = 0, connfd = 0;
  struct sockaddr_in serverAddress, clientAddress;
  pthread_t tid;
  pthread_t pl_tid;
  pthread_t save_tid;
  pthread_t prtid;
  string message;
  //Spreadsheet storage
  map<string,spreadsheet> spreadsheets;
  map<int, string> foci;
  int t = 1;
  struct sigaction act;
  memset(&act,'\0', sizeof(act));
  act.sa_sigaction = &signalHandler;
  act.sa_flags = SA_SIGINFO;
  if (sigaction(SIGINT, &act, NULL) < 0) {
    perror ("sigaction");
    return 1;
  }

  ifstream incoming("saved_spreadsheets.txt");
  string file;
  incoming >>file;
  int count = 0;
  while(true)
    {
      if(incoming.fail())
	break;
      savedFiles.push_back(file);
      incoming >> file;
    }
  incoming.close();

  /* Socket settings */
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(PORT);
  /*Ignore pipe signals */
  signal(SIGPIPE, SIG_IGN);
  /*
    When the network service is restarted it sometimes fails 
    with the error “Address already in use"
    The error can be avoided by setting the SO_REUSEADDR 
    socket option after the socket has been created but before calling bind:*/
  setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&t,sizeof(int));
  
  /*Bind */
  if(bind(listenfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
    {
      errorHelper("Error: binding socket failed~");
    }
  argsStruct * arguments = (argsStruct *)malloc(sizeof(argsStruct));
  arguments->listenfd = listenfd;
  arguments->connfd = connfd;
  arguments->clientAddress = &clientAddress;
  arguments->clients = &clients;
  arguments->interfaces = &interfaces;

  pthread_create(&save_tid,NULL,&persistence,&spreadsheets);
  pthread_create(&pl_tid, NULL,&pingLoop,arguments);
  //  pthread_create(&prtid, NULL, &pingResponse,arguments);
  pthread_create(&tid, NULL, &listenForClients, arguments);
  while(1)
    {
      if(saveBeforeExit)
	{
	  save(&spreadsheets);
	  cout<<"Saved files"<<endl;
	  close(listenfd);
	  //Give everything some time to breathe ;)
	  sleep(0.3);
	  exit(0);
	}
      if(interfaces.size() > 0){
	for(int i =0; i < interfaces.size(); i++)
	  { 
	    message = interfaces[i].popIn();
	    //if(message.length() >0)
	    //stripWhiteSpace(&message);
	    processMessage(message, &interfaces, i, &spreadsheets, &clients, &savedFiles, &foci);
	  }
      }
    }
}

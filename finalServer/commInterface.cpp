#include"commInterface.h"
#include<iostream>
// private:
// std::queue<string> inbound;
// std::queue<string> outbound;

std::mutex lox;
// public:
using namespace std;
commInterface::commInterface()
  {
    //cout << "created interface" << endl;
  }
commInterface::~commInterface()
{
  //cout << "destructed interface" << endl;
}
int commInterface:: checkInboundSize()
{
  int temp;
  lox.lock();
  temp =  inbound.size();
  lox.unlock();
  return temp;
}
int commInterface:: checkOutboundSize()
{
  int temp;
  lox.lock();
  temp =  outbound.size();
  lox.unlock();
  return temp;
}
void commInterface::pushIn(std::string in)
  {
    lox.lock();
    inbound.push(in);
    lox.unlock();
  }
void commInterface::pushOut(std::string in)
  {
    lox.lock();
    outbound.push(in);
    lox.unlock();
  }
std::string commInterface::popIn()
  {
    string front ="";
    lox.lock();
    if(inbound.size() >0)
      {
	front = inbound.front();
	inbound.pop();
      }
    lox.unlock();
    return front;
  }
std::string commInterface::popOut()
  {
    lox.lock();
    string front = outbound.front();
    outbound.pop();
    lox.unlock();
    return front; 
  }
void commInterface::setFileName(string name)
{
  filename = name;
}

string commInterface::getFileName()
{
  return filename;
}

bool commInterface::gotPing()
{
  return recvPing;
}
void commInterface::setPing(bool ping)
{
  recvPing = ping;
}
bool commInterface::getPingResponse()
{
  return pingResponse;
}
void commInterface::setPingResponse(bool truth)
{
  pingResponse = truth;
}
int commInterface::getLife()
{
  return life;
}
void commInterface::nudgeLife()
{
  life--;
}
bool commInterface::getSendPing()
{
  return sendPing;
}
void commInterface::setSendPing(bool truth)
{
  sendPing = truth;
}
bool commInterface::getDisconnect()
{
  return disconnect;
}
void commInterface::setDisconnect(bool truth)
{
  disconnect = truth;
}
bool commInterface::getDisconnected()
{
  return disconnected;
}
void commInterface::setDisconnected(bool truth)
{
  disconnected = truth;
}

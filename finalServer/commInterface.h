#include<queue>
#include<string>
#include<mutex>
#ifndef COMMINTERFACE_H
#define COMMINTERFACE_H

class commInterface
{
 private:
  std::queue<std::string> inbound;
  std::queue<std::string> outbound;
  bool recvPing = false; //If we got a ping, set this so 
  //we know to ping_response ASAP
  bool pingResponse = false; //If we got a ping_response, check this
  //so we know not to disconnect
  bool sendPing = false; //Is it time to send a ping from write?
  bool disconnect = false;
  int life = 6;
  bool disconnected = false;
  
  //  std::mutex lox;
  std::string filename;

 public:
  commInterface();
  ~commInterface();

  void pushIn(std::string in);
  void pushOut(std::string in);
  int checkInboundSize();
  int checkOutboundSize();

  std::string popIn();
  std::string popOut();

  std::string getFileName();
  void setFileName(std::string name);

  bool gotPing();
  void setPing(bool ping);
  bool getPingResponse();
  void setPingResponse(bool truth);
  int getLife();
  void nudgeLife();
  bool getSendPing();
  void setSendPing(bool truth);
  bool getDisconnect();//Do we need to disconnect?
  void setDisconnect(bool truth);
  bool getDisconnected();//Have we been disconnected?
  void setDisconnected(bool truth);

};

#endif

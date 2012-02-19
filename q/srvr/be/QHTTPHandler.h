#ifndef Q_HTTP_HANDLER_H
#define Q_HTTP_HANDLER_H

#include "HTTPRequestHandler.h"

#define SESS_ID_LEN 16

class QHTTPHandler : public HTTPRequestHandler
{
public:
  QHTTPHandler (const char* rootPath, const char* path, TCPSocket* pTCPSocket);
  virtual ~QHTTPHandler ();

  static inline HTTPRequestHandler* inst (const char* rootPath, const char* path, TCPSocket* pTCPSocket) { return new QHTTPHandler(rootPath, path, pTCPSocket); }

  virtual void doGet ();
  virtual void doPost ();
  virtual void doHead ();
  
  virtual void onReadable (); // data has been read
  virtual void onWriteable (); // data has been written & buf is free
  virtual void onClose (); // connection is closing
  
  static char* sample;
  
 private:
  typedef map<string, string> hdr_map_t;
 
  char m_sess_id[SESS_ID_LEN];
 
  bool getSessionId ();
  void setSessionId (); 
  void setHeaders ();
  void sendData (const char*);
};

#endif

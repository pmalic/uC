#include "QHTTPHandler.h"

//#define __DEBUG
#include "dbg/dbg.h"

QHTTPHandler::QHTTPHandler (const char* rootPath, const char* path, TCPSocket* pTCPSocket)
: HTTPRequestHandler(rootPath, path, pTCPSocket)
{
}

QHTTPHandler::~QHTTPHandler ()
{
  DBG("\r\nHandler destroyed\r\n");
}

void QHTTPHandler::setHeaders ()
{
  respHeaders()["Expires"] = "Fri, 03 Sep 1999 01:00:00 GMT";
  respHeaders()["Cache-Control"] = "no-store, no-cache, must-revalidate, post-check=0, pre-check=0";
  respHeaders()["Pragma"] = "no-cache";
  
  respHeaders()["Connection"] = "close";
}

void QHTTPHandler::sendData (const char* data)
{
  size_t len = strlen(data);
  
  setContentLen(len);

  writeData(data, len);  
}

bool QHTTPHandler::getSessionId ()
{
  const hdr_map_t& hdrs = reqHeaders();
  hdr_map_t::const_iterator it = hdrs.find("Cookie");
  
  if (it == hdrs.end())
   return false;
   
  const char* cookie = it->second.c_str();

  sscanf(cookie, "SESSID=%s", m_sess_id);

  printf("FOUND SESSID: %s\r\n", m_sess_id);

  return true;
}

void QHTTPHandler::setSessionId ()
{  
  sprintf(m_sess_id, "%08x%08x", rand(), rand());
 
  char cookie[32];
  
  sprintf(cookie, "SESSID=%s; path=/", m_sess_id);
  
  respHeaders()["Set-Cookie"] = cookie;
}

void QHTTPHandler::doGet ()
{
  DBG("\r\nIn QHTTPHandler::doGet()\r\n");
  
  setHeaders();

  const std::string& req_path = path();
  
  if (req_path == "/login")
     sendData("var Q = { user_id: 'u4' };");
  else
     sendData(QHTTPHandler::sample);
    
  DBG("\r\nExit QHTTPHandler::doGet()\r\n");
}

void QHTTPHandler::doPost ()
{
  DBG("\r\nIn QHTTPHandler::doPost()\r\n");

//  if (!getSessionId())
//   setSessionId();

  int len = dataLen();
  
  char buf[len + 1];
  
  readData(buf, len);
  
  buf[len] = 0;

  printf("POST: %s\r\n", buf);  
  
  setHeaders();
  
//  char out[16];
    
//  snprintf(out, 16, "OK");
  
  sendData(QHTTPHandler::sample);
  
  DBG("\r\nExit QHTTPHandler::doPost()\r\n");
}

void QHTTPHandler::doHead ()
{
}
 
void QHTTPHandler::onReadable () // data has been read
{
}

void QHTTPHandler::onWriteable () // data has been written & buf is free
{
  DBG("\r\nQHTTPHandler::onWriteable() event\r\n");
  
  close(); // data written, we can close the connection
}

void QHTTPHandler::onClose () // connection is closing
{
  // nothing to do
}

char* QHTTPHandler::sample = "{\"u\":{\"u0\":\"mmarkovic\",\"u1\":\"ppetrovic\",\"u2\":\"ssimic\",\"u3\":\"nnikolic\",\"u4\":\"pmalic\"},\"r\":{\"r27\":{\"n\":\"ROOM27\",\"q\":[],\"f\":1},\"r26\":{\"n\":\"ROOM26\",\"q\":[],\"f\":1},\"r25\":{\"n\":\"ROOM25\",\"q\":[\"u1\",\"u3\"],\"f\":0},\"r24\":{\"n\":\"ROOM24\",\"q\":[\"u1\",\"u3\"],\"f\":0},\"r23\":{\"n\":\"ROOM23\",\"q\":[],\"f\":1},\"r22\":{\"n\":\"ROOM22\",\"q\":[],\"f\":1},\"r21\":{\"n\":\"ROOM21\",\"q\":[],\"f\":1},\"r20\":{\"n\":\"ROOM20\",\"q\":[\"u0\",\"u2\"],\"f\":0},\"r19\":{\"n\":\"ROOM19\",\"q\":[\"u2\",\"u0\"],\"f\":0},\"r18\":{\"n\":\"ROOM18\",\"q\":[\"u3\"],\"f\":1},\"r17\":{\"n\":\"ROOM17\",\"q\":[\"u1\"],\"f\":0},\"r16\":{\"n\":\"ROOM16\",\"q\":[\"u0\",\"u3\"],\"f\":1},\"r15\":{\"n\":\"ROOM15\",\"q\":[],\"f\":1},\"r14\":{\"n\":\"ROOM14\",\"q\":[],\"f\":1},\"r13\":{\"n\":\"ROOM13\",\"q\":[\"u1\"],\"f\":0},\"r12\":{\"n\":\"ROOM12\",\"q\":[],\"f\":1},\"r11\":{\"n\":\"ROOM11\",\"q\":[\"u1\",\"u3\"],\"f\":1},\"r10\":{\"n\":\"ROOM10\",\"q\":[\"u1\"],\"f\":0},\"r9\":{\"n\":\"ROOM9\",\"q\":[\"u3\"],\"f\":1},\"r8\":{\"n\":\"ROOM8\",\"q\":[\"u3\"],\"f\":0},\"r7\":{\"n\":\"ROOM7\",\"q\":[\"u2\",\"u0\"],\"f\":0},\"r6\":{\"n\":\"ROOM6\",\"q\":[\"u1\"],\"f\":0},\"r5\":{\"n\":\"ROOM5\",\"q\":[\"u0\",\"u2\",\"u3\"],\"f\":0},\"r4\":{\"n\":\"ROOM4\",\"q\":[\"u1\",\"u2\",\"u0\"],\"f\":1},\"r3\":{\"n\":\"ROOM3\",\"q\":[\"u1\"],\"f\":1},\"r2\":{\"n\":\"ROOM2\",\"q\":[],\"f\":1},\"r1\":{\"n\":\"ROOM1\",\"q\":[\"u1\",\"u3\"],\"f\":1},\"r0\":{\"n\":\"ROOM0\",\"q\":[\"u1\"],\"f\":0}}}";

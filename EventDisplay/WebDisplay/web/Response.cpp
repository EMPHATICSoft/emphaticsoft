#include "web/Response.h"
#include "web/Connection.h"

#include <sys/stat.h>
#include <iostream>

namespace web
{
  Response::Response(const int statusCode): fStatusCode(statusCode)
  {
  }

  StringResponse::StringResponse(const std::string& content, const std::string contentType, const int statusCode): Response(statusCode), fType(contentType), fContent(content)
  {
  }

  int StringResponse::resolve(web::Connection& conn, const int messageSocket)
  {
    fContent.insert(0, "HTTP/1.1 " + std::to_string(fStatusCode) + " OK\nContent-Type:" + fType + "\nContent-Length:" + std::to_string(fContent.length()) + "\n\n");
    return conn.sendString(fContent, messageSocket);
  }

  FileResponse::FileResponse(const std::string& fileName, const std::string contentType, const int statusCode): Response(statusCode), fType(contentType), fFileName(fileName)
  {
  }

  std::string FileResponse::getFullPath(const std::string& fileName) const
  {
    //Find the file by looking at the source code directory
    //TODO: I don't think this will work if we install on /cvmfs.  How do we look in the install directory instead?
    const char* fileLoc = getenv ("CETPKG_SOURCE");
    if(fileLoc == nullptr) std::cerr << "Failed to find " << fileName << " at CETPKG_SOURCE (source directory) because CETPKG_SOURCE is not set!";
    return fileLoc + std::string("/EventDisplay/WebDisplay/") + fileName;
  }

  int FileResponse::resolve(web::Connection& conn, const int messageSocket)
  {
    const std::string fileFullPath = getFullPath(fFileName);

    struct stat fileInfo;
    stat(fileFullPath.c_str(), &fileInfo);
    const int contentLength = fileInfo.st_size;

    const std::string requestHeader = "HTTP/1.1 " + std::to_string(fStatusCode) + " OK\nContent-Type:" + fType + "\nContent-Length:" + std::to_string(contentLength) + "\n\n";

    conn.sendString(requestHeader, messageSocket);
    return conn.sendFile(fileFullPath, messageSocket);
  }

  EndAppResponse::EndAppResponse(const int statusCode): Response(statusCode)
  {
  }

  int EndAppResponse::resolve(web::Connection& conn, const int messageSocket)
  {
    return 1;
  }

  BadRequestResponse::BadRequestResponse(const int statusCode): Response(statusCode)
  {
  }

  int BadRequestResponse::resolve(web::Connection& conn, const int messageSocket)
  {
    return conn.sendString("HTTP/1.1 " + std::to_string(fStatusCode) + " Not found\n\n", messageSocket);
  }
}

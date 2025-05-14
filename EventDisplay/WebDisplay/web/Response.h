//File: Response.h
//Brief: A web::Response is how web::App handlers communicate with
//       the Javascript front end and the web::App loop.  This file
//       provides a base class and 4 implementations that web::App
//       interacts with.
//Author: Andrew Olivier aolivie4@nd.edu

#ifndef WEB_RESPONSE_H
#define WEB_RESPONSE_H

#include <string>

namespace web
{
  class Connection;

  class Response
  {
    public:
      Response(const int statusCode);
      virtual ~Response() = default;

      virtual int resolve(web::Connection& conn, const int messageSocket) = 0;

    protected:
      int fStatusCode;
  };

  //Send a string to the Javascript front end
  class StringResponse: public Response
  {
    public:
      StringResponse(const std::string& content, const std::string contentType = "application/json", const int statusCode = 200);
      virtual ~StringResponse() = default;

      virtual int resolve(web::Connection& conn, const int messageSocket) override;

    private:
      std::string fType;
      std::string fContent;
  };

  //Send a file to the Javascript front end
  class FileResponse: public Response
  {
    public:
      FileResponse(const std::string& fileName, const std::string contentType = "application/json", const int statusCode = 200);
      virtual ~FileResponse() = default;

      virtual int resolve(web::Connection& conn, const int messageSocket) override;

    private:
      std::string fType;
      std::string fFileName;

      std::string getFullPath(const std::string& fileName) const;
  };

  //End the App loop entirely.  I plan to use this when the front end
  //requests a newEvent to trigger a state change in the ART back end.
  class EndAppResponse: public Response
  {
    public:
      EndAppResponse(const int statusCode = 200);
      virtual ~EndAppResponse() = default;

      virtual int resolve(web::Connection& conn, const int messageSocket) override;
  };

  //Return just status code 404 not found
  class BadRequestResponse: public Response
  {
    public:
      BadRequestResponse(const int statusCode = 404);
      virtual ~BadRequestResponse() = default;

      virtual int resolve(web::Connection& conn, const int messageSocket) override;
  };
}

#endif //WEB_RESPONSE_H

////////////////////////////////////////////////////////////////////////
// CONDB_CPP_INTERFACE (Header Only)
// A C++ interface for accessing user data from Conditions Databases
//
// For Conditions Database questions and requests contact Stephen White (swhite@fnal.gov)
// For CONDB_CPP_INTERFACE questions contact Brandon White (bjwhite@fnal.gov) with questions
////////////////////////////////////////////////////////////////////////
#ifndef CONDB_CPP_INTERFACE
#define CONDB_CPP_INTERFACE

#include "../util/csv.h"
#include <sstream>
#include <regex>
#include <wda.h>
#include <iostream> // for debugging reasons
#define LABEL "Conditions DB Access"

const std::string TESTING_URL = "http://localhost:8888/test_condb/app/";

template <std::size_t NUM_COL>
class ConditionsDBResponse {
 public:
 ConditionsDBResponse(const std::string& data, const std::array<std::string, NUM_COL>& column_names) :
  _data(data),
    _validity_interval_start(-1),
    _validity_interval_end(-1),
    _column_names(column_names),
    _csv_stream(_data),
    _csv_parser{"Conditions DB Access", _csv_stream }
  {
    parse_headers(std::make_index_sequence<NUM_COL>());
  }
  ConditionsDBResponse(const ConditionsDBResponse&) = delete;
  ConditionsDBResponse& operator=(const ConditionsDBResponse&) = delete;
  ConditionsDBResponse(ConditionsDBResponse&&);

  const bool has_interval_end() { return _validity_interval_end != -1; }
  const long get_interval_start() { return _validity_interval_start; }
  const long get_interval_end() { return _validity_interval_end; }

  template<typename Var, typename... Vars>
    bool get_row(Var& variable1, Vars&... rest) {
    if ( _csv_parser.read_row(variable1, rest...) )
      return true;
    return false;
  }

 private:
  template <std::size_t... I>
    void parse_headers(std::index_sequence<I...>) {
    parse_headers_impl(_column_names[I]...);
  }

  template <typename... Cols>
    void parse_headers_impl(Cols&... cols) {
    _validity_interval_start = std::stod(_csv_parser.next_line());
    std::string int_end = _csv_parser.next_line();
    if ( int_end != "-" )
      _validity_interval_end = std::stod(int_end);
    _csv_parser.read_header(io::ignore_extra_column, cols...);
    std::string data_types = _csv_parser.next_line(); // Currently unused
  }

  std::string _data;
  long _validity_interval_start; // Initialized to -1
  long _validity_interval_end; // Initialized to -1. -1 symbolizes a validity interval that is open on the right
  const std::array<std::string, NUM_COL> _column_names;
  std::istringstream _csv_stream;
  io::CSVReader<NUM_COL> _csv_parser;
}; // class ConditionsDBResponse

class ConditionsDBBase {
 public:
 ConditionsDBBase(const std::string& base_url) :
  _base_url(base_url)
  {}

 protected:
  std::string do_http_op(const std::string resrc_url, const int timeout) {
    // Make and verify the url
    std::string request_url = _base_url + resrc_url;

    // Check if URI matches format http://dbweb5.fnal.gov:9090/gm2_con_prod/app/data?f=adam_test_one&t=112445000000.000000 (decimal point required)
    /*
      if ( !std::regex_match(request_url, std::regex(R"(^(http|https)://[a-zA-z0-9]+\.[a-zA-z0-9]+\.[a-zA-z]+:[0-9]+/[a-zA-z0-9/?=&]+.[a-zA-z0-9/?=&]+)") ) ) {
      if ( request_url.compare(0, TESTING_URL.length(), TESTING_URL) != 0 ) {
      std::stringstream err_msg;
      err_msg << "The provided URI " << request_url << " is not of a valid form." << std::endl;
      err_msg << "The provided URI " << request_url << std::endl;
      err_msg << "The testing  URI " << TESTING_URL << std::endl;
      throw std::runtime_error(err_msg.str().c_str());
      }
      }
    */

    std::size_t length;
    int status = -1;
    void* resp_buf = getHTTPWithTimeout(request_url.c_str(), nullptr, 0, &length, timeout, &status);

    if ( status != 0 ) { // 0 is CURLEOK
      std::stringstream err_msg;
      err_msg << "After " << timeout << " seconds the HTTP operation to the specified URI failed:\n\t " << request_url << std::endl;
      throw std::runtime_error(err_msg.str().c_str());
    }
    // Now read the resp from the buffer
    std::string response_text(static_cast<const char*>(resp_buf), length);
    return response_text;
  }

 private:
  std::string _base_url;
        
}; // class ConditionsDBBase

class ConditionsDB : private ConditionsDBBase {
 public:
 ConditionsDB(const std::string& base_url) :
  ConditionsDBBase(base_url),
    _validity_interval_start(-1),
    _validity_interval_end(-1)
      {}

  std::string get_folders(const int timeout=30) {
    return do_http_op("folders", timeout);
  }

  template <size_t NUM_COL>
    ConditionsDBResponse<NUM_COL> query(const std::string& folder, 
					const std::string& datatype,
					const std::array<std::string, NUM_COL>& column_names, 
					const long instant, const std::string& tag="", const int timeout=30) {

    // TODO: Replace this with proper checking for a 404 error code once libwda supports it.
    // Validate that the folder exists
    //std::string validation = get_folders(); 
    //if (validation.find(folder) == std::string::npos) {
    //    std::stringstream err_msg;
    //    err_msg << "The provided folder was not present in the queried Conditions Database: " << folder << std::endl;
    //    throw std::runtime_error(err_msg.str().c_str());
    //}

    // Perform the query and orchestrate the parsing/return of the data
    // Note that we still use &t= even for the integer index.
    std::string data_resrc_id = "/get?table=" + folder + "&t=" + std::to_string(instant) + "&type=" + datatype + "&columns=";
    bool isFirst = true;
    for(auto & cname : column_names) {
      if (isFirst) {
	data_resrc_id += cname;
	isFirst = false;
      }
      else {
	data_resrc_id += ("," + cname);
      }
    }
    if (tag != "")	      
      data_resrc_id += "&tag=" + tag;
    std::string data = do_http_op(data_resrc_id, timeout);
	  
    // Check for "Data not found" -- which means you tried to get a calib entry before the first time entry in the DB
    if ( data == "Data not found" ) {
      std::stringstream err_msg;
      err_msg << "Calibration database returned 'Data not found' for folder " << folder << std::endl;
      throw std::runtime_error(err_msg.str().c_str());
    }
    return ConditionsDBResponse<NUM_COL>(data, column_names);
  }

 private:
  int _validity_interval_start; // Start of validity interval
  int _validity_interval_end; // End of validity interval. -1 denotes the interval is unbounded on the right
};
#endif

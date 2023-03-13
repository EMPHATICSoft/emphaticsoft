#ifndef EMPH_QUERY_ENGINE_API
#define EMPH_QUERY_ENGINE_API

#include <tuple>
#include <utility>
#include <string>
#include <array>
#include <vector>
#include <ostream>
#include <sstream>
#include <memory>
#include <iostream>
#include <wda.h>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <iterator>
#include <cstring>
#include "Database/util/convert.h"

template<typename... Types>
class table {
  public:

    struct row_type {
      using data_type = std::tuple<Types...>;
      data_type data;
    };
    using container_type = std::vector<row_type>;
    //using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using size_type = typename container_type::size_type;

    size_type size() const { return _data.size(); }
    //iterator begin() { return _data.begin(); }
    const_iterator cbegin() const { return _data.cbegin(); }
    const_iterator begin() const { return cbegin(); }
    //iterator end() { return _data.end(); }
    const_iterator cend() const { return _data.cend(); }
    const_iterator end() const { return cend(); }


    void add_row(row_type&& row) {
      _data.push_back(std::move(row));
    }
  private:
    container_type _data;
};

// access the column data
template<std::size_t N, typename R>
auto column( const R& row) {
  return std::get<N>(row.data);
}


template<typename V>
class query_where_t {
  public:
    query_where_t(const std::string& col, const V& val) : _col(col), _v(val) {}

    std::string make_q() {
      std::ostringstream os;
      os << _col << ':' << _v;
      return os.str();
    }
  private:
    const std::string& _col;
    const V& _v;
};

// things which aren't dependent on the table result types go into the base class
class QueryEngineBase {
 protected:
 QueryEngineBase(const std::string& url_prefix, const std::string& db, const std::string& ns,
		 const std::string& table)
   : _url_prefix(url_prefix),
    _db(db), _ns(ns),
    _table(table)
    {}

    template<typename T>
    std::string make_where(const std::string& col, T&& value) {
      std::ostringstream os;
      os << col << ':' << std::forward<T>(value);
      return os.str();
    }

    template<typename T>
    std::string make_where(const std::string& col, const std::string& op, T&& value) {
      std::ostringstream os;
      os << col << ':' << op << ':' << std::forward<T>(value);
      return os.str();
    }

    template <typename... Args>
    std::string get_query_params(Args... args) {

      // turn the arguments into the query parameters

      std::string query;
      for (auto& w : _where) {
        query += "&w=" + w;
      }
      // I'm assumming this can be a comma separated list
      if (!_orderby.empty()) {
        query += "&o=" + _orderby[0];
        for (size_t i=1; i < _orderby.size(); ++i) {
          query += ',' + _orderby[i];
        }
      }
      if (_limit >= 0) {
        query += "&l=" + std::to_string(_limit);
      }
      return query;
    }

    std::string do_query(std::string query) {
      int error_rc = 0;
      int *error_rc_ptr = &error_rc;
      std::string uagent_str = "";

      const size_t BUF_SIZE = 256;

      std::unique_ptr<void, decltype(&releaseDataset)> query_result_buf( getData(query.c_str(), uagent_str.c_str(), error_rc_ptr), &releaseDataset);

      if (error_rc != 0) throw std::runtime_error("There was an error acquiring the data via libwda getData()");

      auto curr_tuple = getFirstTuple(query_result_buf.get());
      auto result_header_skip_count = 0; //Use this to skip the data column headers that come back at the front of the tuple
      std::string result_string = "";

      while ( curr_tuple ) {
        if (result_header_skip_count > 0) {
          int num_fields = getNfields(curr_tuple);

          for (auto i=0; i < num_fields; i++) {

            auto err_rc = 0;
            auto* err_rc_ptr = &err_rc;

            char value_buf[BUF_SIZE];

            auto str_len = getStringValue(curr_tuple, i, value_buf, BUF_SIZE, err_rc_ptr);
            if (error_rc != 0) throw std::runtime_error("There was an error parsing the data via libdwa getStringValue()");

            std::string partial_res_str(value_buf, str_len);

            if (i < num_fields-1) //Dont put a comma on the last one
              result_string += partial_res_str + ",";
            else
              result_string += partial_res_str;
          }
          result_string += "\n";
        } else
          ++result_header_skip_count;
        curr_tuple = getNextTuple(query_result_buf.get());
      }
      return result_string;
    }

    std::vector<std::string> split(const std::string& input_string, char delim)
    {
       std::vector<std::string> tokens;
       std::string token;
       std::istringstream tokenStream(input_string);
       while (std::getline(tokenStream, token, delim))
       {
           tokens.push_back(token);
       }
       return tokens;
     }

    // helper for pulling a value from the string
    template<typename V>
    std::string::size_type extract_value(const std::string& r, std::string::size_type start, std::string::size_type end, V& val) {
      auto element_end = r.find(',', start);
      if (element_end == std::string::npos || element_end > end) element_end = end;

      auto val_to_convert = std::string(r, start, element_end-start);
      val = convert::convertTo<V>(val_to_convert);

      return element_end;
    }

    template<typename V>
    std::string::size_type extract_value(const std::string& r, std::string::size_type& start, std::string::size_type& end, std::vector<V>& extracted_result_array) {
      std::string::size_type element_end;
      std::string::size_type closing_bracket;
      bool is_vec = false;


      if ( r.at(start) == '[' ) {
        closing_bracket = r.find(']', start);
        element_end = r.find(',', closing_bracket);
        is_vec = true;
      } else {
        element_end = r.find(',', start);
      }

      if (element_end == std::string::npos || element_end > end) element_end = end;

      if (is_vec) {
        std::string vector_content_str = r.substr(start+1, (closing_bracket)-(start+1));

        auto tokens = split(vector_content_str, ',');


        for (const auto& single_val_str : tokens) {
          V converted_val;
          extract_value(single_val_str, 0, single_val_str.length(), converted_val);
          extracted_result_array.push_back(converted_val);
        }

      }
      return element_end;
    }

    void verify_table() {
      std::string table_query_uri = _url_prefix + "/I/tables?dbname=" + _db + "&ns=" + _ns + "&f=csv";
      std::string result_string = do_query(table_query_uri);
      auto found = result_string.find(_table);
      
      if ( found == std::string::npos )
        throw std::runtime_error("Table does not exist in the specified database.");
    }
    
    std::string _url_prefix;
    std::string _db;
    std::string _ns;
    std::string _table;
    std::vector<std::string> _where;
    std::vector<std::string> _orderby;
    int _limit = -1;
};


// Other datatypes than binary
template<typename... Types>
class QueryEngine : private QueryEngineBase {
 public:
  
  template<typename...Args>
    QueryEngine(const std::string& url_prefix, const std::string& db, const std::string& ns,
		const std::string& table, const std::string& col1, const Args&... cols)
    : QueryEngineBase(url_prefix, db, ns, table),
    _colnames{ {col1, cols...} }
  {
      static_assert(sizeof...(Types) == sizeof...(Args) + 1, "Number of column names does not match number of result types");

      //      verify_table();
      //      verify_columns(col1, cols...);

    }

    using result_type = table<Types...>;

    template<typename Arg>
    void verify_columns(const Arg& arg) {
      verify_columns_impl(arg);
    }

    template<typename First, typename... Rest>
    void verify_columns(const First& first, const Rest&... rest) {
      verify_columns_impl(first);
      verify_columns(rest...);
    }

    template<typename Arg>
    void verify_columns_impl(const Arg& arg) {
      std::string column_query_uri = _url_prefix + "/I/columns?dbname=" + _db + "&t=" + _ns + "." + _table + "&f=csv";
      std::string result_string = do_query(column_query_uri);
      auto found = result_string.find(arg);

      if ( found == std::string::npos )
        throw std::runtime_error("Column does not exist in the specified table.");
    }

    //where clause
    template<typename T>
    QueryEngine<Types...>& where(const std::string& col, T&& val) {
      _where.push_back(make_where(col, std::forward<T>(val)));
      return *this;
    }

    template<typename T>
    QueryEngine<Types...>& where(const std::string& col, const std::string& op, T&& val) {
      _where.push_back(make_where(col, op, std::forward<T>(val)));
      return *this;
    }

    // order by clauses
    QueryEngine<Types...>& order_by(std::string col, const char direction = '+') {
      std::string col_name = std::move(col);
      if (direction == '-')
        col_name = "-" + col_name;

      _orderby.push_back(std::move(col_name));
      return *this;
    }

    // limit clause
    QueryEngine<Types...>& limit(int limit) {
      _limit = limit;
      return *this;
    }

    // query the db and fill the result table
    result_type get() {

      // Build the Query Engine query string
      std::string query = "query?dbname=";
      query += _db;
      query += "&t=";
      query += _ns + "." + _table + "&c=" + _colnames[0];
      for (size_t i=1; i < _colnames.size(); ++i) {
        query += ',' + _colnames[i];
      }
      query += get_query_params();

      std::string qe_base_uri = _url_prefix + "/query";
      std::string qe_query_full_uri = qe_base_uri + "/" + query;
      std::string result = do_query(qe_query_full_uri);

      return parse_results(result);
    }

  private:

    std::array<std::string, sizeof...(Types)> _colnames;

    result_type parse_results(const std::string& r) {
      //std::cout << "String recieved from db: " << r << std::endl;

      result_type result_table;
      std::string::size_type start = 0;
      while (start != r.size()) {
        std::string::size_type end = r.find('\n' , start);
        if (end == std::string::npos) end = r.size();
        typename result_type::row_type row;
        parse_line(r, start, end, row);
        result_table.add_row(std::move(row));
        start = end + 1;
      }
      return result_table;
    }

    // convert the tuple type to a list of arguments
    void parse_line(const std::string& r, std::string::size_type start, std::string::size_type end, typename result_type::row_type& result_entry) {
      parse_line(r, start, end, result_entry, std::make_index_sequence<std::tuple_size<typename result_type::row_type::data_type>::value>{});
    }
    template<std::size_t... I>
    void parse_line(const std::string& r, std::string::size_type start, std::string::size_type end, typename result_type::row_type& result_entry, std::index_sequence<I...> ) {
      parse_line_impl(r, start, end, std::get<I>(result_entry.data)...);
    }

    // parse the first value in the argument list then
    // recurse over the rest of the list
    template<typename V, typename... Values>
    void parse_line_impl(const std::string& r, std::string::size_type start, std::string::size_type end, V& val, Values&... values) {
      std::string::size_type element_end = extract_value(r, start, end, val);
      parse_line_impl(r, element_end+1, end, values...);
    }
    void parse_line_impl(const std::string& r, std::string::size_type start, std::string::size_type end) {
      // end the recursion
      if (start < end) {
        throw std::runtime_error("Incompletely parsed line");
      }
    }
}; // Other datatypes than binary

// Class to interface with UConDB for binary data
class UConDB {
    public:
        UConDB(const std::string& url_prefix)
        : _url_prefix(url_prefix)
        {}

        template<class T>
        T get(const std::string& folder, const std::string& object, const int& max_attempts, const std::string& key="",
               const std::string& tv="", const std::string& tr="", const std::string& tag="") {
            std::string query_url = generate_query_url(folder, object, key, tv, tr, tag);
            T binary_data = get_data<T>(query_url, max_attempts);
            return binary_data; 
        } 

    private:

        template<class T>
        T get_data(const std::string& query_url, const int& max_attempts) {
            int op_status = -1;
            size_t buf_len = 0;
            int sleep_time = 1;
            int attempts = 0;

            while (true) {
                std::unique_ptr<HttpResponse, decltype(&wda_release_response_buffer)> response(wda_get_response_handle(query_url.c_str(), NULL, 0, &buf_len, &op_status), wda_release_response_buffer);
                void *data_buf = wda_get_response_buffer(response.get());

                if (http_operation_success(op_status)) {
                    T binary_data;
                    binary_data.resize(buf_len);
                    std::copy_n(static_cast<char*>(data_buf), buf_len, binary_data.begin());
                    return binary_data;
                }

                // Retry if unsuccessful
                sleep(sleep_time);
                if (sleep_time < 120)
                    sleep_time *= 2; 
                attempts++;
                if (attempts >= max_attempts)
                    throw 1;
            }
        }

        std::string generate_query_url(const std::string& folder, const std::string& object, const std::string& key,
                const std::string& tv, const std::string& tr, const std::string& tag) 
        {
            std::string query_url = _url_prefix + "/app/data/" + folder;
            if (key != "") { 
                query_url += "/version_id=" + key;
            }
            else {
                query_url += "/" + object;
                if (tv != "") { query_url += "/tv=" + tv; } // tv: validity time
                if (tr != "") { query_url += "/tr=" + tr; } // tr: record time
                if (tag != "") { query_url += "&tag=" + tag; }
            }
            return query_url;
        }

        bool http_operation_success(const int& status) {
            return (status == 0);
        }

        std::string _url_prefix;

}; // class UConDB
#endif

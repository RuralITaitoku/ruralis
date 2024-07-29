#include "ruralis.h"
#include <sstream>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
using namespace std;



std::string ruralis_sqltext(const std::string& sql) {
  std::string escape_sql ="\'";


  for (size_t i = 0; i < sql.size(); ++i) {
    if (sql[i] == '\'') {
        escape_sql += "\'\'";
    } else {
        escape_sql += sql[i];
    }
  }
  escape_sql += "\'";
  return escape_sql;
}

RuraliSql::RuraliSql() {
    cout << FILE_LINE "コンストラクタ" << endl;
    db = NULL;
}
RuraliSql::~RuraliSql() {
    cout << FILE_LINE "デストラクタ" << endl;
    if (db) {
        close();
    }
}

void RuraliSql::open(const char *file_name) {
    if (db == NULL) {
        int result;
        result = sqlite3_open(file_name, &db);
        if (result) {
            cerr << FILE_LINE " file_name:" << file_name << endl;
            throw runtime_error(FILE_LINE " read error!");
        }
    }
}
std::string RuraliSql::create_table(const string &table_name, const vector<string> &columns) {
    string result;
    string sql;

    sql += "CREATE TABLE  if not exists ";
    sql += table_name;
    sql += " (";
    for (std::size_t i = 0; i < columns.size(); i++) {
        if (i) {
            sql += ", ";
        }
        sql += columns[i];
    }
    sql += ")";
    result = exec(sql);
    return result;
}

std::string RuraliSql::update(
        const string &table_name, 
        const vector<string> &columns,
        const vector<string> &values,
        const string &where) {

    string sql;
    sqlite3_stmt *stmt;
    const char *err_msg;
    string result;
    
    sql += "update ";
    sql += table_name;
    sql += " set ";
    for (size_t i = 0; i < columns.size(); i++) {
        if (i) sql += ", ";
        sql += columns[i];
        sql += " = ?";
    }
    sql += " where ";
    sql += where;
    cout << FILE_LINE " SQL:" << sql << endl;

    int r = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, &err_msg);
    if (r != SQLITE_OK) {
        cerr << FILE_LINE " SQL:" << sql << endl;
        cerr << FILE_LINE "SQLエラー" << result << endl;
        cerr << FILE_LINE "SQLメッセージ" << err_msg << endl;
        
        result += FILE_LINE "SQLエラー ";
        result += err_msg;
        result += "    ";
        result += sql;
        sqlite3_free((void*)err_msg);
        return result;
    }
    for (size_t i = 0; i < columns.size(); i++) {
        r = sqlite3_bind_text(stmt, i + 1, values[i].c_str(), -1, SQLITE_TRANSIENT);
        cout << FILE_LINE << (i + 1) << ":" << values[i] << endl;
        if (r != SQLITE_OK) {
            result += FILE_LINE "バインドエラー";
            result += std::to_string(r);
            result += " ";
            result += sqlite3_errmsg(db);
            result += " i=";
            result += std::to_string(i);
            return result;
        }
    }
    r = sqlite3_step(stmt);
    if (r != SQLITE_DONE) {
        result += FILE_LINE "SQLエラー";
        result += to_string(r);
        result += " ";
        result += sqlite3_errmsg(db);

        return result;
    }

    sqlite3_finalize(stmt);
    return "";
}
std::string RuraliSql::insert(
        const string &table_name, 
        const vector<string> &columns,
        const vector<string> &values) {
    
    vector<vector<string> > vec;
    vec.push_back(values);
    return insert(table_name, columns, vec);
}

std::string RuraliSql::insert(
        const string &table_name, 
        const vector<string> &columns,
        const vector<vector<string> > &values) {
    string sql;
    sqlite3_stmt *stmt;
    const char *err_msg;
    string result;
    
    sql += "insert into ";
    sql += table_name;
    sql += " (";
    for (size_t i = 0; i < columns.size(); i++) {
        if (i) sql += ", ";
        sql += columns[i];
    }
    sql += ") values (";
    for (size_t i = 0; i < columns.size(); i++) {
        if (i) sql += ", ";
        sql += "?";
    }
    sql += ")";
    cout << FILE_LINE " SQL:" << sql << endl;

    int r = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, &err_msg);
    if (r != SQLITE_OK) {
        cerr << FILE_LINE " SQL:" << sql << endl;
        cerr << FILE_LINE "SQLエラー" << result << endl;
        cerr << FILE_LINE "SQLメッセージ" << err_msg << endl;
        
        result += FILE_LINE "SQLエラー ";
        result += err_msg;
        result += "    ";
        result += sql;
        sqlite3_free((void*)err_msg);
        return result;
    }
    for (size_t j = 0; j < values.size(); j++) {
        for (size_t i = 0; i < columns.size(); i++) {
            r = sqlite3_bind_text(stmt, i + 1, values[j][i].c_str(), -1, SQLITE_TRANSIENT);
            cout << FILE_LINE << (i + 1) << ":" +values[j][i] << endl;
            if (r != SQLITE_OK) {
                result += FILE_LINE "バインドエラー";
                result += std::to_string(r);
                result += " ";
                result += sqlite3_errmsg(db);
                result += " i=";
                result += std::to_string(i);
                return result;
            }
        }
        r = sqlite3_step(stmt);
        if (r != SQLITE_DONE) {
            result += FILE_LINE "SQLエラー";
            result += to_string(r);
            result += " ";
            result += sqlite3_errmsg(db);

            return result;
        }
    }

    sqlite3_finalize(stmt);
    return "";
}




std::string RuraliSql::exec(
        const string &sql, 
        const vector<vector<string> > &values) {
    sqlite3_stmt *stmt;
    const char *err_msg;
    string result;
    int r = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, &err_msg);
    if (r != SQLITE_OK) {
        cerr << FILE_LINE " SQL:" << sql << endl;
        cerr << FILE_LINE "SQLエラー" << result << endl;
        cerr << FILE_LINE "SQLメッセージ" << err_msg << endl;
        
        result += FILE_LINE "SQLエラー ";
        result += err_msg;
        result += "    ";
        result += sql;
        sqlite3_free((void*)err_msg);
        return result;
    }
    for (size_t j = 0; j < values.size(); j++) {
        for (size_t i = 0; i < values[j].size(); i++) {
            r = sqlite3_bind_text(stmt, i + 1, values[j][i].c_str(), -1, SQLITE_TRANSIENT);
            cout << FILE_LINE << (i + 1) << ":" +values[j][i] << endl;
            if (r != SQLITE_OK) {
                result += FILE_LINE "バインドエラー";
                result += std::to_string(r);
                result += " ";
                result += sqlite3_errmsg(db);
                result += " i=";
                result += std::to_string(i);
                return result;
            }
        }
        r = sqlite3_step(stmt);
        if (r != SQLITE_DONE) {
            result += FILE_LINE "SQLエラー";
            result += to_string(r);
            result += " ";
            result += sqlite3_errmsg(db);

            return result;
        }
    }

    sqlite3_finalize(stmt);
    return "";
}


string RuraliSql::exec(const string &sql) {
    string result;
    cout << FILE_LINE " SQL:" << sql << endl;
 
    char *err_msg;
    int r = sqlite3_exec(db, sql.c_str(), NULL, NULL, &err_msg);
    if(r != SQLITE_OK){
        result += "SQLエラー";
        result += to_string(r);
        result += err_msg;
        result += "    ";
        result += sql;
        sqlite3_free(err_msg);
        return result;
    }
    return "";
}

string RuraliSql::select(const string &sql
                    , vector<vector<string> > &result_data) {
    string result;
    sqlite3_stmt *stmt;
    const char *err_msg;
    int r = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, &err_msg);
    if(r){
        result += FILE_LINE "SQLエラー ";
        result += err_msg;
        result += "    ";
        result += sql;
        sqlite3_free((void*)err_msg);
        return result;
    }
    while((r = sqlite3_step(stmt)) == SQLITE_ROW) {
        int cols = sqlite3_column_count(stmt);
        vector<string> row_data;
        for (int i = 0; i < cols; i++) {
            row_data.push_back((const char *)sqlite3_column_text(stmt, i)); 
        }
        result_data.push_back(row_data);
    }




    r = sqlite3_finalize(stmt);
    if(r){
        result = " finalize error!";
        return result;
    }
    return "";
}
void RuraliSql::close() {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}


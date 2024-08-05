#include "ruralis.h"


//#include <unistd.h>
//#include <fcntl.h>
//#include <stdlib.h>
//#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sstream>

#include <thread>

#include <pthread.h>

#include <fcntl.h>
#include <regex>
#include <string.h>
using namespace std;

#define RED(x) "\033[31m" x "\033[0m"

RuralisHttp::RuralisHttp() {

    cout << FILE_LINE "RuralisHttp コンストラクタ" << endl;
    server_fd = 0;
    client_fd = 0;

    buf_i = 0;
    buf_e = 0;

    send_buf_i = 0;

    content_func = NULL;

}

RuralisHttp::~RuralisHttp() {
    cout << FILE_LINE << " " << client_fd << " RuralisHttp デストラクタ" << server_fd << endl; 
    close_fd();
    cout << endl;
}

void RuralisHttp::close_fd() {
    cout << FILE_LINE " fdを閉じる。client:" << client_fd << ", server:" << server_fd << endl;
    if (client_fd) {
        close(client_fd);
    }
    client_fd = 0;

    if (server_fd) {
        close(server_fd);
    }
    server_fd = 0;
}

void hello_world() {
    cout << "Hellow World" << endl;
}

void ruralis_http_call() {
    cout << FILE_LINE << endl;

}

void *ruralis_http_recv_thread(void *arg) {

    RuralisHttp *http =  (RuralisHttp *)arg;
    // int a_client_fd = http_server->client_fd;
    cout << FILE_LINE << http->client_fd << endl;
    cout << FILE_LINE << http->port_no << endl;

    http->recv_thread(http->port_no, http->client_fd);
    http->client_fd = 0;
    delete http;
    return NULL;
}

void RuralisHttp::start() {
    //RuralisHttp *clientHttp;
    cout << "ポート番号：" << port_no << endl;

    //struct sockaddr client_addr;
    //socklen_t client_addrlen;
    struct sockaddr_in serv_addr;
    // ソケットを作成する
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        throw runtime_error(FILE_LINE " ソケット作成失敗");
    }
    // ソケットにアドレスを割り当てる
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_no);
    int bind_result = bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (bind_result < 0) {
        close(server_fd);
        cerr << FILE_LINE "server_fd:" << server_fd << endl;
        throw runtime_error(FILE_LINE " binding error!");
    }
    // クライアントからの接続を待つ
    listen(server_fd, 16);
    while(1) {
        cout << FILE_LINE RED(" 接続待ち") " server_fd=" << server_fd << endl;
        // int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        // int newfd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addrlen);
        int newfd = accept(server_fd, NULL, NULL);
        if (newfd < 0) {
            cout << FILE_LINE << "newfd=" << newfd << endl;
            cout << FILE_LINE << "errno=" << errno << endl;
            throw runtime_error(FILE_LINE " accept error");
        }
        /*
        char * cip = inet_ntoa(((struct sockaddr_in *)&client_addr)->sin_addr);
        cout << FILE_LINE  << newfd << " 接続:" 
            <<  cip << ":" << ntohs(((struct sockaddr_in *)&client_addr)->sin_port)
            << endl;
            */
        //clientHttp = new RuralisHttp();
        client_fd = newfd;
        cout << FILE_LINE << newfd << " 受信スレッド開始" << endl;
        pthread_t thread;
        RuralisHttp *client;
        client = new RuralisHttp();
        client->port_no = port_no;
        client->client_fd = newfd;
        client->top_dir = top_dir;
        client->content_func = content_func;
        pthread_create(&thread, NULL, ruralis_http_recv_thread, client);
        pthread_detach(thread);
        //  thread th(&RuralisHttp::recv_thread, this);


        // th.join();
        //delete clientHttp;
    }
    close(server_fd);
}

void RuralisHttp::add_res_http200() {
    string empty;
    add_res_http200(empty);
}

void RuralisHttp::add_res_http200(string& body) {
    cout << FILE_LINE << client_fd << " レスポンス作成" << request_map["Host"] << endl;

    string location;
    location += "http://";
    location += request_map["Host"];
    location += "/";
    char buffer[100];
    response += "HTTP/1.1 200 OK\r\n";
    if (response_content.content_type.size() > 0) {
        response += "Content-Type: ";
        response += response_content.content_type;
        response += "\r\n";
    } else {
        response += "Content-Type: text/html\r\n";
    }
    response += "Connection: keep-alive\r\n";
    // response += "Vary: negotiate\r\n";
    // response += "TCN: choice\r\n";

    if (body.size() > 0) {
        snprintf(buffer, sizeof(buffer), "%ld\r\n", body.size());
        response += "Content-Length: "; 
        // response += buffer;
        response += to_string(body.size());
    } else if (response_content.size > 0) {
        response += "Content-Length: "; 
        response += to_string(response_content.size);        
    }
    response += "\r\n";

    response += "Location: ";
    response += location;
    response += "\r\n";
    response += "Content-Location: ";
    response += location;
    response += "\r\n";
    response += "\r\n";
    response += body;
}

void RuralisHttp::load_temp(char *file_name) {
    cout << FILE_LINE " file_name=" << file_name << endl;
    char buffer[256];
    int fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        cerr << FILE_LINE " file_name=" << file_name << endl;
        throw runtime_error(FILE_LINE " file open error!");
    }
    int n =  0;
    do {
        n = read(fd, buffer, sizeof(buffer) - 1);
        // cout << FILE_LINE "  n:" << n << endl;
        if (n < 0) {
            throw runtime_error(FILE_LINE " read error!");
        }
        buffer[n] = '\0';
        temp += buffer;
    } while(n != 0);
}

#define WHITESPACE " \n\r\t\f\v"
void ruralis_line_splitting(string &src, vector<string> &dst) {

    istringstream iss(src);
    while(1) {
        dst.push_back("");
        size_t last = dst.size() - 1;
        string line;
        if (getline(iss, line)) {
            size_t ltrim = line.find_last_not_of(WHITESPACE);
            if (ltrim == string::npos) {
                dst[last] = line;
            } else {
                dst[last] = line.substr(0, ltrim + 1);
            }
            // cout << FILE_LINE << last << ":" << dst[last] << endl;
        } else {
            break;
        }
    }
}
size_t ruralis_skip_space(string &src, size_t pos) {
    size_t spos = src.find_first_not_of(WHITESPACE, pos);
    return (spos == string::npos) ? pos : spos;
}

size_t ruralis_next_space(string &src, size_t pos) {
    return src.find_first_of(WHITESPACE, pos);
}

void ruralis_load_file(const char *file_name, string& dst) {
    // cout << FILE_LINE " file_name=" << file_name << endl;
    char buffer[256];
    int fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        cerr << FILE_LINE " file_name=" << file_name << endl;
        throw runtime_error(FILE_LINE " file open error!");
    }
    int n =  0;
    do {
        n = read(fd, buffer, sizeof(buffer) - 1);
        // cout << FILE_LINE "  n:" << n << endl;
        if (n < 0) {
            throw runtime_error(FILE_LINE " read error!");
        }
        buffer[n] = '\0';
        dst += buffer;
    } while(n != 0);
    close(fd);
}

void ruralis_replace_temp(string &temp, map<string, string> param_map, string &dst) {
    size_t spos_s = 0;
    while(1) {
        size_t spos = temp.find("{{", spos_s);
        if (spos == string::npos) break;
        // cout << FILE_LINE << spos << endl;
        size_t epos = temp.find("}}",spos);
        if (epos == string::npos) break;
        // cout << FILE_LINE << epos << endl;

        dst += temp.substr(spos_s, spos - spos_s);
        string name = temp.substr(spos + 2, epos - spos - 2);

        spos_s = epos + 2;
        // cout << FILE_LINE << name << "="<< param_map[name] << endl;
        dst += param_map[name];
    }
    dst += temp.substr(spos_s);
}


void ruralis_random_key(string &dst) {
    char k[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
        , 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
        , 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

    for (int i = 0; i < 16; i++) {
        dst += k[rand() % sizeof(k)];
    }

}

bool is_hex_digit(char c) {
  return (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || (isdigit(c));
}


std::string ruralis_urldecode(const std::string& encoded_str) {
  std::string decoded_str;

  for (size_t i = 0; i < encoded_str.size(); ++i) {
    if (encoded_str[i] == '%') {
      if (i + 2 < encoded_str.size() && is_hex_digit(encoded_str[i + 1]) &&
          is_hex_digit(encoded_str[i + 2])) {
        // 16進数コードの場合
        char hex_char[3] = {encoded_str[i + 1], encoded_str[i + 2], '\0'};
        unsigned int code = std::stoul(hex_char, nullptr, 16);
        decoded_str += static_cast<char>(code);
        i += 2;
      } else {
        // エラー処理
        std::cerr << "Invalid URL encoding: " << encoded_str << std::endl;
        return "";
      }
    } else if (encoded_str[i] == '+') {
        decoded_str += ' ';
    } else {
        decoded_str += encoded_str[i];

    }
  }

  return decoded_str;
}

std::string ruralis_urlencode(const std::string& str) {
    std::string encoded_str;

    for (size_t i = 0; i < str.size(); i++) {
        unsigned char c = str[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '*' ||
            c == ',' || c == ':' || c == '@' || c == '=' || c == '+' ||
            c == '$' || c == '(' || c == ')' || c == '[' || c == ']' ||
            c == '{' || c == '}' || c == ';' || c == '&' || c == '%' ||
            c == '<' || c == '>' || c == ',' || c == '?' || c == '/' ||
            c == '\\' || c == '^' || c == '`' || c == '|' || c == '~') {
            encoded_str += c;
        } else {
            encoded_str += "%";
            char hex_digits[3];
            snprintf(hex_digits, 3, "%02X", (int) c);
            hex_digits[2] = '\0';
            encoded_str.append(hex_digits, 2);
        }
    }
    return encoded_str;
}
string ruralis_esc_html(const string& str) {
    string esc_html;
    for (size_t i = 0; i < str.size(); i++) {
        unsigned char c = str[i];
        if (c == '<') {
            esc_html += "&lt;";
        } else if (c == '>') {
            esc_html += "&gt;";
        } else if ( c == '&') {
            esc_html += "&amp;";
        } else if ( c == '"') {
            esc_html += "&quot;";
        } else {
            esc_html += c;
        }
    }
    return esc_html;
}




void RuralisHttp::set_param_map(std::string &arg) {

    // size_t spos = 0;
    size_t p_spos = arg.find('?');
    if (p_spos == string::npos) {
        p_spos = 0;
    } else {
        p_spos++;
    }
    while (p_spos != string::npos) {
        size_t p_epos = arg.find('=', p_spos);
        if (p_epos == string::npos) break;
        size_t v_spos = p_epos + 1;
        size_t v_epos = arg.find_first_of("&#", v_spos);
        if (v_epos == string::npos) {
            cout << FILE_LINE << client_fd << " " << arg.substr(p_spos, p_epos - p_spos) << "=" << ruralis_urldecode(arg.substr(v_spos)) << endl;
            param_map[arg.substr(p_spos, p_epos - p_spos)]
                 = ruralis_urldecode(arg.substr(v_spos));

            break;
        }
        if (arg[v_epos] == '#') {
            return;
        }
        cout << FILE_LINE << client_fd << " " << arg.substr(p_spos, p_epos - p_spos) << "="
            << ruralis_urldecode(arg.substr(v_spos, v_epos - v_spos)) << endl;
        param_map[arg.substr(p_spos, p_epos - p_spos)]
             = ruralis_urldecode(arg.substr(v_spos, v_epos - v_spos));
        p_spos = v_epos;
        p_spos++;
    }
}


int RuralisHttp::recv_buf() {

    buf_i++;
    if (buf_e <= buf_i) {
        buf_i = 0;
        buf_e = recv(client_fd, recv_buffer, sizeof(recv_buffer) - 1, 0);
        if (buf_e < 0) {
            cout << FILE_LINE << client_fd 
                << " n=" << buf_e << ", errno=" << errno << endl;
            switch(errno) {
            case ECONNRESET:
                cout << FILE_LINE << client_fd <<  "ECONNRESET(切断されてます。)"
                 << endl;               
                return -2;
            case ECONNABORTED:
               cout << FILE_LINE << client_fd <<  "ECONNABORTED(切断されてます。)"
                 << endl;               
                return -2;
            case ENOTSOCK:
               cout << FILE_LINE << client_fd <<  "ENOTSOCK"
                 << endl;               
                return -2;
            case EBADF:
               cout << FILE_LINE << client_fd <<  "EBADF Bad File Descriptor"
                 << endl;               
                return -2;
            }

            char msg[1024];
            snprintf(msg, sizeof(msg), "%s %d read error!!", FILE_LINE, client_fd);
            throw runtime_error(msg);
        }
        if (buf_e == 0) {
            cout << FILE_LINE << client_fd <<  "受信終了" << endl;
            return -1;
        }
    }
    return (int)recv_buffer[buf_i];
}
    // char recv_buffer[256];


int RuralisHttp::add_send_buf(char c) {
    // send_buffer[256];
    // send_buf_e;
    // send_buf_i;
    send_buffer[send_buf_i] = c;
    send_buf_i++;

    if (send_buf_i == sizeof(send_buffer)) {
        return send_buf();
    }
    return 0;
}

int RuralisHttp::send_buf() {
    if (send_buf_i == 0) {
        return 0;
    }
    int send_result = send(client_fd, send_buffer, send_buf_i, 0); 
    if (send_result < 0) {
        cout << FILE_LINE << client_fd << "送信エラー" 
            << " n=" << send_result << ", errno=" << errno << endl;
        return send_result;
    }
    send_buf_i = 0;
    return 0;
}

int RuralisHttp::send_content() {
    return 0;
}


// std::mutex mtx;
void RuralisHttp::recv_thread(int a_port_no, int a_client_fd) {
    RuralisHttp http;
    http.port_no = a_port_no;
    http.server_fd = 0;
    http.client_fd = a_client_fd;
    http.top_dir = top_dir;
    cout << FILE_LINE << http.client_fd << "▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼" 
        << " ポート番号:" << http.port_no << endl;

    while(1) {
        cout << FILE_LINE << http.client_fd << "▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼" << endl;
        http.request.clear();
        int ch;
        while ((ch = http.recv_buf()) >= 0) {
            http.request += (char)ch;
            int size =  http.request.length();
            if (ch == 0x0a) {
                if (size > 3 && http.request[size - 3] == 0x0a) break;
            }
        }
        if (ch < 0) goto thread_end;
        cout << FILE_LINE << "要求内容" << endl;
        cout << http.request << endl;
        http.request_lines.clear();
        ruralis_line_splitting(http.request, http.request_lines);
        http.request_map.clear();
        http.param_map.clear();
        for (size_t i = 0; i < http.request_lines.size(); i++) {
            string& line = http.request_lines[i];
            if (i) {
                size_t pos = line.find(":");
                if (pos == string::npos) {
                    break;
                }
                size_t vpos = ruralis_skip_space(line, pos + 1);
                size_t v_epos = line.find_last_not_of(WHITESPACE) + 1;

                // cout << FILE_LINE << line.substr(0, pos) << "---" << line.substr(vpos) << endl; 
                if (v_epos != string::npos) {
                    http.request_map[line.substr(0, pos)] = line.substr(vpos, v_epos - vpos);
                } else {
                    http.request_map[line.substr(0, pos)] = line.substr(vpos);
                }
            } else {
                // cout << FILE_LINE << http.client_fd << " 要求：" << line << endl;
                size_t cmd_spos = 0;
                size_t cmd_epos = ruralis_next_space(line, cmd_spos);
                if (cmd_epos != string::npos) {
                    http.request_map["Command"] = line.substr(cmd_spos, cmd_epos - cmd_spos);
                    cout << FILE_LINE << http.client_fd << " コマンド："
                    << http.request_map["Command"] << endl;
                }
                size_t url_spos = ruralis_skip_space(line, cmd_epos);
                size_t url_epos = ruralis_next_space(line, url_spos);

                if (url_epos != string::npos) {
                    http.request_map["Path"] = line.substr(url_spos, url_epos - url_spos);
                    cout << FILE_LINE  << http.client_fd << " 引数:"
                    << http.request_map["Path"] << endl;
                    http.set_param_map(http.request_map["Path"]);
                }
            }
        }
        if (http.request_map.count("Content-Length")) {
            string ctls = http.request_map["Content-Length"];
            cout << FILE_LINE  << http.client_fd << "|" << ctls << "|" << endl;
            cout << FILE_LINE << ctls.size() << endl;
            int content_length = stoi( http.request_map["Content-Length"]);
            cout << FILE_LINE << http.client_fd << " content-length=" << content_length << endl;
            string content;
            for (int i = 0; i < content_length; i++) {
                int ch = http.recv_buf();
                if (ch < -1) goto thread_end;
                content += (char)ch;
            }
            // cout << FILE_LINE << http.client_fd << "コンテンツ" << content_length << endl;
            cout << content << endl;
            http.set_param_map(content);
        }

        cout << FILE_LINE << "page=" << http.param_map["page"] << endl;

        (*content_func)(http);

        if (http.response.size() > 0) {
            cout << FILE_LINE << http.client_fd << RED("送信！！！！！") << endl;
            cout << http.response << endl;
            const char *c_response = http.response.c_str();
            int c_size = (int)http.response.size();
            int send_result;
            for (int i = 0; i < c_size; i++) {
                send_result = add_send_buf(c_response[i]);
                if (send_result < 0) {
                    cout << FILE_LINE << client_fd << "送信エラー" 
                    << " n=" << send_result << ", errno=" << errno << endl;
                        goto thread_end;
                }
            }
            c_size = http.response_content.size;
            for (int i = 0; i < c_size; i++) {
                send_result = add_send_buf(http.response_content.body[i]);
                if (send_result < 0) {
                    cout << FILE_LINE << client_fd << "送信エラー" 
                    << " n=" << send_result << ", errno=" << errno << endl;
                        goto thread_end;
                }
            }
            //　バスファ内の残りデータを送信。
            send_result = send_buf();
            if (send_result < 0) {
                cout << FILE_LINE << client_fd << "送信エラー" 
                << " n=" << send_result << ", errno=" << errno << endl;
                    goto thread_end;
            }
            http.response.clear();
            http.response_content.clear();


        }
        cout << FILE_LINE << http.client_fd << "▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲" << endl;
    }
thread_end:
    cout << FILE_LINE << http.client_fd << "▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲スレッド終了" << endl;
}


//std::string s ("samurai juku");
//std::regex e ("(w+) (w+)"); 
//std::regex_replace (s, e,"$2 $1");

/*
std::string replaceAll(std::string &replacedStr, std::string from, std::string to) {
    unsigned int pos = replacedStr.find(from);
    int toLen = to.length();
 
    if (from.empty()) {
        return replacedStr;
    }
}
 
std::string replaceAll(std::string &replacedStr, std::string from, std::string to) {
    unsigned int pos = replacedStr.find(from);
    int toLen = to.length();
 
    if (from.empty()) {
        return replacedStr;
    }
 
    while ((pos = replacedStr.find(from, pos)) != std::string::npos) {
        replacedStr.replace(pos, from.length(), to);
        pos += toLen;
    }
    return replacedStr;
}
*/

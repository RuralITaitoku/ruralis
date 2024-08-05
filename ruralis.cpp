#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <iostream>
#include "zubolite.h"
#include <string>
#include <map>
#include <regex>
using namespace std;

RuralisHttp ruralis_server_http;


#define RED(x) "\033[31m" x "\033[0m"


void ouch(int sig) 
{
    printf("I got signal %d\n", sig);
    ruralis_server_http.close_fd();
    signal(SIGINT, SIG_DFL);
}
void sig_pipe(int sig) {
    printf("I got signal %d\n", sig);
}


void test_regex() {
    string src = "<<<aaaa>>>>>{{ test }}aaaavvddccc";
    regex e("\\{\\{\\s*test\\s*\\}\\}");
    string dst = "こんにちは";
    regex_replace(src, e, dst);
    cout << FILE_LINE" "<< src << endl;
}

void test_random_key() {
    srand((unsigned int)time(NULL)); // 現在時刻をシード値として設定
    string rand;
    rand.clear();ruralis_random_key(rand);
    cout << "乱数：" << rand << endl;
    rand.clear();ruralis_random_key(rand);
    cout << "乱数：" << rand << endl;
    rand.clear();ruralis_random_key(rand);
    cout << "乱数：" << rand << endl;
    rand.clear();ruralis_random_key(rand);
    cout << "乱数：" << rand << endl;
    rand.clear();ruralis_random_key(rand);
    cout << "乱数：" << rand << endl;
    rand.clear();ruralis_random_key(rand);
    cout << "乱数：" << rand << endl;
    rand.clear();ruralis_random_key(rand);
    cout << "乱数：" << rand << endl;
    rand.clear();ruralis_random_key(rand);
    cout << "乱数：" << rand << endl;
}

void test_url_encode() {
    string test = "2024年7月";
    string dst;
    cout << FILE_LINE << ruralis_urlencode(test) << endl;
}

void test_file() {
    RuralisBytes file;
    string filename = "/Users/yuruyuru/relax/taiki/message.xml";
    file.load(filename);
    cout << FILE_LINE << file.body << endl;
    cout << FILE_LINE << "size = " << file.size << endl;
    cout << FILE_LINE << "content type = " << file.content_type << endl;
}

void test_esc_html() {
    string html = " <test> & \" </test>";
    cout << FILE_LINE << ruralis_esc_html(html) << endl;

}

int main(int argv, char *argc[])
{
    // test_sql();
    // test_regex();
     test_url_encode();
    // test_zubolite();
    test_file();
    test_esc_html();
    signal(SIGINT, ouch);
    signal(SIGPIPE, sig_pipe);
    string page_name = "こんにちは";
    cout << ruralis_sqltext(page_name) << endl;

    try {
        int port_no = 1581;
        string top_dir = "/Library/WebServer/Documents";
        for (int i = 1; i < argv; i++) {
            string opt;
            opt = argc[i];
            if (opt == "-p") {
                if (i + 1 < argv) {
                    i++;
                    port_no = stoi(argc[i]);
                }
            } else {
                top_dir = opt;
            }
        }

        if (argv > 1) {
        }
        cout << FILE_LINE << "ポート番号:" << port_no << endl;
        cout << FILE_LINE << "トップフォルダ:" << top_dir << endl;
        ruralis_server_http.top_dir = top_dir;
        ruralis_server_http.content_func = &zubolite_content;
        ruralis_server_http.port_no = port_no;
        ruralis_server_http.start();
    } catch (const exception& e) {
        cerr << RED("エラー発生\r\n") << e.what() << "\r\n" << endl;
    }
}

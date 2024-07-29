#include "ruralis.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

RuralisBytes::RuralisBytes() {
    cout << FILE_LINE << "RuralisBytes　コンストラクタ" << endl;
    size = 0;
    capacity = 4096;
    body = NULL;
}
RuralisBytes::~RuralisBytes() {
    cout << FILE_LINE << "RuralisBytes　デストラクタ" << endl;
    if (body != NULL) {
        free(body);
        body = NULL;
    }
}

void RuralisBytes::determine_content_type(char* type) {
    size_t idx;
    content_type = "text/html";
    if (file_name.size() < 6) {
        return;
    }
    idx = file_name.find(".xml", file_name.size() - 5);
    if (idx != string::npos) {
        content_type = "text/xml";
        return;
    }
    idx = file_name.find(".jpg", file_name.size() - 5);
    if (idx != string::npos) {
        content_type = "image/jpeg";
        return;
    }
    idx = file_name.find(".mp4", file_name.size() - 5);
    if (idx != string::npos) {
        content_type = "video/mp4";
        return;
    }
    idx = file_name.find(".mp4", file_name.size() - 5);
    if (idx != string::npos) {
        content_type = "video/mp4";
        return;
    }
    idx = file_name.find(".ico", file_name.size() - 5);
    if (idx != string::npos) {
        content_type = "image/x-icon";
        return;
    }
}


int RuralisBytes::load(const string &name) {
    char buffer[256];
    if (body == NULL) {
        body = (char*)malloc(capacity);
        if (!body) {
            throw runtime_error(FILE_LINE " malloc error!");
        }
    }
    file_name = name;
    cout << FILE_LINE " file_name=" << file_name << endl;
    int fd = open(file_name.c_str(), O_RDONLY);
    if (fd < 0) {
        cerr << FILE_LINE " file_name=" << file_name
            << " file open error!" << endl;
        return 1;
    }
    int n = 0;
    do {
        n = read(fd, buffer, sizeof(buffer) - 1);
        if (n < 0) {
            cerr << FILE_LINE " file_name=" << file_name
                << " file read error!" << endl;
            return 2;
        }
        buffer[n] = '\0';
        for (int i = 0; i < n; i++) {
            add(buffer[i]);
        }
    } while(n != 0);

    body[size] = '\0';
    determine_content_type();
    return 0;
}

void RuralisBytes::get_string(string &dst) {
    for (int i = 0; i < size; i++) {
        dst += body[i];
    }
}


void RuralisBytes::add(char c){
    if (size == capacity) {
        size_t new_cap = capacity * 2;
        char *new_data = (char*)realloc(body, new_cap);
        if (!new_data) {
            cerr << FILE_LINE << "realloc error!" << endl;
            return;
        }
        capacity = new_cap;
        body = new_data;
    }

    body[size] = c;
    size++;
}



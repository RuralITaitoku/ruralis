#include <stdio.h>

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <iomanip>

using namespace std;

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define FILE_LINE __FILE__ ":" TOSTRING(__LINE__) " "


void get_point(vector<double> &vec, string &line) {
    // cout << FILE_LINE << line << endl;
    // int count = 0;
    int s_i = 0;
    while (1) {
        s_i = line.find("[", s_i);
        if (s_i == string::npos) {
            break;
        }
        s_i += 1;
        int e_i = s_i;
        e_i = line.find(",", e_i);
        if (e_i == string::npos) {
            break;
        }
        string keido = line.substr(s_i, e_i - s_i);
        // cout << FILE_LINE << "経度" << count << "：" << keido << "|" << stod(keido) << endl;
        s_i = e_i + 1;
        // s_i += 1;
        e_i = line.find("]", s_i);
        if (e_i == string::npos) {
            break;
        }
        string ido = line.substr(s_i, e_i - s_i);
        //cout << FILE_LINE << "緯度" << count << "：" << ido << "|" << std::setprecision(16) << stod(ido) << endl;
        vec.push_back(stod(ido));
        vec.push_back(stod(keido));
        // cout << FILE_LINE << endl;
        // count++;
        s_i = e_i;
    }
    vec.push_back(-1.0);


}
void print_insert(vector<double>  &vec) {

    cout << "insert into farmland_polygon (center_ido, center_keido, coordinates) values(";

    double sum_ido = 0.0;
    double sum_keido = 0.0;
    for (int i = 0; i < vec.size(); i+= 2) {
        sum_ido += vec[i];
        sum_keido += vec[i + 1];
    }
    double c_ido = sum_ido / (vec.size() / 2);
    double c_keido = sum_keido / (vec.size() / 2);
    cout << std::setprecision(16) << c_ido << "," << std::setprecision(16) << c_keido << ",";
    cout << "'";
    for (int i = 0; i < vec.size(); i+= 2) {
        if (i > 0) {
            cout << ",";
        }
        cout << "[" << std::setprecision(16) << vec[i] << "," << std::setprecision(16)<< vec[i +1] << "]";
    }
    cout << "');" << endl;

}


int main(int argv, char *argc[]) {

    if (argv > 1) {
        vector<double> vec; 
        //cout << "fp2sql" << argc[1] << endl;

        FILE *fp;
        char str[5096];

        // ファイルを開く
        fp = fopen(argc[1], "r");
        if (fp == NULL) {
            printf("ファイルが開けません\n");
            return 1;
        }
        // 1行ずつ読み込む
        while (fgets(str, sizeof(str), fp) != NULL) {
            string line = str;
            int s_i = line.find("coordinates", 0);
            // cout << FILE_LINE << ":" << s_i << endl;
            if (s_i == string::npos) {
                // cout << FILE_LINE << line << endl;
                continue;
            }
            s_i += 15;
            int e_i = line.find("]]", s_i);
            // cout << FILE_LINE << ":" << e_i << endl;
            if (e_i == string::npos) {
                continue;
            }
            string coostr = line.substr(s_i, e_i - s_i);
            // cout << FILE_LINE << coostr << endl;
            get_point(vec, coostr);
        }
        vector<double> stack;
        for (int i = 0 ; i < vec.size(); i++) {
            if (vec[i] >= 0) {
                stack.push_back(vec[i]);
            } else {
                print_insert(stack);
                stack.clear();
            }
        }

        printf("\n");
        // ファイルを閉じる
        fclose(fp);
    }
    return 0;
}

#include "zubolite.h"
using namespace std;


RuraliSql sqlite;

string a_link(string &href, string &label) {
    string result;
    result += "<a href='";
    result += href;
    result += "'>";
    result += label;
    result += "</a>\r\n";
    return result;
}

void zubolite_square_brackets(string &src, string&dst) {
    // [] の処理
    size_t s_sbi = 0;
    size_t sbi = src.find_first_of("[", s_sbi);
    if (sbi != string::npos) {
        bool image_href = false;
        cout << FILE_LINE << "[]の処理。" << sbi 
                << "|" << src[sbi] << "|"
                << "|" << src[sbi - 1] << "|"
                 << endl;
        if (sbi > 1 && src[sbi - 1] == '!') {
            image_href = true;
        }
        size_t ebi = src.find_first_of("]", sbi);
        if (ebi != string::npos) {
            // []の処理
            string label = src.substr(sbi + 1, ebi - sbi -1);
            int next = (char)src[ebi];
            cout << FILE_LINE "[" << label << "]" << to_string(next) << endl;
            string before;
            string href;
            if (ebi < src.size() - 2 && src[ebi + 1] == '(') {
                size_t ebracket = src.find_first_of(")", ebi + 1);
                if (ebracket != string::npos) {
                    href = src.substr(ebi + 2, ebracket - ebi - 2);
                    ebi = (ebracket < src.size() - 1) ? ebracket : src.size() - 1;
                }
            }
            if (image_href) {
                before =  src.substr(s_sbi, sbi - s_sbi - 1);
            } else {
                before =  src.substr(s_sbi, sbi - s_sbi);
            }
            if (href.size() == 0) {
                href += "./?page=";
                href += ruralis_urlencode(label); 
            }
            dst += before;
            if (image_href) {
                dst += "<img src='";
                dst += href;
                dst += "' alt='";
                dst += label;
                dst += "' />";
            } else {
                dst += "<a href='";
                dst += href;
                dst += "'>";
                dst += label;
                dst += "</a>";
            }
            string after = src.substr(ebi + 1);
            zubolite_square_brackets(after, dst);
            // dst += after;

        } else {
            string after = src.substr(s_sbi);
            zubolite_square_brackets(after, dst);
            //dst += after;
        }
    } else {
        string after = src.substr(s_sbi);
        dst += after;
    }
}

void zubolite_md_to_html(int row, int br_pass, string &src, string &table_of_contents, string &dst) {
    //　行単位の処理　↓↓↓↓↓↓↓↓↓↓↓
    // #見出しの処理
    if (src[0] == '#') {
        string dst2;
        string id = "row";
        id += to_string(row);
        string href = "#";
        href += id;

        // id += to_string(row);

        if (src[1] == '#') {
            if (src[2] == '#') {
                string src2 = (src[3] == '-') ? src.substr(4): src.substr(3);
                zubolite_md_to_html(row, 1, src2, table_of_contents, dst2);
                if (src[3] != '-') {
                    table_of_contents += "　　・";
                    table_of_contents += a_link(href, dst2);
                    table_of_contents += "</br>\r\n";
                }
                dst += "<h4 id='";
                dst += id;
                dst += "'>";
                dst += dst2;
                dst += "</h4>\r\n";
            } else {
                string src2 = (src[2] == '-') ? src.substr(3): src.substr(2);
                zubolite_md_to_html(row, 1, src2, table_of_contents, dst2);
                if (src[2] != '-') {
                    table_of_contents += "　・";
                    table_of_contents += a_link(href, dst2);
                    table_of_contents += "</br>\r\n";
                }
                dst += "<h3 id='";
                dst += id;
                dst += "'>";
                dst += dst2;
                dst += "</h3>\r\n";
            }
        } else {
            string src2 = (src[1] == '-') ? src.substr(2): src.substr(1);
            zubolite_md_to_html(row, 1, src2, table_of_contents, dst2);
            if (src[1] != '-') {
                table_of_contents += "・";
                table_of_contents += a_link(href, dst2);
                table_of_contents += "</br>\r\n";
            }
            dst += "<h2 id='";
            dst += id;
            dst += "'>";
            dst += dst2;
            dst += "</h2>\r\n";
        }
        return;
    } else {
        int idx = src.find("```");
        if (idx == 0) {
            if (src.size() > 4) {
                dst += "<div class='inline_frame'>\r\n";
                return;
            } else {
                dst += "</div>\r\n";
                return;
            }
        }
    }
    // ---- の処理
    size_t f_hr = src.find("----");
    if (f_hr == 0) {
        dst += "<hr/>\r\n";
        return;
    }
    //　行単位の処理　↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
    // [] の処理
    zubolite_square_brackets(src, dst);
    if (br_pass) return;
    dst += "</br>\r\n";
}
void zubolite_wlines_to_html(vector<string> &lines, string &table_of_contents, string &line_type, string &dst) {


    for (size_t i = 0; i < lines.size(); i++) {
        zubolite_md_to_html(i, 0, lines[i], table_of_contents, dst);
    }
}

void zubolite_wtml_to_html(string &src, string &dst) {
    string table_of_contents;
    string contents;
    string line_type;
    vector<string> lines;
    ruralis_line_splitting(src, lines);

    zubolite_wlines_to_html(lines, table_of_contents, line_type, contents);

    if (table_of_contents.size() > 0) {
        dst += "<div class='table_of_contents'>";
        dst += table_of_contents;
        dst += "</div></br>\r\n";
    }
    dst += contents;
};



int zubolite_get_page(const string &page_name, string &wtml, string &html) {

    string sql = "select name, wtml, html from page where name=";
    sql += ruralis_sqltext(page_name);
    vector< vector<string> > result_data;
    string result = sqlite.select(sql, result_data);
    if (result_data.size() == 0) {
        cout << FILE_LINE << "そのページありません。" << page_name << endl;
        return 1;
    } else {
        wtml = result_data[0][1];
        html = result_data[0][2];

        // cout << FILE_LINE << wtml << endl;
        // cout << FILE_LINE << bodyhtml << endl;
        return 0;
    }
}

void test_zubolite() {
    string src;
    string dst;
    src += "<<<< >>>>  [テスト]の文字列\r\n[テスト]a\r\n";
    src += "      \r\n";
    src += "abcd[テスト]\r\n";
    src += "# 見出し１\r\n";
    src += "## 見出し２\r\n";
    src += "### 見出し 3\r\n";
    src += "``` test:title\r\n";
    src += "ああああ\r\n";
    src += "いいいい\r\n";
    src += "```\r\n";
    src += "うううう\r\n";
    src += "\r\n";
    src += "\r\n";
    zubolite_wtml_to_html(src, dst);
    cout << FILE_LINE << src << endl;
    cout << FILE_LINE << "----------------" << endl;
    cout << FILE_LINE << dst << endl;
}

void zubolite_content(RuralisHttp &http) {
    pthread_t tid;
    char str_tid[256];
    tid = pthread_self();
    snprintf(str_tid, sizeof(str_tid), "%lu\n", (unsigned long)tid);
    cout << FILE_LINE << "スレッドID：" << str_tid << endl;
    cout << FILE_LINE << http.client_fd << " page=" << http.param_map["page"] << endl;
    cout << FILE_LINE << http.client_fd << " Path=" << http.request_map["Path"] << endl;
    string request_path = http.request_map["Path"];
    // ソープリクエストの処理。
    size_t find_soap = request_path.find("/soap");
    if (find_soap == 0) {
        cout << FILE_LINE << "SOAP要求！！！！！！" << endl;
        string filename = "/Users/yuruyuru/relax/taiki/send.xml";
        http.response_content.load(filename);
        //http.response_content.content_type = "application/soap+xml";
        http.response_content.content_type = "text/xml;charset=utf-8";

        string soap_xml;
        http.response_content.get_string(soap_xml);
        cout << FILE_LINE << soap_xml << endl;
        http.add_res_http200(soap_xml);
        return;
    }
    // ファイルが存在する場合の処理。
    size_t find_q = request_path.find("?");
    string file_path = http.top_dir;
    if (find_q != string::npos) {
        file_path += request_path.substr(0, find_q);
    } else {
        file_path += request_path;
    }
    cout << FILE_LINE << "パス：" << file_path << endl;
    int load_err = http.response_content.load(file_path);
    if (!load_err) {
        http.add_res_http200();
        return;
    }




    // zuboliteのアプリ処理。
    vector<string> values;

    sqlite.open("zubolite.db");

    string table_name = "page";
    vector<string> columns;
    columns.push_back("name PRIMARY KEY");
    columns.push_back("wtml");
    columns.push_back("html");
    string result = sqlite.create_table(table_name, columns);
    if (result.size()) {
        cout << result << endl;
        return;
    }


    string page_name = http.param_map["page"];
    if (http.param_map["page"].size() == 0) {
        page_name = "こんにちは";
    }

    map<string, string> param_map;
    param_map["title"] = ruralis_esc_html(page_name);
    param_map["page"] = ruralis_esc_html(page_name);

    // さいどメニューの処理
    string side_menu_wtml;
    string side_menu_html;
    int select_result = zubolite_get_page("SideMenu", side_menu_wtml, side_menu_html);
    if (select_result) {
        cout << FILE_LINE << "サイドメニューがない。" << endl;
    }
    param_map["sidemenu"] = side_menu_html;

    // ページデータの取得
    string page_wtml;
    string page_html;
    select_result = zubolite_get_page(page_name, page_wtml, page_html);
    if (select_result) {
        cout << FILE_LINE << "ページデータはありません。" << page_name << endl;
    }



    if (http.param_map.count("regist")) {
        // 登録ボタンの処理
        cout << "登録ボタンの処理" << endl;
        string wtml;
        string html;

        wtml = http.param_map["wtml"];
        zubolite_wtml_to_html(wtml, html);


        columns.clear();
        columns.push_back("name");
        columns.push_back("wtml");
        columns.push_back("html");
        values.push_back(http.param_map["page"]);
        values.push_back(wtml);
        values.push_back(html);
        // values = {"hisadomi2", "yuruyuru", "<html></html>"};
        result = sqlite.insert(table_name, columns, values);
        if (result.size()) {
            cout << result << endl;
        }
        param_map["body"] = html;

    } else if (http.param_map.count("update")) {
        // 更新ボタンの処理
        cout << "更新ボタンの処理" << endl;
        string wtml;
        string html;

        wtml = http.param_map["wtml"];
        zubolite_wtml_to_html(wtml, html);

        columns.clear();
        columns.push_back("wtml");
        columns.push_back("html");
        values.push_back(wtml);
        values.push_back(html);
        // values = {"hisadomi2", "yuruyuru", "<html></html>"};
        string where = "name=";
        where += ruralis_sqltext(page_name);
        result = sqlite.update(table_name, columns, values, where);
        if (result.size()) {
            cout << result << endl;
        }
        param_map["body"] = html;

    } else if (http.param_map.count("edit") || page_wtml.size() == 0) {
        cout << "編集ボタンの処理" << endl;
        string tinput;
        ruralis_load_file("html/input_wtml.html", tinput);

        param_map["wtml"] = ruralis_esc_html(page_wtml);
        if (page_wtml.size() > 0) {
            param_map["button_name"] = "update";
            param_map["button_label"] = "更新";
        } else{
            param_map["button_name"] = "regist";
            param_map["button_label"] = "登録";
        }
        string input_html;
        ruralis_replace_temp(tinput, param_map, input_html);
        param_map["body"] = input_html;
    } else {
        cout << "表示の処理" << endl;
        param_map["body"] = page_html;
    }
    http.response.clear();

    string ttemp;
    string user_agent = http.request_map["User-Agent"];
    size_t android = user_agent.find("Android");
    cout << FILE_LINE << "User-Agent='" << user_agent << "'" << endl;

    if (android != string::npos) {
        cout << FILE_LINE << "00000android = " << android << endl;
        ruralis_load_file("html/zubolite_android.html", ttemp);
    } else {
        // cout << FILE_LINE << "android = " << android << endl;
        ruralis_load_file("html/zubolite.html", ttemp);
    }
    string html;
    ruralis_replace_temp(ttemp, param_map, html);
    http.add_res_http200(html);

    // string sql = "select * from page";
    // sqlite.select(sql);



    cout << FILE_LINE << "コンテンツ作成終了" << endl;

}

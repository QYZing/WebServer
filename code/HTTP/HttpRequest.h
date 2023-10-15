//
// Created by dying on 10/11/23.
//

#ifndef WEBSERVER_HTTPREQUEST_H
#define WEBSERVER_HTTPREQUEST_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <regex>

#include <Buffer.h>
#include <Log.h>

namespace dying {
    class HttpRequest {
    public:
        enum PARSE_STATE{
            REQUEST_LINE,
            HEADERS,
            BODY,
            FINISH
        };

        enum HTTP_CODE{
            NO_REQUEST = 0,
            GET_REQUEST,
            BAD_REQUEST,
            NO_RESOURCE,
            FORBIDDENT_REQUEST,
            FILE_REQUEST,
            INTERNAL_ERROR,
            CLOSED_CONNECTION
        };
    public:
        HttpRequest(){
            init();
        }
        ~HttpRequest() = default;

        void init();
        bool parse(Buffer& buff); //解析请求

        std::string path() const;
        std::string& path();
        std::string method() const;
        std::string version() const;
        std::string getPost(const std::string& key) const;
        std::string getPost(const char* key) const;

        bool isKeepAlive() const;

    private:
        bool parseRequestLine(const std::string& line); //解析请求行
        void parseHeader(const std::string& line); //解析头部
        void parseBody(const std::string& line); // 解析body

        void parsePath(); // 解析请求路径
        void parsePost(); // 解析post请求
        void parseFromUrlencoded();

        PARSE_STATE m_state;
        std::string m_method;
        std::string m_path;
        std::string m_version;
        std::string m_body;
        std::unordered_map<std::string,std::string> m_header;
        std::unordered_map<std::string,std::string> m_post;

        static const std::unordered_set<std::string> DEFAULT_HTML;
        static const std::unordered_map<std::string,int> DEFAULT_HTML_TAG;

        static bool userVerify(const std::string& name, const std::string& pwd, bool isLogin);
        static int convertHex(char ch);
    };
}

#endif //WEBSERVER_HTTPREQUEST_H

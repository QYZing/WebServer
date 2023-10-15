//
// Created by dying on 10/11/23.
//

#include "HttpRequest.h"


const std::unordered_set<std::string> dying::HttpRequest::DEFAULT_HTML{
        "/index", "/register", "/login",
        "/welcome", "/video", "/picture",
};

const std::unordered_map<std::string,int> dying::HttpRequest::DEFAULT_HTML_TAG{
        {"/register.html", 0}, {"/login.html", 1},
};

void dying::HttpRequest::init() {
    m_method = m_path = m_version = m_body = "";
    m_state = REQUEST_LINE;
    m_header.clear();
    m_post.clear();
}

bool dying::HttpRequest::parse(dying::Buffer &buff) {
    const char CRLF[] = "\r\n";
    if(buff.readableBytes() <= 0){
        return false;
    }
    while(buff.readableBytes() && m_state != FINISH){
        //找到 第一个crlf  也就是每行请求
        const char * lineEnd = std::search(buff.peek() , buff.beginWriteConst() , CRLF , CRLF + 2);
        std::string line(buff.peek() , lineEnd);
        switch (m_state) {
            case REQUEST_LINE:{
                if(!parseRequestLine(line)){
                    return false;
                }
                parsePath();
                break;
            }
            case HEADERS:{
                parseHeader(line);
                if(buff.readableBytes() <= 2){
                    m_state = FINISH;
                }
                break;
            }
            case BODY:{
                parseBody(line);
                break;
            }
            default:break;
        }
        if(lineEnd == buff.beginWrite()){//全部处理完
            break;
        }
        buff.retrieveUntil(lineEnd + 2);
    }
    buff.retrieveUntil(buff.beginWriteConst());
    LOG_FMT_DEBUG_DEFAULT("[%s] , [%s] , [%s]" , m_method.c_str() , m_path.c_str() , m_version.c_str());
    return true   ;
}

std::string dying::HttpRequest::path() const {
    return m_path;
}

std::string &dying::HttpRequest::path() {
    return m_path;
}

std::string dying::HttpRequest::method() const {
    return m_method;
}

std::string dying::HttpRequest::version() const {
    return  m_version;
}

std::string dying::HttpRequest::getPost(const std::string &key) const {
    if(key == ""){
        throw std::exception();
    }
    if(m_post.count(key) == 1){
        return m_post.find(key)->second;
    }
    return "";
}

std::string dying::HttpRequest::getPost(const char *key) const {
    if(key == nullptr){
        throw std::exception();
    }
    if(m_post.count(key) == 1){
        return m_post.find(key)->second;
    }
    return "";
}

bool dying::HttpRequest::isKeepAlive() const {
    if(m_header.count("Connection") == 1){
        return m_header.find("Connection")->second == "keep-alive" && m_version == "1.1";
    }
    return false;
}

bool dying::HttpRequest::parseRequestLine(const std::string &line) {
    std::regex pattern("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch subMatch;
    if(std::regex_match(line , subMatch , pattern)){
        m_method = subMatch[1];
        m_path = subMatch[2];
        m_version = subMatch[3];
        m_state = HEADERS;
        return true;
    }
    LOG_ERROR_DEFAULT << "RequestLine Error";
    return false;
}

void dying::HttpRequest::parseHeader(const std::string &line) {
    std::regex pattern("^([^:]*): ?(.*)$");
    std::smatch  subMatch;

    if(std::regex_match(line , subMatch , pattern)){
        m_header[subMatch[1]] = subMatch[2];
        LOG_DEBUG_DEFAULT<<"deal header " << subMatch[1] <<"   "<< subMatch[2];
    }else{
        m_state = BODY;
    }
}

void dying::HttpRequest::parseBody(const std::string &line) {
    m_body = line;
    parsePost();
    m_state = FINISH;
    LOG_FMT_DEBUG_DEFAULT("Body:%s, len:%d", line.c_str(), line.size());
}

void dying::HttpRequest::parsePath() {
    LOG_DEBUG_DEFAULT<<"request m_path = " << m_path;
    m_state = HEADERS;
    if(m_path == "/"){
        m_path = "/index.html";
    }else{
        for(auto & item : DEFAULT_HTML){
            if(item == m_path){
                m_path += ".html";
                return ;
            }
        }
    }
}

void dying::HttpRequest::parsePost() {
    if(m_method == "POST" && m_header["Content-Type"] == "application/x-www-form-urlencoded"){
        parseFromUrlencoded();
        if(DEFAULT_HTML_TAG.count(m_path)){
            int tag = DEFAULT_HTML_TAG.find(m_path)->second;
            LOG_FMT_DEBUG_DEFAULT("Tag:%d" , tag);
            if(tag == 0 || tag == 1){
                bool isLogin = (tag == 1);
                if(userVerify(m_post["username"] , m_post["passwd"] , isLogin)){
                    m_path = "/welcome.html";
                }else{
                    m_path = "/error.html";
                }
            }

        }
    }
}

void dying::HttpRequest::parseFromUrlencoded() {
    if(m_body.size() == 0) { return; }

    std::string key, value;
    int num = 0;
    int n = m_body.size();
    int i = 0, j = 0;

    for(; i < n; i++) {
        char ch = m_body[i];
        switch (ch) {
            case '=':
                key = m_body.substr(j, i - j);
                j = i + 1;
                break;
            case '+':
                m_body[i] = ' ';
                break;
            case '%':
                num = convertHex(m_body[i + 1]) * 16 + convertHex(m_body[i + 2]);
                m_body[i + 2] = num % 10 + '0';
                m_body[i + 1] = num / 10 + '0';
                i += 2;
                break;
            case '&':
                value = m_body.substr(j, i - j);
                j = i + 1;
                m_post[key] = value;
                LOG_FMT_DEBUG_DEFAULT("%s = %s", key.c_str(), value.c_str());
                break;
            default:
                break;
        }
    }
    if(j > i){
        throw std::exception();
    }
    if(m_post.count(key) == 0 && j < i) {
        value = m_body.substr(j, i - j);
        m_post[key] = value;
    }

}

bool dying::HttpRequest::userVerify(const std::string &name, const std::string &pwd, bool isLogin) {
    return true;
}

int dying::HttpRequest::convertHex(char ch) {
    if(ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }
    if(ch >= 'a' && ch <= 'f'){
        return ch - 'a' + 10;
    }
    if(ch >= '0' && ch <= '9'){
        return ch - '0';
    }
    return ch;
}

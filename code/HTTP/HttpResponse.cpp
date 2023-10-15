//
// Created by dying on 10/11/23.
//

#include "HttpResponse.h"


using std::unordered_map;
using std::string;

const unordered_map<string, string> dying::HttpResponse::SUFFIX_TYPE = {
        { ".html",  "text/html" },
        { ".xml",   "text/xml" },
        { ".xhtml", "application/xhtml+xml" },
        { ".txt",   "text/plain" },
        { ".rtf",   "application/rtf" },
        { ".pdf",   "application/pdf" },
        { ".word",  "application/nsword" },
        { ".png",   "image/png" },
        { ".gif",   "image/gif" },
        { ".jpg",   "image/jpeg" },
        { ".jpeg",  "image/jpeg" },
        { ".au",    "audio/basic" },
        { ".mpeg",  "video/mpeg" },
        { ".mpg",   "video/mpeg" },
        { ".avi",   "video/x-msvideo" },
        { ".gz",    "application/x-gzip" },
        { ".tar",   "application/x-tar" },
        { ".css",   "text/css "},
        { ".js",    "text/javascript "},
};

const unordered_map<int, string> dying::HttpResponse::CODE_STATUS = {
        { 200, "OK" },
        { 400, "Bad Request" },
        { 403, "Forbidden" },
        { 404, "Not Found" },
};

const unordered_map<int, string> dying::HttpResponse::CODE_PATH = {
        { 400, "/400.html" },
        { 403, "/403.html" },
        { 404, "/404.html" },
};

dying::HttpResponse::HttpResponse() {
    m_code = -1;
    m_path = m_srcDir = "";
    m_isKeepAlive = false;
    m_mmFile = nullptr;
    m_mmFileStat = {0};
}

dying::HttpResponse::~HttpResponse() {
    unmapFile();
}

void dying::HttpResponse::init(const std::string &srcDir, std::string &path, bool isKeepAlive, int code) {
    if(srcDir == ""){
        throw std::exception();
    }
    if(m_mmFile){
        unmapFile();
    }
    m_code = code;
    m_isKeepAlive = isKeepAlive;
    m_path = path;
    m_srcDir = srcDir;
    m_mmFile = nullptr;
    m_mmFileStat = {0};
}

void dying::HttpResponse::makeResponse(dying::Buffer &buff) {
    //判断请求的资源文件

    //请求文件不存在或者是目录
    if(stat((m_srcDir + m_path).data() , &m_mmFileStat) < 0 || S_ISDIR(m_mmFileStat.st_mode)){
        m_code = 404;
    }else if(!(m_mmFileStat.st_mode & S_IROTH)){ // 请求没有可读权限
        m_code = 403;
    }else if(m_code == -1){
        m_code = 200;
    }
    LOG_DEBUG_DEFAULT << "response file path= "<<m_srcDir + m_path ;
    errorHtml();
    addStateLine(buff);
    addHeader(buff);
    addContent(buff);
}

void dying::HttpResponse::unmapFile() {
    if(m_mmFile){
        munmap(m_mmFile , m_mmFileStat.st_size);
        m_mmFile = nullptr;
    }
}

char *dying::HttpResponse::file() {
    return m_mmFile;
}

size_t dying::HttpResponse::fileLen() const {
    return m_mmFileStat.st_size;
}

void dying::HttpResponse::errorContent(dying::Buffer &buff, std::string message) {
    string body;
    string status;
    body += "<html><title>Error</title>";
    body += "<body bgcolor=\"ffffff\">";
    if(CODE_STATUS.count(m_code) == 1){
        status = CODE_STATUS.find(m_code)->second;
    }else{
        status = "Bad Request";
    }

    body += std::to_string(m_code) + " : " + status  + "\n";
    body += "<p>" + message + "</p>";
    body += "<hr><em>TinyWebServer</em></body></html>";

    buff.append("Content-length: " + std::to_string(body.size()) + "\r\n\r\n");
    buff.append(body);
}

void dying::HttpResponse::addStateLine(Buffer &buff) {
    string status;
    if(CODE_STATUS.count(m_code) == 1){
        status = CODE_STATUS.find(m_code)->second;
    }else{
        m_code = 400;
        status = CODE_PATH.find(400)->second;
    }
    buff.append("HTTP/1.1 " + std::to_string(m_code) + " " + status + "\r\n");
}

void dying::HttpResponse::addHeader(Buffer &buff) {
    buff.append("Connection: ");
    if(m_isKeepAlive){
        buff.append("keep-alive\r\n");
        buff.append("keep-alive: max=6, timeout=120\r\n");
    }else{
        buff.append("close\r\n");
    }
    buff.append("Content-type: " + getFileType() + "\r\n");
}

void dying::HttpResponse::addContent(Buffer &buff) {
    int srcFd = open((m_srcDir + m_path).data(),O_RDONLY);
    if(srcFd < 0){
        errorContent(buff , "File NotFound");
        return ;
    }
    if(CODE_PATH.count(m_code) == 1){
        m_path = CODE_PATH.find(m_code)->second;
        stat((m_srcDir + m_path).data() , &m_mmFileStat);
    }
    LOG_DEBUG_DEFAULT << "file path "<<(m_srcDir + m_path).data();
    auto mmRet = (int *)mmap(0 , m_mmFileStat.st_size , PROT_READ , MAP_PRIVATE , srcFd , 0);
    if(*mmRet == -1){
        errorContent(buff , "File NotFound!");
        return;
    }
    m_mmFile = (char *)mmRet;
    close(srcFd);
    buff.append("Content-length: " + std::to_string(m_mmFileStat.st_size) + "\r\n\r\n");
}

void dying::HttpResponse::errorHtml() {
    if(CODE_PATH.count(m_code) == 1) {
        m_path = CODE_PATH.find(m_code)->second;
        stat((m_srcDir + m_path).data(), &m_mmFileStat);
    }
}

std::string dying::HttpResponse::getFileType() {
    string::size_type  idx = m_path.find_last_of('.');
    if(idx == string::npos){
        return "text/plain";
    }
    string suffix = m_path.substr(idx);
    if(SUFFIX_TYPE.count(suffix) == 1){
        return SUFFIX_TYPE.find(suffix)->second;
    }
    return "text/plain";
}

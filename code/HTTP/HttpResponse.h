//
// Created by dying on 10/11/23.
//

#ifndef WEBSERVER_HTTPRESPONSE_H
#define WEBSERVER_HTTPRESPONSE_H


#include <sys/stat.h>
#include <string>
#include <unordered_map>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <Log.h>
#include <Buffer.h>

namespace dying {
    class HttpResponse {
    public:
        HttpResponse();
        ~HttpResponse();

        void init(const std::string& srcDir, std::string& path, bool isKeepAlive = false, int code = -1);
        void makeResponse(Buffer& buff);
        void unmapFile();
        char* file();                //回复文件指针
        size_t fileLen() const;      //回复文件stat
        void errorContent(Buffer& buff, std::string message);
        int code() const { return m_code; }

    private:
        void addStateLine(Buffer &buff);
        void addHeader(Buffer &buff);
        void addContent(Buffer &buff);

        void errorHtml();
        std::string getFileType();

        int m_code;
        bool m_isKeepAlive;

        std::string m_path;
        std::string m_srcDir;

        char* m_mmFile;             //回复文件指针
        struct stat m_mmFileStat;   //回复文件stat

        static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
        static const std::unordered_map<int, std::string> CODE_STATUS;
        static const std::unordered_map<int, std::string> CODE_PATH;
    };
}

#endif //WEBSERVER_HTTPRESPONSE_H

//
// Created by dying on 10/12/23.
//

#define CLOSELOG

#include <WebServer.h>
#include <iostream>
#include <Config.h>

int main(int argc , char **argv){
    daemon(1 , 0);
    dying::Config config = YAML::LoadFile("../etc/config.yaml").as<dying::Config>();
    dying::WebServer server(config);
    server.start();
}
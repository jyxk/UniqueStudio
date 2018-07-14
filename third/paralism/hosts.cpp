//
// Created by jyxk on 5/19/18.
//

#include <unordered_map>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <iostream>
#include "hosts.h"
using namespace std;

int Hosts::StatHosts(const char* file_name) {
    FILE *fp;
    if ((fp = fopen(file_name,"r")) == nullptr) {
        printf("cannot open %s", file_name);
        return -1;
    }

    char str_line[MAXLINE];
    while (fgets(str_line, MAXLINE, fp) != nullptr) {
        size_t len = strlen(str_line);
        str_line[len-1] = '\0';
        ParseDomainsByLine(str_line);
    }
    printf("OK\n");
    fclose(fp);
    return 0;
}

int Hosts::Update(const char *ip, const char *domain) {
    if (!strlen(ip) && !strlen(domain))
        return -1;
    else
        InsertDomainToMap(ip, domain);
    return 0;
}


int Hosts::ParseDomainsByLine(char* str_line) {
    if(strlen(str_line) == 0) {
        return -1;
    }

    const char* needle = " \n\t";
    char *ip = nullptr;
    char *domain = nullptr;
    while((ip = strtok(str_line, needle)) != nullptr) {
        domain = strtok(str_line, needle);
        InsertDomainToMap(ip, domain);
    }
    return 0;
}

void Hosts::InsertDomainToMap(const char *ip, const char *domain) {

    if(!strlen(ip) || !strlen(domain))
        return;

    string ip_str = ip;
    string domain_str = domain;
    result_index_[domain_str] = ip_str;
}
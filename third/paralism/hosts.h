//
// Created by jyxk on 5/19/18.
//

#ifndef DNSSERVER_HOSTS_H
#define DNSSERVER_HOSTS_H

#include <unordered_map>

#define MAXLINE 1024

using namespace std;

class Hosts
{
public:
    Hosts(){}
    ~Hosts(){result_index_.clear();}

    int StatHosts(const char *file_name);
    int Update(const char *ip, const char *domain);
    unordered_map<string,string> &result_index(){return result_index_;}


private:
    //存放缓存，IP与domain
    unordered_map<string,string> result_index_;

    int ParseDomainsByLine(char *str_line);
    void InsertDomainToMap(const char *ip, const char *domain);
};

#endif //DNSSERVER_HOSTS_H

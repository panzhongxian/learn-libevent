#pragma once

#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

constexpr char kDemoHttpServerHostName[] = {"www.baidu.com"};

#ifdef DST_LOCAL
constexpr char kDemoHttpServerIp[] = {"127.0.0.1"};
constexpr int kDemoHttpServerPort = 80;
#elif DST_UNREACHABLE
constexpr char kDemoHttpServerIp[] = {"10.0.0.1"};
constexpr int kDemoHttpServerPort = 80;
#elif DST_DEV_DOCKER
constexpr char kDemoHttpServerIp[] = {"9.135.29.28"};
constexpr int kDemoHttpServerPort = 10080;
#else
constexpr char kDemoHttpServerIp[] = {"14.215.177.39"};
constexpr int kDemoHttpServerPort = 80;
#endif

constexpr char kDemoHttpRequestStr[] = {
    "GET / HTTP/1.1\r\n"
    "Connection: close\r\n"
    "Host: www.baidu.com\r\n\r\n"};

constexpr int kMaxHttpClientNum = 8000;

inline void LoadChinaTop100Websites(std::vector<std::string>& url_vec) {
  std::ifstream ifs("china_top_100_websites.txt", std::ios::in);
  std::string name, url, encoding;
  while (ifs >> name >> url >> encoding) {
    if (!url.empty()) {
      url_vec.push_back(url);
    }
  }
}

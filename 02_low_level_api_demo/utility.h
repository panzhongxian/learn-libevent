#pragma once

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
// constexpr char kDemoHttpServerIp[] = {"14.215.177.39"};
constexpr char kDemoHttpServerIp[] = {"183.47.233.9"};
constexpr int kDemoHttpServerPort = 80;
#endif

constexpr char kDemoHttpRequestStr[] = {
    "GET / HTTP/1.1\r\n"
    "Host: www.163.com\r\n\r\n"};

constexpr int kMaxHttpClientNum = 5000;

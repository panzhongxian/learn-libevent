#pragma once

constexpr char kDemoHttpServerHostName[] = {"www.baidu.com"};

#ifdef LOCAL_CONN
constexpr char kDemoHttpServerIp[] = {"127.0.0.1"};
#elif UNREACH_CONN
constexpr char kDemoHttpServerIp[] = {"10.0.0.1"};
#else
constexpr char kDemoHttpServerIp[] = {"14.215.177.39"};
#endif
constexpr int kDemoHttpServerPort = 80;

constexpr char kDemoHttpRequestStr[] = {
    "GET / HTTP/1.1\r\n"
    "Host: www.baidu.com\r\n\r\n"};

constexpr int kMaxHttpClientNum = 5000;

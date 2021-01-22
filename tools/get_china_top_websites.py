import requests
import math
import re
import sys


class ChinaTopWebsiteFetcher:
    base_url = "https://alexa.chinaz.com/Country/index_CN{page_number}.html"
    url_list = []

    def get_page_n(self, n: int):
        page_number = "" if n == 1 else ("_" + str(n))
        url = self.base_url.format(page_number=page_number)
        r = requests.get(url)
        content = str(r.content, "utf-8")
        m = re.findall(r'<li class="clearfix">.*?</li>', content, re.DOTALL)

        for li in m:
            mm = re.match(
                r'.*<span>(.*?)</span>\r\n<a href="(.*?)" rel="nofollow" target="_blank" class="tohome">.*',
                li, re.DOTALL)
            # There may be some exception cases.
            if mm is not None:
                self.url_list.append(tuple(mm.groups()[:2]))

    def get_top_websites(self, n: int):
        assert n <= 500
        pages = math.ceil(n / 25.0)
        for i in range(pages):
            self.get_page_n(i + 1)

        return self.url_list[:n]

    def dump_top_websites(self, n: int, f=None):
        top_websites_list = self.get_top_websites(top_count)
        max_website_name_len = 0
        max_url_len = 0
        for website_name, url in top_websites_list:
            if len(website_name) > max_website_name_len:
                max_website_name_len = len(website_name)
            if len(url) > max_url_len:
                max_url_len = len(url)

        for website_name, url in top_websites_list:
            line = "{} {} {}".format(
                website_name.ljust(max_website_name_len + 2),
                url.ljust(max_url_len + 2), get_website_encode(url))

            if f is None:
                print(line)
            else:
                f.write(line + "\n")


def print_usage():
    print("""Usage:
    python3 get_china_top_websites.py top_count [file_name]""")


def get_website_encode(url: str):
    try:
        r = requests.get(url, timeout=3)
    except:
        return "unknown"

    try:
        content = str(r.content, "utf-8")
        return "utf8"
    except:
        try:
            content = str(r.content, "gbk")
            return "gbk"
        except:
            content = r.content
            return "unknown"


if __name__ == "__main__":
    fetcher = ChinaTopWebsiteFetcher()
    f = None
    top_count = 100
    if len(sys.argv) < 1:
        print_usage()

    top_count = int(sys.argv[1])

    if len(sys.argv) > 2:
        file_path = sys.argv[2]
        try:
            f = open(file_path, "w")
        except:
            raise Exception("Can't create the file:", file_path)
            exit(0)
    fetcher.dump_top_websites(top_count, f)

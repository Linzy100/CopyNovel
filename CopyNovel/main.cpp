#include <iostream>
#include <string>
#include <curl/curl.h>
#include <windows.h>
#include <regex>
#include <fstream>
#include <thread>
#include <chrono>
#include <random>
#include <tlhelp32.h>

bool flag_web_open = false;
DWORD browserProcessID = 0;

// 回调函数，用于处理接收到的数据
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// 转换字符串类型
std::wstring stringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// 打开浏览器窗口
void openBrowser(const std::string& url) {
    std::wstring wUrl = stringToWString(url);
    SHELLEXECUTEINFO shExecInfo = { 0 };
    shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    shExecInfo.hwnd = NULL;
    shExecInfo.lpVerb = NULL;
    shExecInfo.lpFile = wUrl.c_str();
    shExecInfo.lpParameters = NULL;
    shExecInfo.lpDirectory = NULL;
    shExecInfo.nShow = SW_SHOWNORMAL;
    shExecInfo.hInstApp = NULL;

    ShellExecuteEx(&shExecInfo);
    if (shExecInfo.hProcess != NULL) {
        browserProcessID = GetProcessId(shExecInfo.hProcess);
        CloseHandle(shExecInfo.hProcess);
    }
}

// 关闭浏览器窗口
void closeBrowser() {
    if (browserProcessID != 0) {
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, browserProcessID);
        if (hProcess != NULL) {
            TerminateProcess(hProcess, 0);
            CloseHandle(hProcess);
            browserProcessID = 0;
        }
    }
}

// 方法：从HTML内容中提取网页标题
std::string extractTitle(const std::string& htmlContent) {
    std::regex titleRegex("<title>(.*?)</title>");
    std::smatch titleMatch;

    if (std::regex_search(htmlContent, titleMatch, titleRegex) && titleMatch.size() > 1) {
        return titleMatch.str(1);
    }
    else {
        std::cerr << "Title not found in the HTML content." << std::endl;
        return "";
    }
}

// 获取指定URL的内容，带有重试机制和模拟人类行为
std::string fetchURL(const std::string& url, int retries = 5, int minDelay = 5, int maxDelay = 10) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(minDelay, maxDelay);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    for (int attempt = 0; attempt < retries; ++attempt) {
        curl = curl_easy_init();

        if (curl) {
            // 设置Clash代理
            curl_easy_setopt(curl, CURLOPT_PROXY, "http://127.0.0.1:7890"); // Clash默认HTTP代理端口为7890
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            // 模拟浏览器的请求头
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
            headers = curl_slist_append(headers, "Referer: https://www.google.com/");
            headers = curl_slist_append(headers, "Accept-Language: en-US,en;q=0.9");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            res = curl_easy_perform(curl);

            if (res == CURLE_OK) {
                curl_slist_free_all(headers);
                curl_easy_cleanup(curl);
                curl_global_cleanup();
                return readBuffer;
            }
            else {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                curl_easy_cleanup(curl);
                curl_slist_free_all(headers);

                if (attempt < retries - 1) {
                    // 打开浏览器访问页面
                    openBrowser(url);
                    flag_web_open = true;
                    //int delay = dist(gen);
                    //std::cerr << "Retrying in " << delay << " seconds..." << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(3/*delay*/));
                }
                else {
                    std::cerr << "Maximum retry attempts reached. Fetching URL failed." << std::endl;
                }
            }
        }
    }

    curl_global_cleanup();
    return "";
}

// 设置控制台为UTF-8编码
void setConsoleToUTF8() {
    SetConsoleOutputCP(CP_UTF8);
}

void setConsoleToGBK() {
    SetConsoleOutputCP(936); // GBK的代码页
    SetConsoleCP(936);       // 设置输入代码页为GBK
}

// 方法：去除小说正文中偶尔出现的div块
void RemoveDivContent(std::string& htmlContent) {
    size_t startPos = 0;

    while ((startPos = htmlContent.find("<div", startPos)) != std::string::npos) {
        size_t endPos = htmlContent.find("</div>", startPos);
        if (endPos != std::string::npos) {
            endPos += 6; // 包括"</div>"的长度
            htmlContent.erase(startPos, endPos - startPos);
        }
        else {
            // 如果没有找到匹配的结束标签，退出循环
            break;
        }
    }
}

// 方法：从网页源代码提取小说正文并进行相应处理
std::string SplitHtmlContentToNovelPart(std::string& htmlContent) {
    std::string startTag = "&emsp;&emsp;";
    std::string endTag = "&emsp;&emsp;(本章完)";
    std::string result;

    // 查找开始标签的位置
    size_t startPos = htmlContent.find(startTag);
    if (startPos != std::string::npos) {
        // 查找结束标签的位置
        size_t endPos = htmlContent.rfind(endTag);
        if (endPos != std::string::npos) {
            // 提取正文部分
            result = htmlContent.substr(startPos, endPos + endTag.length() - startPos);

            // 替换 &emsp; 为 空格
            size_t pos = result.find("&emsp;");
            while (pos != std::string::npos) {
                result.replace(pos, 6, "   ");
                pos = result.find("&emsp;", pos + 3);
            }

            // 删除 <br /> 
            pos = result.find("<br />");
            while (pos != std::string::npos) {
                result.replace(pos, 6, "");
                pos = result.find("<br />", pos);
            }

            // 删除 <p> 和 </p> 标签
            pos = result.find("<p>");
            while (pos != std::string::npos) {
                result.replace(pos, 3, "");
                pos = result.find("<p>", pos);
            }

            pos = result.find("</p>");
            while (pos != std::string::npos) {
                result.replace(pos, 4, "");
                pos = result.find("</p>", pos);
            }
        }
        else {
            std::cerr << "End tag not found in the HTML content." << std::endl;
        }
    }
    else {
        std::cerr << "Start tag not found in the HTML content." << std::endl;
    }
    RemoveDivContent(result);
    return result;
}


// 方法：将小说正文追加写入文件
void AppendNovelContentToFile(const std::string& novelContent, const std::string& fileName) {
    std::ofstream outFile;

    // 以追加写方式打开文件（如果文件不存在则创建文件）
    outFile.open(fileName, std::ios::out | std::ios::app);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return;
    }

    // 写入小说正文
    outFile << novelContent << std::endl << std::endl;

    // 关闭文件
    outFile.close();
}

int main() {
    setConsoleToGBK();
    bool flag = true;
    std::string NovelName;// 输出txt文件名
    std::string url; // 替换为你要获取的网页URL
    int count = 1;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(5, 10);

    std::cout << "请输入输出文件名(example.txt)：";
    std::cin >> NovelName;
    std::cout << "请输入目标首章URL：";
    std::cin >> url;

    while (flag) {
        std::string htmlContent = fetchURL(url);
        if (!htmlContent.empty()) {
            // 获取网页标题
            std::string title = extractTitle(htmlContent);
            std::cout << "网页标题: " << title << std::endl;

            // 对htmlContent进行处理，只保留小说正文部分
            std::string novel = SplitHtmlContentToNovelPart(htmlContent);
            // 以追加写模式输出小说内容到txt
            AppendNovelContentToFile(novel, NovelName);
            std::cout << "\n第" << count << "章内容已写入TXT文件\n";
            count++;
        }
        else {
            std::cerr << "Error: could not fetch content from URL" << std::endl;
        }
        // 从htmlContent中寻找下一个章节url并进行更新，如果是/end.html就停止循环
        size_t nextPos = htmlContent.find("next_page:");
        if (nextPos != std::string::npos) {
            size_t urlStart = htmlContent.find("\"", nextPos) + 1;
            size_t urlEnd = htmlContent.find("\"", urlStart);
            url = htmlContent.substr(urlStart, urlEnd - urlStart);
            if (url.find("end.html") != std::string::npos)
                flag = false;
        }
        else {
            std::cerr << "Next chapter URL not found. Ending process." << std::endl;
            flag = false;
        }
        if (flag_web_open) {
            // 尝试关闭浏览器窗口
            closeBrowser();
            flag_web_open = false;
        }
        // 添加随机延迟
        //int delay = dist(gen);
        //std::this_thread::sleep_for(std::chrono::seconds(delay));
        std::this_thread::sleep_for(std::chrono::seconds(1/*delay*/));
    }
    std::cout << "\n生成完成，共" << count-1 << "章" << std::endl;
    return 0;
}

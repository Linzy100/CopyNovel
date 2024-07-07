//#include <iostream>
//#include <string>
//#include <curl/curl.h>
//#include <windows.h>
//#include <regex>
//#include <fstream>
//#include <thread>
//#include <chrono>
//#include <random>
//
//// �ص����������ڴ�����յ�������
//static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
//    ((std::string*)userp)->append((char*)contents, size * nmemb);
//    return size * nmemb;
//}
//
//// ��ȡָ��URL������
//std::string fetchURL(const std::string& url) {
//    CURL* curl;
//    CURLcode res;
//    std::string readBuffer;
//
//    curl_global_init(CURL_GLOBAL_DEFAULT);
//    curl = curl_easy_init();
//
//    // ����Clash����
//    curl_easy_setopt(curl, CURLOPT_PROXY, "http://127.0.0.1:7890"); // ClashĬ��HTTP����˿�Ϊ7890
//    if (curl) {
//        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
//
//        // �����ض���
//        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
//
//        // ���ó�ʱʱ��Ϊ30��
//        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
//
//        // ���õ�����Ϣ
//        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
//
//
//        res = curl_easy_perform(curl);
//        if (res != CURLE_OK) {
//            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
//        }
//
//        // ����
//        curl_easy_cleanup(curl);
//    }
//    curl_global_cleanup();
//    return readBuffer;
//}
//
//// ���ÿ���̨ΪUTF-8����
//void setConsoleToUTF8() {
//    SetConsoleOutputCP(CP_UTF8);
//}
//
//void setConsoleToGBK() {
//    SetConsoleOutputCP(936); // GBK�Ĵ���ҳ
//    SetConsoleCP(936);       // �����������ҳΪGBK
//}
//
////������ȥ��С˵������ż�����ֵ�div��
//void RemoveDivContent(std::string& htmlContent) {
//    size_t startPos = 0;
//
//    while ((startPos = htmlContent.find("<div", startPos)) != std::string::npos) {
//        size_t endPos = htmlContent.find("</div>", startPos);
//        if (endPos != std::string::npos) {
//            endPos += 6; // ����"</div>"�ĳ���
//            htmlContent.erase(startPos, endPos - startPos);
//        }
//        else {
//            // ���û���ҵ�ƥ��Ľ�����ǩ���˳�ѭ��
//            break;
//        }
//    }
//}
////����������ҳԴ������ȡС˵���Ĳ�������Ӧ����
//std::string SplitHtmlContentToNovelPart(std::string& htmlContent) {
//    std::string startTag = "&emsp;&emsp;";
//    std::string endTag = "&emsp;&emsp;(������)";
//    std::string result;
//
//    // ���ҿ�ʼ��ǩ��λ��
//    size_t startPos = htmlContent.find(startTag);
//    if (startPos != std::string::npos) {
//        // ���ҽ�����ǩ��λ��
//        size_t endPos = htmlContent.rfind(endTag);
//        if (endPos != std::string::npos) {
//            // ��ȡ���Ĳ���
//            result = htmlContent.substr(startPos, endPos + endTag.length() - startPos);
//
//            // �滻 &emsp; Ϊ �ո�
//            size_t pos = result.find("&emsp;");
//            while (pos != std::string::npos) {
//                result.replace(pos, 6, " ");
//                pos = result.find("&emsp;", pos + 1);
//            }
//
//            // �滻 <br /> Ϊ ���з�
//            pos = result.find("<br />");
//            while (pos != std::string::npos) {
//                result.replace(pos, 6, "\n");
//                pos = result.find("<br />", pos + 1);
//            }
//        }
//        else {
//            std::cerr << "End tag not found in the HTML content." << std::endl;
//        }
//    }
//    else {
//        std::cerr << "Start tag not found in the HTML content." << std::endl;
//    }
//    RemoveDivContent(result);
//    return result;
//}
//
//// ��������С˵����׷��д���ļ�
//void AppendNovelContentToFile(const std::string& novelContent, const std::string& fileName) {
//    std::ofstream outFile;
//
//    // ��׷��д��ʽ���ļ�������ļ��������򴴽��ļ���
//    outFile.open(fileName, std::ios::out | std::ios::app);
//    if (!outFile.is_open()) {
//        std::cerr << "Failed to open file: " << fileName << std::endl;
//        return;
//    }
//
//    // д��С˵����
//    outFile << novelContent << std::endl<<std::endl;
//
//    // �ر��ļ�
//    outFile.close();
//}
//
//int main() {
//    setConsoleToGBK();
//    bool flag = true;
//    std::string NovelName=" �ҵ�Ů���Ƕ��Ӵ�С�� .txt";
//    std::string url = "https://www.69shuba.pro/txt/35041/29337106";// �滻Ϊ��Ҫ��ȡ����ҳURL
//    int count = 1;
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_int_distribution<> dist(5, 10);
//
//    while (flag) {
//        std::string htmlContent = fetchURL(url);
//        if (!htmlContent.empty()) {
//            //��htmlContent���д���ֻ����С˵���Ĳ���
//            std::string novel = SplitHtmlContentToNovelPart(htmlContent);
//            //��׷��дģʽ���С˵���ݵ�txt
//            AppendNovelContentToFile(novel, NovelName);
//            std::cout << "\n��" << count << "��������д��TXT�ļ�\n";
//        }
//        else {
//            std::cerr << "Error: could not fetch content from URL" << std::endl;
//        }
//        //��htmlContent��Ѱ����һ���½�url�����и��£������/end.html��ֹͣѭ��
//        size_t nextPos = htmlContent.find("next_page:");
//        if (nextPos != std::string::npos) {
//            size_t urlStart = htmlContent.find("\"", nextPos) + 1;
//            size_t urlEnd = htmlContent.find("\"", urlStart);
//            url = htmlContent.substr(urlStart, urlEnd - urlStart);
//            if (url.find("end.html") != std::string::npos)
//                flag = false;
//            else
//                count++;
//        }
//        // �������ӳ�
//        std::this_thread::sleep_for(std::chrono::seconds(dist(gen)));
//    }
//    std::cout << "\n�������,��" << count << "��";
//    return 0;
//}
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <windows.h>
#include <regex>
#include <fstream>
#include <thread>
#include <chrono>
#include <random>

// �ص����������ڴ�����յ�������
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// ��ȡָ��URL�����ݣ��������Ի��ƺ�ģ��������Ϊ
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
            // ����Clash����
            curl_easy_setopt(curl, CURLOPT_PROXY, "http://127.0.0.1:7890"); // ClashĬ��HTTP����˿�Ϊ7890
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            // ģ�������������ͷ
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
                    int delay = dist(gen);
                    std::cerr << "Retrying in " << delay << " seconds..." << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(delay));
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

// ���ÿ���̨ΪUTF-8����
void setConsoleToUTF8() {
    SetConsoleOutputCP(CP_UTF8);
}

void setConsoleToGBK() {
    SetConsoleOutputCP(936); // GBK�Ĵ���ҳ
    SetConsoleCP(936);       // �����������ҳΪGBK
}

//������ȥ��С˵������ż�����ֵ�div��
void RemoveDivContent(std::string& htmlContent) {
    size_t startPos = 0;

    while ((startPos = htmlContent.find("<div", startPos)) != std::string::npos) {
        size_t endPos = htmlContent.find("</div>", startPos);
        if (endPos != std::string::npos) {
            endPos += 6; // ����"</div>"�ĳ���
            htmlContent.erase(startPos, endPos - startPos);
        }
        else {
            // ���û���ҵ�ƥ��Ľ�����ǩ���˳�ѭ��
            break;
        }
    }
}

//����������ҳԴ������ȡС˵���Ĳ�������Ӧ����
std::string SplitHtmlContentToNovelPart(std::string& htmlContent) {
    std::string startTag = "&emsp;&emsp;";
    std::string endTag = "&emsp;&emsp;(������)";
    std::string result;

    // ���ҿ�ʼ��ǩ��λ��
    size_t startPos = htmlContent.find(startTag);
    if (startPos != std::string::npos) {
        // ���ҽ�����ǩ��λ��
        size_t endPos = htmlContent.rfind(endTag);
        if (endPos != std::string::npos) {
            // ��ȡ���Ĳ���
            result = htmlContent.substr(startPos, endPos + endTag.length() - startPos);

            // �滻 &emsp; Ϊ �ո�
            size_t pos = result.find("&emsp;");
            while (pos != std::string::npos) {
                result.replace(pos, 6, " ");
                pos = result.find("&emsp;", pos + 1);
            }

            // �滻 <br /> Ϊ ���з�
            pos = result.find("<br />");
            while (pos != std::string::npos) {
                result.replace(pos, 6, "\n");
                pos = result.find("<br />", pos + 1);
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

// ��������С˵����׷��д���ļ�
void AppendNovelContentToFile(const std::string& novelContent, const std::string& fileName) {
    std::ofstream outFile;

    // ��׷��д��ʽ���ļ�������ļ��������򴴽��ļ���
    outFile.open(fileName, std::ios::out | std::ios::app);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return;
    }

    // д��С˵����
    outFile << novelContent << std::endl << std::endl;

    // �ر��ļ�
    outFile.close();
}

int main() {
    setConsoleToGBK();
    bool flag = true;
    std::string NovelName = "�ҵ�Ů���Ƕ��Ӵ�С��.txt";
    std::string url = "https://www.69shuba.pro/txt/35041/29968117"; // �滻Ϊ��Ҫ��ȡ����ҳURL
    int count = 1;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(5, 10);

    while (flag) {
        std::string htmlContent = fetchURL(url);
        if (!htmlContent.empty()) {
            // ��htmlContent���д���ֻ����С˵���Ĳ���
            std::string novel = SplitHtmlContentToNovelPart(htmlContent);
            // ��׷��дģʽ���С˵���ݵ�txt
            AppendNovelContentToFile(novel, NovelName);
            std::cout << "\n��" << count << "��������д��TXT�ļ�\n";
        }
        else {
            std::cerr << "Error: could not fetch content from URL" << std::endl;
        }
        // ��htmlContent��Ѱ����һ���½�url�����и��£������/end.html��ֹͣѭ��
        size_t nextPos = htmlContent.find("next_page:");
        if (nextPos != std::string::npos) {
            size_t urlStart = htmlContent.find("\"", nextPos) + 1;
            size_t urlEnd = htmlContent.find("\"", urlStart);
            url = htmlContent.substr(urlStart, urlEnd - urlStart);
            if (url.find("end.html") != std::string::npos)
                flag = false;
            else
                count++;
        }
        // �������ӳ�
        int delay = dist(gen);
        std::this_thread::sleep_for(std::chrono::seconds(delay));
    }
    std::cout << "\n�������,��" << count << "��";
    return 0;
}

//#include <iostream>
//#include <string>
//#include <curl/curl.h>
//#include <windows.h>
//#include <regex>
//#include <fstream>
//#include <thread>
//#include <chrono>
//#include <random>
//#include <vector>
//
//// �ص����������ڴ�����յ�������
//static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
//    ((std::string*)userp)->append((char*)contents, size * nmemb);
//    return size * nmemb;
//}
//
//// ��ȡ����IP��
//std::vector<std::string> loadProxies(const std::string& filename) {
//    std::vector<std::string> proxies;
//    std::ifstream file(filename);
//    std::string line;
//    while (std::getline(file, line)) {
//        if (!line.empty()) {
//            proxies.push_back(line);
//        }
//    }
//    return proxies;
//}
//
//// ��ȡָ��URL�����ݣ��������Ի��ƺ�ģ��������Ϊ
//std::string fetchURL(const std::string& url, const std::vector<std::string>& proxies, int retries = 5, int minDelay = 5, int maxDelay = 10) {
//    CURL* curl;
//    CURLcode res;
//    std::string readBuffer;
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_int_distribution<> dist(minDelay, maxDelay);
//    std::uniform_int_distribution<> proxyDist(0, proxies.size() - 1);
//
//    curl_global_init(CURL_GLOBAL_DEFAULT);
//
//    for (int attempt = 0; attempt < retries; ++attempt) {
//        curl = curl_easy_init();
//
//        if (curl) {
//            // ���ѡ��һ������
//            std::string proxy = proxies[proxyDist(gen)];
//            curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());
//
//            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
//            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
//            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
//            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
//
//            // ģ�������������ͷ
//            struct curl_slist* headers = nullptr;
//            headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
//            headers = curl_slist_append(headers, "Referer: https://www.google.com/");
//            headers = curl_slist_append(headers, "Accept-Language: en-US,en;q=0.9");
//            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//
//            res = curl_easy_perform(curl);
//
//            if (res == CURLE_OK) {
//                curl_slist_free_all(headers);
//                curl_easy_cleanup(curl);
//                curl_global_cleanup();
//                return readBuffer;
//            }
//            else {
//                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
//                curl_easy_cleanup(curl);
//                curl_slist_free_all(headers);
//
//                if (attempt < retries - 1) {
//                    int delay = dist(gen);
//                    std::cerr << "Retrying in " << delay << " seconds..." << std::endl;
//                    std::this_thread::sleep_for(std::chrono::seconds(delay));
//                }
//                else {
//                    std::cerr << "Maximum retry attempts reached. Fetching URL failed." << std::endl;
//                }
//            }
//        }
//    }
//
//    curl_global_cleanup();
//    return "";
//}
//
//// ���ÿ���̨ΪUTF-8����
//void setConsoleToUTF8() {
//    SetConsoleOutputCP(CP_UTF8);
//}
//
//void setConsoleToGBK() {
//    SetConsoleOutputCP(936); // GBK�Ĵ���ҳ
//    SetConsoleCP(936);       // �����������ҳΪGBK
//}
//
////������ȥ��С˵������ż�����ֵ�div��
//void RemoveDivContent(std::string& htmlContent) {
//    size_t startPos = 0;
//
//    while ((startPos = htmlContent.find("<div", startPos)) != std::string::npos) {
//        size_t endPos = htmlContent.find("</div>", startPos);
//        if (endPos != std::string::npos) {
//            endPos += 6; // ����"</div>"�ĳ���
//            htmlContent.erase(startPos, endPos - startPos);
//        }
//        else {
//            // ���û���ҵ�ƥ��Ľ�����ǩ���˳�ѭ��
//            break;
//        }
//    }
//}
//
////����������ҳԴ������ȡС˵���Ĳ�������Ӧ����
//std::string SplitHtmlContentToNovelPart(std::string& htmlContent) {
//    std::string startTag = "&emsp;&emsp;";
//    std::string endTag = "&emsp;&emsp;(������)";
//    std::string result;
//
//    // ���ҿ�ʼ��ǩ��λ��
//    size_t startPos = htmlContent.find(startTag);
//    if (startPos != std::string::npos) {
//        // ���ҽ�����ǩ��λ��
//        size_t endPos = htmlContent.rfind(endTag);
//        if (endPos != std::string::npos) {
//            // ��ȡ���Ĳ���
//            result = htmlContent.substr(startPos, endPos + endTag.length() - startPos);
//
//            // �滻 &emsp; Ϊ �ո�
//            size_t pos = result.find("&emsp;");
//            while (pos != std::string::npos) {
//                result.replace(pos, 6, " ");
//                pos = result.find("&emsp;", pos + 1);
//            }
//
//            // �滻 <br /> Ϊ ���з�
//            pos = result.find("<br />");
//            while (pos != std::string::npos) {
//                result.replace(pos, 6, "\n");
//                pos = result.find("<br />", pos + 1);
//            }
//        }
//        else {
//            std::cerr << "End tag not found in the HTML content." << std::endl;
//        }
//    }
//    else {
//        std::cerr << "Start tag not found in the HTML content." << std::endl;
//    }
//    RemoveDivContent(result);
//    return result;
//}
//
//// ��������С˵����׷��д���ļ�
//void AppendNovelContentToFile(const std::string& novelContent, const std::string& fileName) {
//    std::ofstream outFile;
//
//    // ��׷��д��ʽ���ļ�������ļ��������򴴽��ļ���
//    outFile.open(fileName, std::ios::out | std::ios::app);
//    if (!outFile.is_open()) {
//        std::cerr << "Failed to open file: " << fileName << std::endl;
//        return;
//    }
//
//    // д��С˵����
//    outFile << novelContent << std::endl << std::endl;
//
//    // �ر��ļ�
//    outFile.close();
//}
//
//int main() {
//    setConsoleToGBK();
//    bool flag = true;
//    std::string NovelName = "���Ƕ���������ʦ.txt";
//    std::string url = "https://www.69shuba.pro/txt/58022/37443459"; // �滻Ϊ��Ҫ��ȡ����ҳURL
//    int count = 1;
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_int_distribution<> dist(5, 10);
//
//    // ��ȡ����IP��
//    std::vector<std::string> proxies = loadProxies("proxies.txt");
//
//    while (flag) {
//        std::string htmlContent = fetchURL(url, proxies);
//        if (!htmlContent.empty()) {
//            // ��htmlContent���д���ֻ����С˵���Ĳ���
//            std::string novel = SplitHtmlContentToNovelPart(htmlContent);
//            // ��׷��дģʽ���С˵���ݵ�txt
//            AppendNovelContentToFile(novel, NovelName);
//            std::cout << "\n��" << count << "��������д��TXT�ļ�\n";
//            count++;
//        }
//        else {
//            std::cerr << "Error: could not fetch content from URL" << std::endl;
//        }
//        // ��htmlContent��Ѱ����һ���½�url�����и��£������/end.html��ֹͣѭ��
//        size_t nextPos = htmlContent.find("next_page:");
//        if (nextPos != std::string::npos) {
//            size_t urlStart = htmlContent.find("\"", nextPos) + 1;
//            size_t urlEnd = htmlContent.find("\"", urlStart);
//            url = htmlContent.substr(urlStart, urlEnd - urlStart);
//            if (url.find("end.html") != std::string::npos)
//                flag = false;
//        }
//        // �������ӳ�
//        int delay = dist(gen);
//        std::this_thread::sleep_for(std::chrono::seconds(delay));
//    }
//    std::cout << "\n�������,��" << count << "��";
//    return 0;
//}

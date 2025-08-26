#include <curl/curl.h>
#include <string>
#include <vector>

#include <iostream>
#include <libxml/HTMLparser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#define html_read_mem htmlReadMemory 

#define NULL_       nullptr
typedef std::string str;
typedef htmlDocPtr  html_doc;

class scanner {
  private:
    static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
      ((std::string*)userp)->append((char*)contents, size * nmemb);
      return size * nmemb;
    }
    static int get_text(xmlNode *node, str &buf) { /* print all text info from html tree by recursive method */
        for (xmlNode *curNode = node; curNode; curNode = curNode->next) {
            if (curNode->type == XML_TEXT_NODE) {
              buf += (const char*)curNode->content;
            }
            get_text(curNode->children, buf);
        }
        return 0;
    }


  public:
    str download_page(const str& url) {
      CURL* curl = curl_easy_init();
      std::string readBuffer;
      if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, scanner::write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Maxim/1.0");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
          std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
      }
      return readBuffer;
    }
    str get_content_from_page(const str& url, const str& html_code) {
      str res;

      // Разбираем HTML из памяти
      html_doc doc = htmlReadMemory(html_code.c_str(), html_code.size(), nullptr, nullptr, HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR);
      if (!doc) {
        std::cerr << "Cannot parse HTML" << std::endl;
        return "";
      }

      // Получаем корневой элемент документа
      xmlNode *root = xmlDocGetRootElement(doc);
      if (!root) {
        std::cerr << "Empty doc" << std::endl;
        xmlFreeDoc(doc);
        return "";
      }

      scanner::get_text(root, res);

      // Освобождаем ресурсы
      xmlFreeDoc(doc);

      return res;
    }

    inline html_doc parse_html(const str& html_cont) {return html_read_mem(html_cont.c_str(), html_cont.length(), NULL_, NULL_, HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR);}

    std::vector<str> extract_data(html_doc doc) {
      /* INIT */
      xmlXPathContextPtr  context = xmlXPathNewContext(doc);
      std::vector<str>    links;

      /* GET LINKS */
      xmlXPathObjectPtr linksObj = xmlXPathEvalExpression((xmlChar *)"//a/@href", context);
      for(int i = 0; linksObj && i < linksObj->nodesetval->nodeNr;)
        links.push_back((char *)xmlNodeGetContent(linksObj->nodesetval->nodeTab[i++]));

      /* FREE */
      xmlXPathFreeObject(linksObj);
      xmlXPathFreeContext(context);

      return links;
    }
};



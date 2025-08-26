#include <curl/curl.h>

#include <string>
#include <vector>
#include <iostream>

#include <libxml/HTMLparser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#define html_read_mem              htmlReadMemory 
#define xml_doc_get_root_element   xmlDocGetRootElement
#define xml_xpath_new_context      xmlXPathNewContext
#define xml_xpath_eval_expression  xmlXPathEvalExpression
#define xml_node_get_content       xmlNodeGetContent

#define xml_free_doc               xmlFreeDoc
#define xml_xpath_free_object      xmlXPathFreeObject
#define xml_xpath_free_context     xmlXPathFreeContext

#define NULL_                      nullptr
#define node_tab                   nodeTab
#define node_nr                    nodeNr

typedef xmlXPathContextPtr         xml_xpath_context_ptr;
typedef xmlXPathObjectPtr          xml_xpath_object_ptr;
typedef std::string                str;
typedef htmlDocPtr                 html_doc;
typedef xmlNode                    xml_node;

class scanner {
  private:
    static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
      ((std::string*)userp)->append((char*)contents, size * nmemb);
      return size * nmemb;
    }
    static int get_text(xml_node *node, str &buf) { /* print all text info from html tree by recursive method */
        for (xml_node *cur_node = node; cur_node; cur_node = cur_node->next) {
            if (cur_node->type == XML_TEXT_NODE)
              buf += (const char*)cur_node->content;
            get_text(cur_node->children, buf);
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
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "User/1.0");
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

      /* HTML from mem */
      html_doc doc = html_read_mem(html_code.c_str(), html_code.size(), nullptr, nullptr, HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR);
      if (!doc) {
        std::cerr << "Cannot parse HTML" << std::endl;
        return "";
      }

      /* GET ROOT ELEMENT FROM DOCUMENT */
      xml_node *root = xml_doc_get_root_element(doc);
      if (!root) {
        std::cerr << "Empty doc" << std::endl;
        xml_free_doc(doc);
        return "";
      }

      scanner::get_text(root, res);

      xml_free_doc(doc);
      return res;
    }

    inline html_doc parse_html(const str& html_cont) {return html_read_mem(html_cont.c_str(), html_cont.length(), NULL_, NULL_, HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR);}

    std::vector<str> extract_data(html_doc doc) {
      /* INIT */
      xml_xpath_context_ptr context = xml_xpath_new_context(doc);
      std::vector<str>      links;

      /* GET LINKS */
      xml_xpath_object_ptr links_obj = xml_xpath_eval_expression((xmlChar *)"//a/@href", context);
      for(int i = 0; links_obj && i < links_obj->nodesetval->node_nr;)
        links.push_back((char *)xml_node_get_content(links_obj->nodesetval->node_tab[i++]));

      /* FREE */
      xml_xpath_free_object(links_obj);
      xml_xpath_free_context(context);

      return links;
    }
};



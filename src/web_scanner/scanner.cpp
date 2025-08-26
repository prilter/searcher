#include "stuff.hpp"

std::vector<str>
scan(const str &website)
{
  std::vector<str> res;

  std::vector<str>  links;
  class scanner     web_scanner;
  html_doc          doc;
  str               html_code;

  html_code = web_scanner.download_page(website);
  doc       = web_scanner.parse_html(html_code);
  links     = web_scanner.extract_data(doc);

  for (str & url : links) {
    res.push_back(url);
    res.push_back(web_scanner.get_content_from_page(url, html_code));
  }

  return res;
}

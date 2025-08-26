#include <iostream>
#include <vector>
#include <string>

extern std::vector<std::string> scan(const std::string&);

int
main(int argc, const char **argv)
{
  std::vector<std::string> base;

  base = scan(argv[1]);
  for (int i = 1; i < base.size(); i+=2)
    std::cout << base[i] << "\n";
  
  return 0;
}

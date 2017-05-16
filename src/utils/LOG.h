#ifndef _LOGGER_H_
#define _LOGGER_H_
#include <string>
#include <iostream>
#include <cstdlib>
class LogMessage {
 public:
 LogMessage(const std::string &type="") : fatal_(type == "FATAL") {
    std::cerr << type << ": ";
  }
  ~LogMessage() {
    std::cerr << std::endl;
    if(fatal_)
      exit(1);
  }
  std::ostream &stream() { return std::cerr; }

 private:
  bool fatal_;
};

#define LOG(type) std::cout << "[" << #type << "] "<<  __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ ; LogMessage().stream()
//#define LOG(type) LogMessage(#type).stream()
#define VLOG(level) if ((level) <= FLAGS_v) LOG(INFO)


#endif

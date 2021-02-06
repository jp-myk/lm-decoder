#ifndef _STRING_UTIL_H
#define _STRING_UTIL_H
#include<string>
#include<sstream>
#include<vector>
#include<iostream>

const int utf8_len_data[] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
  // length of 0xfe and 0xff should be 1 because they may be used
  // as a Byte Order Mark
};


inline std::vector<std::string> split(std::string delim, const std::string &str){
  
  std::vector<std::string> retval;
  int cutAt;
  if(delim == ""){
    size_t s_size = str.size();
    size_t i = 0;
    while(i < s_size ){
      unsigned char c = str[i];
      int char_len = utf8_len_data[c];
      std::string s1 = str.substr(i, char_len);
      retval.push_back(s1);
      i += char_len;
    }
  }else{
    size_t i = 0;
    std::string tmp = str;
    while((cutAt = tmp.find(delim)) != (int)str.npos){
      //cout << tmp << endl;
      retval.push_back(str.substr(i, cutAt));
      i += cutAt+delim.length();
      tmp = &str[i];
    }
    if(i != str.size()){
      retval.push_back(str.substr(i, str.size()));
    }
  }
  return retval;
}

inline std::string join(std::string delim, std::vector<std::string> &vec){
  std::string retval;
  std::vector<std::string>::iterator itr;
  for(itr= vec.begin(); itr != vec.end(); itr++){
    retval.append(*itr);
    if((itr +1) != vec.end())
      retval.append(delim);
  }
  return retval;
}

template<typename T_num>
inline std::string toString(T_num t_num){
  std::stringstream ss;
  ss << t_num;
  return ss.str();
}

inline size_t strlen_utf8(const std::string &str){
  size_t s_size = str.size();
  size_t i = 0;
  size_t len = 0;
  while(i < s_size ){
    unsigned char c = str[i];
    int char_len = utf8_len_data[c];
    //std::string s1 = str.substr(i, char_len);
    i += char_len;
    len++;
  }

  return len;
}

inline std::string substr_utf8(const std::string &str, int pos){
  size_t s_size = str.size();
  size_t i = 0;
  size_t len = 0;
  std::string retval = "";
  while(i < s_size ){
    unsigned char c = str[i];
    int char_len = utf8_len_data[c];
    if(len>=(size_t)pos)
      retval += str.substr(i, char_len);
    i += char_len;
    len++;
  }

  return retval;
}

#endif

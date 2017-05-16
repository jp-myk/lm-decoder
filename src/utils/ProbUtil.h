#include <cmath>
#ifndef M_E
#define M_E 2.7182818284590452354
#endif
#ifndef M_LN10
#defind M_LN10 2.30258509299404568402
#endif

template<typename T_prob>
inline float LogPtoProb(T_prob t_prob){
  if(t_prob == 0){
    return 0;
  }else{
    return exp(t_prob * M_LN10);
  }
}

template<typename T_prob>
inline float ProbtoLogP(T_prob t_prob){
  return log10(t_prob);
}

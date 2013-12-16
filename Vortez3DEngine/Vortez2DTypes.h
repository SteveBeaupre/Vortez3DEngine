#ifndef _VORTEZ2DTYPES_H_
#define _VORTEZ2DTYPES_H_
#ifdef __cplusplus

/*struct LTWHf{float L,T,W,H;};
struct LRTBf{float L,R,T,B;};

struct LTWHd{int L,T,W,H;};
struct LRTBd{int L,R,T,B;};*/

struct LTWHf{float l,t,w,h;};
struct LRTBf{float l,r,t,b;};

struct LTWHd{int l,t,w,h;};
struct LRTBd{int l,r,t,b;};

typedef LTWHd LTWH;
typedef LRTBd LRTB;

#endif
#endif //--_VORTEZ2DTYPES_H_

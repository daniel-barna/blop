#ifndef __BLOP_DEBUG_H__
#define __BLOP_DEBUG_H__
#ifndef __MAKECINT__

inline int &debug_level()
{
    static int i = 0;
    return i;
}

#define debug(a) \
  cerr<<endl; for(int i=0; i<debug_level(); ++i) cerr<<"   "; ++debug_level(); \
  cerr<<__FILE__<<" "<<__LINE__<<": "<<#a<<" ... ";\
  a; \
  cerr<<"OK"<<endl; --debug_level();

#endif
#endif

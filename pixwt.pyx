# distutils: language = c++
# distutils: extra_compile_args = -O3

import numpy as np

cdef extern from "pixwt.hpp":
    float pixwt_c(float xc,float yc,float r,int x,int y)

def pixwt(float xc,float yc,float r,x,y):
    if np.isscalar(x): 
        if np.isscalar(y): return pixwt_c(xc,yc,r,x,y)
        else:
            print('# pixwt: x and y dimensions must match!')
            return None
    if len(x)!=len(y): 
        print('# pixwt: x and y dimensions must match!')
        return None
    return np.array([pixwt_c(xc,yc,r,x[i],y[i]) for i in range(len(x))])

def disk(float xc,float yc,float r,int nx):
    ran = range(nx)
    x,y = np.tile(ran,nx),np.repeat(ran,nx)
    return np.reshape(pixwt(xc,yc,r,x,y),(nx,nx))

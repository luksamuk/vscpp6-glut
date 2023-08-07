#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

#ifndef signbit
#define signbit(x) (x < 0 ? -1 : 1)
#endif

#ifndef clamp
#define clamp(x, min, max) (x < min ? min : (x > max ? max : x))
#endif

#endif
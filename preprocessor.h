#ifndef preprocessor_h
#define preprocessor_h

/**
 *  Serie di macro per simulare funzioni variadiche con parametri opzionali
 *  Da: http://www.boost.org
 */

#if _MSC_VER

    #define PP_VARIADIC_SIZE(...) PP_CAT(PP_VARIADIC_SIZE_I(__VA_ARGS__, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,),)

    #define PP_CAT_I(a, b) PP_CAT_II(a ## b)
    #define PP_CAT_II(res) res

    #define OVERLOAD(function, ...) \
        PP_CAT(PP_OVERLOAD(function,__VA_ARGS__)(__VA_ARGS__),PP_EMPTY())

#else

    #define PP_VARIADIC_SIZE(...) PP_VARIADIC_SIZE_I(__VA_ARGS__, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,)

    #define PP_CAT_I(a, b) a ## b

    #define OVERLOAD(function, ...) \
        PP_OVERLOAD(function,__VA_ARGS__)(__VA_ARGS__)

#endif  // _MSC_VER

#define PP_EMPTY()

#define PP_VARIADIC_SIZE_I(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, size, ...) size

#define PP_CAT(a, b) PP_CAT_I(a, b)

#define PP_OVERLOAD(prefix, ...) PP_CAT(prefix, PP_VARIADIC_SIZE(__VA_ARGS__))

#endif  // preprocessor_h
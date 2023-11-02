#ifndef __DEBUG_H
#define __DEBUG_H

#include <stdio.h>

#define NO_USE_SWD
#define DEBUG 1

#if DEBUG == 1


#define PRINT(title, fmt, args...) printf("{"#title"}"fmt"\n", ##args)

#define DEBUG_LINE() 																												\
  printf("Log: [%s:%s] line = %d \n", __FILE__, __func__, __LINE__)
#define DEBUG_INFO(fmt, ...)                                                \
  printf("Log: %s: " fmt "\r\n", __func__, \
         ##__VA_ARGS__)
#define DEBUG_PRINTF(fmt, ...)                                                \
  printf("" fmt "",  \
         ##__VA_ARGS__)
/*
#define DEBUG_INFO(fmt, ...)                                                \
  printf("Log: [%s:%s] line = %d\n" fmt "\n", __FILE__, __func__, __LINE__, \
         ##__VA_ARGS__)
#define DEBUG_INFO(fmt, ...)                                                \
  printf("Log: %s: " fmt "\r\n", __func__, \
         ##__VA_ARGS__)
*/

#else

#define DEBUG_LINE()
#define DEBUG_INFO(fmt, ...)
#define DEBUG_PRINTF(fmt, ...)
#define PRINT(title, fmt, args...)

#endif

#endif /* __DEBUG_H */

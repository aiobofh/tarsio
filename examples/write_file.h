#ifndef _WRITE_FILE_H_
#define _WRITE_FILE_H_

enum write_file_retval_e
  {
   WRITE_FILE_EVERYTHING_IS_OK = 0,
   WRITE_FILE_FOPEN_FAILED = -1,
   WRITE_FILE_FCLOSE_FAILED = -2,
   WRITE_FILE_FWRITE_FAILED = -3
  };
typedef enum write_file_retval_e write_file_retval_t;

#endif

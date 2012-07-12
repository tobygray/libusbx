/* Dummy file to support lack of errno.h on WinCE. */

#define EPERM   1
#define EINTR   4
#define EIO     5
#define EBADF   9
#define ENOMEM 12
#define EACCES 13
#define EBUSY  16
#define EINVAL 22

extern int errno;

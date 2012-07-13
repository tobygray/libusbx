/* config.h.  Manual config for WinCE.  */

#ifndef _MSC_VER
#error "Building the WinCE backend outside of MSVC is currently not supported."
#endif

/* Default visibility */
#define DEFAULT_VISIBILITY /**/

/* Debug message logging */
//#define ENABLE_DEBUG_LOGGING 1

/* Message logging */
#define ENABLE_LOGGING 1

/* Windows CE backend */
#define OS_WINCE 1

/* type of second poll() argument */
#define POLL_NFDS_TYPE unsigned int

AC_PREREQ([2.69])
AC_INIT([host-local], [0.1])
AC_CONFIG_AUX_DIR([aux])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall -Werror])

# Enable non-standard functions and other goodies
AC_USE_SYSTEM_EXTENSIONS

# Checks for programs.
AC_PROG_CC
AM_PROG_AR

# Initialise libtool
LT_INIT

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([sys/socket.h netdb.h netinet/in.h arpa/inet.h])

# Checks for library functions.
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([getaddrinfo getnameinfo gai_strerror])
AC_CHECK_FUNCS([inet_ntop htons])

# Output
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

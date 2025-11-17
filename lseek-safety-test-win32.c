//#define _XOPEN_SOURCE 500
//#define _FILE_OFFSET_BITS 64
//#define _LARGE_FILES 1 /* For AIX */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
//#include <setjmp.h>
//#include <signal.h>
//#include <sys/resource.h>
#include <io.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <windows.h>

#define OFF_T unsigned __int64
#define SSIZE_T __int64

//static jmp_buf jb;

#if 0
static void sighandler(int signum) {
    longjmp(jb, signum);
}
#endif

static int
set_errno_from_getlasterror()
{
    int e;
    unsigned long winerr = GetLastError();
    switch (winerr) {
	case ERROR_FILENAME_EXCED_RANGE:
	case ERROR_FILE_NOT_FOUND:
	case ERROR_PATH_NOT_FOUND:
	case ERROR_INVALID_DRIVE:
	case ERROR_NO_MORE_FILES:
	case ERROR_BAD_NETPATH:
	case ERROR_BAD_NET_NAME:
	case ERROR_BAD_PATHNAME:
	    e = ENOENT;
	    break;
	case ERROR_ARENA_TRASHED:
	case ERROR_NOT_ENOUGH_MEMORY:
	case ERROR_INVALID_BLOCK:
	case ERROR_NOT_ENOUGH_QUOTA:
	    e = ENOMEM;
	    break;
	case ERROR_LOCK_VIOLATION:
	case ERROR_LOCK_FAILED:
	case ERROR_SEEK_ON_DEVICE:
	case ERROR_NETWORK_ACCESS_DENIED:
	case ERROR_NOT_LOCKED:
	case ERROR_ACCESS_DENIED:
	case ERROR_CANNOT_MAKE:
	case ERROR_FAIL_I24:
	case ERROR_DRIVE_LOCKED:
	case ERROR_CURRENT_DIRECTORY:
	    e = EACCES;
	    break;
	case ERROR_INVALID_FUNCTION:
	case ERROR_INVALID_ACCESS:
	case ERROR_NEGATIVE_SEEK:
	case ERROR_INVALID_DATA:
	case ERROR_INVALID_PARAMETER:
	    e = EINVAL;
	    break;
	case ERROR_NO_PROC_SLOTS:
	case ERROR_NESTING_NOT_ALLOWED:
	case ERROR_MAX_THRDS_REACHED:
	    e = EAGAIN;
	    break;
	case ERROR_INVALID_HANDLE:
	case ERROR_INVALID_TARGET_HANDLE:
	case ERROR_DIRECT_ACCESS_HANDLE:
	    e = EBADF;
	    break;
	case ERROR_ALREADY_EXISTS:
	case ERROR_FILE_EXISTS:
	    e = EEXIST;
	    break;
	case ERROR_BROKEN_PIPE:
	    e = EPIPE;
	    break;
	case ERROR_DISK_FULL:
	    e = ENOSPC;
	    break;
	case ERROR_TOO_MANY_OPEN_FILES:
	    e = EMFILE;
	    break;
	case ERROR_WAIT_NO_CHILDREN:
	case ERROR_CHILD_NOT_COMPLETE:
	    e = ECHILD;
	    break;
	case ERROR_DIR_NOT_EMPTY:
	    e = ENOTEMPTY;
	    break;
	case ERROR_BAD_ENVIRONMENT:
	    e = E2BIG;
	    break;
	case ERROR_BAD_FORMAT:
	    e = ENOEXEC;
	    break;
	case ERROR_NOT_SAME_DEVICE:
	    e = EXDEV;
	    break;
	default:
	    if (winerr >= ERROR_WRITE_PROTECT && winerr <= ERROR_SHARING_BUFFER_EXCEEDED)
		e = EACCES;
	    else if (winerr >= ERROR_INVALID_STARTING_CODESEG && winerr <= ERROR_INFLOOP_IN_RELOC_CHAIN)
		e = ENOEXEC;
	    else
		e = EINVAL;
	    break;
    }
    errno = e;
    return -1;
}

SSIZE_T
pread(int fd, char * p, size_t n, OFF_T o)
{
    HANDLE h = (HANDLE)(intptr_t)_get_osfhandle(fd);
    if (h == INVALID_HANDLE_VALUE) {
	// _get_osfhandle() sets errno to EBADF.
	return -1;
    }

    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.Offset = (DWORD)o;
    if (sizeof(OFF_T) > 4) {
	overlapped.OffsetHigh = o >> 32;
    }
    DWORD c;
    if (!ReadFile(h, p, n, &c, &overlapped)) {
	return set_errno_from_getlasterror();
    }
    return c;
}

SSIZE_T
pwrite(int fd, const char * p, size_t n, OFF_T o)
{
    HANDLE h = (HANDLE)(intptr_t)_get_osfhandle(fd);
    if (h == INVALID_HANDLE_VALUE) {
	// _get_osfhandle() sets errno to EBADF.
	return -1;
    }

    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.Offset = (DWORD)o;
    if (sizeof(OFF_T) > 4) {
	overlapped.OffsetHigh = o >> 32;
    }
    DWORD c;
    if (!WriteFile(h, p, n, &c, &overlapped)) {
	return set_errno_from_getlasterror();
    }
    return c;
}

static int openoverlapped(const char* filename, int test_read) {
    DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
    /* Subsequent operations may open this file to read, write or delete it */
    DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

    DWORD dwCreationDisposition = test_read ? OPEN_EXISTING : CREATE_ALWAYS;

    HANDLE handleWin =
	CreateFile(filename,
		   dwDesiredAccess,
		   dwShareMode,
		   NULL,
		   dwCreationDisposition,
		   FILE_ATTRIBUTE_NORMAL,
		   NULL);
    if (handleWin == INVALID_HANDLE_VALUE) {
	return set_errno_from_getlasterror();
    }

    /* Return a standard file descriptor. */
    return _open_osfhandle((intptr_t)(handleWin), O_BINARY);
}

int main(int argc, char ** argv) {
    if (argc < 2 || argc > 3) {
	fprintf(stderr, "Usage: %s TESTFILE [r]\n", argv[0]);
	return 1;
    }

    const char * filename = argv[1];

    int test_read = argv[2] != NULL;

    int fd = openoverlapped(filename, test_read);
    if (fd < 0) {
        perror("open failed");
	return 1;
    }

    if (!test_read) {
	errno = 0;
	int r = write(fd, "", 1);
	fprintf(stderr, "write() returned %lld (%s)\n", (long long)r, strerror(errno));
    }

    char data[8192];
    size_t jj;
    if (!test_read) {
	for (jj = 0; jj < sizeof(data); ++jj) {
	    data[jj] = (char)(42 ^ jj);
	}

	for (jj = 0; jj < 1024 * 1024 * 1024; jj += sizeof(data)) {
	    if (pwrite(fd, data, sizeof(data), jj) < 0) {
		perror("pwrite failed");
		return 1;
	    }
	}
    } else {
	for (jj = 0; jj < 1024 * 1024 * 1024; jj += sizeof(data)) {
	    if (pread(fd, data, sizeof(data), jj) < 0) {
		perror("pread failed");
		return 1;
	    }
	}
    }
    return 0;
}

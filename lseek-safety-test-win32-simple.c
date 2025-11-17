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

SSIZE_T
pread(int fd, char * p, size_t n, OFF_T o)
{
    size_t total = 0;
    if (lseek(fd, o, SEEK_SET) < 0)
	return -1;
    while (1) {
	SSIZE_T c = read(fd, p, n);
	// We should get a full read most of the time, so streamline that case.
	if (c == (SSIZE_T)n)
	    return total + n;
	if (c <= 0) {
	    if (c == 0) {
		return total;
	    }
	    // We get EINTR if the syscall was interrupted by a signal.
	    // In this case we should retry the read.
	    if (errno == EINTR) continue;
	    return -1;
	}
	total += c;
	p += c;
	n -= c;
	if (n == 0)
	    return total;
    }
}

SSIZE_T
pwrite(int fd, const char * p, size_t n, OFF_T o)
{
    size_t total = n;
    if (lseek(fd, o, SEEK_SET) < 0)
	return -1;
    while (n) {
	SSIZE_T c = write(fd, p, n);
	if (c < 0) {
	    if (errno == EINTR) continue;
	    return -1;
	}
	p += c;
	n -= c;
    }
    return total;
}

int main(int argc, char ** argv) {
    if (argc < 2 || argc > 3) {
	fprintf(stderr, "Usage: %s TESTFILE [r]\n", argv[0]);
	return 1;
    }

    const char * filename = argv[1];

    int test_read = argv[2] != NULL;

    int fd = open(filename, O_RDWR | O_BINARY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        perror("open failed");
	return 1;
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

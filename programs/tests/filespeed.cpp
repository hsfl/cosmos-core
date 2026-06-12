#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

/* Buffer large enough for any block size we'll test.
   Must be aligned for O_DIRECT (which requires sector-aligned memory). */
#define BUF_SIZE 1100000000
#define DIRECT_BLOCK 4096      /* native block size for O_DIRECT alignment */

static char raw_buf[BUF_SIZE + DIRECT_BLOCK];  /* extra room for alignment */

struct timeval tp;
struct stat sbuf;

static void usage(const char *prog)
{
    fprintf(stderr,
        "Usage: %s [options]\n"
        "  -t <seconds>   Test duration per block size (default: 10)\n"
        "  -d             Use O_DIRECT (bypass page cache); block sizes rounded\n"
        "                 up to multiples of %d bytes\n"
        "  -b <bytes>     O_DIRECT alignment/block size (default: %d);\n"
        "                 must be a power-of-two sector multiple\n"
        "  -h             Show this help\n",
        prog, DIRECT_BLOCK, DIRECT_BLOCK);
}

int main(int argc, char *argv[])
{
    int opt;
    long test_secs  = 10;
    int  use_direct = 0;
    long direct_block = DIRECT_BLOCK;

    while ((opt = getopt(argc, argv, "t:db:h")) != -1) {
        switch (opt) {
        case 't':
            test_secs = atol(optarg);
            if (test_secs <= 0) {
                fprintf(stderr, "Error: -t requires a positive integer\n");
                return 1;
            }
            break;
        case 'd':
            use_direct = 1;
            break;
        case 'b':
            direct_block = atol(optarg);
            if (direct_block <= 0 || (direct_block & (direct_block - 1)) != 0) {
                fprintf(stderr, "Error: -b must be a positive power of two\n");
                return 1;
            }
            break;
        case 'h':
            usage(argv[0]);
            return 0;
        default:
            usage(argv[0]);
            return 1;
        }
    }

    long limit_u = test_secs * 1000000L;

    /* Align buffer to direct_block boundary for O_DIRECT */
    char *buffer = (char *)(((unsigned long)raw_buf + direct_block - 1)
                            & ~((unsigned long)(direct_block - 1)));

    /* Fill buffer with known pattern */
    for (long i = 0; i < BUF_SIZE; i++)
        buffer[i] = (char)(i % 256);

    int base_flags = O_RDWR | O_CREAT;
    if (use_direct) {
#ifdef O_DIRECT
        base_flags |= O_DIRECT;
#else
        fprintf(stderr, "Warning: O_DIRECT not available on this platform; ignoring -d\n");
        use_direct = 0;
#endif
    }

    printf("Test duration : %ld second%s per block size\n",
           test_secs, test_secs == 1 ? "" : "s");
    printf("I/O mode      : %s\n",
           use_direct ? "O_DIRECT (cache bypass)" : "normal (O_SYNC writes)");
    if (use_direct)
        printf("Direct block  : %ld bytes\n", direct_block);
    printf("\n");

    /* Starting block size: 32 for normal, direct_block for O_DIRECT */
    ssize_t size = use_direct ? direct_block : 32;

    for (int pass = 0; pass < 8; pass++) {
        /* For O_DIRECT, block size must be a multiple of direct_block */
        ssize_t io_size = size;
        if (use_direct) {
            /* Round up to the next multiple of direct_block */
            io_size = ((size + direct_block - 1) / direct_block) * direct_block;
        }

        if (io_size > BUF_SIZE) {
            fprintf(stderr, "Block size %zd exceeds buffer; stopping.\n", io_size);
            break;
        }

        /* ---- WRITE ---- */
        int write_flags = use_direct ? base_flags : (base_flags | O_SYNC | O_TRUNC);
        int fd = open("testfile", write_flags, 00664);
        if (fd < 0) {
            perror("open (write)");
            return 1;
        }

        gettimeofday(&tp, NULL);
        long start_s = tp.tv_sec;
        long start_u = tp.tv_usec;
        long j = 0, werr = 0;
        long diff_u = 0;

        do {
            ssize_t iw = write(fd, buffer, io_size);
            if (iw != io_size) {
                werr++;
            }
            gettimeofday(&tp, NULL);
            diff_u = 1000000L * (tp.tv_sec - start_s) + (tp.tv_usec - start_u);
            j++;
        } while (diff_u < limit_u);

        close(fd);

        printf("Blocks: %7ld x %8zd  ", j, io_size);
        printf("Write: %6.3f MB/sec (%ld err)  ",
               (double)(io_size * j) / diff_u, werr);
        fflush(stdout);

        /* ---- READ ---- */
        int read_flags = O_RDONLY;
        if (use_direct) {
#ifdef O_DIRECT
            read_flags |= O_DIRECT;
#endif
        }
        fd = open("testfile", read_flags, 00664);
        if (fd < 0) {
            perror("open (read)");
            return 1;
        }

        fstat(fd, &sbuf);
        off_t tsize = sbuf.st_size;

        gettimeofday(&tp, NULL);
        start_s = tp.tv_sec;
        start_u = tp.tv_usec;
        j = 0;
        long rerr = 0;

        do {
            ssize_t ir = read(fd, buffer, io_size);
            if (ir != io_size)
                rerr++;
            gettimeofday(&tp, NULL);
            diff_u = 1000000L * (tp.tv_sec - start_s) + (tp.tv_usec - start_u);
            j++;
            tsize -= io_size;
            if (tsize < io_size) {
                lseek(fd, 0, SEEK_SET);
                tsize = sbuf.st_size;
            }
        } while (diff_u < limit_u);

        close(fd);

        printf("Read: %6.3f MB/sec (%ld err)\n",
               (double)(io_size * j) / diff_u, rerr);
        fflush(stdout);

        size *= 8;
    }

    return 0;
}

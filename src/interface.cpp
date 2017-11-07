#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>              /* low-level i/o */
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

static int xioctl(int fh, int request, void *arg)
{
    int r;
    do {
        r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);
    return r;
}

int allocCamera(const char* file) 
{
    struct v4l2_capability cap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;

    int camera_fd = open(file, O_RDONLY);

    if (-1 == xioctl (camera_fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf (stderr, "%s is no V4L2 device\n", file);
            exit (EXIT_FAILURE);
        } else {
            printf("\nError in ioctl VIDIOC_QUERYCAP\n\n");
            exit(0);
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf (stderr, "%s is no video capture device\n", file);
        exit (EXIT_FAILURE);
    }

    if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
        fprintf (stderr, "%s does not support read i/o\n", file);
        exit (EXIT_FAILURE);
    }

    memset(&fmt, 0, sizeof(fmt));
    fmt.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = 320; 
    fmt.fmt.pix.height      = 240;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
    if (-1 == xioctl(camera_fd, VIDIOC_S_FMT, &fmt)) {
        printf("VIDIOC_S_FMT");
    }
    return camera_fd;
}

int main () {
}

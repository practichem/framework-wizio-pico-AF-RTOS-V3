// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef FOR_FUTURE

#include <stddef.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <sys/errno.h>
#include <sys/param.h>

int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    struct timeval tv = {
        // timeout is in milliseconds
        .tv_sec = timeout / 1000,
        .tv_usec = (timeout % 1000) * 1000,
    };
    int ret = 0;
    int max_fd = -1;
    fd_set readfds;
    fd_set writefds;
    fd_set errorfds;

    extern struct _reent *__getreent(void);
    struct _reent *r = __getreent();

    if (fds == NULL)
    {
        __errno_r(r) = ENOENT;
        return -1;
    }

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&errorfds);

    for (unsigned int i = 0; i < nfds; ++i)
    {
        fds[i].revents = 0;

        if (fds[i].fd < 0)
        {
            // revents should remain 0 and events ignored (according to the documentation of poll()).
            continue;
        }

        if (fds[i].fd >= FD_SETSIZE)
        {
            fds[i].revents |= POLLNVAL;
            ++ret;
            continue;
        }

        if (fds[i].events & (POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI))
        {
            FD_SET(fds[i].fd, &readfds);
            FD_SET(fds[i].fd, &errorfds);
            max_fd = MAX(max_fd, fds[i].fd);
        }

        if (fds[i].events & (POLLOUT | POLLWRNORM | POLLWRBAND))
        {
            FD_SET(fds[i].fd, &writefds);
            FD_SET(fds[i].fd, &errorfds);
            max_fd = MAX(max_fd, fds[i].fd);
        }
    }

    const int select_ret = select(max_fd + 1, &readfds, &writefds, &errorfds, timeout < 0 ? NULL : &tv);

    if (select_ret > 0)
    {
        ret += select_ret;

        for (unsigned int i = 0; i < nfds; ++i)
        {
            if (FD_ISSET(fds[i].fd, &readfds))
            {
                fds[i].revents |= POLLIN;
            }

            if (FD_ISSET(fds[i].fd, &writefds))
            {
                fds[i].revents |= POLLOUT;
            }

            if (FD_ISSET(fds[i].fd, &errorfds))
            {
                fds[i].revents |= POLLERR;
            }
        }
    }
    else
    {
        ret = select_ret;
        // keeping the errno from select()
    }

    return ret;
}

#endif
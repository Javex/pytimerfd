import timerfd
import select
import os

timer = timerfd.timerfd(timerfd.CLOCK_MONOTONIC, timerfd.TFD_NONBLOCK)
timer.settime(0, 0.5)
timer.settime(0, 0.5)

epoll = select.epoll()
epoll.register(timer.fileno(), select.EPOLLIN)
for fd, mask in epoll.poll(-1):
    print(timerfd.unpack(os.read(fd, 8)))

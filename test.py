import timerfd
import select
import os

timer = timerfd.timerfd(timerfd.CLOCK_MONOTONIC, timerfd.TFD_NONBLOCK)
timer.settime(0, value=0.5, interval=0.5)
print(timer.gettime())
timer.settime(0, value=0.5, interval=0.5)
print(timer.gettime())

epoll = select.epoll()
epoll.register(timer.fileno(), select.EPOLLIN)
while True:
    for fd, mask in epoll.poll(-1):
        print(timer.gettime())
        print(timerfd.unpack(os.read(fd, 8)))

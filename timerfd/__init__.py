import _timerfd
import math
import struct
from _timerfd import *


class timerfd(object):

    def __init__(self, clockid, flags):
        self._obj = _timerfd._timerfd(clockid, flags)

    def settime(self, flags, interval=None, value=None):
        i_int, i_frac = self._split_time(interval)
        v_int, v_frac = self._split_time(value)
        return self._obj.settime(flags, (v_int, v_frac, i_int, i_frac))

    def gettime(self):
     	raise NotImplementedError

    def fileno(self):
        return self._obj.fd

    def _split_time(self, value=None):
        if value is None:
            return 0, 0
        else:
            fraction, integer = math.modf(value)
            return int(integer), int(fraction * 1000000000)

def unpack(buf):
    count, = struct.unpack("Q", buf)
    return count

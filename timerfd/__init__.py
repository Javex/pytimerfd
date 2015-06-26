import _timerfd
import math
import struct
from _timerfd import *




class timerfd(_timerfd._timerfd):

    def settime(self, flags, interval=None, value=None):
        i_int, i_frac = self._split_time(interval)
        v_int, v_frac = self._split_time(value)
        return _timerfd._timerfd.settime(self, flags, (i_int, i_frac, v_int, v_frac))

    def fileno(self):
        return self.fd

    def _split_time(self, value=None):
        if value is None:
            return 0, 0
        else:
            fraction, integer = math.modf(value)
            return int(integer), int(fraction * 1000000000)

def unpack(buf):
    count, = struct.unpack("Q", buf)
    return count

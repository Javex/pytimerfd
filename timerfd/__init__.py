import _timerfd
import math
import struct
import os
from _timerfd import *


"""
A thin wrapper around the timerfd interface.

The following constants are defined here (they have the same meaning as
on the original ``timerfd_create(2)`` documentation).

* CLOCK_REALTIME
* CLOCK_MONOTONIC
* TFD_NONBLOCK
* TFD_CLOEXEC
* TFD_TIMER_ABSTIME
"""


class timerfd(_timerfd._timerfd):
    """
    Create a new timerfd instance by ``timerfd.timerfd(clockid, flags)`` in the same manner
    as with ``timerfd_create(2)``.
    """

    def settime(self, flags, interval=None, value=None):
        """
        Set the timer to run in time ``value`` and then every ``interval`` seconds again.
        Both arguments can be floats or ints.
        The ``flags`` argument is as described for ``timerfd_settime(2)``.
        """
        i_int, i_frac = self._split_time(interval)
        v_int, v_frac = self._split_time(value)
        return _timerfd._timerfd.settime(self, flags, (i_int, i_frac, v_int, v_frac))

    def fileno(self):
        """Return the associated file descriptor"""
        return self.fd

    def _split_time(self, value=None):
        if value is None:
            return 0, 0
        else:
            fraction, integer = math.modf(value)
            return int(integer), int(fraction * 1000000000)

    def gettime(self):
        """
        Get the time left on a file descriptor. Returns a pair of ``(interval, value)``
        similar to the values passed in to ``settime``.
        """
        (i_int, i_frac, v_int, v_frac) = _timerfd._timerfd.gettime(self)
        interval = i_int + float(i_frac) / 1000000000
        value = v_int + float(v_frac) / 1000000000
        return interval, value

    def close(self):
        """
        Close the associated file descriptor.
        """
        os.close(self.fd)

    def get_count(self):
        """
        Return the number of times the timer has expired. Usually this will be one but
        if you don't check it fast enough it might grow. Returns an int with the number
        of expirations.

        Note that when you call it without it having expired and the descriptor is created
        as a blocking one, this call will block until the timer has expired.
        """
        return unpack(os.read(self.fd, 8))

def unpack(buf):
    """
    Convenience function to be called on the result read from the descriptor. Use like this:

    ``timerfd.unpack(os.read(timer.filen(), 8))``

    Or just use ``timerfd.timerfd.get_count()``.
    """
    count, = struct.unpack("Q", buf)
    return count

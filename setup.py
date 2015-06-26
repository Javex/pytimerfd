import distutils.core

distutils.core.setup(
    name="timerfd",
    packages=["timerfd"],
    ext_modules=[distutils.core.Extension("_timerfd", ["timerfd/timerfd.c"])]
)

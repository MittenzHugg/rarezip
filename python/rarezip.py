from cffi import FFI
import os
import numpy as np

ffi = FFI()

PATH = os.path.dirname(__file__)


ffi.cdef("""
    size_t deflate(uint8_t *in_file, size_t in_len, uint8_t *out_file, size_t out_cap);
    size_t inflate(uint8_t *in_file, size_t in_len, uint8_t *out_file, size_t out_cap);

    size_t zip(uint8_t *in_file, size_t in_len, uint8_t *out_file, size_t out_cap);
    size_t unzip(uint8_t *in_file, size_t in_len, uint8_t *out_file, size_t out_cap);

    size_t bk_zip(uint8_t *in_file, size_t in_len, uint8_t *out_file, size_t out_cap);
    size_t bk_unzip(uint8_t *in_file, size_t in_len, uint8_t *out_file, size_t out_cap);
""") # here CFFI is using the cdef from cython.

ffi.set_source("rarezip",
    '#include "../gzip/librarezip.h"',
    libraries=['rarezip'],
    library_dirs=[os.path.join(PATH, "../gzip")],
)

ffi.compile()
import rarezip.lib as rarezip

def deflate(in_array):
    # change array to numpy array
    in_arr = np.array(in_array, dtype=np.uint8)
    in_len = len(in_arr)
    out_arr = np.zeros(in_len + 0x800, dtype=np.uint8) #allocate out array
    #C function call
    out_len = rarezip.bk_zip( ffi.cast("uint8_t*", in_arr.ctypes.data), in_len, ffi.cast("uint8_t*", out_arr.ctypes.data), len(out_arr))

    #resize and align/pad end with (0xaa)
    return out_arr[:out_len]

def bk_zip(in_array):
    # change array to numpy array
    in_arr = np.array(in_array, dtype=np.uint8)
    in_len = len(in_arr)
    out_arr = np.zeros(in_len + 0x800, dtype=np.uint8)
    #C function call
    out_len = rarezip.bk_zip( ffi.cast("uint8_t*", in_arr.ctypes.data), in_len, ffi.cast("uint8_t*", out_arr.ctypes.data), len(out_arr))

    #resize and align/pad end with (0xaa)
    new_out = np.zeros((out_len + 7) & ~0x7, dtype=np.uint8)
    new_out.fill(0xaa)
    new_out[:out_len] = out_arr[:out_len]
    return new_out


if __name__ == "__main__":
    in_array = [0]*20
    out_array = bk_zip(in_array)
    assert (out_array == [ 0x11, 0x72, 0x00, 0x00, 0x00, 0x14, 0x63, 0x60, 0xC0, 0x04, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA]).all()

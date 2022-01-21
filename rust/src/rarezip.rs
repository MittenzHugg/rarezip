use inflate as __inflate;

mod __rarezip {
    use libc;
    #[link(name = "rarezip", kind = "static")]
    extern {
        pub fn deflate(in_file : *const u8, in_len: libc::size_t, out_file: *mut u8, out_cap: libc::size_t) -> libc::size_t;
        pub fn inflate(in_file : *const u8, in_len: libc::size_t, out_file: *mut u8, out_cap: libc::size_t) -> libc::size_t;
        pub fn zip(in_file : *const u8, in_len: libc::size_t, out_file: *mut u8, out_cap: libc::size_t)  -> libc::size_t;
        pub fn unzip(in_file : *const u8, in_len: libc::size_t, out_file: *mut u8, out_cap: libc::size_t) -> libc::size_t;
        pub fn bk_zip(in_file : *const u8, in_len: libc::size_t, out_file: *mut u8, out_cap: libc::size_t) -> libc::size_t;
    }
}

pub fn deflate(in_buffer : &[u8]) -> Vec<u8>{
    let mut out_buffer: Vec<u8> = Vec::new();
    out_buffer.resize(in_buffer.len() + 0x800, 0);
    let out_length = unsafe{__rarezip::deflate(in_buffer.as_ptr(), in_buffer.len(), out_buffer.as_mut_ptr(), out_buffer.len())};
    out_buffer.resize(out_length, 0);
    return out_buffer
}

pub fn inflate(in_buffer : &[u8]) -> Vec<u8>{
    // since we don't know the inflate size, it is better to have a library that can dynamically allocates out buffer
    return __inflate::inflate_bytes(&in_buffer).unwrap();
}

pub fn zip(in_buffer : &[u8]) -> Vec<u8>{
    let mut out_buffer: Vec<u8> = Vec::new();
    out_buffer.resize(in_buffer.len() + 0x800, 0);
    let out_length = unsafe{__rarezip::zip(in_buffer.as_ptr(), in_buffer.len(), out_buffer.as_mut_ptr(), out_buffer.len())};
    out_buffer.resize(out_length, 0);
    return out_buffer
}

pub fn unzip(in_buffer : &[u8]) -> Vec<u8>{
    let mut out_buffer: Vec<u8> = Vec::new();
    let expected_len = u32::from_le_bytes(in_buffer[in_buffer.len() - 4..].try_into().unwrap()) as usize;
    let out_length = unsafe{__rarezip::unzip(in_buffer.as_ptr(), in_buffer.len(), out_buffer.as_mut_ptr(), out_buffer.len())};
    out_buffer.resize(expected_len, 0);
    return out_buffer
}

pub mod bk{
    use inflate as __inflate;
    use super::__rarezip;

    pub fn zip(in_buffer : &[u8]) -> Vec<u8>{
        let mut out_buffer: Vec<u8> = Vec::new();
        out_buffer.resize(in_buffer.len() + 0x800, 0);
        let out_length = unsafe{__rarezip::bk_zip(in_buffer.as_ptr(), in_buffer.len(), out_buffer.as_mut_ptr(), out_buffer.len())};
        out_buffer.resize(out_length, 0);
        out_buffer.resize((out_buffer.len() + (8-1)) & !(8-1), 0xAA);
        return out_buffer
    }

    pub fn unzip(in_buffer : &[u8]) -> Vec<u8>{
        assert_eq!(in_buffer[0..2],[0x11, 0x72], "in_buffer does not have bk header");
        let expected_len = u32::from_be_bytes(in_buffer[2..6].try_into().unwrap()) as usize;
        let out_buffer = __inflate::inflate_bytes(&in_buffer[6..]).unwrap();
        assert_eq!(out_buffer.len(), expected_len, "inflated length does not match expected inflated length");
        return out_buffer
    }
}

#[cfg(test)]
mod tests;
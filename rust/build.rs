fn main() {
    println!("cargo:rustc-link-search=../gzip", );
    println!("cargo:rustc-link-lib=rarezip", );
}
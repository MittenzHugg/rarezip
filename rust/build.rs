use std::env;
use std::path::PathBuf;
use std::process::Command;

fn main() {
    let manifest_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let gzip_dir = PathBuf::from(&manifest_dir)
        .join("../gzip")
        .canonicalize()
        .expect("Could not resolve rarezip/gzip directory");
    let lib_path = gzip_dir.join("librarezip.a");

    // make if C library does not exist
    if !lib_path.exists() {
        let status = Command::new("make")
            .arg("librarezip.a")
            .current_dir(&gzip_dir)
            .status()
            .expect("Failed to execute make");

        if !status.success() {
            panic!("make failed to build librarezip.a in {}", gzip_dir.display());
        }
    }

    //re-build if library changed
    println!("cargo:rerun-if-changed={}", lib_path.display());

    //link library 
    println!("cargo:rustc-link-search=native={}", gzip_dir.display());
    println!("cargo:rustc-link-lib=static=rarezip");
}

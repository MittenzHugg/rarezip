use std::process::Command;
use std::path::Path;

fn main() {
    // make if C library does not exist
    if(!std::path::Path::new("../gzip/librarezip.a").exists()){
        Command::new("make").arg("librarezip.a")
                .current_dir(&Path::new("../gzip"))
                .status().unwrap();
    }

    //re-build if library changed
    println!("cargo:rerun-if-changed=../gzip/librarezip.a");
    
    //link library 
    println!("cargo:rustc-link-search=../gzip", );
    println!("cargo:rustc-link-lib=rarezip", );
}
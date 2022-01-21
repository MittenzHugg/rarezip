use std::process::Command;
use std::path::Path;

fn main() {
    // make if static C library does not exist
    if(!std::path::Path::new("../gzip/librarezip.a").exists()){
        Command::new("make")
                .current_dir(&Path::new("../gzip"))
                .status().unwrap();
    }

    //re-build if static library changed
    println!("cargo:rerun-if-changed=../gzip/librarezip.a");
    
    //link library 
    println!("cargo:rustc-link-search=../gzip", );
    println!("cargo:rustc-link-lib=rarezip", );
}
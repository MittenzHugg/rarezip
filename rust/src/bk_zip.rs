use std::fs::{self, File};
use std::io::Write;
use std::env;
use rarezip;

fn main() {
    
    let in_path = env::args().nth_back(1).expect("No in_path specified");
    let mut out_path = env::args().nth_back(2).expect("No out_path specified");
    let in_bin : Vec<u8> = std::fs::read(in_path).expect("Could not read file");

    let out_bin = if env::args().any(|x| {return x == "-d"}){
        rarezip::bk::unzip(&in_bin)
    }else{
        rarezip::bk::zip(&in_bin)
    };
    let mut out_file = std::fs::File::create(out_path).unwrap();
    out_file.write_all(&out_bin).unwrap();
}
# rarezip
Library version gzip v1.2.4 with foreign function interfaces to multiple languages.
Supports compression formats for popular N64 titles based on gzip(v1.2.4)'s deflate algorithm.

# Requirements
Ubuntu 18.04 or higher.
```sh
#rust dependencies
sudo apt-get update && sudo apt-get install -y curl
curl https://sh.rustup.rs -sSf | sh #cargo

#python dependencies
sudo apt-get update && sudo apt-get install -y python3 python3-pip
python3 -m pip install -r cffi numpy
```

# Building
```sh
make [language]
```

supported `[language]` variable `c`, `cpp`, `rust`, `python`
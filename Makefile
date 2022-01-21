SUPPORTED_LANGS = c cpp rust python

CPP = g++
CARGO = cargo
PYTHON = python3

GZIP_DIR = gzip

C_DIR = c
CPP_DIR = cpp
RUST_DIR = rust
PY_DIR = python

C_STATIC_LIBRARY = $(GZIP_DIR)/librarezip.a
C_HEADER = $(GZIP_DIR)/librarezip.h

all: $(SUPPORTED_LANGS)

$(GZIP_DIR)/%:
	cd $(GZIP_DIR) && $(MAKE) $*

$(GZIP_DIR)/librarezip.a:

c: $(GZIP_DIR)/librarezip.a
	cp $< $(C_DIR)/librarezip.a
	cp $(GZIP_DIR)/librarezip.h $(C_DIR)/librarezip.h

cpp: $(GZIP_DIR)/librarezip.a
	cp $(GZIP_DIR)/librarezip.h $(CPP_DIR)/librarezip.h
	$(CPP) -shared $(GZIP_DIR)/librarezip.a -o $(CPP_DIR)/librarezip.so

rust: #dependencies handled by cargo build.rs
	cd $(RUST_DIR) && $(CARGO) build --release
	cd $(RUST_DIR) && $(CARGO) test -- --test-threads=1

python: $(GZIP_DIR)/librarezip.a
	cd $(PY_DIR) && $(PYTHON) rarezip.py

clean:
	rm -rf $(CPP_DIR)/*
	rm -rf $(C_DIR)/*
	rm -rf $(PY_DIR)/*.c $(PY_DIR)/*.o $(PY_DIR)/*.so
	cd $(RUST_DIR) && $(CARGO) clean
	cd $(GZIP_DIR) && $(MAKE) clean

.PHONY: $(SUPPORTED_LANGS)
For the sake of simplicity, this file loads up a database created by `process_corpus.py` populates the data into C++ maps, and then returns the map sizes and resulting data to the terminal.

The file assumes SQLite3 is available on the system (feel free to use the `corpus/CondInstall.sh` script to install Python and all required libraries if they are unavailable).

Currently, the file can be built using:
```bash
g++ ../tests/db_tests/test_read_tags.cpp ../src/read_tags.cpp -lsqlite3 -std=c++11 -I../include -o test_db
```
and assuming the `qnlp_tagged_corpus.sqlite` database exists in the current directory, it will be loaded.

The entire procedure is as follows:
```bash
cd corpus
chmod +x ./CondInstall.sh
./CondInstall.sh
source load_env.sh
cd ../tests/db_tests
g++ ./test_read_tags.cpp ../../src/read_tags.cpp -lsqlite3 -std=c++11 -I../../include -o test_db
python ../../py/process_corpus.py ../../corpus/<corpus_file_to_process>
./test_db
```

The compilation procedure will be added to the eventual CMake build structure in time.
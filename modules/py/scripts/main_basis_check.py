from QNLP.proc.basis_check import *

if __name__ == "__main__":
    if len(sys.argv) > 2:
        db_file = os.sys.argv[2]
        basis_check(db_file)
    else:
        basis_check()

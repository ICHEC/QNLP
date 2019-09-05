import QNLP.proc.process_corpus

if __name__ == "__main__":
    if len(os.sys.argv) < 2:
        raise("Please specify the path to basis words as arg 1, corpus as arg 2, and processing mode as arg 3")
    BasisPath = os.sys.argv[1]
    CorpusPath = os.sys.argv[2]
        
    if len(os.sys.argv) == 4:
        proc_mode = os.sys.argv[3]
    else:
        proc_mode=0

    run(BasisPath, CorpusPath, proc_mode, db_name="qnlp_tagged_corpus")
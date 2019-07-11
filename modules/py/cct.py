"""
This file creates a quantum circuit .tex file for viewing the nCU deocmposition output.
Assumes the availability of the quantikz LaTeX package.
"""

def ncu_cct_depth(num_ctrl):
    """
    Calculate the required depth for the circuit using the implemented nCU decomposition
    """
    if num_ctrl > 2:
        return 2 + 3*ncu_cct_depth(num_ctrl-1)
    else:
        return 5

def ctrl_line_column(gate_name, ctrl, tgt, num_qubits):
    """
    Creates a columnar slice of the circuit with the given gate name, control and target lines.
    The number of qubits specifies the length of the column.
    """
    column = ["\qw & "]*num_qubits
    column[tgt] = r"\gate{{{}}} & ".format(gate_name)
    column[ctrl] = r"\ctrl{{{}}} & ".format(tgt - ctrl)
    return column

def load_data_csv(csv_file):
    """
    Loads the data from a CSV file. Assumes the following layout:
    gate_name, control_qubit_number, target_qubit_number, gate_matrix_values
    """
    import csv
    cct = []
    with open(csv_file, 'r') as csvfile:
        filereader = csv.reader(csvfile, delimiter=',')
        for row in filereader:
            cct.append(ctrl_line_column(row[0], int(row[1]), int(row[2]), 5) )
    return cct

def latex_cct(data_file, file_name="cct", max_depth=16):
    """
    LaTeX file outputter for quantum circuit generation.
    """

    cct_array = load_data_csv(data_file)
    num_qubits = len(cct_array[0])
    depth = ncu_cct_depth(num_qubits-1)

    with open(file_name + ".tex", "w") as f:
        f.write("\\documentclass{standalone} \n \\usepackage{amsmath} \\usepackage{tikz} \\usetikzlibrary{quantikz} \n \\begin{document}\n")
        # Split the circuit on a given depth boundary

        # Due to issues with latex variables ending with numeric indices, appending letters to the temporary savebox commands allows us to generate multiple of these.
        # As depth is an issue with circuits, we currently expect the output not to exceed n-choose-k of 52-C-4 = 270725 combinations
        # This will suffice until later.
        import string, itertools
        s_list = [i for i in string.ascii_letters]
        box_str = r"\Box"
        label_iterator = itertools.combinations(s_list,4)
        box_labels = []
        for i in range(0, depth, max_depth):
            local_label = box_str + "".join( next(label_iterator))
            box_labels.append(local_label)
            f.write(r"\newsavebox{{{}}}".format(local_label))
            f.write(r"\savebox{{{}}}{{".format(local_label))
            f.write("\\begin{quantikz}[row sep={0.7cm,between origins}]")
            #Transposes the data so that q rows of length n exist, rather than n cols of length q
            if(i + max_depth < depth):
                cct_list_qubit= list(map(list, zip(*cct_array[i:i+max_depth])))
            else:
                cct_list_qubit= list(map(list, zip(*cct_array[i:])))
            for q in cct_list_qubit:
                f.write("\\qw &".join(q) + "\\qw \\\\")
            f.write(" \\end{quantikz} }\n")
            f.write("\n")
        f.write("\\begin{tabular}{c}\n")
        for l in box_labels:
            f.write(r"\usebox{{{}}} \\".format(l))
            f.write("\n \\vspace{2em} \\\\")

        f.write("\\end{tabular}\n")
        f.write(r"\end{document}")

if __name__== "__main__":
    import os
    args = os.sys.argv
    if len(args) < 2:
        print("Please specify the file to load, and output filename to save: python cct.py <CSV> <>")
        exit()
    
    latex_cct(args[1], args[2])
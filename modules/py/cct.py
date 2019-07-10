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

def latex_cct(data_file, file_name="cct"):
    """
    LaTeX file outputter for quantum circuit generation.
    """

    cct_array = load_data_csv(data_file)
    num_qubits = len(cct_array[0])
    depth = ncu_cct_depth(num_qubits-1)

    with open(file_name + ".tex", "w") as f:
        f.write("\\documentclass[crop]{standalone} \n \\usepackage{amsmath} \\usepackage{tikz} \\usetikzlibrary{quantikz} \n \\begin{document} \\begin{quantikz}\n")
        #Transposes the data so that q rows of length n exist, rather than n cols of length q
        cct_list_qubit= list(map(list, zip(*cct_array)))
        for q in cct_list_qubit:
            f.write("".join(q) + "\\\\ \n")
        f.write(r"""\end{quantikz}""")
        f.write(r"\end{document}")

if __name__== "__main__":
    import os
    args = os.sys.argv[]
    if len(args < 1):
        print("Please specify the file to load")
        return
    
    latex_cct(args[1])
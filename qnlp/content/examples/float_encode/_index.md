---
title: "Encoding data into state coefficients"
date: 2020-03-01T10:48:38Z
draft: false
---

This work presents some preliminary result of the project into encoding numerical values directly into the state coefficients. See notebook `EntanglementVectorCompare_py.ipynb` for a runnable example.


# Encoding data into state coefficients (analogue encoding)


```python
import os
os.environ["OMP_NUM_THREADS"] = "4"
```


```python
# Load the wrqpped simulator as before
import PyQNLPSimulator as pqnlp
from PyQNLPSimulator import PyQNLPSimulator as p

# Load the wrapped 2x2 matrix type; dcm => double complex matrix
from PyQNLPSimulator import DCMatrix as dcm

# Load the QNLP python packages
import QNLP as q

# Useful for examining various product types
from itertools import product

# Load numpy and matrix-based functions from scipy
import numpy as np
from scipy.linalg import expm, sinm, cosm
```


```python
# Use nonlinear least squares to determine the coefficients of the states to encode the numerical vector data.
# Given the 2*num_qubits coefficients needed for each individual qubit, we solve f(x)=b, wherein the b is the 
# data to be encoded, x are the qubit coefficients, where a 1d array of length N holds N/2 qubits, with the 
#|0> and |1> states in alternating odd/even patterns, and f(x) is the kronecker producted coefficients of 
# these individual qubit pure states.
import numpy as np
import scipy.optimize

num_qubits = 2

#Function that computes the kronecker product of each individual qubit, and returns an array with each state coeff
def func(x):
    qubits = []
    for ii in range(0,len(x),2):
        qubits.append(np.array([x[ii],x[ii+1]]))
    coeffs = [1]
    for ii in range(0,len(x)//2):
        coeffs = np.kron(coeffs, qubits[ii])
    return np.array(coeffs)

#f(x) - b = 0 to be solved
def nl_eqs(x, *rhs):
    res = func(x)-rhs/np.linalg.norm(rhs)
    return res

#Vector data to encode
data_vals = [3.42, 1.24, 1.97, 0.72] #np.ones(2**num_qubits)

#Initial values for the qubits. 
qubit_init_coeff = 1/(np.sqrt(2))*np.ones(2*num_qubits)

#Constrained NL LSQ, with coefficients between -1 and 1. 
x = scipy.optimize.least_squares(nl_eqs, qubit_init_coeff, bounds=([-1,1]), args=data_vals).x
for ii in range(0,len(x),2):
    x[ii:ii+2] = x[ii:ii+2]/np.linalg.norm(x[ii:ii+2])
print(x)
```

    [0.86632121 0.4994873  0.93989541 0.34146248]


Following the work of PRL 114 110504 (2015) we can achieve similar state coeffs by using the given data state. We can wrap the above method up into a function as follows.


```python
def encoding_data(data_vec):
    #Ensure we have enough qubits to encode string, and pad if data is non power of 2
    dvec = np.array(data_vec)
    lg2_len = np.log2(len(dvec))
    required_qubits = np.ceil(lg2_len)
    num_qubits = int(required_qubits)
    if lg2_len != required_qubits:
        dz = np.zeros(2**num_qubits)
        dz[:dvec.shape[0]] = dvec
        dvec = dz

    #Assign initial values
    qubit_init_coeff = 1/(np.sqrt(2))*np.ones(2*num_qubits)
    #Perform NLLSQ
    x =  scipy.optimize.least_squares(nl_eqs, qubit_init_coeff, bounds=([-1,1]), args=dvec).x
    
    #Ensure the resulting states are normalised according to each qubit's coefficients
    for ii in range(0,len(x),2):
        x[ii:ii+2] = x[ii:ii+2]/np.linalg.norm(x[ii:ii+2])
    return x
    
```


```python
y = encoding_data([0.5,0.5,0.9,0.1])
```


```python
num_samples = 3000
```

Examine the encoding of data into a single qubit first. Extract the unitary evolution matrix corresponding with a $R_y(2\theta)$ rotation to place the qubit into the appropriate state



```python
use_fusion = False
sim = p(1, use_fusion)
normalise = True
gateOps = pqnlp.GateOps(sim)
```


```python
theta=np.pi/4
```


```python
expm(-0.5*theta*1j*np.array([[1,0],[0,1]]))
```




    array([[0.92387953-0.38268343j, 0.        +0.j        ],
           [0.        +0.j        , 0.92387953-0.38268343j]])




```python
sim.applyGateRotY(0, 2*np.arccos(x[ii]))
u_mat = gateOps.RY(2*np.arccos(x[ii]))

print("U matrix:= ")
print (u_mat)
print("Applying matrix to state |0> = [1,0]^T")
print (np.matmul(u_mat, [1, 0]))
```

    U matrix:= 
    [[ 0.93989541+0.j -0.34146248+0.j]
     [ 0.34146248+0.j  0.93989541+0.j]]
    Applying matrix to state |0> = [1,0]^T
    [[0.93989541+0.j 0.34146248+0.j]]


Now that we have a method to encode data into a single qubit, we can encode the full vector data into the entire register as follows. We begin by defining the starting location of the GHZ state we wish to encode data into. For that, we need to examine the control qubit (0) and apply a controlled $R_y$ operation if it is set, to ensure the test data is set into state $|00\ldots\rangle$ and the reference data in state $|11\ldots\rangle$.


```python
# In memory DB for storing values
import sqlite3
if 'conn' in globals():
    conn.close()
conn = sqlite3.connect(':memory:')
# Get a cursor object
cursor = conn.cursor()
cursor.execute('''
    CREATE TABLE q_states(id INTEGER PRIMARY KEY, state_key TEXT,
                       dtheta REAL, counts INTEGER, num_qubits INTEGER, measured_reg TEXT)
''')
```




    <sqlite3.Cursor at 0x1408fb6c0>



In the next step, we compare two vectors, wherein one of the two is rotated through an angle, $\Delta \theta$, from $0$ to $2\pi$. The resulting counts are recorded into the previously created database, with the correspodning angle and state label.

In the previously coded examples, the measurement was required on the full set of qubits to examine the outcome. However, by applying another H gate to the auxiliary qubit, we can modify the phase given by the encoded states. Taking an estimate of the population as the counts in $\vert 0\rangle$ auxiliary state over the total number of samples gives an approx. for the dot product, as seen by the comparison with the numpy `dot(x,y)` function below the next cell.


```python
circ = p(num_qubits + 1 , use_fusion)

work_qubits = range(num_qubits)
aux_qubit = len(work_qubits)
```


```python
circ.getNumQubits()
```




    3




```python
counts = {}
for i in range(num_samples):
    circ.initRegister()
    circ.applyGateH(aux_qubit);           

    for ii in range(0,len(x),2):
        circ.applyGateCRotY(aux_qubit, work_qubits[ii//2], 2*np.arccos(x[ii]) )

    #Next, encode the reference data by flipping the first bit to allow it into the |11> state
    circ.applyGateX(aux_qubit)
    for ii in range(0,len(y),2):
        circ.applyGateCRotY(aux_qubit, work_qubits[ii//2], 2*np.arccos(y[ii]) )

    #Encode the information into the phase of the auxiliary qubit
    circ.applyGateH(aux_qubit)

    #ghz_circ.collapseToBasisZ(aux_qubit, 0)
    result = circ.applyMeasurementToRegister([aux_qubit], True)

    if str(num_qubits) + "_" + str(result) in counts:
        counts[str(num_qubits) + "_" + str(result)] += 1
    else:
        counts[str(num_qubits) + "_" + str(result)] = 1
    
print(counts)
```

    {'2_0': 2867, '2_1': 133}



```python
counts.keys()
```




    dict_keys(['2_0', '2_1'])



We can compare the similarity of the results using Numpy dot product of data x and y, with that of the above measurements. As we can see, the results are over 99.9% similar.


```python
print("% difference between numpy to quantum encoded and measured result: {}%".format(
    100*((np.dot(x,y)/num_qubits) - (counts[str(num_qubits) + "_" + str(0)]/num_samples)) / (np.dot(x,y)/num_qubits)
))
```

    % difference between numpy to quantum encoded and measured result: 0.14998285317752477%



```python
num_qubits = 2

#Register indices
qreg_work = range(num_qubits)
qreg_aux = len(qreg_work)

#Prepare the GHZ state on qubits 1,2, and use 0 as the control qubit for Rotations
q_circ = p(num_qubits + 1, use_fusion); #Entangled state for storing vectors and aux qubit
    
for th in np.linspace(0, 2*np.pi, 100, endpoint=True):
    counts = {}
    
    for n in range(num_samples):
        q_circ.initRegister()
        q_circ.applyGateH(qreg_aux);           

        #Encode the data to test
        x = [1,0]*num_qubits
        y = [np.cos(th),np.sin(th)]*num_qubits
        for ii in range(0,len(x),2):
            q_circ.applyGateCRotY(qreg_aux, qreg_work[ii//2], 2*np.arccos(x[ii]))

        #Next, encode the reference data by flipping the first bit to allow it into the |11> state
        q_circ.applyGateX(qreg_aux)
        for ii in range(0,len(y),2):
            q_circ.applyGateCRotY(qreg_aux, qreg_work[ii//2], 2*np.arccos(y[ii]))

        #Encode the information into the phase of the auxiliary qubit        
        q_circ.applyGateH(qreg_aux)

        #Extract result from aux qubit measurement
        result = q_circ.applyMeasurementToRegister([qreg_aux], True)
        
        if str(num_qubits) + "_" + str(result) in counts:
            counts[str(num_qubits) + "_" + str(result)] += 1
        else:
            counts[str(num_qubits) + "_" + str(result)] = 1

    for k,v in counts.items():
        cursor.execute('''INSERT INTO q_states(state_key, dtheta, counts, num_qubits, measured_reg) 
                    VALUES(?,?,?,?,?)''', (k, th, v, num_qubits, 'aux'))                         
    conn.commit()
```


```python
#Extract the unique state labels and angles for plotting
cursor.execute('''SELECT DISTINCT state_key FROM q_states ''')
uniq_states = [s[0] for s in cursor.fetchall()]
cursor.execute('''SELECT DISTINCT dtheta FROM q_states ''')
angles = [s[0] for s in cursor.fetchall()]
```


```python
#Extract the appropriate data from the DB and plot the results
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

counts_arr = np.zeros((len(uniq_states), len(angles)));
for c_jj, v_jj in enumerate(uniq_states):
    for c_ii, v_ii in enumerate(angles):
        cursor.execute('''SELECT counts, state_key, dtheta FROM q_states WHERE dtheta=? AND state_key=?''', (v_ii, v_jj,) )
        all_rows = cursor.fetchall()
        for row in all_rows:
            counts_arr[c_jj][c_ii] = row[0];
            
plt.rcParams['figure.figsize'] = [20, 15]
fig, ax = plt.subplots()
im = ax.imshow(counts_arr/num_samples, cmap='RdBu')
ax.set_aspect(20)
plt.xticks([i*16 for i in [0, np.pi/2, np.pi, 3*np.pi/2, 2*np.pi]], ['0', 'pi/2', 'pi', '3pi/2', '2pi'])
plt.yticks(np.arange(len(uniq_states)), uniq_states)
cbar = ax.figure.colorbar(im)
```


```python
#Equivalent data in lineplot, which hides overlap of crossed states
for c_jj, v_jj in enumerate(uniq_states):
    plt.plot(angles, counts_arr[c_jj][:]/num_samples, linewidth=2, label=v_jj)
plt.legend()
```




![png](/QNLP/img/small_encode.png)


For a 2 qubit (4 float) data set, as the vector is rotated through a variety of angles relative to the reference, we can see occupation of the $|00\rangle$ state (which is $100\%$ at $\Delta\theta=0$), lessens, and states $|01\rangle$ and $|10\rangle$ have non-negligible counts (not shown as requires measurement of the register). As the vector is rotated to $\pi/2$, the occupation of the $|01\rangle$ and $|10\rangle$ drops off, and the $|11\rangle$ state approaches $50\%$, along with $|00\rangle$. This implies that the states are now maximally entangled, in state $\left(|00\rangle + |11\rangle\right)/\sqrt{2}$, with further rotations causing an oscillation through these probabilities back to the original states.

As such, when the vector is the same, $P(|00\rangle) = 100\%$ $P(\textrm{all others}) = 0\%$, and when they are orthogonal $P(|00\rangle) =50\%$ and $P(|11\rangle)=50\%$.

Measuring the auxiliary bit as implied by PRL 114, 110504 (2015) however does not allow us to extract any information, as measurements of the full state is required.  Applying an additional H gate to the circuit does change this, but leaves us with a large plateau in the region approaching vector orthogonality for larger qubit registers.

# 4 qubit register (upto 16 floating-point values)
If we choose to measure the auxiliary qubit first, we essentially transform the state as follows: for an angle of 0 the auxiliary qubit result is completely separable from the register values as the states are 
$\left(1/\sqrt{2}\right)\left(\vert 0 \rangle + \vert 1 \rangle \right)\vert 0000 \rangle $. When the states are orthogonal, it becomes $\left( \vert 0 \rangle \vert 0000\rangle + \vert 1 \rangle \vert 1111 \rangle \right)(1/\sqrt{2})$ which is maximally entangled.

However, more realistically, we can assume the states of the system can be described as $\left( \vert 0 \rangle \vert \alpha\rangle + \vert 1 \rangle \vert \beta \rangle \right)(c/\sqrt{2})$, where $\alpha, ~\beta$ are the respective arbitrary states to be compared, and $c$ is the normalisation coefficient of the register excluding the auxiliary qubit. Given the above state, performing a Hadamard gate on the resulting auxiliary qubit modifies the overall state to $$\vert \psi \rangle = d\left(\vert 0 \rangle \left( \vert \alpha\rangle + \vert\beta\rangle \right) + \vert 1 \rangle \left( \vert \alpha\rangle - \vert\beta\rangle \right)\right) \\\ = d\left(\vert 0 \rangle + \vert 1 \rangle \frac{\vert \alpha\rangle - \vert\beta\rangle}{ \vert \alpha\rangle + \vert\beta\rangle }\right)
$$. 


```python
num_qubits = 4

#Register indices
qreg_work = range(num_qubits)
qreg_aux = len(qreg_work)

q_circ = p(num_qubits + 1, use_fusion); #Entangled state for storing vectors and aux qubit
    
for th in np.linspace(0, 2*np.pi, 100, endpoint=True):
    counts = {}
    
    for n in range(num_samples):
        q_circ.initRegister()
        q_circ.applyGateH(qreg_aux);           

        #Encode the data to test
        x = [1,0]*num_qubits
        y = [np.cos(th),np.sin(th)]*num_qubits
        for ii in range(0,len(x),2):
            q_circ.applyGateCRotY(qreg_aux, qreg_work[ii//2], 2*np.arccos(x[ii]))

        #Next, encode the reference data by flipping the first bit to allow it into the |11> state
        q_circ.applyGateX(qreg_aux)
        for ii in range(0,len(y),2):
            q_circ.applyGateCRotY(qreg_aux, qreg_work[ii//2], 2*np.arccos(y[ii]))

        #Encode the information into the phase of the auxiliary qubit        
        q_circ.applyGateH(qreg_aux)

        #Extract result from aux qubit measurement
        result = q_circ.applyMeasurementToRegister([qreg_aux], True)
        
        if str(num_qubits) + "_" + str(result) in counts:
            counts[str(num_qubits) + "_" + str(result)] += 1
        else:
            counts[str(num_qubits) + "_" + str(result)] = 1

    for k,v in counts.items():
        cursor.execute('''INSERT INTO q_states(state_key, dtheta, counts, num_qubits, measured_reg) 
                    VALUES(?,?,?,?,?)''', (k, th, v, num_qubits, 'aux'))                         
    conn.commit()
```


```python
# Update unique list of states from DB
cursor.execute('''SELECT DISTINCT state_key FROM q_states ''')
uniq_states = [s[0] for s in cursor.fetchall()]
```


```python
#Extract the appropriate data from the DB and plot the results
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

counts = np.zeros((len(uniq_states), len(angles)));
for c_jj, v_jj in enumerate(uniq_states):
    for c_ii, v_ii in enumerate(angles):
        cursor.execute('''SELECT counts, state_key, dtheta FROM q_states WHERE dtheta=? AND state_key=?''', (v_ii, v_jj,) )
        all_rows = cursor.fetchall()
        for row in all_rows:
            counts[c_jj][c_ii] = row[0];
            
plt.rcParams['figure.figsize'] = [20, 15]
fig, ax = plt.subplots()
im = ax.imshow(counts/num_samples, cmap='RdBu')
ax.set_aspect(10)
plt.xticks([i*16 for i in [0, np.pi/2, np.pi, 3*np.pi/2, 2*np.pi]], ['0', 'pi/2', 'pi', '3pi/2', '2pi'])
plt.yticks(np.arange(len(uniq_states)), uniq_states)
cbar = ax.figure.colorbar(im)
```


![png](/QNLP/img/small_encode_pcolor.png)



```python
#Equivalent data in lineplot, which hides overlap of crossed states
for c_jj, v_jj in enumerate(uniq_states):
    plt.plot(angles, counts[c_jj][:]/num_samples, linewidth=2)
    plt.legend(uniq_states)
```


![png](/QNLP/img/multiline_encode.png)


As can be seen, the central region platueaus for increased qubit register size.

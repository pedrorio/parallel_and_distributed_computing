import numpy


def matrix_factorization(R, P, Q, K, steps=5000, alpha=0.001):
    for step in range(steps):
        # xx: update the P and Q
        for i in range(len(R)):
            for j in range(len(R[i])):
                if R[i][j] > 0:
                    eij = R[i][j] - numpy.dot(P[i, :], Q[:, j])
                    for k in range(K):
                        P[i][k] = P[i][k] + alpha * (2 * eij * Q[k][j])
                        Q[k][j] = Q[k][j] + alpha * (2 * eij * P[i][k])
    return P, Q


A = [
    [2.000000, 0.000000, 3.000000, 0.000000, 0.000000],
    [0.000000, 0.000000, 3.000000, 0.000000, 1.000000],
    [0.000000, 5.000000, 3.000000, 4.000000, 0.000000]
]

L = [
    [0.420094, 0.197191],
    [0.391550, 0.399220],
    [0.455824, 0.098776]
]
R = [
    [0.167611, 0.384115, 0.138887, 0.276985, 0.238699],
    [0.314435, 0.182392, 0.256700, 0.476115, 0.458098]
]

A = numpy.array(A)
R = numpy.array(R)
L = numpy.array(L)

L, R = matrix_factorization(A, L, R, 2, steps=5000, alpha=0.0002)
print(L)
print(R)

print(numpy.dot(L,R))

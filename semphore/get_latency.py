import pandas as pd

sr=pd.read_csv('srshm.csv', header=None)[1]
sw=pd.read_csv('swshm.csv', header=None)[1]

print((sr-sw).mean())

r=pd.read_csv('srzmq.csv', header=None)[1]
w=pd.read_csv('swzmq.csv', header=None)[1]

print((r-w).mean())

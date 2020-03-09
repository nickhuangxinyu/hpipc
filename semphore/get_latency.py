import pandas as pd

sr=pd.read_csv('rshm.csv', header=None)
sw=pd.read_csv('wshm.csv', header=None)

a = (sr[0]-sw[0])*1000000 + sr[1]-sw[1]


r=pd.read_csv('rzmq.csv', header=None)
w=pd.read_csv('wzmq.csv', header=None)
b = (r[0]-w[0])*1000000 + r[1]-w[1]

print(len(a))
print(len(b))
print('for %d data push and pop, SHM avg_latency=%lfus, ZMQ avg_latency=%lfus' % (len(a), a.mean(), b.mean()))

from numpy import *
import pylab
#DataIn = loadtxt('input.dat')
#DataIn = loadtxt('example.txt')

x, y = loadtxt('lastdatapoint.dat',unpack=True, usecols=[0,1],delimiter=',')

#print x.shape[0]

pylab.plot(x,y)

pylab.show()
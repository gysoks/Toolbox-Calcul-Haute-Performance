import numpy as np
import matplotlib.pyplot as plt
import os


os.system("echo calcul reduction sur cpu : scalaire")
os.system("echo '...compilation'")
os.system('gcc -O3 -march=native sum-cpu.c -o sum-cpu.x -lm')
os.system("echo '...execution'")
os.system('./sum-cpu.x')

os.system("echo calcul reduction sur cpu : openmp")
os.system("echo '...compilation'")
os.system('gcc -O3 -march=native -fopenmp sum-omp.c -o sum-omp.x -lm')
os.system("echo '...execution'")
os.system('./sum-omp.x')

os.system("echo calcul reduction sur gpu")
os.system("echo '...compilation'")
os.system('nvcc -O3 sum-ocl.c -o sum-ocl.x -lOpenCL')
os.system("echo '...execution'")
os.system("touch reduction-ocl.txt")
os.system("rm reduction-ocl.txt")
for i in range(10,28):
    j=2**i
    cmd="./sum-ocl.x "+str(j)
    os.system(cmd)

os.system("echo '...analyse des resultats'")
data_gpu=np.loadtxt('./reduction-ocl.txt')
data_cpu=np.loadtxt('./SumScalar.txt')
print("shape",data_cpu.shape)
data_omp=np.loadtxt('./SumOmp.txt')
print("shape",data_omp.shape)
dimdata=data_omp.shape
nblines  =dimdata[0]
nbthreads=dimdata[1]

mycol=['red','blue','green','orange','cyan','purple','red','blue','black']
plt.figure(1)
plt.clf()
plt.grid(True)
plt.xscale('log')
plt.yscale('log')
plt.xscale('log')
plt.yscale('log')
plt.xlabel('$\log(N)$')
plt.ylabel('$\log(t [s])$')
plt.plot(data_cpu[:,0],data_cpu[:,1],'ro-',label='cpu')
plt.plot(data_gpu[:,0],data_gpu[:,1],'ko-',label='gpu')
for n in range(1,nbthreads):
   lab='nbthreads='+str(n)
   plt.plot(data_omp[:,0],data_omp[:,n],color=mycol[n-1],marker='*',linestyle='-',label=lab)

plt.title("Time required to compute $\sum\limits_{i=1}^Nu_i$ with OpenMP and OpenCL")
plt.legend(loc='best')
plt.savefig('logt-logn-ocl-openmp.png',dpi=400)
plt.close(1)
os.system("echo")
os.system("echo '--- script ended'")


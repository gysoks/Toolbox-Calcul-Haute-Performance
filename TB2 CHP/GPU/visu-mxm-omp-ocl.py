import os


os.system("echo calcul mxm sur cpu : scalaire")
os.system("echo '...compilation'")
os.system('gcc -O3 -march=native mxm_omp.c -o mxm_omp.x -lm')
os.system("echo '...execution'")
os.system('./mxm_omp.x')

os.system("echo calcul mxm sur cpu : openmp")
os.system("echo '...compilation'")
os.system('gcc -O3 -march=native -fopenmp mxm_omp.c -o mxm_omp.x -lm')
os.system("echo '...execution'")
os.system('./mxm_omp.x')
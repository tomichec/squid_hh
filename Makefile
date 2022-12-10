LDFLAGS=-lm
CC = gcc

OPT =set term postscript eps color enhanced font 'Helvetica' 12


AUX = vm.eps ina.eps ik.eps

.PHONY: clean veryclean

all: squid_hh.pdf

squid_driver.o: squid_driver.c squid_var.h

squid_driver: squid_driver.o

squid.dat: ./squid_driver
	./$< > $@

vm.eps: squid.dat common.gnu
	gnuplot -e "\
	$(OPT) size 9cm, 3cm;\
	load 'common.gnu';\
	set ylabel 'TVM';\
	set y2label 'TIKKINa';\
	set y2tics 300;\
	plot '$<' u 1:2 w l t 'VM' ls 1, '' u 1:6 w l axis x1y2 t 'INa' ls 2, '' u 1:7 w l axis x1y2 t 'IKK' ls 3;" > $@

ik.eps: squid.dat common.gnu
	gnuplot -e "\
	$(OPT) size 5cm, 3cm;\
	load 'common.gnu';\
	set ylabel 'TIKK';\
	set y2label 'N';\
	set y2tics 0.1;\
	set ytics 200;\
	set xtics 2;\
	plot '$<' u 1:7 w l t 'IKK' ls 3, '' u 1:3 w l axis x1y2 t 'N' ls 4\
	" > $@

ina.eps: squid.dat common.gnu
	gnuplot -e "\
	$(OPT) size 5cm, 3cm;\
	load 'common.gnu';\
	set ylabel 'TINa';\
	set y2label 'MH';\
	set y2tics 0.2;\
	set ytics 200;\
	set xtics 2;\
	plot '$<' u 1:6 w l t 'INa' ls 2, '' u 1:4 w l axis x1y2 t 'M' ls 5, '' u 1:5 w l axis x1y2 t 'H' ls 6\
	" > $@

%.pdf: %.dvi
	dvipdf $<
	pdfcrop $@ $@

%.dvi: %.tex $(AUX)
	latex $<

clean:
	-rm *.{o,aux,log}

veryclean:
	-make clean
	-rm *.eps *.dat *~ 
	-rm squid_driver

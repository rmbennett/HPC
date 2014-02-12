#!/bin/bash

T1=‘mktemp ../tmp/T1.XXXXXXX‘;
T2=‘mktemp ../tmp/T2.XXXXXXX‘;
T3=‘mktemp ../tmp/T3.XXXXXXX‘;
T4=‘mktemp ../tmp/T4.XXXXXXX‘;
T5=‘mktemp ../tmp/T5.XXXXXXX‘;
T6=‘mktemp ../tmp/T6.XXXXXXX‘;


./fir_filter coeffs/f500.csv > $T1;
cat $T1 | ./fir_filter coeffs/f600.csv > $T2;
cat $T2 | ./fir_filter coeffs/f800.csv > $T3;
cat $T3 | ./fir_filter coeffs/f1000.csv > $T4;
cat $T4 | ./fir_filter coeffs/f1200.csv > $T5;
cat $T6 | ./fir_filter coeffs/f1400.csv; 

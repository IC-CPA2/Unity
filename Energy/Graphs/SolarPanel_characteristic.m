%Individual solar panel IV and PI characteristic in different light level
current1 = [0.051
0.05
0.048
0.048
0.047
0.045
0.044
0.051
0.051
0.049
0.05
0.047
0.048
0.051
0.047
0.049
0.046
0.042
0.045
0.045
0.044
0.035
0.036
0.03
0.028
0.017
0.002
0.001
0.001];

voltage1 = [0.0025
0.248
0.56
0.824
1.047
1.305
1.671
1.96
2.27
2.54
2.83
2.9
3.19
3.48
3.81
4.08
4.38
4.55
4.59
4.64
4.68
4.656
4.885
4.835
4.814
4.84
5.33
5.37
5.54];

current2 = [0.029
0.031
0.03
0.032
0.035
0.03
0.03
0.034
0.029
0.028
0.021
0.022
0.02
0.016
0.017
0.016
0.004
0.005
0.002];

voltage2 = [0
0.653
1.348
2.03
2.7
3.37
4.079
4.668
4.825
4.87
4.91
5.123
5.082
5.06
5.05
5.06
5.3
5.28
5.29];

current3 = [0.016
0.017
0.016
0.017
0.017
0.017
0.017
0.015
0.014
0.01
0.005
0.006];

voltage3 = [0
0.754
1.493
2.16
2.59
3.55
4.46
4.8
4.85
5.02
5.09
5.12];

power1 = [0.0001275
0.0124
0.02688
0.039552
0.049209
0.058725
0.073524
0.09996
0.11577
0.12446
0.1415
0.1363
0.15312
0.17748
0.17907
0.19992
0.20148
0.1911
0.20655
0.2088
0.20592
0.16296
0.17586
0.14505
0.134792
0.08228
0.01066
0.00537
0.00554];

power2 = [0
0.020243
0.04044
0.06496
0.0945
0.1011
0.12237
0.158712
0.139925
0.13636
0.10311
0.112706
0.10164
0.08096
0.08585
0.08096
0.0212
0.0264
0.01058];

power3= [0
0.012818
0.023888
0.03672
0.04403
0.06035
0.07582
0.072
0.0679
0.0502
0.02545
0.03072];

figure(1);
plot (current1, voltage1,'b*',LineWidth=1.5)
hold on; 
plot (current2, voltage2,'rx', LineWidth=1.5)
hold on;
plot (current3, voltage3,'go', LineWidth=1.5)

legend('Light level 1', 'Light level 2', 'Light level 3')
xlabel("Current [A]")
ylabel("Voltage [V]")
title("Indiv SolarPanel V-I curve")
set(gca,'LineWidth',1.5,'FontWeight','bold')
saveas(gcf,"Indiv SolarPanel_VI.jpg",'jpg')

figure(2);
plot (current1, power1, 'b*',LineWidth=1.5)
hold on; 
plot (current2, power2, 'rx', LineWidth=1.5)
hold on;
plot (current3, power3, 'go', LineWidth=1.5)

legend('Light level 1', 'Light level 2', 'Light level 3')
ylabel("Power [W]")
xlabel("Current [A]")
title("Indiv SolarPanel P-I curve")
set(gca,'LineWidth',1.5,'FontWeight','bold')
saveas(gcf,"Indiv SolarPanel_PI.jpg",'jpg')
%% 
% 2x2 solar panel arrangement under sunlight

%Light level 1
current2x2_L1 = [0.066
0.064
0.065
0.067
0.068
0.071
0.064
0.06
0.057
0.048
0.042
0.037
0.035
0.024
0.01
0.007];

voltage2x2_L1 = [0.0033
2.46
5.12
6.51
7.44
8.12
9.98
10.68
10.88
10.04
10.06
10.26
10.36
11.44
10.78
11.03];

power2x2_L1 = [0.0002178
0.15744
0.3328
0.43617
0.50592
0.57652
0.63872
0.6408
0.62016
0.48192
0.42252
0.37962
0.3626
0.27456
0.1078
0.07721];

%Light level 2
current2x2_L4 = [0.4
0.392
0.387
0.498
0.388
0.488
0.375
0.332
0.262
0.216
0.18
0.15
0.094
0.11
0.054
0.024
0.005];

voltage2x2_L4 = [0.211
0.885
1.93
5.09
5.06
7.44
7.64
10.46
10.63
10.9
11.07
11.37
11.46
11.05
11.16
11.52
11.99];

power2x2_L4 = [0.0844
0.34692
0.74691
2.53482
1.96328
3.63072
2.865
3.47272
2.78506
2.3544
1.9926
1.7055
1.07724
1.2155
0.60264
0.27648
0.05995];

%Light level 3
current2x2_L3 = [0.251
0.251
0.254
0.228
0.198
0.168
0.162
0.143
0.106
0.092
0.054
0.051
0.03];

voltage2x2_L3 = [0.17
2.63
7.68
9.22
9.86
10.25
10.4
10.65
10.73
10.97
11.12
11.06
11.16];
power2x2_L3 = [0.04267
0.66013
1.95072
2.10216
1.95228
1.722
1.6848
1.52295
1.13738
1.00924
0.60048
0.56406
0.3348];

figure(3);
plot (current2x2_L1,voltage2x2_L1, 'b*',LineWidth=1.5)
hold on ; 
plot (current2x2_L4,voltage2x2_L4,'rx',LineWidth=1.5)
hold on ; 
plot (current2x2_L3,voltage2x2_L3,'go',LineWidth=1.5)
xlim([0 0.55])
ylim([0 12.5])
xlabel("current [A]")
ylabel("voltage [V]")
legend('level 1', 'level 2','level 3')
title("2x2 solar panel V-I curve")
set(gca,'LineWidth',1.5,'FontWeight','bold')
saveas(gcf,"2x2 SolarPanel_VI.jpg",'jpg')

figure(4);
plot(current2x2_L1,power2x2_L1, 'b*',LineWidth=1.5)
hold on; 
plot(current2x2_L4,power2x2_L4,'rx',LineWidth=1.5)
hold on; 
plot(current2x2_L3,power2x2_L3,'go',LineWidth=1.5)
xlim([0 0.55])
ylabel("Power [W]")
xlabel("Current [A]")
legend("level 1", "level 2","level 3")
title("2x2 solar panel P-I curve")
set(gca,'LineWidth',1.5,'FontWeight','bold')
saveas(gcf,"2x2 SolarPanel_PI.jpg",'jpg')
%% 
%1x4 solar panel arrangement under sunlight

current1x4_L1 = [0.03
0.029
0.032
0.033
0.038
0.03
0.025
0.026
0.015
0.009
0.008];
voltage1x4_L1 = [0.0003
0.353
2.32
2.44
4.22
4.95
4.8
4.99
5.13
5.12
5.18];
power1x4_L1 = [0.000009
0.010237
0.07424
0.08052
0.16036
0.1485
0.12
0.12974
0.07695
0.04608
0.04144];

current1x4_L2 = [0.042
0.041
0.027
0.035
0.028
0.025
0.032
0.021
0.019
0.021
0.018
0.016
0.009
0.007
0.002];

voltage1x4_L2 = [0.0177
0.324
1.352
2.58
2.76
2.64
3.35
3.76
4.06
4.63
4.57
4.48
4.75
4.82
4.89];
power1x4_L2 = [0.0007434
0.013284
0.036504
0.0903
0.07728
0.066
0.1072
0.07896
0.07714
0.09723
0.08226
0.07168
0.04275
0.03374
0.00978];

current1x4_L3 = [0.886
0.518
0.975
0.069
0.049
1.079
1.053
0.948
0.909
0.901
0.976];
voltage1x4_L3 = [0.57
5.44
2.06
5.56
5.54
1.57
3.2
1.32
3.84
3.9
1.142];
power1x4_L3 = [0.50502
2.81792
2.0085
0.38364
0.27146
1.69403
3.3696
1.25136
3.49056
3.5139
1.114592];
current1x4_L4 = [0.4
0.165
0.371
0.159
0.279
0.505
0.294
0.25
0.21
0.185
0.141
0.112
0.082
0.064
0.053];
voltage1x4_L4 = [0.275
0.164
0.818
0.286
1.359
4.9
4.29
4.86
5.14
5.34
5.44
5.45
5.97
5.57
5.87];
power1x4_L4 = [0.11
0.02706
0.303478
0.045474
0.379161
2.4745
1.26126
1.215
1.0794
0.9879
0.76704
0.6104
0.48954
0.35648
0.31111];

current1x4_L5 = [1.162
1.046
1.136
1.155
1.01
1.03
1.09
0.721
0.8
0.632
0.527
0.073
0.048
0.052
0.025];
voltage1x4_L5 = [0.625
1.068
2.34
2.63
3.07
3.14
4.07
5.01
4.96
4.67
5.36
5.67
5.74
5.8
5.86];
power1x4_L5 = [0.72625
1.117128
2.65824
3.03765
3.1007
3.2342
4.4363
3.61221
3.968
2.95144
2.82472
0.41391
0.27552
0.3016
0.1465];

current1x4_L6 = [0.987
0.97
0.995
0.989
0.964
0.921
0.843
0.775
0.514
0.079
0.055
0.049
0.269
0.115
0.007];

voltage1x4_L6 = [0.534
1.014
1.827
2.26
2.91
3.59
4.3
4.74
5.35
5.73
5.7
5.75
5.55
5.65
5.71];

power1x4_L6 = [0.527058
0.98358
1.817865
2.23514
2.80524
3.30639
3.6249
3.6735
2.7499
0.45267
0.3135
0.28175
1.49295
0.64975
0.03997];


figure(5);
plot (current1x4_L1,voltage1x4_L1, 'b*',LineWidth=1.5)
hold on ; 
plot (current1x4_L4,voltage1x4_L4,'rx',LineWidth=1.5)
hold on ; 
plot (current1x4_L3,voltage1x4_L3,'go',LineWidth=1.5)
hold on ; 
plot (current1x4_L5,voltage1x4_L5,'m+',LineWidth=1.5)
hold on; 
plot (current1x4_L6,voltage1x4_L6,'cd',LineWidth=1.5)
xlabel("current [A]")
ylabel("voltage [V]")
legend('level 1', 'level 2','level 3','level 4','level 5')
title("1x4 solar panel V-I curve")
set(gca,'LineWidth',1.5,'FontWeight','bold')
saveas(gcf,"1x4 SolarPanel_VI.jpg",'jpg')

figure(6);
plot(current1x4_L1,power1x4_L1, 'b*',LineWidth=1.5)
hold on; 
plot(current1x4_L4,power1x4_L4,'rx',LineWidth=1.5)
hold on; 
plot(current1x4_L3,power1x4_L3,'go',LineWidth=1.5)
hold on; 
plot(current1x4_L5,power1x4_L5,'m+',LineWidth=1.5)
hold on;
plot(current1x4_L6,power1x4_L6,'cd',LineWidth=1.5)
ylabel("Power [W]")
xlabel("Current [A]")
legend("level 1 (rainy)", "level 2","level 3","level 4",'level 5')
title("1x4 solar panel P-I curve")
set(gca,'LineWidth',1.5,'FontWeight','bold')
saveas(gcf,"1x4 SolarPanel_PI.jpg",'jpg')


%% 
%Battery IV characteristics
figure(7);
bvoltage = [4.51
4.55
4.61
4.67
4.71
4.77
4.82
4.87
4.91
4.94
5.02
5.06
5.13
5.21];
bcurrent = [0.01
0.01
0.202
0.422
0.596
0.857
1.047
1.27
1.39
1.546
1.83
1.881
1.878
1.882];

bpower = [0.0451
0.0455
0.93122
1.97074
2.80716
4.08789
5.04654
6.1849
6.8249
7.63724
9.1866
9.51786
9.63414
9.80522];
plot(bvoltage, bcurrent,'b--*',LineWidth=1.5)
ylabel("Current [A]")
xlabel("Voltage [V]")
grid on
title("Battery IV characteristic")
saveas(gcf,"Battery_IV.jpg",'jpg')

figure(9)
plot(bcurrent, bvoltage,'b*',LineWidth=1.5);
xlabel("Current [A]")
ylabel("Voltage [V]")
grid on
title("Battery VI characteristic")
set(gca,'LineWidth',1.5,'FontWeight','bold')
saveas(gcf,"Battery_VI.jpg",'jpg')

figure(10)
plot(bvoltage,bpower,'b--*',LineWidth=1.5)
ylabel("Power [W]")
xlabel('Voltage [V]')
grid on 
title ("Battery PV characteristic")
set(gca,'LineWidth',1.5,'FontWeight','bold')
saveas(gcf,"Battery_PV.jpg",'jpg')
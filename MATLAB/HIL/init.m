L = 15e-3;
R = 1;
Vdc = 1;
Ts = 1e-4;

reference_frequency_hz = 100;
n_levels = 9;

Kp = 6.3;
Ki = 630;
Kd = 0;

Ron = 0.025;
C = 100e-4;
tfModel1 =tf([Ron*C 1],[Ron*C*L, L+R*Ron*C R +  4*Ron]);
tfModel2 =tf([1],[L R]);

hold on
bode(tfModel1);
bode(tfModel2);
bode(tf3);

legend('Ron','Roff','SysId','HW')
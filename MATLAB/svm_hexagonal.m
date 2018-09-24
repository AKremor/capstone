close all;
clear all;

% hexagon
Vdc = 1;

transform = 1/(3*Vdc) * [
    2 -1 -1;
    -1 2 -1;
    1 1 1];

% Plot a vector for a bit
magnitude = 3 * 0.866;
time = linspace(0,1,10000);	   
frequency_hz = 2;
PI = 3.14;
a = magnitude * cos(2 * PI * frequency_hz * time); 
b = magnitude * cos(2 * PI * frequency_hz * time - 2*pi/3); 
c = magnitude * cos(2 * PI * frequency_hz * time + 2*pi/3); 

vref = transform * [a;b;c];

sel_a = [];
sel_b = [];
sel_c = [];

sel_g = [];
sel_h = [];

for k=1:length(time)		% cycle through all positions
    %set(node_1_vector,'xdata',[node_1(1) x(k)],'ydata', [node_1(2) y(k)])
    % but [g h]
    % My format is [x y]
    Vul = [ceil(vref(1,k)) floor(vref(2,k))];
    Vlu = [floor(vref(1,k)) ceil(vref(2,k))];
    Vuu = [ceil(vref(1,k)) ceil(vref(2,k))];
    Vll = [floor(vref(1,k)) floor(vref(2,k))];
    
    node_1 = Vul;
    node_2 = Vlu;
    
    if sign(vref(1,k) + vref(2,k) - (Vul(1) + Vul(2))) == 1
        node_3 = Vuu;
    else
        node_3 = Vll;
    end
    
    sel_g = [sel_g node_1(1)];
    sel_h = [sel_h node_1(2)];
end




 %value.a, value.b, value.c, a_phase, b_phase, c_phase, ab, bc, ca
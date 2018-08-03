close all;
clear all;

% hexagon
Vdc = 1;

transform = 1/(3*Vdc) * [
    2 -1 -1;
    -1 2 -1;
    1 1 1];

% Plot a vector for a bit
magnitude = 3;
theta = linspace(0,8*pi,1000);	     
a = magnitude * sin(theta); 
b = magnitude * sin(theta - 2*pi/3); 
c = magnitude * sin(theta + 2*pi/3); 

vref = transform * [a;b;c];

for k=1:length(theta)		% cycle through all positions
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
end




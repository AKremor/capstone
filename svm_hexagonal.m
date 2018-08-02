close all;
clear all;
n_levels_per_phase = 6;
phase_upper = floor(n_levels_per_phase / 2);
phase_lower = -1 * phase_upper;

graph_magnitude = 8;

three_ph_levels = [];

for a=phase_lower:phase_upper
   for b = phase_lower:phase_upper
       for c=phase_lower:phase_upper
           %three_ph_levels = [three_ph_levels; [a b c]];
           three_ph_levels = [three_ph_levels; [a-b b-c c-a]];
       end
   end
end

% hexagon
Vdc = 1;

transform = 1/(3*Vdc) * [
    2 -1 -1;
    -1 2 -1;
    1 1 1];

% TODO Need a Vdc at some point when we go to 'real' voltages
svm_nodes = [];
for i=1:length(three_ph_levels)
    svm_nodes = [svm_nodes; (transform * three_ph_levels(i, :)')'];
end

%scatter3(three_ph_levels(:,1), three_ph_levels(:,2), three_ph_levels(:,3));


figure
scatter(svm_nodes(:, 1), svm_nodes(:, 2))
hold on;
plot(-1*graph_magnitude:graph_magnitude, zeros(1, length(-1*graph_magnitude:graph_magnitude)))
plot(zeros(1, length(-1*graph_magnitude:graph_magnitude)), -1*graph_magnitude:graph_magnitude)

% Plot a vector for a bit
magnitude = 3;
theta = linspace(0,8*pi,1000);	     
a = magnitude * sin(theta); 
b = magnitude * sin(theta - 2*pi/3); 
c = magnitude * sin(theta + 2*pi/3); 

vref = transform * [a;b;c];
hbead = plot(vref(2,1), vref(2,1), '*', 'markersize',5);  % draw the bead at the initial 
ref_vector = plot([0 vref(1,1)], [0 vref(2,1)]);  % draw the bead at the initial 

node_1_vector = plot([0 vref(1,1)], [0 vref(2,1)]);  % draw the bead at the initial 
node_2_vector = plot([0 vref(1,1)], [0 vref(2,1)]);  % draw the bead at the initial 
node_3_vector = plot([0 vref(1,1)], [0 vref(2,1)]);  % draw the bead at the initial 

node_1 = [0 0];
node_2 = [0 0];
node_3 = [0 0];

node_1_bead = plot(node_1(1), node_1(2), 'r*', 'markersize', 5);
node_2_bead = plot(node_2(1), node_2(2), 'b*', 'markersize', 5);
node_3_bead = plot(node_3(1), node_3(2), 'k*', 'markersize', 5);


for k=1:length(theta)		% cycle through all positions
    set(hbead,'xdata',vref(1,k),'ydata', vref(2,k))
  
    set(ref_vector,'xdata',[0 vref(1,k)],'ydata', [0 vref(2,k)])
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
    
    set(node_1_bead ,'xdata', node_1(1),'ydata', node_1(2))
    set(node_2_bead ,'xdata', node_2(1),'ydata', node_2(2))
    set(node_3_bead ,'xdata', node_3(1),'ydata', node_3(2))
    
    set(node_1_vector,'xdata',[node_1(1) vref(1,k)],'ydata', [node_1(2) vref(2,k)])
    set(node_2_vector,'xdata',[node_2(1) vref(1,k)],'ydata', [node_2(2) vref(2,k)])
    set(node_3_vector,'xdata',[node_3(1) vref(1,k)],'ydata', [node_3(2) vref(2,k)])
    
    disp([Vul 0] / transform);
    drawnow
end




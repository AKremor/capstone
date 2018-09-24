close all;
clear all;
n_levels_per_phase = 9;
phase_upper = floor(n_levels_per_phase / 2);
phase_lower = -1 * phase_upper;

graph_magnitude = 8;

three_ph_levels = [];

for a=phase_lower:phase_upper
   for b = phase_lower:phase_upper
       for c=phase_lower:phase_upper
           three_ph_levels = [three_ph_levels; [a b c]];
       end
   end
end

% Apply Clarke transform to move each node onto the alpha-beta plane
clarke_transform = 2/3 * [
    1 -1/2 -1/2
    0 sqrt(3)/2 -sqrt(3)/2;
    1/2 1/2 1/2
    ];

svm_nodes = [];
for i=1:length(three_ph_levels)
    svm_nodes = [svm_nodes; (clarke_transform * three_ph_levels(i, :)')'];
end


% Adjust for integer usage
% TODO We need to apply the same scaling later on
x_scaling_factor = 3/2;
y_scaling_factor = 1/0.5774;
svm_nodes(:, 1) = x_scaling_factor .* svm_nodes(:, 1);
svm_nodes(:, 2) = y_scaling_factor .* svm_nodes(:, 2);
hold on;
scatter(svm_nodes(:, 1), svm_nodes(:, 2))
plot(-1*graph_magnitude:graph_magnitude, zeros(1, length(-1*graph_magnitude:graph_magnitude)))
plot(zeros(1, length(-1*graph_magnitude:graph_magnitude)), -1*graph_magnitude:graph_magnitude)

% Plot a vector for a bit
magnitude = 3;
theta = linspace(0,4*pi,1000);	     
x= x_scaling_factor * magnitude * cos(theta); 
y= y_scaling_factor * magnitude * sin(theta); 
hbead = plot(x(1), y(1), '*', 'markersize',5);  % draw the bead at the initial 
ref_vector = plot([0 x(1)], [0 y(1)]);  % draw the bead at the initial 

node_1_vector = plot([0 x(1)], [0 y(1)]);  % draw the bead at the initial 

node_1 = [0 0];
node_2 = [0 0];
node_3 = [0 0];
node_1_bead = plot(node_1(1), node_1(2), 'r*', 'markersize',5);
node_2_bead = plot(node_2(1), node_2(2), 'b*', 'markersize',5);
node_3_bead = plot(node_3(1), node_3(2), 'k*', 'markersize',5);


for k=1:length(theta)		% cycle through all positions
    set(hbead,'xdata',x(k),'ydata', y(k))
    
    node_1(2) = round(y(k));
    node_1(1) = round(x(k)) + sign(x(k) - round(x(k))) * 0.5 * mod(node_1(2), 2);
    
    set(node_1_bead ,'xdata', node_1(1),'ydata', node_1(2))
    %set(node_2_bead ,'xdata', node_2(1),'ydata', node_2(2))
    %set(node_3_bead ,'xdata', node_3(1),'ydata', node_3(2))
    set(ref_vector,'xdata',[0 x(k)],'ydata', [0 y(k)])
    set(node_1_vector,'xdata',[node_1(1) x(k)],'ydata', [node_1(2) y(k)])
    
    % Inverse clark to recover abc levels
    node_1_abc = inv(clarke_transform) * [node_1 0]';
    disp(node_1_abc);
    drawnow
end




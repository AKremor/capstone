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
svm_nodes(:, 1) = 3/2 .* svm_nodes(:, 1);
svm_nodes(:, 2) = 1/0.5774 .* svm_nodes(:, 2);
hold on;
scatter(svm_nodes(:, 1), svm_nodes(:, 2))
plot(-1*graph_magnitude:graph_magnitude, zeros(1, length(-1*graph_magnitude:graph_magnitude)))
plot(zeros(1, length(-1*graph_magnitude:graph_magnitude)), -1*graph_magnitude:graph_magnitude)


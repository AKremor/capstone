% Two dimensional lookup table
% g is represented by the first array index, h by the second
% g0 is located at 1 because MATLAB so we need to use g+1 everywhere
% Table stores k values
widest_dimension = 7;
n_levels=9;
n = n_levels;

lookup_table = zeros(widest_dimension*2 + 1,widest_dimension*2 + 1);

for g=-1*(widest_dimension + 1):(widest_dimension + 1)
    for h=-1*(widest_dimension + 1):(widest_dimension + 1)
        
        constraints_satisfied = 0;
        k = 0;
        while(~constraints_satisfied)
            if (k >= 0 && k - g >= 0 && k - g - h >= 0 && k <= n - 1 &&  k - g <= n - 1 && k - g - h <= n - 1) 
                constraints_satisfied = true;
                lookup_table(g+2+widest_dimension, h+2+widest_dimension) = k;
                break;
            end
            if (k >= n_levels) 
                lookup_table(g+2+widest_dimension, h+2+widest_dimension) = -1;
                % Saturation/error in modulator
                break
            end
            k = k + 1;
        end
        
    end
end
function serial_plotter_phases()
clear all;
close all;


s = serial('COM16', 'BaudRate', 921600);
fopen(s);
cleanupObj = onCleanup(@() cleanUpSerial(s));

hold on;

t = 0;
subplot(4,1,1)
levels = animatedline;
reference = animatedline;
subplot(4,1,2)
nine_cell = animatedline;
title('Nine cell')
subplot(4,1,3)
three_cell = animatedline;
title('Three cell')
subplot(4,1,4)
one_cell = animatedline;
title('One cell')


while(1)
    
    sync = 0;
    while(~sync)
        A = fread(s,2,'uint8'); 
        if(A(1) == hex2dec('CA') && A(2) == hex2dec('FE'))
            sync = 1;
        end
    end
    A = fread(s,3,'int8'); 
    ref = fread(s,2,'int8');
    B = fread(s,3,'int8'); 
    
    ref = ref(1);
    % Reassemble the reference
    reference_value = ref / 1;
    addpoints(levels, t, A(2))
    axis([t-200 t+200 -20 20])
    addpoints(reference, t, reference_value)
    axis([t-200 t+200 -20 20])
    
    addpoints(nine_cell, t, B(1))
    axis([t-200 t+200 -2 2])
    addpoints(three_cell, t, B(2))
    axis([t-200 t+200 -2 2])
    addpoints(one_cell, t, B(3))
    axis([t-200 t+200 -2 2])
    
    drawnow
    
    t = t + 1;
end

end
function cleanUpSerial(s)
    %% Clean up the serial port
    fclose(s);
    delete(s);
    clear s;
end


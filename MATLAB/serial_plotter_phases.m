function serial_plotter_phases()
clear all;
close all;


s = serial('COM5', 'BaudRate', 921600);
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

figure
load_Van_sensed = animatedline;
title("Load Van sensor")

figure
load_IAa_sensed = animatedline;
title("Load IAa sensor")


while(1)
    
    sync = 0;
    while(~sync)
        A = fread(s,2,'uint8'); 
        if(A(1) == 'A' && A(2) == 'a')
            sync = 1;
        end
    end
    
    %disp("Sync acquired")
    A = fread(s,3,'int8'); 
    ref = fread(s,3,'int8');
    B = fread(s,3,'int8'); 
    B = fread(s,3,'int8'); 
    B = fread(s,3,'int8');
    load_Vxn_sensed = fread(s,3,'int8'); 
    load_Vxn_sensed = load_Vxn_sensed / 10;
    load_IXx_sensed = fread(s,3,'int8'); 
    load_IXx_sensed = load_IXx_sensed / 20;
    
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
    addpoints(one_cell, t, B(3))
    axis([t-200 t+200 -2 2])
    
    addpoints(load_Van_sensed, t, load_Vxn_sensed(1))
    xlim([t-200,t+200])
    axis 'auto y'

    addpoints(load_IAa_sensed, t, load_IXx_sensed(1))
    xlim([t-200,t+200])
    axis 'auto y'
    
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


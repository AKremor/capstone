function [is_data, u] = getSerial()
    sync = 0;
    persistent s;
    if isempty(s) %true only the first time
        instrreset();
        s = serial('COM11', 'BaudRate', 500000);
        fopen(s);
    end
    
    if(s.BytesAvailable)
        while(~sync)
            A = fread(s,2,'uint8'); 
            if(A(1) == 'A' && A(2) == 'a')
                sync = 1;
            end
        end
        [u, data_read] = fread(s,3,'int8');
        
        if data_read == 3
           is_data = 1; 
        else
            is_data = 0;
        end
    else
        u = zeros(3,1);
        is_data = 0;
    end
end
  

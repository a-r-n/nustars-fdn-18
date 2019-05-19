clc
close all
clear

port_name = "/dev/cu.usbmodem14301";
BaudRate = 115200;
s = serial(port_name, 'BaudRate', BaudRate);

big_index = 1;

% infinite loop!!!
while 1==1
fopen(s);
data = fread(s);
fclose(s);
packet_size = 69; % lol
dataLength = length(data)/packet_size;

entries = [];

for i = 1:dataLength
  
  offset = (i - 1) * packet_size + 1;
  headers{i} = [data(offset), data(offset + 1), data(offset + 2)];
  times(i) = typecast(uint8(data(offset + 4:offset+7)), 'uint32');
  xAccMUX(i) = typecast(uint8(data(offset + 8:offset + 11)), 'single');
  yAccMUX(i) = typecast(uint8(data(offset + 12:offset + 15)), 'single');
  zAccMUX(i) = typecast(uint8(data(offset + 16:offset + 19)), 'single');
  xRotMUX(i) = typecast(uint8(data(offset + 20:offset + 23)), 'single');
  yRotMUX(i) = typecast(uint8(data(offset + 24:offset + 27)), 'single');
  zRotMUX(i) = typecast(uint8(data(offset + 28:offset + 31)), 'single');
  
  xAccDXL(i) = typecast(uint8(data(offset + 32:offset + 35)), 'single');
  yAccDXL(i) = typecast(uint8(data(offset + 36:offset + 39)), 'single');
  zAccDXL(i) = typecast(uint8(data(offset + 40:offset + 43)), 'single');
  
  pressure(i) = typecast(uint8(data(offset + 44:offset + 47)), 'single');
  temperature(i) = typecast(uint8(data(offset + 48:offset + 51)), 'single');
  
  lat(i) = typecast(uint8(data(offset + 52:offset + 55)), 'single');
  lng(i) = typecast(uint8(data(offset + 56:offset + 59)), 'single');
  gpsAlt(i) = typecast(uint8(data(offset + 60:offset + 63)), 'single');
  numSat(i) = typecast(uint8(data(offset + 64:offset+67)), 'uint32');
  
  raven(i) = data(offset + 68);
  
  
    % big fuckin' arrays
    
    big_times(big_index) = times(i);
    big_xAccMUX(big_index) = xAccMUX(i);
    big_yAccMUX(big_index) = yAccMUX(i);
    big_zAccMUX(big_index) = zAccMUX(i);
    big_xRotMUX(big_index) = xRotMUX(i);
    big_yRotMUX(big_index) = yRotMUX(i);
    big_zRotMUX(big_index) = zRotMUX(i);
    big_xAccDXL(big_index) = xAccDXL(i);
    big_yAccDXL(big_index) = yAccDXL(i);
    big_zAccDXL(big_index) = zAccDXL(i);
    big_pressure(big_index) = pressure(i);
    big_temperature(big_index) = temperature(i);
    big_lat(big_index) = lat(i);
    big_lng(big_index) = lng(i);
    big_gpsAlt(big_index) = gpsAlt(i);
    big_numSat(big_index) = numSat(i);
    big_raven(big_index) = raven(i);
    
    big_index = big_index + 1;
  
end


[times, tord] = sort(times);
yAccMUX = yAccMUX(tord);
xAccMUX = xAccMUX(tord);
zAccMUX = zAccMUX(tord);
pressure = pressure(tord);

absAccMUX = sqrt(xAccMUX.^2 + yAccMUX.^2 + zAccMUX.^2);


dv = diff(pressure)./cast(diff(times),'single');

end
%plot(times, absAccMUX);

% Calculating altitude
pressure_d = big_pressure./100;
seaLevelhPa = mean(pressure_d(1:50));
altitude = 44330 * (1.0 - ((pressure_d./ seaLevelhPa).^0.1903));

% Differentiating altitude to get velocity
a_time = cast(big_times, 'single') ./1000;
v = diff(altitude) ./ diff(a_time);

%Fixing acceleration (incase this is necessary)
% Assumes it flipped at apogee
[m, i] = max(altitude);
f_acc = [abs(big_yAccMUX(1:i)-9.81), (big_yAccMUX(i+1:length(big_yAccMUX))+9.81)];

% Kalman filter
start_index = 1;
end_index = length(a_time);
[xf, pf] = Kalman_example(a_time, altitude, f_acc, start_index, end_index);
[xs, ps] = Kalman_example(a_time, altitude, big_yAccMuX, start_index, end_index);

% Converting to imperial
altitude_imp = altitude * 3.2808;
v_imp = 3.2808 * v;
vs_imp = 3.2808 * xs(2, :);
vf_imp = 3.2808 * xf(2, :);

% Plotting things
figure('Position', [300, 900, 700, 800])
subplot(3, 1, 1);
plot(a_time, f_acc);
title('Acceleration');
subplot(3, 1, 2);
hold on
plot(a_time(2:end), v, a_time(2:end), xf(2, 2:end));
legend('Differentiated altitude', 'Kalman Filter');
title('Velocity');
hold off
subplot(3, 1, 3);
plot(e_time, altitude_l);
title('Altitude');




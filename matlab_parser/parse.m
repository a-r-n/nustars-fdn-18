clear;
dfile = fopen('received.log');
data = fread(dfile, Inf);
v = data(find(data == 10));
%COMMENT OUT THIS LINE WHEN ANALYZING FLASH DATA
%data = data(v(2) + 1 : end);
%END LINE TO COMMENT OUT

dataLength = length(data)
eMatch = (data == ['N' 'U' 'x']);
xMatch = zeros(1, length(eMatch));
for i = 1:length(eMatch)-2
    if eMatch(i, 1) && eMatch(i+1, 2) && eMatch(i+2, 3);
      xMatch(i) = 1;
    end
  end
match = find(xMatch);

entries = [];
for i = 1:length(match)-1
  offset = match(i);
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


end

%{
[times, tord] = sort(times);
yAccMUX = yAccMUX(tord);
xAccMUX = xAccMUX(tord);
zAccMUX = zAccMUX(tord);
pressure = pressure(tord);
%}

absAccMUX = sqrt(xAccMUX.^2 + yAccMUX.^2 + zAccMUX.^2);


dv = diff(pressure)./cast(diff(times),'single');

%plot(times, absAccMUX);

% Calculating altitude
pressure_d = pressure./100;
seaLevelhPa = mean(pressure_d(1:50));
altitude = 44330 * (1.0 - ((pressure_d./ seaLevelhPa).^0.1903));

% Differentiating altitude to get velocity
a_time = cast(times, 'single') ./1000;
v = diff(altitude) ./ diff(a_time);

%Fixing acceleration (incase this is necessary)
% Assumes it flipped at apogee
[m, i] = max(altitude);
f_acc = [abs(yAccMUX(1:i)-9.81), (yAccMUX(i+1:length(yAccMUX))+9.81)];

% Kalman filter
start_index = 1;
end_index = length(a_time);
[xf, pf] = Kalman_example(a_time, altitude, f_acc, start_index, end_index);
[xs, ps] = Kalman_example(a_time, altitude, yAccMUX, start_index, end_index);

% Converting to imperial
altitude_imp = altitude * 3.2808;
v_imp = 3.2808 * v;
vs_imp = 3.2808 * xs(2, :);
vf_imp = 3.2808 * xf(2, 2:end);
f_acc_imp = 3.2808 * f_acc;
yAccMUX_imp = 3.2808 * yAccMUX;

% Plotting things in metric
% f = figure('Position', [300, 900, 700, 800]);
% subplot(3, 1, 1);
% plot(a_time, f_acc);
% title('Acceleration');
% subplot(3, 1, 2);
% hold on
% plot(a_time(2:end), v, a_time(2:end), xf(2, 2:end));
% legend('Differentiated altitude', 'Kalman Filter');
% title('Velocity');
% hold off
% subplot(3, 1, 3);
% plot(a_time, altitude);
% title('Altitude');
% movegui(f,'south');

% Plotting things in imperial 
f = figure('Position', [300, 900, 700, 800]);
subplot(3, 1, 1);
plot(a_time, f_acc_imp);
xlabel("time (seconds)");
ylabel("flipped acceleration (ft/s^2)");
title('Acceleration');
subplot(3, 1, 2);
hold on
plot(a_time(2:end), v_imp, a_time(2:end), vf_imp);
legend('Differentiated altitude', 'Kalman Filter');
title('Velocity');
xlabel("time (seconds)");
ylabel("velocity (ft/s)");
hold off
subplot(3, 1, 3);
plot(a_time, altitude_imp);
xlabel("time (seconds)");
ylabel("altitude (feet)");
title('Altitude');

movegui(f,'south');

figure;
plot(a_time, yAccMUX);

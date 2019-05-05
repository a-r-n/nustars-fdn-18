dfile = fopen('combo.hex');
data = fread(dfile, Inf);
dataLength = length(data)/70;

entries = [];

for i = 1:dataLength
  offset = (i - 1) * 70 + 1;
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

[times, tord] = sort(times);
yAccMUX = yAccMUX(tord);
xAccMUX = xAccMUX(tord);
zAccMUX = zAccMUX(tord);
pressure = pressure(tord);

absAccMUX = sqrt(xAccMUX.^2 + yAccMUX.^2 + zAccMUX.^2);


dv = diff(pressure)./diff(times);

plot(times, absAccMUX);

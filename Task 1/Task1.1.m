
N = 128;          % samples per cycle
RES = 4095;       % 12-bit max value

% Generate sample indices
n = 0:N-1;

% 1. Sine wave LUT 
sine_LUT = round((sin(2*pi*n/N) + 1) * (RES/2));

% 2. Sawtooth wave LUT 
saw_LUT = round((n / (N-1)) * RES);

% 3. Triangle wave LUT 
tri_LUT = zeros(1, N);
for i = 1:N
    if i <= N/2
        tri_LUT(i) = ((i-1) / (N/2)) * RES;
    else
        tri_LUT(i) = ((N - (i-1)) / (N/2)) * RES;
    end
end
tri_LUT = round(tri_LUT);

% --- Plotting ---
figure;

subplot(3,1,1);
plot(sine_LUT, 'r.-');
title('Sine Wave LUT (128 samples)');
ylabel('Amplitude');

subplot(3,1,2);
plot(saw_LUT, 'g.-');
title('Sawtooth Wave LUT (128 samples)');
ylabel('Amplitude');

subplot(3,1,3);
plot(tri_LUT, 'b.-');
title('Triangle Wave LUT (128 samples)');
ylabel('Amplitude');
xlabel('Sample index');

% --- Display arrays in command window (copy-paste for C code) ---
disp('Sine LUT:');
disp(sine_LUT);

disp('Sawtooth LUT:');
disp(saw_LUT);

disp('Triangle LUT:');
disp(tri_LUT);
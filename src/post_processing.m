tab = readtable("../data.csv");
arr = tab{:,:};
x = arr(:,1);


%x = x(1000:120000);
s_t = size(x);
L = s_t(1);
t = 1:s_t(1);

% Lowpass filter 500Hz
Freq = 12500;
NyqFreq = Freq/2; 

x_processed = x - mean(x);
%[B,A] = butter (4,[49,51]/NyqFreq,'stop');
%x_processed =filtfilt(B,A,x_processed);
x_processed = bandstop(x_processed,[48,50],Freq);
x_processed = bandstop(x_processed,[96,99],Freq);
x_processed = bandstop(x_processed,[145,148],Freq);
%x_processed = lowpass(x_processed,500,Freq);
%x_processed = highpass(x_processed,1,Freq);
%x_processed = lowpass(x_processed,1000,Freq);
[B,A] = butter (4,200/NyqFreq,'low');
x_processed =filtfilt(B,A,x_processed);
[B,A] = butter (4,20/NyqFreq,'high');
x_processed =filtfilt(B,A,x_processed);

Y= fft(x_processed);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);

f = Freq*(0:(L/2))/L;

plot(f,P1) 
title('Single-Sided Amplitude Spectrum of X(t)')
xlabel('f (Hz)')
ylabel('|P1(f)|')
xlim([0,500]);






% [B,A] = butter (1,10/NyqFreq,'high');
% x_processed =filtfilt(B,A,x_processed);
% 



% x_processed = abs(x_processed);

% Lowpass = 5;
% Wn = Lowpass/NyqFreq;
% [B,A] = butter (4,5/NyqFreq,'low');
% x_processed =filtfilt(B,A,x);

figure;
 
 plot(t,x);
 hold on;
plot(t,x_processed);

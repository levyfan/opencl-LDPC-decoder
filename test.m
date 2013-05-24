%% Clear workspace
clear all; close all; clc;
% profile on

%% Parameters
%1. Noise
snr_db = 2.17; sigma2 = 1/10^(snr_db/10); sigma_n = sqrt(sigma2);

%2. Random Seed
seed = sum(fix(clock)); RandStream.setGlobalStream(RandStream('mt19937ar','seed',seed));

%3. LDPC H/A matrix
rate = 3/5; %  1/4, 1/3, 2/5, 1/2, 3/5, 2/3, 3/4, 4/5, 5/6, 8/9, and 9/10
H = dvbs2ldpc(rate);
[m,n] = size(H); k = n-m;
% load p50\p50ok1.mat;
% load p50\p50ok1_alphas.mat;

%4. LDPC encoder
henc = fec.ldpcenc(H);

%5. LDPC decoder
hdec = ldpcdec(H, 'cl\Kernels_sp.cl', 1);

%6. nFrames & nErrors
nframes = 0.5 * 1e3; % 1 million
err_lim = 1e5; % 0.1 million

%7. log
fid = fopen(['test_sp_snr' num2str(snr_db) '.log'], 'w');
fprintf(fid, 'snr = %g dB\n', snr_db);
fprintf(fid, 'QPSK, AWGN\n');
fprintf(fid, 'seed = %d\n', seed);

%% Ber Test
err = 0; total_bits = 0; err_idx = 0;
tic
for frame = 1:nframes
    inf_bits = randi([0,1],1,k);
    codeword = encode(henc,inf_bits);
    % Modulate the signal (map bit 0 to 1 + 0i, bit 1 to -1 + 0i)
    tx = 1-double(codeword)*2;
    % awgn channel
    rx = awgn(tx, snr_db);
    % Compute log-likelihood ratios (AWGN channel)
    llr = rx * 2 / sigma2;
    
    [iter, decoded_bits] = hdec.decode(llr, 50);
    
    currentErr = nnz(decoded_bits(1:k) - inf_bits); % inf bits
    err = err + currentErr;
    total_bits = total_bits + k; % inf bits only
    if currentErr ~= 0
        err_idx = err_idx + 1;
        fprintf(fid, '%d: frame = %d; errInThisFrame = %d; ber = %g; fer = %g', err_idx, frame, currentErr, err/total_bits, err_idx/frame);
        if iter ~= 50
            fprintf(fid, '; undetected error!');
        end
        fprintf(fid, '\n');
    end
    if mod(frame,100) == 0
        disp([num2str(toc) ', snr = ' num2str(snr_db) 'dB, frame = ' num2str(frame) ', err = ' num2str(err) ', ber = ' num2str(err/total_bits) ', fer = ' num2str(err_idx/frame)]);
    end
    if err > err_lim
        break;
    end
end
disp([num2str(toc) ', frame = ' num2str(frame) ', err = ' num2str(err) ', ratio = ' num2str(err/total_bits) ', fer = ' num2str(err_idx/frame)]);
fprintf(fid, 'frame = %d, err = %d, ber = %g, fer = %g\n', frame, err, err/total_bits, err_idx/frame);

%% Finalize
hdec.delete();
clear hdec;
fclose(fid);
% profile viewer;
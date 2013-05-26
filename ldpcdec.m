classdef ldpcdec < handle
    properties
        hdec;
        decoded_softs_buffer;
    end
    
    methods
      function obj = ldpcdec(H, kernel_cl_file, alpha)
            [m, n] = size(H);
            [row, col] = find(H);
            [chkind, itlver] = sort((row-1)*n + col);
            chkind = mod(chkind-1, n);
            itlver = itlver-1;
            l = length(itlver);
            rowsta = cumsum([0 full(sum(H,2))']);
            colsta = cumsum([0 full(sum(H,1))]);
            ROW_MAX = max(sum(H,2));
            COL_MAX = max(sum(H));
            
            src = fileread(kernel_cl_file);
            
            obj.hdec = mexCreateLDPCDecoder(uint32(rowsta), uint32(colsta), uint32(itlver), uint32(chkind), m, n, l, ROW_MAX, COL_MAX, alpha, src);
            obj.decoded_softs_buffer = single(zeros(1, n));
      end
      
      function [iter, decoded_bits] = decode(obj, llr, maxIter)
          iter = mexDecodeLDPC(obj.hdec, single(llr), maxIter, obj.decoded_softs_buffer);
          decoded_bits = (obj.decoded_softs_buffer<0);
      end
      
      function delete(obj)
          mexDestroyLDPCDecoder(obj.hdec);
      end
   end
end
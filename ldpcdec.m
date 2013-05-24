classdef ldpcdec < handle
    properties
        hdec;
        decoded_softs_buffer;
    end
    
    methods
      function obj = ldpcdec(H, kernel_cl_file, alpha)
            [m, n] = size(H);
            [row, col] = find(H);
            [~, itlver] = sort((row-1)*n + col);
            chkind = int32(col(itlver)'-1);
            itlver = int32(itlver'-1);
            l = length(itlver);
            rowsta = int32(cumsum([0 full(sum(H,2))']));
            colsta = int32(cumsum([0 full(sum(H,1))]));
            ROW_MAX = max(sum(H,2));
            COL_MAX = max(sum(H));
            
            src = fileread(kernel_cl_file);
            
            obj.hdec = mexCreateLdpcDec(rowsta, colsta, itlver, chkind, m, n, l, ROW_MAX, COL_MAX, single(alpha), src);
            obj.decoded_softs_buffer = single(zeros(1, n));
      end
      
      function [iter, decoded_bits] = decode(obj, llr, maxIter)
          iter = mexDecodeLdpc(obj.hdec, single(llr), maxIter, obj.decoded_softs_buffer);
          decoded_bits = (obj.decoded_softs_buffer<0);
      end
      
      function delete(obj)
          mexDestroyLdpcDec(obj.hdec);
      end
   end
end
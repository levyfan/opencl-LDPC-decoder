clear all; close all; clc;

mex -g -I"C:\Program Files (x86)\ATI Stream\include" -lOpenCL -L"C:\Program Files (x86)\ATI Stream\lib\x86_64" COMPFLAGS="$COMPFLAGS /MD /Qopenmp /O2" LINKFLAGS="$LINKFLAGS /MACHINE:X64" mexCreateLdpcDec.cpp clLDPCdec.cpp clException.cpp Util.cpp
mex -g -I"C:\Program Files (x86)\ATI Stream\include" -lOpenCL -L"C:\Program Files (x86)\ATI Stream\lib\x86_64" COMPFLAGS="$COMPFLAGS /MD /Qopenmp /O2" LINKFLAGS="$LINKFLAGS /MACHINE:X64" mexDestroyLdpcDec.cpp clLDPCdec.cpp clException.cpp Util.cpp
mex -g -I"C:\Program Files (x86)\ATI Stream\include" -lOpenCL -L"C:\Program Files (x86)\ATI Stream\lib\x86_64" COMPFLAGS="$COMPFLAGS /MD /Qopenmp /O2" LINKFLAGS="$LINKFLAGS /MACHINE:X64" mexDecodeLdpc.cpp clLDPCdec.cpp clException.cpp Util.cpp

% mex('-I', ...
%     'C:\Program Files (x86)\ATI Stream\include', ...
%     '-lOpenCL', ...
%     '-L', ...
%     'C:\Program Files (x86)\ATI Stream\lib\x86_64', ...
%     'COMPFLAGS=/Qopenmp', ...
%     'mexCreateLdpcDec.cpp', 'clLDPCdec.cpp');
function [ cell_string ] = mex_tokenize( string )
%TOKENIZE Summary of this function goes here
%   Detailed explanation goes here
i = 0;
while true
    [token, string] = strtok(string);
    if isempty(token)
        break;
    end
    i = i+1;
    cell_string(i) = cellstr(token);
end
end


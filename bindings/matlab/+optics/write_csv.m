function write_csv(path, xName, yNames, x, ySets)
%WRITE_CSV Write multi-column CSV (examples helper).
    if ~exist('output', 'dir')
        mkdir('output');
    end
    fid = fopen(path, 'w');
    fprintf(fid, '%s', xName);
    for k = 1:numel(yNames)
        fprintf(fid, ',%s', yNames{k});
    end
    fprintf(fid, '\n');
    n = numel(x);
    for i = 1:n
        fprintf(fid, '%.17g', x(i));
        for k = 1:numel(ySets)
            fprintf(fid, ',%.17g', ySets{k}(i));
        end
        fprintf(fid, '\n');
    end
    fclose(fid);
    fprintf('wrote %s\n', path);
end

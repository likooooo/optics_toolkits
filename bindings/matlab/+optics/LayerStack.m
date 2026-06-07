classdef LayerStack < handle
    %LAYERSTACK RAII wrapper around optics_layer_stack C handle.
    properties (Access = private)
        handle = 0
    end

    methods
        function obj = LayerStack()
            obj.handle = optics.tmm('create');
        end

        function delete(obj)
            if obj.handle ~= 0
                optics.tmm('destroy', obj.handle);
                obj.handle = 0;
            end
        end

        function addLayer(obj, nkRe, nkIm, depth)
            optics.tmm('add_layer', obj.handle, nkRe, nkIm, depth);
        end

        function h = getHandle(obj)
            h = obj.handle;
        end
    end
end

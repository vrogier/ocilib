-- Initialization

create package test_plsqltables as
    type string_tab is table of varchar2(50) index by binary_integer;
    procedure combine_values(value1 in string_tab, value2 in string_tab, result out string_tab);
end;
/

create package body test_plsqltables as
    procedure combine_values(value1 in string_tab, value2 in string_tab, result out string_tab) is
    begin
        for i in 1 .. value1.count 
        loop
            result(i) := value1(i) || ' - ' || value2(i);
        end loop;      
    end;
end;
/

-- Cleanup

drop package test_plsqltables;

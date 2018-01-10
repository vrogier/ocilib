-- Initialization

create type base_type as object
( 
    code  number, 
    name  varchar2(30)
) not final; 
/

create type derived_type under base_type
( 
    price  float
); 
/

-- Cleanup

drop type derived_type;
drop type base_type;

-- Initialization

create type vehicule_type as object
( 
    code  number, 
    name  varchar2(30)
) not final; 
/

create type car_type under vehicule_type
( 
    price  number
) not final; 
/

create type racing_car_type under car_type
( 
    speed  number
); 
/

-- Cleanup

drop type racing_car_type;
drop type car_type;
drop type vehicule_type;

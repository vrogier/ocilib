-- Initialization

create type t_vendor as object
( 
    code  number, 
    name  varchar2(30)
); 
/
		
create type t_sale as object 
( 
    code  number, 
    price  float, 
    name  varchar2(30),
    ref  varchar2(30), 
    date_sale date, 
    vendor  t_vendor
); 
/

create table sales(item t_sale);

-- Cleanup

drop table sales;
drop type t_sale;
drop type t_vendor;

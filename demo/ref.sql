-- Initialization

create type vendor_t as object
( 
    code  number, 
    name  varchar2(30)
); 
/		

create table vendors of vendor_t;
insert into vendors values (vendor_t(1, 'John Doe'));
commit;

-- Cleanup

drop table vendors;
drop type vendor_t;


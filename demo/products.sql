-- Initialization

create table products (code int, name varchar2(50));
insert into products values (1, 'product 1');
insert into products values (2, 'product 2');
commit;

-- Cleanup
 
drop table products;

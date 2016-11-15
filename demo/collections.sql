-- Initialization

create type product_t as object (code int, name varchar2(50));
/
create type product_varray_t as varray(3) of product_t;
/
create type product_table_t as table of product_t;
/
create table products_varray (category varchar2(50), products product_varray_t);
create table products_nested_table(category varchar2(50), products product_table_t) nested table products store as products_nested_table_products;

insert into products_varray values ('category A', product_varray_t(product_t(1, 'name 1'), product_t(2, 'name 2'), product_t(3, 'name 3')));
insert into products_varray values ('category B', product_varray_t(product_t(4, 'name 4'), product_t(5, 'name 5'), product_t(6, 'name 6')));
insert into products_nested_table values ('category C', product_table_t(product_t(5, 'name 5'), product_t(7, 'name 7')));
insert into products_nested_table values ('category D', product_table_t(product_t(6, 'name 6'), product_t(8, 'name 8')));
commit;

-- Cleanup
 
drop table products_nested_table;
drop table products_varray;
drop type product_table_t;
drop type product_varray_t;
drop type product_t;

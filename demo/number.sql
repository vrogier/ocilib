-- Initialization

create type test_num_coll_t as varray(10) of number;
/
create type test_num_t as object (value number);
/
create table test_number (value number);

insert into test_number values(3.14);
insert into test_number values(5.28);

-- Cleanup

drop table test_number;
drop type test_num_coll_t;
drop type test_num_t;

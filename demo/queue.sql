-- Initialization

create type message_t as object
( 
    title   varchar2(50), 
    content varchar2(100)
); 
/

-- Cleanup
 
drop type message_t;

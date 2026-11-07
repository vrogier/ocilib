-- =============================================================================
-- OCILIB tutorials — running project schema
--
-- A tiny "employee directory" used by every tutorial page.
-- Run it once as the user you will connect with in the samples, e.g.:
--
--     sqlplus scott/tiger@//localhost:1521/XEPDB1 @tutorial-schema.sql
-- =============================================================================

DROP TABLE employees    CASCADE CONSTRAINTS;
DROP TABLE departments  CASCADE CONSTRAINTS;

CREATE TABLE departments
(
    deptno      NUMBER(4)       CONSTRAINT pk_departments PRIMARY KEY,
    dname       VARCHAR2(30)    NOT NULL,
    loc         VARCHAR2(30)
);

CREATE TABLE employees
(
    empno       NUMBER(6)       CONSTRAINT pk_employees PRIMARY KEY,
    ename       VARCHAR2(30)    NOT NULL,
    job         VARCHAR2(20),
    mgr         NUMBER(6)       REFERENCES employees (empno),
    hiredate    DATE            DEFAULT SYSDATE NOT NULL,
    sal         NUMBER(8, 2),
    comm        NUMBER(8, 2),
    deptno      NUMBER(4)       REFERENCES departments (deptno)
);

INSERT INTO departments (deptno, dname, loc) VALUES (10, 'ACCOUNTING', 'NEW YORK');
INSERT INTO departments (deptno, dname, loc) VALUES (20, 'RESEARCH',   'DALLAS');
INSERT INTO departments (deptno, dname, loc) VALUES (30, 'SALES',      'CHICAGO');
INSERT INTO departments (deptno, dname, loc) VALUES (40, 'OPERATIONS', 'BOSTON');

INSERT INTO employees (empno, ename, job, mgr, hiredate, sal, comm, deptno)
VALUES (7369, 'SMITH',  'CLERK',     7902, DATE '1980-12-17',  800, NULL, 20);
INSERT INTO employees (empno, ename, job, mgr, hiredate, sal, comm, deptno)
VALUES (7499, 'ALLEN',  'SALESMAN',  7698, DATE '1981-02-20', 1600,  300, 30);
INSERT INTO employees (empno, ename, job, mgr, hiredate, sal, comm, deptno)
VALUES (7521, 'WARD',   'SALESMAN',  7698, DATE '1981-02-22', 1250,  500, 30);
INSERT INTO employees (empno, ename, job, mgr, hiredate, sal, comm, deptno)
VALUES (7566, 'JONES',  'MANAGER',   7839, DATE '1981-04-02', 2975, NULL, 20);
INSERT INTO employees (empno, ename, job, mgr, hiredate, sal, comm, deptno)
VALUES (7698, 'BLAKE',  'MANAGER',   7839, DATE '1981-05-01', 2850, NULL, 30);
INSERT INTO employees (empno, ename, job, mgr, hiredate, sal, comm, deptno)
VALUES (7782, 'CLARK',  'MANAGER',   7839, DATE '1981-06-09', 2450, NULL, 10);
INSERT INTO employees (empno, ename, job, mgr, hiredate, sal, comm, deptno)
VALUES (7839, 'KING',   'PRESIDENT', NULL, DATE '1981-11-17', 5000, NULL, 10);
INSERT INTO employees (empno, ename, job, mgr, hiredate, sal, comm, deptno)
VALUES (7902, 'FORD',   'ANALYST',   7566, DATE '1981-12-03', 3000, NULL, 20);

COMMIT;

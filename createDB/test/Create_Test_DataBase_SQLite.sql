CREATE TABLE Sequence (
	name text,
	curr_val integer
);

INSERT INTO Sequence (name, curr_val) VALUES ('sequence_id', 1);

CREATE TABLE test_table (
	test_id INTEGER PRIMARY KEY,
	test_text text,
	test_char200 character varying(200),
	test_date timestamp with time zone,
	test_int_not_null integer NOT NULL
);
CREATE TRIGGER autoincrement_test_table AFTER INSERT ON test_table 
  BEGIN
    UPDATE test_table SET test_id=(select curr_val+1 from Sequence where name='sequence_id') where test_id is null;
    UPDATE Sequence SET curr_val=curr_val+1 where             		 name='sequence_id';
  END;

CREATE TABLE table_for_thread (
    field integer
);

